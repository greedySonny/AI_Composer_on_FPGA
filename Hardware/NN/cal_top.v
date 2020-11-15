




// 流水线结�?


module cal_top
#(
    parameter DW                    = 16,
    parameter BATCH_LENGTH          = 16


)(

    input                           clk,
    input                           rst_n,

//    input                           enable_one_minus,


    input   [DW*BATCH_LENGTH-1:0]   din,
    input   [DW*BATCH_LENGTH-1:0]   weight_in,
    input   [DW-1:0]                bias_in,

    input                           en,
    output                          valid,

    input                           extend_en,
    //output reg [DW-1:0]             channel_sum
    output [DW-1:0]                 channel_sum,

    output                          product_valid,
    output [DW*BATCH_LENGTH-1:0]    product,

    // debug
    output [15:0]                   cal_din_dbg,
    output [15:0]                   cal_weight_dbg,
    output [15:0]                   p_dbg

);


    wire signed [DW-1:0]                cal_din[0:BATCH_LENGTH-1];
    wire signed [DW-1:0]                cal_weight[0:BATCH_LENGTH-1];
    wire signed [DW-1:0]                p[0:BATCH_LENGTH-1];

    wire signed [DW-1:0]                cal_A[0:BATCH_LENGTH-1];
    wire signed [DW-1:0]                cal_B[0:BATCH_LENGTH-1];

    wire signed [DW-1:0]                p_add[0:BATCH_LENGTH-1];

    genvar multiadd_i;
    generate

        for(multiadd_i=0; multiadd_i<BATCH_LENGTH; multiadd_i=multiadd_i+1) begin

            assign cal_din[multiadd_i] = din[multiadd_i*DW+:DW];
            assign cal_weight[multiadd_i] = weight_in[multiadd_i*DW+:DW];

            assign cal_A[multiadd_i] = cal_din[multiadd_i];
            assign cal_B[multiadd_i] = cal_weight[multiadd_i];
            assign p_add[multiadd_i] = extend_en ? (p[multiadd_i]>>>2) : p[multiadd_i];

            mult_gen_0 multi(
                .CLK            (clk),
                .A              (cal_A[multiadd_i]),
                .B              (cal_B[multiadd_i]),
                .P              (p[multiadd_i])
            );
            assign product[multiadd_i*DW+:DW] = p[multiadd_i];
        end

    endgenerate

    wire [DW-1:0] sum[0:BATCH_LENGTH-2];

    genvar add_i, add_j, add_k;
    generate
        for(add_i=0; add_i<BATCH_LENGTH; add_i=add_i+2) begin

            // 16 -> 8
            ip_add add_16_8(
                .CLK            (clk),
                .A              (p_add[add_i]),
                .B              (p_add[add_i+1]),
                .S              (sum[add_i/2])
            );
        end

        for(add_j=0; add_j<BATCH_LENGTH/2; add_j=add_j+2) begin

            ip_add add_8_4(
                .CLK            (clk),
                .A              (sum[add_j]),
                .B              (sum[add_j+1]),
                .S              (sum[(BATCH_LENGTH+add_j)/2])
            );
        end
        for(add_k=0; add_k<BATCH_LENGTH/4; add_k=add_k+2) begin

            ip_add add_4_2(
                .CLK            (clk),
                .A              (sum[add_k+(BATCH_LENGTH/2)]),
                .B              (sum[add_k+(BATCH_LENGTH/2)+1]),
                .S              (sum[((BATCH_LENGTH+add_k)/2)+(BATCH_LENGTH/4)])
            );

        end
        ip_add add_2_1(
            .CLK            (clk),
            .A              (sum[12]),
            .B              (sum[13]),
            .S              (sum[14])
        );
    endgenerate





    reg [6:0] valid_shift;

    always @(posedge clk) valid_shift <= {valid_shift[5:0], en};
    assign valid = valid_shift[6];

    reg [2:0] product_valid_shift;
    always @(posedge clk) product_valid_shift <= {product_valid_shift[1:0], en};
    assign product_valid = product_valid_shift[2];

    assign channel_sum = sum[14];

    assign cal_din_dbg = cal_din[0];
    assign cal_weight_dbg = cal_weight[0];
    assign p_dbg = p[0];




endmodule



