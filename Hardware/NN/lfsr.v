















module lfsr#(

    parameter DW = 40
)
(
    input clk,
    input rst_n,

    input seed_en,
    input [31:0] seed_in,
    output wire [DW-1:0] dout

);

    reg [DW:1] r_LFSR;
    wire r_XNOR = r_LFSR[32] ^~ r_LFSR[22] ^~ r_LFSR[2] ^~ r_LFSR[1];

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) r_LFSR <= 0;
        else if(seed_en) r_LFSR <= seed_in;
        else r_LFSR <= {r_LFSR[DW-1:1], r_XNOR};
    end
        
    assign dout = r_LFSR[DW:1];

    wire dout_line = dout < {(DW-1){1'b1}}+1;
    reg [31:0]dout_less_cnt; reg [31:0]dout_more_cnt;

    always @(posedge clk,negedge rst_n) begin
        if(!rst_n) dout_less_cnt <= 0;
        else if(dout_line) dout_less_cnt <= dout_less_cnt + 1;
    end
    
    always @(posedge clk,negedge rst_n) begin
        if(!rst_n) dout_more_cnt <= 0;
        else if(!dout_line) dout_more_cnt <= dout_more_cnt + 1;
    end

endmodule



