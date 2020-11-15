



/* ----------------------------------------------
 *
 * 这并不是一个精准的exponent计算，而是考虑到一些因素后进行了修改和简化
 *
 * 
 * 当x<0时，令结果为1，保证每个元素依然有被取到的可能性
 * 
 * x的取值主要分布在0~10(y=1~e^10)。我们暂时取unsigned 20bit也就是0~2^20，再高用最大值封顶。
 *
 *
 *
 *
 *
 */


module exp(

    input clk,
    input en,
    input [15:0]in,
    output reg [39:0]out,
    output valid

);

    wire [5:0] int;
    assign int = in[15] ? 6'b111111 : (in >>> 10);

    localparam EMINUS= 1;
    localparam E0   = 40'd2;
    localparam E1   = 40'd4;
    localparam E2   = 40'd12;
    localparam E3   = 40'd33;
    localparam E4   = 40'd90;
    localparam E5   = 40'd245;
    // localparam EMINUS= 0;
    // localparam E0   = 40'd0;
    // localparam E1   = 40'd0;
    // localparam E2   = 40'd0;
    // localparam E3   = 40'd0;
    // localparam E4   = 40'd0;
    // localparam E5   = 40'd0;
    localparam E6   = 40'd665;
    localparam E7   = 40'd1808;
    localparam E8   = 40'd4915;
    localparam E9   = 40'd13360;
    localparam E10  = 40'd36315;
    localparam E11  = 40'd98716;
    localparam E12  = 40'd268337;
    localparam E13  = 40'd729416;
    localparam E14  = 40'd1982759;
    localparam E15  = 40'd5389698;
    localparam E16  = 40'd14650719;
    localparam E17  = 40'd39824784;
    localparam E18  = 40'd108254987;
    localparam E19  = 40'd294267566;
    localparam E20  = 40'd799902177;
    localparam E21  = 40'd2174359553;
    localparam E22  = 40'd5910522063;
    localparam E23  = 40'd16066464720;
    localparam E24  = 40'd43673179097;
    localparam E25  = 40'd118716009130;
    localparam E26  = 40'd322703570366;
    localparam E27  = 40'd877199251304;

    reg [39:0] result;
    
    always @(*) begin
        if(!en) result <= 0;
        else begin
            case(int)
                0:  result <= E0    ;
                1:  result <= E1    ;                
                2:  result <= E2    ;
                3:  result <= E3    ;
                4:  result <= E4    ;
                5:  result <= E5    ;
                6:  result <= E6    ;
                7:  result <= E7    ;
                8:  result <= E8    ;
                9:  result <= E9    ;
                10: result <= E10   ;
                11: result <= E11   ;
                12: result <= E12   ;
                13: result <= E13   ;
                14: result <= E14   ;
                15: result <= E15   ;
                16: result <= E16   ;
                17: result <= E17   ;
                18: result <= E18   ;
                19: result <= E19   ;
                20: result <= E20   ;
                21: result <= E21   ;
                22: result <= E22   ;
                23: result <= E23   ;
                24: result <= E24   ;
                25: result <= E25   ;
                26: result <= E26   ;
                27: result <= E27   ;
                default: result <= EMINUS;
            endcase
        end
    end

    always @(posedge clk) out <= result;
    reg valid_r;
    always @(posedge clk) valid_r <= en;
    assign valid = valid_r;

endmodule


