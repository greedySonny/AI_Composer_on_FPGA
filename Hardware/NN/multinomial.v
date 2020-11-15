


/* -------------------------------
 * 
 * 从lfsr拿到一个目标值
 * 对输入进行遍历自加。
 * 当累加和大于目标值时，则返回大小关系发生改变时的数值的索引
 *
 *
 *
 *
 *
 *
 *
 * -------------------------------
 */






module multinomial
#(
    parameter DW = 40
)
(

    input clk,
    input rst_n,

    input [DW-1:0] din,
    input add_en,
    input reload,

    input [31:0] seed_in,
    input seed_en,

    input chord_flag,

    output reg valid,
    output reg [7:0] index_dout,

    output wire [DW-1:0] target_dbg,
    output wire [DW-1:0] sum_dbg,
    output wire [DW-1:0] accum_dbg,
    output wire [3:0] state_dbg,
    output wire [6:0] accum_cnt_dbg,
    output wire [DW-1: 0] rdata_dbg 
    
);
    wire [DW-1:0] data_in;


    reg [DW-1:0] sum;
    reg [DW-1:0] accum;
    reg accum_en;
    reg [6:0] accum_cnt;

    assign data_in =    ((!chord_flag) & (accum_cnt<48)) ? 
                            din:
                            (chord_flag & ((accum_cnt>=48)|(accum_cnt==0))) ?
                                din:
                                0;

    // assign data_in = din;
    
    wire [DW-1:0] lfsr_out;
    reg [DW-1:0] target;
    wire [DW-1:0] target_res;
    wire add_en_falling, add_en_raising;
    wire mod_en;
    wire mod_valid;
    (* ram_style="block" *) reg [DW-1:0] data[0:97];


    localparam  ST_IDLE         = 0,
                ST_SUM          = 1,
                ST_MOD          = 2,
                ST_FIND         = 3;
    reg [3:0] state;
    reg [3:0] state_next;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) state <= 0;
        else state <= state_next;
    end

    always @(*) begin
        if(!rst_n) state_next <= 0;
        else begin
            case(state)
                ST_IDLE: begin
                    if(add_en) state_next <= ST_SUM;
                    else state_next <= state;
                end
                ST_SUM: begin
                    if(add_en_falling) state_next <= ST_MOD;
                    else state_next <= state;
                end
                ST_MOD: begin
                    if(mod_valid) state_next <= ST_FIND;
                    else state_next <= state;
                end
                ST_FIND: begin
                    if(valid) state_next <= ST_IDLE;
                    else state_next <= state;
                end
                default: state_next <= ST_IDLE;
            endcase
        end
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) target <= 0;
        else if(mod_valid) target <= target_res;
    end

    assign mod_en = add_en_falling;

    my_mod u_mymod(
        .clk(clk),
        .en(mod_en),
        .valid(mod_valid),
        .in0(lfsr_out),
        .in1(sum),
        .out(target_res)
    );

    // step1: get sum by 98 cycles around
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) sum <= 0;
        else if(reload) sum <= 0;
        //else if(add_en) sum <= sum + din;
        else if(add_en) sum <= sum + data_in;
    end
    reg add_en_d;
    
    always @(posedge clk) add_en_d <= add_en;
    assign add_en_falling = add_en_d & (!add_en);
    assign add_en_raising = add_en & (!add_en_d);

    // step2: get accum cycle by cycle and judge with target.
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) accum_en <= 0;
        else if((state==ST_FIND)) accum_en <= 1;
        else accum_en <= 0;
    end


    reg valid_d1; reg valid_d2;
    always @(posedge clk) valid_d1 <= valid;
    always @(posedge clk) valid_d2 <= valid_d1;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) accum_cnt <= 0;
        else if(valid_d2) accum_cnt <= 0;
        else if((accum_cnt==97)&((state==ST_SUM))) accum_cnt <= 0;
        else if((accum_cnt==99)&((state==ST_FIND))) accum_cnt <= 0;
        else if((state==ST_FIND)|add_en)
            accum_cnt <= accum_cnt + 1;
    end

    // always @(posedge clk, negedge rst_n) begin
    //     if(!rst_n) accum_cnt <= 0;
    //     else if(accum_cnt==97) accum_cnt <= 0;
    //     else if(valid) accum_cnt <= 0;
    //     else if((state==ST_FIND)|add_en)
    //         accum_cnt <= accum_cnt + 1;
    //     else accum_cnt <= 0;
    // end

    reg [DW-1:0] rdata;
    always @(posedge clk) rdata <= data[accum_cnt];
    always @(posedge clk) begin
        if(add_en)
            // data[accum_cnt] <= din;
            data[accum_cnt] <= data_in;
    end
    
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) accum <= 0;
        else if(accum_en) accum <= accum + rdata;
        else accum <= 0;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) index_dout <= 0;
        else if(state==ST_SUM) index_dout <= 0;
        else if((accum>target) & (state==ST_FIND)) 
            index_dout <= accum_cnt - 2;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) valid <= 0;
        else if(valid) valid<= 0;
        else if((accum>target) & (state==ST_FIND))
            valid <= 1;
    end

    
    lfsr#(
        .DW(40)
    ) u_lfsr
    (
        .clk        (clk),
        .rst_n      (rst_n),

        .seed_en    (seed_en),
        //.seed_en    (0),
        .seed_in    (seed_in),
        //.seed_in    (1),
        .dout       (lfsr_out)
    );

    assign target_dbg = target;
    assign sum_dbg = sum;
    assign accum_dbg = accum;
    assign state_dbg = state;
    assign accum_cnt_dbg = accum_cnt;
    assign rdata_dbg = rdata;

endmodule



