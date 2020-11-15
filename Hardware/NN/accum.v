









module accum(

    input   clk,
    input   rst_n,
    input   clr,

    input   [15:0]  load,
    input   [15:0]  din,
    input           enable,
    output  [15:0]  dout,
    output reg      valid

);

    reg [15:0]  data;
    wire [15:0] S;
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)      data <= load;
        else if(clr)    data <= load;
        else if(enable) data <= S;
    end

    always @(posedge clk) valid <= enable;

    ip_add_no_latency add(
        .A              (data),
        .B              (din),
        .S              (S)
    );
    assign dout = data;

endmodule


