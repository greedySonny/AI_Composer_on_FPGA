






// in0 mod in1 = in0 - (in0/in1)*in1
module my_mod(

    input clk,

    input en,
    output valid,

    input [39:0] in0,
    input [39:0] in1,
    output [39:0] out

);

    wire [39:0] quotient;
    wire [39:0] remainder;
    wire div_valid;
    assign valid = div_valid;

    wire [79:0] m_axis_dout_tdata;

    assign quotient = m_axis_dout_tdata[79:40];
    assign remainder = m_axis_dout_tdata[39:0];

    assign out = remainder;

    div_for_mod u_div(

        .aclk                   (clk),

        .s_axis_dividend_tdata  (in0),
        .s_axis_dividend_tvalid (en),

        .s_axis_divisor_tdata   (in1),
        .s_axis_divisor_tvalid  (en),

        .m_axis_dout_tdata      (m_axis_dout_tdata),
        .m_axis_dout_tvalid     (div_valid)

    );


    // 仿真看结果。综合会被优化掉
    wire [39:0] _result;
    assign _result = in0 % in1;



endmodule

