


/* ------------------------------------------------
 *
 * sigmoid 实现
 *
 *
 *  pipeline结构。
 *  
 *  计算过程为 y = k0*x*x + k1*x + b
 *  1. square = x*x                 1 cycle
 *  2. add0 = k0*square             1 cycle
 *  3. add1 = k1*x                  1 cycle
 *  4. sum = add0 + add1 + b        0 cycle
 *  5. result = inv_abs(sum)        1 cycle
 *
 * ------------------------------------------------ 
 */


module sigmoid(

    input                       clk,
    input                       rst_n,

    input   [15:0]              din,
    input                       en,

    output  [15:0]              dout,
    output                      valid

);

    reg    [15:0]              din_abs[0:3];
    wire   [15:0]              din_abs_w;

    reg     [3:0]         sign;
    always @(posedge clk) sign <= {sign[2:0], din[15]};

    // 考虑要不要加入小数点动态调整功能防止溢出
    reg  [15:0]     square;
    reg  [15:0]     add0[0:1];
    reg  [15:0]     add1;
    wire signed [15:0]     sum;
    reg  [15:0]     result;

    wire [15:0] mult_square_0;
    wire [15:0] mult_square_1;
    wire [15:0] mult_square_p;

    wire [15:0] mult_add0_0;
    wire [15:0] mult_add0_1;
    wire signed [15:0] mult_add0_p;

    wire [15:0] mult_add1_0;
    wire [15:0] mult_add1_1;
    wire [15:0] mult_add1_p;

