

// 0x00 CR
//
//      [0] enable


// 这一模块仅作寄存器配置，读写
// 用来通过软件对神经网络进行配置和状态读取

`include "NN_defs.vh"


module NN_ahbif#(

)
(

    input               HCLK,
    input               HRESETn,
    input               HSEL,
    input               HWRITE,
    input   [31:0]      HADDR,
    input   [31:0]      HWDATA,

    output  [31:0]      HRDATA,
    output              HREADYOUT,
    output              HRESP,

    input               nn_clk,
    input               nn_rst,

    output  reg [7:0]   ahb_cmd,
    output  reg [31:0]  ahb_bias_addr,
    output  reg [31:0]  ahb_weight_addr,
    output  reg [31:0]  ahb_in_ch,
    output  reg [31:0]  ahb_out_ch,
    output  reg [31:0]  ahb_compensation,
    output  reg [31:0]  ahb_layer,
    output      [31:0]  random_seed,

    input       [15:0]  midi_wave,
    input               NN_result_valid,

    input       [31:0]  SR,

    output              nn_interrupt

);

    reg     [31:0]      NN_CR;
    reg     [0: 0]      NN_INT;
    
    reg     [31:0]      NN_BIAS_ADDR;
    reg     [31:0]      NN_WEIGHT_ADDR;
    reg     [31:0]      NN_IN_CH;
    reg     [31:0]      NN_OUT_CH;
    reg     [31:0]      NN_CMD;
    reg     [31:0]      NN_COMPENSATION;
    reg     [31:0]      NN_LAYER;
    reg     [31:0]      NN_RAND_SEED;

    reg                 hsel_r;
    reg                 hwrite_r;
    reg     [31:0]      haddr_r;

    wire                int_en;
    
    assign int_en = NN_CR[1];
    assign nn_interrupt = NN_INT;

    always @(posedge HCLK) hsel_r <= HSEL;
    always @(posedge HCLK) hwrite_r <= HWRITE;
    always @(posedge HCLK) haddr_r <= HADDR;

    reg [31:0] rddata_r;

    wire write_enable_00;
    wire write_enable_04;
    wire write_enable_08;
    wire write_enable_0C;
    wire write_enable_10;
    wire write_enable_14;
    wire write_enable_18;
    wire write_enable_1C;
    wire write_enable_20;
    wire write_enable_24;
    wire write_enable_28;


    assign write_enable_00 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h00);
    assign write_enable_04 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h01);
    assign write_enable_08 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h02);
    assign write_enable_0C = hsel_r & hwrite_r & (haddr_r[11:2]==10'h03);
    assign write_enable_10 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h04);
    assign write_enable_14 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h05);
    assign write_enable_18 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h06);
    assign write_enable_1C = hsel_r & hwrite_r & (haddr_r[11:2]==10'h07);
    assign write_enable_20 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h08);
    assign write_enable_24 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h09);
    assign write_enable_28 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h0a);
    assign write_enable_44 = hsel_r & hwrite_r & (haddr_r[11:2]==10'h11);

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_CR <= 0;
        else if(write_enable_00) NN_CR <= HWDATA;
    end

    reg NN_result_valid_keep;
    always @(posedge nn_clk, negedge HRESETn) begin
        if(!HRESETn) NN_result_valid_keep <= 0;
        else if(NN_INT) NN_result_valid_keep <= 0;
        else if(NN_result_valid) NN_result_valid_keep <= 1;
    end
    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_INT <= 0;
        else if(write_enable_08) NN_INT <= 0;
        else if(NN_result_valid_keep & int_en) 
            NN_INT <= 1;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_RAND_SEED <= 1;
        else if(write_enable_0C) NN_RAND_SEED <= HWDATA;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_BIAS_ADDR <= 0;
        else if(write_enable_10) NN_BIAS_ADDR <= HWDATA;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_WEIGHT_ADDR <= 0;
        else if(write_enable_14) NN_WEIGHT_ADDR <= HWDATA;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_IN_CH <= 0;
        else if(write_enable_18) NN_IN_CH <= HWDATA;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_OUT_CH <= 0;
        else if(write_enable_1C) NN_OUT_CH <= HWDATA;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_CMD <= 0;
        else if(write_enable_20) NN_CMD <= HWDATA;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_COMPENSATION <= 0;
        else if(write_enable_24) NN_COMPENSATION <= HWDATA;
    end

    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) NN_LAYER <= 0;
        else if(write_enable_28) NN_LAYER <= HWDATA;
    end



    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn)            rddata_r <= 0;
        else if(HSEL & (!HWRITE) & (HADDR[11:2]==10'h0))
                                rddata_r <= NN_CR;
        else if(HSEL & (!HWRITE) & (HADDR[11:2]==10'h1))
                                rddata_r <= SR;
        else if(HSEL & (!HWRITE) & (HADDR[11:2]==10'h2))
                                rddata_r <= {31'b0, NN_INT};
        else if(HSEL & (!HWRITE) & (HADDR[11:2]==10'h3))
                                rddata_r <= {16'b0, midi_wave};

    end 




    reg cmd_done;
    // always @(posedge nn_clk, posedge nn_rst) begin
    //     if(nn_rst) cmd_done<=0;
    //     else if(write_enable_20) cmd_done<=0;
    //     else if(NN_CMD!=`CMD_NONE) cmd_done<=1;
    // end
    always @(posedge HCLK, negedge HRESETn) begin
        if(!HRESETn) cmd_done<=0;
        else if(write_enable_20) cmd_done<=0;
        else if(NN_CMD!=`CMD_NONE) cmd_done<=1;
    end

    // generate cmd pulse from HCLK to nn_clk
    always @(posedge nn_clk) begin
        if(nn_rst)                              ahb_cmd <= `CMD_NONE;
        //else if(NN_CMD == `CMD_NONE)            ahb_cmd <= `CMD_NONE;
        else if(cmd_done)                       ahb_cmd <= `CMD_NONE;
        else if(ahb_cmd != `CMD_NONE)           ahb_cmd <= `CMD_NONE;
        // else if(NN_CMD == `CMD_LOAD_EMBEDDING)  ahb_cmd <= `CMD_LOAD_EMBEDDING;
        // else if(NN_CMD == `CMD_LOAD_ZEMBEDDING) ahb_cmd <= `CMD_LOAD_ZEMBEDDING;
        // else if(NN_CMD == `CMD_LOAD_BIAS)       ahb_cmd <= `CMD_LOAD_BIAS;
        // else if(NN_CMD == `CMD_LOAD_WEIGHT)     ahb_cmd <= `CMD_LOAD_WEIGHT;
        else ahb_cmd <= NN_CMD;
    end

    always @(posedge nn_clk) ahb_bias_addr <= NN_BIAS_ADDR;
    always @(posedge nn_clk) ahb_weight_addr <= NN_WEIGHT_ADDR;
    always @(posedge nn_clk) ahb_in_ch <= NN_IN_CH;
    always @(posedge nn_clk) ahb_out_ch <= NN_OUT_CH;
    always @(posedge nn_clk) ahb_compensation <= NN_COMPENSATION;
    always @(posedge nn_clk) ahb_layer <= NN_LAYER;

    assign HRDATA = rddata_r;
    assign random_seed = NN_RAND_SEED;
    

    assign HREADYOUT = 1;



    // 一开始hresp这里不小心是1。造成的结果是，在读这个外设的时候，hrdata正确，但读到内核寄存器中就变成了全f。

    assign HRESP = 0;

endmodule