// stage 0:
    reg stage_0_valid;
    always @(posedge clk) stage_0_valid <= en;

    // 判断输入区间
    localparam  RANGE_0_2048        = 0,
                RANGE_2049_10035    = 1,
                RANGE_10036_11585   = 2,
                // 从这里开始，在din>11585(2.828)时，在平方时会溢出
                RANGE_11586_16384   = 3,
                RANGE_16485_22938   = 4,
                RANGE_22939_30720   = 5,
                RANGE_MORE          = 6;

    reg [3:0]   range_sel[0:2];
    
    wire [14:0] din_data_comp;
    assign din_data_comp = ~(din[14:0]-1);
    assign din_abs_w = din[15] ? {1'b0, din_data_comp} : din;
    genvar stage_0_i;
    generate
        always @(posedge clk) begin
            if(din_abs_w<=16'd2048)       range_sel[0] <= RANGE_0_2048;
            else if(din_abs_w<=16'd10035) range_sel[0] <= RANGE_2049_10035;
            else if(din_abs_w<=16'd11585) range_sel[0] <= RANGE_10036_11585;
            else if(din_abs_w<=16'd16384) range_sel[0] <= RANGE_11586_16384;
            else if(din_abs_w<=16'd22938) range_sel[0] <= RANGE_16485_22938;
            else if(din_abs_w<=16'd30720) range_sel[0] <= RANGE_22939_30720;
            else                          range_sel[0] <= RANGE_MORE;
        end
        // 由于sigmoid是中心对称，因此求一边绝对值运算，再还原即可
        always@(posedge clk) begin
            if(din[15]) din_abs[0] <= {1'b0, din_data_comp};
            else din_abs[0] <= din;
        end

        for(stage_0_i=1;stage_0_i<3;stage_0_i=stage_0_i+1) begin
            always @(posedge clk) range_sel[stage_0_i] <= range_sel[stage_0_i-1];
            always @(posedge clk) din_abs[stage_0_i] <= din_abs[stage_0_i-1];
        end

    endgenerate

// stage 1:
    reg stage_1_valid;
    always @(posedge clk) stage_1_valid <= stage_0_valid;

    reg signed [15:0] k0;

    genvar stage_1_i;
    generate
        always @(posedge clk) begin
            square <= mult_square_p;
        end
        always @(posedge clk) begin
            case(range_sel[0])
                RANGE_0_2048:       k0 <= 0;
                RANGE_2049_10035:   k0 <= 16'hff48;   // -184    0.045
                RANGE_10036_11585:  k0 <= 16'hffb8;   // -71
                RANGE_11586_16384:  k0 <= 16'hffb8;   // -71
                RANGE_16485_22938:  k0 <= 16'hffef;   // -17
                RANGE_22939_30720:  k0 <= 0;
                RANGE_MORE:         k0 <= 0;
                default:            k0 <= 0;
            endcase
        end

    endgenerate

// stage 2:
    reg stage_2_valid;
    always @(posedge clk) stage_2_valid <= stage_1_valid;

    reg signed [15:0] k1;
    
    genvar stage_2_i;
    generate
        always @(posedge clk) begin
            case(range_sel[1])
                RANGE_0_2048:       k1 <= 16'h3ec ;   // 1004     0.245
                RANGE_2049_10035:   k1 <= 16'h48f ;   // 1167     0.285
                RANGE_10036_11585:  k1 <= 16'h267 ;   // 615      0.15
                RANGE_11586_16384:  k1 <= 16'h267 ;   // 615      0.15
                RANGE_16485_22938:  k1 <= 16'hc6  ;   // 198      0.048
                RANGE_22939_30720:  k1 <= 16'h6   ;   // 6        0.0015
                RANGE_MORE:         k1 <= 0;
                default:            k1 <= 0;
            endcase
        end
        
        always @(posedge clk) begin
            // release overflow
            if((range_sel[1]>=RANGE_11586_16384))
                add0[0] <= mult_add0_p <<< 2;
            else
                add0[0] <= mult_add0_p;
        end
        always @(posedge clk) add0[1] <= add0[0];

    endgenerate

// stage 3:
    reg stage_3_valid;
    always @(posedge clk) stage_3_valid <= stage_2_valid;
    reg signed [15:0] b;

    genvar stage_3_i;
    generate
        always @(posedge clk) begin
            case(range_sel[2])
                RANGE_0_2048:       b <= 16'h802 ;   // 2050     0.5
                RANGE_2049_10035:   b <= 16'h7dc ;   // 2012     0.491
                RANGE_10036_11585:  b <= 16'ha87 ;   // 2695     0.658
                RANGE_11586_16384:  b <= 16'ha87 ;   // 2695     0.658
                RANGE_16485_22938:  b <= 16'hfd2 ;   // 4050     0.989
                RANGE_22939_30720:  b <= 16'h1000;   // 4096     1
                RANGE_MORE:         b <= 16'h1000;   // 4096     1
                default:            b <= 0;
            endcase
        end
        always @(posedge clk) add1 <= mult_add1_p;
    endgenerate

// stage 4:
    reg stage_4_valid;
    always @(posedge clk) stage_4_valid <= stage_3_valid;
    assign sum = add0[1] + add1 + b;
    always @(posedge clk) begin
        if(sign[3]) 
            result <= 16'h1000 - sum;
        else
            result <= sum;
    end

    assign mult_square_0 =  (stage_0_valid) ? 
                                (range_sel[0]>=RANGE_11586_16384) ?
                                    (din_abs[0]>>>1) : din_abs[0] :
                                0;
    assign mult_square_1 =  (stage_0_valid) ? 
                                (range_sel[0]>=RANGE_11586_16384) ?
                                    (din_abs[0]>>>1) : din_abs[0] :
                                0;

    mult_no_latency mult_square(
        .A              (mult_square_0),
        .B              (mult_square_1),
        .P              (mult_square_p)
    );

    assign mult_add0_0 = (stage_1_valid) ? k0 : 0;
    assign mult_add0_1 = (stage_1_valid) ? square : 0;

    mult_no_latency mult_add0(
        .A              (mult_add0_0),
        .B              (mult_add0_1),
        .P              (mult_add0_p)
    );

    assign mult_add1_0 = (stage_2_valid) ? k1 : 0;
    assign mult_add1_1 = (stage_2_valid) ? din_abs[2] : 0;

    mult_no_latency mult_add1(
        .A              (mult_add1_0),
        .B              (mult_add1_1),
        .P              (mult_add1_p)
    );

    assign valid = stage_4_valid;
    assign dout = result;

endmodule


