
`include "NN_defs.vh"


module NN_Top2#(

    parameter LOADPARAMS_ILA_DBG = 0
)(

    input                   clk,
    input                   rst_n,
    input                   HCLK,

    input  [31:0]           NN_CR,
    output [31:0]           NN_SR,

    output reg [28:0]               sdram_addr,
    output reg [2:0]                sdram_write,
    output reg                      sdram_en,

    output reg [255:0]              sdram_wdata,
    input [255:0]                   sdram_rdata,
    input                           sdram_ready,
    input                           sdram_rdata_vld,
    //input                     app_rdy,

    input [7:0]   ahb_cmd,
    input [31:0]  ahb_bias_addr,
    input [31:0]  ahb_weight_addr,
    input [31:0]  ahb_in_ch,
    input [31:0]  ahb_out_ch,
    input [31:0]  ahb_compensation,
    input [31:0]  ahb_layer,
    input [31:0]    random_seed,

    output  reg [15:0]            midi_wave,
    output  reg                     NN_result_valid

);

    localparam  WRST_IDLE                   = 0,
                WRST_WRITE                  = 1,    // 读sdram参数
                WRST_WAIT                   = 2,    // 等待sdram返回数据，并写入gru
                WRST_READ                   = 3;    // 将参数从gru读出

    reg [7:0] calstate;
    reg [7:0] calstate_next;

    localparam  CALST_IDLE              = 0,
                CALST_WAIT              = 1,
                CALST_R_I_L0            = 2,
                CALST_R_H_L0            = 3,
                CALST_MULTADD           = 4;

    reg [3:0]   wrstate;
    reg [3:0]   wrstate_next;

    reg [7:0]   cmdstate;
    reg [7:0]   cmdstate_next;

    reg [9:0]   load_cnt;


    /* ----------- test begin ------------ */
    reg [9:0] gru_weight_out_addr;
    reg [9:0] gru_bias_out_addr;  
    wire gru_param_en, gru_param_write;
    wire [3:0]   gru_sel;
    wire [255:0]    gru_param_in;
    reg [9:0] data_out_addr;
    reg [9:0] out_ch_store_cnt;
    reg [31:0] out_ch_load_cnt;
    reg [255:0] din;
    reg [7:0] accum_valid_cnt;
    wire [15:0] cal_sum;
    wire [15:0] accum_sum;
    wire accum_clr, accum_valid;
    reg [9:0]wr_read_cnt;
    reg  cal_en; wire cal_valid;
    reg [9:0]   gru_addr;
    wire [15:0] sigmoid_din;
    reg sigmoid_en;
    reg [9:0]sigmoid_addr;
    reg [9:0] sigmoid_vld_cnt;
    wire [15:0] sigmoid_dout;
    wire sigmoid_valid;
    wire [15:0] tanh_din;
    reg tanh_en;
    reg [9:0]tanh_addr;
    reg [9:0] tanh_vld_cnt;
    wire [15:0] tanh_dout;
    wire tanh_valid;
    reg data_in_valid;

    reg [9:0] mult_addr;
    wire accum_valid_ok;
    reg [255:0] accum_shift;
    reg [255:0] accum_shift_sync;
    reg [9:0] accum_parellel_cnt;
    reg [9:0] accum_parellel_cal_cnt_d1;
    reg [9:0] accum_parellel_cal_cnt;
    wire [15:0] exp_in;
    wire [39:0] exp_out;
    reg exp_en;
    wire exp_valid;
    reg [9:0] exp_addr;
    reg [9:0] exp_vld_cnt;

    reg [9:0] data_in_addr;
    reg [255:0] data_in_rdata;
    wire [255:0] data_in_wdata;
    wire data_in_en;
    wire data_in_write;
    reg [9:0] data_h_addr;
    reg [255:0] data_h_rdata;
    wire [255:0] data_h_wdata;
    wire data_h_en;
    wire data_h_write;
    reg data_h_valid;
    reg [9:0] data_layer_addr;
    reg [255:0] data_layer_rdata;
    wire [255:0] data_layer_rdata_trans;
    wire [255:0] data_h_rdata_trans;
    wire [255:0] data_out_parellel_trans;
    wire [255:0] data_layer_wdata;
    wire data_layer_en;
    wire data_layer_write;
    reg data_layer_valid;
    reg [9:0] data_r_addr;
    reg [255:0] data_r_rdata;
    wire [255:0] data_r_wdata;
    wire data_r_en;
    wire data_r_write;
    reg [9:0] data_z_addr;
    reg [255:0] data_z_rdata;
    wire [255:0] data_z_wdata;
    wire data_z_en;
    wire data_z_write;
    reg [9:0] data_tmp0_addr;
    reg [255:0] data_tmp0_rdata;
    wire [255:0] data_tmp0_wdata;
    wire data_tmp0_en;
    wire data_tmp0_write;
    reg data_tmp0_valid;
    reg [9:0] data_tmp1_addr;
    reg [255:0] data_tmp1_rdata;
    wire [255:0] data_tmp1_wdata;
    wire data_tmp1_en;
    wire data_tmp1_write;
    wire [255:0] data_out_parellel[0:15];

    wire [255:0] add_parellel0;
    wire [255:0] add_parellel1;
    wire [255:0] add_parellel_sum;
    reg add_parellel_valid;

    wire [255:0] cal_mult_0;
    wire [255:0] cal_mult_1;
    wire [255:0] cal_bias;
    wire [255:0] cal_product;
    wire cal_product_valid;
    reg [9:0] cal_product_cnt;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) begin
            sdram_en <= 0;
            sdram_addr <= 0;
            sdram_write <= 0;
            sdram_wdata <= 0;
        end
        else if(!sdram_ready) begin
            sdram_en <= sdram_en;
            sdram_addr <= sdram_addr;
            sdram_write <= sdram_write;
            sdram_wdata <= sdram_wdata;
        end
        else if(cmdstate==`CMD_LOAD_TEST) begin
            sdram_en <= 1;
            sdram_addr <= ahb_bias_addr + load_cnt;
            sdram_write <= 0;
        end
        else if((wrstate==WRST_WRITE)&(cmdstate==`CMD_LOAD_BIAS)) begin
            sdram_en <= 1;
            sdram_addr <= ahb_bias_addr + load_cnt;
            sdram_write <= 0;
        end
        else if((wrstate==WRST_WRITE)&(cmdstate==`CMD_LOAD_WEIGHT_I)) begin
            sdram_en <= 1;
            sdram_addr <= ahb_weight_addr + load_cnt + out_ch_load_cnt;
            sdram_write <= 0;
        end
        else if((wrstate==WRST_WRITE)&(cmdstate==`CMD_LOAD_WEIGHT_H)) begin
            sdram_en <= 1;
            sdram_addr <= ahb_weight_addr + load_cnt + out_ch_load_cnt;
            sdram_write <= 0;
        end
        else if((wrstate==WRST_WRITE)&
                ((cmdstate==`CMD_LOAD_EMBEDDING)|
                (cmdstate==`CMD_LOAD_ZEMBEDDING))) begin
            sdram_en <= 1;
            sdram_addr <= ahb_bias_addr + load_cnt;
            sdram_write <= 0;
        end
        else begin
            sdram_en <= 0;
            sdram_addr <= 0;
            sdram_write <= 1;
            sdram_wdata <= 0;
        end
    end


    wire enable;
    assign enable = NN_CR[0];

    /* ----------- cmdstate ------------ */


    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) cmdstate <= `CMD_NONE;
        else cmdstate <= cmdstate_next;
    end

    always @(*) begin
        if(!rst_n) cmdstate_next <= `CMD_NONE;
        else begin
            case(cmdstate)
                `CMD_NONE: begin
                    cmdstate_next <= ahb_cmd;
                end
                `CMD_LOAD_ZEMBEDDING: begin
                    if((data_in_addr==1) & sdram_rdata_vld)
                        cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_EMBEDDING: begin
                    if((data_in_addr==17 & sdram_rdata_vld))
                        cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_BIAS: begin
                    if(sdram_rdata_vld & (load_cnt==(ahb_out_ch-32)) & (gru_addr==(ahb_out_ch[12:5]-1))) 
                        cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_WEIGHT_I: begin
                    if((out_ch_store_cnt==(ahb_out_ch[12:1]-1)) & 
                        (accum_valid_cnt==(ahb_in_ch[12:5]-1)) & accum_valid) 
                        cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_WEIGHT_H: begin
                    if((out_ch_store_cnt==255) & 
                        (accum_valid_cnt==(ahb_out_ch[12:5]-1)) & accum_valid) 
                        cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_RESULT: begin
                    if(data_in_addr==15) 
                        cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end     
                `CMD_LOAD_SIGMOID: begin
                    if(sigmoid_addr==255) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_TANH: begin
                    if(tanh_addr==255) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_EXP: begin
                    if(exp_addr==97) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_R, `CMD_LOAD_Z, `CMD_LOAD_ZMINUS: begin
                    if(cal_product_cnt==15) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_STORE_R, `CMD_STORE_Z: begin
                    if(data_r_addr==15) cmdstate_next <= `CMD_NONE;
                    else if(data_z_addr==15) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_STORE_H, `CMD_INIT_H: begin
                    if(data_h_addr==15) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_H: begin
                    if((data_in_addr==15)&(ahb_layer!=32)) cmdstate_next <= `CMD_NONE;
                    else if(data_in_addr==17) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_ADD: begin
                    //if(accum_parellel_cal_cnt==15) cmdstate_next <= `CMD_NONE;
                    if(data_tmp0_addr==15) cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                `CMD_LOAD_TEST: begin
                    if(load_cnt==(ahb_out_ch-32))
                        cmdstate_next <= `CMD_NONE;
                    else cmdstate_next <= cmdstate;
                end
                default: cmdstate_next <= `CMD_NONE;
            endcase
        end
    end


    /* ----------- wrstate ------------ */
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) wrstate <= WRST_IDLE;
        else wrstate <= wrstate_next;
    end

    always @(*) begin
        if(!rst_n) wrstate_next <= WRST_IDLE;
        else begin
            case(wrstate)
                WRST_IDLE: begin
                    if(ahb_cmd==`CMD_LOAD_BIAS) 
                        wrstate_next <= WRST_WRITE;
                    else if((ahb_cmd==`CMD_LOAD_EMBEDDING)|(ahb_cmd==`CMD_LOAD_ZEMBEDDING)) 
                        wrstate_next <= WRST_WRITE;
                    else if((cmdstate==`CMD_LOAD_WEIGHT_I)&(calstate==CALST_IDLE))
                        wrstate_next <= WRST_WRITE;
                    else if((cmdstate==`CMD_LOAD_WEIGHT_H)&(calstate==CALST_IDLE))
                        wrstate_next <= WRST_WRITE;
                    else wrstate_next <= wrstate;
                end
                WRST_WRITE: begin
                    // 将weight由sdram读到gru
                    if(sdram_ready & (load_cnt==(ahb_in_ch-32))&(cmdstate==`CMD_LOAD_WEIGHT_I)) 
                        wrstate_next <= WRST_WAIT;
                    // 将weight由sdram读到gru
                    else if(sdram_ready & (load_cnt==(ahb_in_ch-32))&(cmdstate==`CMD_LOAD_WEIGHT_H)) 
                        wrstate_next <= WRST_WAIT;
                    // 将bias由sdram读到gru
                    else if(sdram_ready & (load_cnt==(ahb_out_ch-32))&
                            (cmdstate==`CMD_LOAD_BIAS)) 
                        wrstate_next <= WRST_WAIT;
                    // 将embedding由sdram读到gru
                    else if(sdram_ready & (load_cnt==(ahb_out_ch-32))&((cmdstate==`CMD_LOAD_ZEMBEDDING)|(cmdstate==`CMD_LOAD_EMBEDDING)))
                        wrstate_next <= WRST_WAIT;
                    else wrstate_next <= wrstate;
                end
                WRST_WAIT: begin
                    // 等待剩余weight从sdram读出
                    if(sdram_rdata_vld&(wr_read_cnt==(ahb_out_ch[12:5]-1))&
                            (cmdstate!=`CMD_LOAD_WEIGHT_I) & (cmdstate!=`CMD_LOAD_WEIGHT_H))
                        wrstate_next <= WRST_IDLE;
                    else if(sdram_rdata_vld&(wr_read_cnt==(ahb_in_ch[12:5]-1))&
                            ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
                        wrstate_next <= WRST_READ;
                    else 
                        wrstate_next <= wrstate;
                end
                WRST_READ: begin
                    if(((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H))&
                        (gru_addr==(ahb_in_ch[12:5]-1)))
                        wrstate_next <= WRST_IDLE;
                    else 
                        wrstate_next <= wrstate;
                end
                default: wrstate_next <= WRST_IDLE;
            endcase
        end
    end

    
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) wr_read_cnt <= 0;
        else if(sdram_rdata_vld&(wr_read_cnt==(ahb_out_ch[12:5]-1))&(cmdstate!=`CMD_LOAD_WEIGHT_I))
            wr_read_cnt <= 0;
        else if(sdram_rdata_vld&(wr_read_cnt==(ahb_in_ch[12:5]-1))&(cmdstate==`CMD_LOAD_WEIGHT_I))
            wr_read_cnt <= 0;
        else if(sdram_rdata_vld & (cmdstate!=`CMD_NONE) & (cmdstate!=`CMD_LOAD_TEST)) 
            wr_read_cnt <= wr_read_cnt + 1;
    end
    /* ----------- calstate ------------ */
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) calstate <= CALST_IDLE;
        else calstate <= calstate_next;
    end

    always @(*) begin
        if(!rst_n) calstate_next <= CALST_IDLE;
        else begin
            case(calstate) 
                CALST_IDLE: begin
                    if((wrstate==WRST_WAIT) & 
                        (wrstate_next==WRST_READ) &
                        (cmdstate==`CMD_LOAD_WEIGHT_I))
                        calstate_next <= CALST_R_I_L0;
                    else if((wrstate==WRST_WAIT) & 
                        (wrstate_next==WRST_READ) &
                        (cmdstate==`CMD_LOAD_WEIGHT_H))
                        calstate_next <= CALST_R_H_L0;
                    else if((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z)|(cmdstate==`CMD_LOAD_ZMINUS))
                        calstate_next <= CALST_MULTADD;
                    else calstate_next <= calstate;
                end
                CALST_R_I_L0, CALST_R_H_L0: begin
                    //if((gru_weight_out_addr==(ahb_in_ch[12:5]-1)))
                    if((gru_addr==(ahb_in_ch[12:5]-1)))
                        calstate_next <= CALST_WAIT;
                    else calstate_next <= calstate;
                end
                CALST_WAIT: begin
                    if((!cal_valid) & 
                        ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H) )) 
                        calstate_next <= CALST_IDLE;
                    else if((!cal_valid) & ((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z)|(cmdstate==`CMD_LOAD_ZMINUS)))
                        calstate_next <= CALST_IDLE;
                    else calstate_next <= calstate;
                end
                CALST_MULTADD: begin
                    if(data_r_addr==15) calstate_next <= CALST_WAIT;
                    else if(data_z_addr==15) calstate_next <= CALST_WAIT;
                    else calstate_next <= calstate;
                end
                default : calstate_next <= CALST_IDLE;

            endcase
        end
    end

    reg [9:0] cal_en_cnt;
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) cal_en_cnt <= 0;
        else if((cal_en_cnt==(ahb_out_ch[12:5]-1))&(cmdstate!=`CMD_LOAD_WEIGHT_I)&(cmdstate!=`CMD_LOAD_WEIGHT_H))
            cal_en_cnt <= 0;
        else if((cal_en_cnt==(ahb_in_ch[12:5]-1))&((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
            cal_en_cnt <= 0;
        else if(calstate==CALST_IDLE) cal_en_cnt<=0;
        else if(calstate!=CALST_IDLE) cal_en_cnt <= cal_en_cnt+1;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) cal_en <= 0;
        else if((calstate==CALST_R_I_L0)|(calstate==CALST_R_H_L0)|(calstate==CALST_MULTADD))
            cal_en <= 1;
        else cal_en <= 0;
    end



    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) gru_bias_out_addr <= 0;
        else if((calstate==CALST_IDLE) & 
                (calstate_next!=CALST_IDLE) &
                (gru_bias_out_addr==(ahb_out_ch[12:1]-1)))
            gru_bias_out_addr <= 0;
        else if((calstate==CALST_IDLE) & (calstate_next!=CALST_IDLE) & 
                ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
            gru_bias_out_addr <= gru_bias_out_addr+1;
        else if((gru_bias_out_addr==255)&(calstate==CALST_MULTADD))
            gru_bias_out_addr <= 0;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) load_cnt<=0;
        else if(sdram_ready & (load_cnt==(ahb_in_ch-32)) & ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
            load_cnt <= 0;
        else if(sdram_ready & (load_cnt==(ahb_out_ch-32)) & (cmdstate == `CMD_LOAD_BIAS))
            load_cnt <= load_cnt;
        else if(sdram_ready & (load_cnt==(ahb_out_ch-32)) & (cmdstate == `CMD_LOAD_EMBEDDING))
            load_cnt <= load_cnt;
        else if(sdram_ready & (load_cnt==(ahb_out_ch-32)) & (cmdstate == `CMD_LOAD_ZEMBEDDING))
            load_cnt <= load_cnt;
        else if(sdram_ready & (load_cnt==(ahb_out_ch-32)) & (cmdstate == `CMD_LOAD_TEST))
            load_cnt <= 0;
        else if(sdram_ready & (cmdstate == `CMD_LOAD_TEST))
            load_cnt <= load_cnt+32;
        else if(sdram_ready & (cmdstate==`CMD_NONE)) load_cnt <= 0;
        else if(sdram_ready & (wrstate==WRST_WRITE))
            load_cnt <= load_cnt + 32;
    end
    
    /* ------------------ test begin ------------------ */
    // 使用bram
    (* ram_style="block" *) reg [255:0] data_in[0:17];
    (* ram_style="block" *) reg [255:0] data_h[0:47];
    (* ram_style="block" *) reg [255:0] data_layer[0:15];
    (* ram_style="block" *) reg [255:0] data_tmp0[0:15];
    (* ram_style="block" *) reg [255:0] data_tmp1[0:15];
    (* ram_style="block" *) reg [255:0] data_r[0:15];
    (* ram_style="block" *) reg [255:0] data_z[0:15];
    // 使用disturbed ram
    (*ram_style = "distributed"*) reg signed [15:0] data_out[0:255];
    

    /* --------------  data_in begin -------------- */
    always @(posedge clk) data_in_valid <= data_in_en & (!data_in_write);

    always @(posedge clk) begin
        if(!rst_n) data_in_rdata <= 0;
        else if(!data_in_en) data_in_rdata <= 0;
        else if(!data_in_write) data_in_rdata <= data_in[data_in_addr];
    end

    always @(posedge clk) begin
        if(data_in_en & data_in_write) 
            data_in[data_in_addr] <= data_in_wdata;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_in_addr <= 0;
        // 加载zembedding
        else if((cmdstate==`CMD_LOAD_ZEMBEDDING)&sdram_rdata_vld)
            data_in_addr <= data_in_addr + 1;
        // 加载embedding
        else if((data_in_addr==17)&(cmdstate==`CMD_LOAD_EMBEDDING)&sdram_rdata_vld)
            data_in_addr <= 0;
        else if((cmdstate==`CMD_LOAD_EMBEDDING)&sdram_rdata_vld)
            data_in_addr <= data_in_addr + 1;
        // 计算
        else if((data_in_addr==(ahb_in_ch[12:5]-1)) & data_in_en & 
                (cmdstate==`CMD_LOAD_WEIGHT_I))
            data_in_addr <= 0;
        else if(data_in_en&(cmdstate==`CMD_LOAD_WEIGHT_I))
            data_in_addr <= data_in_addr + 1;
        // data_out -> data_in
        else if((data_in_addr==15) &
                (cmdstate==`CMD_LOAD_RESULT))
            data_in_addr <= 0;
        else if(cmdstate==`CMD_LOAD_RESULT)
            data_in_addr <= data_in_addr + 1;
        // data_h -> data_in
        else if((data_in_addr==15) & (ahb_layer!=32) &
                (cmdstate==`CMD_LOAD_H))
            data_in_addr <= 0;
        else if((data_in_addr==17) &
                (cmdstate==`CMD_LOAD_H))
            data_in_addr <= 0;
        else if(data_h_valid & (cmdstate==`CMD_LOAD_H))
            data_in_addr <= data_in_addr + 1;
        // sigmoid /tanh / exp
        // else if( (((sigmoid_addr%16)==14)|((tanh_addr%16)==14)|((exp_addr%16)==14)) & 
        //         (data_in_addr==15) & ((cmdstate==`CMD_LOAD_SIGMOID)|(cmdstate==`CMD_LOAD_TANH)|(cmdstate==`CMD_EXP)))
        //     data_in_addr <= 0;
        else if( (((sigmoid_addr%16)==14) | ((tanh_addr%16)==14)) & 
                (data_in_addr==15) & ((cmdstate==`CMD_LOAD_SIGMOID)|(cmdstate==`CMD_LOAD_TANH)))
            data_in_addr <= 0;
        else if( exp_addr==97 )
            data_in_addr <= 0;
        else if( (((sigmoid_addr%16)==14)|((tanh_addr%16)==14)|((exp_addr%16)==14)) &
                ((cmdstate==`CMD_LOAD_SIGMOID)|(cmdstate==`CMD_LOAD_TANH)|(cmdstate==`CMD_EXP)))
            data_in_addr <= data_in_addr + 1;
        // r*(h*wh) / n*(1-z)
        else if((data_in_addr==15)&((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_ZMINUS)))
            data_in_addr <= 0;
        else if(((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_ZMINUS))&(calstate==CALST_MULTADD))
            data_in_addr <= data_in_addr+1;

    end

    assign data_in_en = (((cmdstate==`CMD_LOAD_EMBEDDING)|(cmdstate==`CMD_LOAD_ZEMBEDDING)) & sdram_rdata_vld) | 
                            gru_param_en |
                            (cmdstate==`CMD_LOAD_RESULT) | 
                            (data_h_valid&(cmdstate==`CMD_LOAD_H)) |
                            ((cmdstate==`CMD_LOAD_SIGMOID)|(cmdstate==`CMD_LOAD_TANH)|(cmdstate==`CMD_EXP));
    assign data_in_write = (sdram_rdata_vld&((cmdstate==`CMD_LOAD_EMBEDDING)|(cmdstate==`CMD_LOAD_ZEMBEDDING))) |
                            (cmdstate==`CMD_LOAD_RESULT) | 
                            (data_h_valid&(cmdstate==`CMD_LOAD_H));
    assign data_in_wdata = sdram_rdata_vld ? 
                            sdram_rdata : 
                            (cmdstate==`CMD_LOAD_RESULT) ?
                                data_out_parellel[data_out_addr]:
                                (cmdstate==`CMD_LOAD_H) ?
                                    //data_layer_rdata :
                                    data_h_rdata :
                                    0;

    /* --------------  data_in end -------------- */

    /* --------------  data_h begin -------------- */
    always @(posedge clk) begin
        if(!rst_n) data_h_rdata <= 0;
        else if(!data_h_en) data_h_rdata <= 0;
        else if(!data_h_write) data_h_rdata <= data_h[data_h_addr+ahb_layer];
    end
    always @(posedge clk) data_h_valid <= data_h_en&(!data_h_write);
    always @(posedge clk) begin
        if(data_h_en & data_h_write) 
            data_h[data_h_addr+ahb_layer] <= data_h_wdata;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_h_addr <= 0;
        else if(cmdstate==`CMD_NONE) data_h_addr <= 0;
        // 初始化
        else if((data_h_addr==15) & ((cmdstate==`CMD_INIT_H)))
            data_h_addr <= 0;
        else if(data_h_en & (cmdstate==`CMD_INIT_H))
            data_h_addr <= data_h_addr+1;
        // 保存
        else if((data_h_addr==15) & ((cmdstate==`CMD_STORE_H)))
            data_h_addr <= 0;
        else if(data_h_en & (cmdstate==`CMD_STORE_H))
            data_h_addr <= data_h_addr+1;
        // 加载
        else if((data_h_addr==15) & ((cmdstate==`CMD_LOAD_H)))
            data_h_addr <= 0;
        else if(data_h_en & (cmdstate==`CMD_LOAD_H))
            data_h_addr <= data_h_addr+1;
        // 计算h*wh+b
        else if((data_h_addr==(ahb_in_ch[12:5]-1)) & data_h_en & 
                (cmdstate==`CMD_LOAD_WEIGHT_H))
            data_h_addr <= 0;
        else if(data_h_en & (cmdstate==`CMD_LOAD_WEIGHT_H))
            data_h_addr <= data_h_addr + 1;
        // 计算h*z
        else if((data_h_addr==15) & 
                (cmdstate==`CMD_LOAD_Z))
            data_h_addr <= 0;
        else if((calstate==CALST_MULTADD) &(cmdstate==`CMD_LOAD_Z))
            data_h_addr <= data_h_addr + 1;
    end

    assign data_h_en = gru_param_en | 
                        ((cmdstate==`CMD_STORE_H)) | 
                        ((cmdstate==`CMD_LOAD_Z) & (calstate==CALST_MULTADD)) |
                        (cmdstate==`CMD_INIT_H) | (cmdstate==`CMD_LOAD_H);
    assign data_h_write = ((cmdstate==`CMD_STORE_H)) |
                            (cmdstate==`CMD_INIT_H);
    assign data_h_wdata = sdram_rdata_vld ? 
                            sdram_rdata : 
                            (cmdstate==`CMD_STORE_H) ? 
                                data_out_parellel_trans :
                                //data_out_parellel[data_out_addr]:
                                    (cmdstate==`CMD_INIT_H) ?
                                        data_out_parellel[data_out_addr]:
                                        0;

    integer i;
    initial begin
        for(i=0;i<16;i=i+1) begin
            data_h[i] = {16{16'h0000}};
        end
    end
    /* --------------  data_h end -------------- */
    /* --------------  data_layer begin -------------- */
    always @(posedge clk) begin
        if(!rst_n) data_layer_rdata <= 0;
        else if(!data_layer_en) data_layer_rdata <= 0;
        else if(!data_layer_write) data_layer_rdata <= data_layer[data_layer_addr];
    end
    always @(posedge clk) data_layer_valid <= data_layer_en;
    always @(posedge clk) begin
        if(data_layer_en & data_layer_write) 
            data_layer[data_layer_addr] <= data_layer_wdata;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_layer_addr <= 0;
        else if(cmdstate==`CMD_NONE) data_layer_addr <= 0;
        else if((data_layer_addr==15)&
                ((cmdstate==`CMD_STORE_LAYER)|(cmdstate==`CMD_LOAD_LAYER)|(cmdstate==`CMD_STORE_H)))
            data_layer_addr <= 0;
        else if((cmdstate==`CMD_STORE_LAYER)|(cmdstate==`CMD_LOAD_LAYER)|(cmdstate==`CMD_STORE_H))
            data_layer_addr <= data_layer_addr+1;
    end

    assign data_layer_en = (cmdstate==`CMD_STORE_LAYER)|(cmdstate==`CMD_LOAD_LAYER)|(cmdstate==`CMD_STORE_H);
    assign data_layer_write = (cmdstate==`CMD_STORE_LAYER);
    assign data_layer_wdata = (cmdstate==`CMD_STORE_LAYER) ? data_out_parellel[data_out_addr] : 0;
    /* --------------  data_layer end -------------- */

    /* --------------  data_tmp begin -------------- */
    // data_tmp0和data_tmp1在计算x*wi和h*wh时，会自动加载结果
    always @(posedge clk) begin
        if(!rst_n) data_tmp0_rdata <= 0;
        else if(!data_tmp0_en) data_tmp0_rdata <= 0;
        else if(!data_tmp0_write) data_tmp0_rdata <= data_tmp0[data_tmp0_addr];
    end
    always @(posedge clk) begin
        if(!rst_n) data_tmp1_rdata <= 0;
        else if(!data_tmp1_en) data_tmp1_rdata <= 0;
        else if(!data_tmp1_write) data_tmp1_rdata <= data_tmp1[data_tmp1_addr];
    end

    always @(posedge clk)
        if(data_tmp0_en & data_tmp0_write) 
            data_tmp0[data_tmp0_addr] <= data_tmp0_wdata;
    always @(posedge clk)
        if(data_tmp1_en & data_tmp1_write) 
            data_tmp1[data_tmp1_addr] <= data_tmp1_wdata;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_tmp0_addr <= 0;
        else if(cmdstate==`CMD_NONE) data_tmp0_addr <= 0;
        // 加载
        else if((cmdstate==`CMD_LOAD_WEIGHT_I)&(accum_parellel_cnt==15)&
                (accum_valid_ok) & (data_tmp0_addr==15))
            data_tmp0_addr <= 0;
        else if((cmdstate==`CMD_LOAD_WEIGHT_I)&(accum_parellel_cnt==15)&
                (accum_valid_ok))
            data_tmp0_addr <= data_tmp0_addr + 1;
        //
        else if((cmdstate==`CMD_LOAD_ZMINUS) &
                (cal_product_valid) & (data_tmp0_addr==15))
            data_tmp0_addr <= 0;
        else if((cmdstate==`CMD_LOAD_ZMINUS) & (cal_product_valid))
            data_tmp0_addr <= data_tmp0_addr + 1;
        // 计算add
        else if((data_tmp0_addr==15) & (cmdstate==`CMD_ADD))
            data_tmp0_addr <= 0;
        else if(cmdstate==`CMD_ADD)
            data_tmp0_addr <= data_tmp0_addr + 1;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_tmp1_addr <= 0;
        else if(cmdstate==`CMD_NONE) data_tmp1_addr <= 0;
        // 保存h*wh+b计算结果到tmp1
        else if((cmdstate==`CMD_LOAD_WEIGHT_H)&(accum_parellel_cnt==15)&
                (accum_valid_ok) & (data_tmp1_addr==15))
            data_tmp1_addr <= 0;
        else if((cmdstate==`CMD_LOAD_WEIGHT_H)&(accum_parellel_cnt==15)&
                (accum_valid_ok))
            data_tmp1_addr <= data_tmp1_addr + 1;
        // 保存loadr/loadz的计算结果到tmp1
        else if(((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z)) &
                (cal_product_valid) & (data_tmp1_addr==15))
            data_tmp1_addr <= 0;
        else if(((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z)) & (cal_product_valid))
            data_tmp1_addr <= data_tmp1_addr + 1;
        // 计算add
        else if((data_tmp1_addr==15) & (cmdstate==`CMD_ADD))
            data_tmp1_addr <= 0;
        else if(cmdstate==`CMD_ADD)
            data_tmp1_addr <= data_tmp1_addr + 1;
    end

    assign data_tmp0_en = ((accum_parellel_cnt==15) & accum_valid_ok & (cmdstate==`CMD_LOAD_WEIGHT_I)) | 
                            (cmdstate==`CMD_ADD) |
                            ((cmdstate==`CMD_LOAD_ZMINUS)&cal_product_valid);
    assign data_tmp0_write = (accum_valid_ok & (cmdstate==`CMD_LOAD_WEIGHT_I))|
                            ((cmdstate==`CMD_LOAD_ZMINUS)&cal_product_valid);
    assign data_tmp0_wdata = (cmdstate==`CMD_LOAD_ZMINUS) ? cal_product : accum_shift;
    always @(posedge clk) data_tmp0_valid <= data_tmp0_en;

    assign data_tmp1_en = (accum_parellel_cnt==15)&accum_valid_ok & (cmdstate==`CMD_LOAD_WEIGHT_H) |
                            (cmdstate==`CMD_ADD)| 
                            (((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z))&cal_product_valid);
    assign data_tmp1_write = (accum_valid_ok & (cmdstate==`CMD_LOAD_WEIGHT_H))|
                            (((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z))&cal_product_valid);
    assign data_tmp1_wdata = (cmdstate==`CMD_LOAD_WEIGHT_H) ? 
                                accum_shift : 
                                (((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z))&cal_product_valid) ?
                                    cal_product:
                                    0;

    /* --------------  data_tmp end -------------- */
    /* --------------  data_r begin -------------- */
    always @(posedge clk) begin
        if(!rst_n) data_r_rdata <= 0;
        else if(!data_r_en) data_r_rdata <= 0;
        else if(!data_r_write) data_r_rdata <= data_r[data_r_addr];
    end

    always @(posedge clk) begin
        if(data_r_en & data_r_write) 
            data_r[data_r_addr] <= data_r_wdata;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_r_addr <= 0;
        // 加载
        else if((data_r_addr==15)&(cmdstate==`CMD_STORE_R))
            data_r_addr <= 0;
        else if((cmdstate==`CMD_STORE_R))
            data_r_addr <= data_r_addr+1;
        // 计算
        else if((data_r_addr==15)&(cmdstate==`CMD_LOAD_R))
            data_r_addr <= 0;
        else if((cmdstate==`CMD_LOAD_R)&(calstate==CALST_MULTADD))
            data_r_addr <= data_r_addr+1;

    end

    assign data_r_en = (cmdstate==`CMD_STORE_R) | 
                        ((cmdstate==`CMD_LOAD_R)&(calstate==CALST_MULTADD));
    assign data_r_write = (cmdstate==`CMD_STORE_R);
    assign data_r_wdata = (cmdstate==`CMD_STORE_R) ? data_out_parellel[data_out_addr] : 0;
    /* --------------  data_r end -------------- */
    /* --------------  data_z begin -------------- */
    wire [255:0] data_z_rdata_minus;
    wire signed [15:0] data_z_rdata_detach[0:15];
    wire signed [15:0] data_z_minus[0:15];
    genvar z;
    generate
        
        for(z=0;z<16;z=z+1) begin
            assign data_z_rdata_minus[z*16+:16] = data_z_minus[z];
            assign data_z_minus[z] = 16'sd4096 - data_z_rdata_detach[z];
            assign data_z_rdata_detach[z] = data_z_rdata[z*16+:16];
            assign data_layer_rdata_trans[z*16+:16] = data_layer_rdata[(15-z)*16+:16];
            assign data_h_rdata_trans[z*16+:16] = data_h_rdata[(15-z)*16+:16];
            assign data_out_parellel_trans[z*16+:16] = data_out_parellel[data_out_addr][(15-z)*16+:16];
        end
    endgenerate
    
    always @(posedge clk) begin
        if(!rst_n) data_z_rdata <= 0;
        else if(!data_z_en) data_z_rdata <= 0;
        else if(!data_z_write) data_z_rdata <= data_z[data_z_addr];
    end

    always @(posedge clk) begin
        if(data_z_en & data_z_write) 
            data_z[data_z_addr] <= data_z_wdata;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_z_addr <= 0;
        // 加载
        else if((data_z_addr==15)&(cmdstate==`CMD_STORE_Z))
            data_z_addr <= 0;
        else if((cmdstate==`CMD_STORE_Z))
            data_z_addr <= data_z_addr+1;
        // 计算
        else if((data_z_addr==15) & 
                ((cmdstate==`CMD_LOAD_Z)|(cmdstate==`CMD_LOAD_ZMINUS)))
            data_z_addr <= 0;
        else if((calstate==CALST_MULTADD) & 
                ((cmdstate==`CMD_LOAD_Z)|(cmdstate==`CMD_LOAD_ZMINUS)))
            data_z_addr <= data_z_addr+1;
    end

    assign data_z_en = (cmdstate==`CMD_STORE_Z) | 
                        ((cmdstate==`CMD_LOAD_Z)&(calstate==CALST_MULTADD))|
                        ((cmdstate==`CMD_LOAD_ZMINUS)&(calstate==CALST_MULTADD));
    assign data_z_write = (cmdstate==`CMD_STORE_Z);
    assign data_z_wdata = (cmdstate==`CMD_STORE_Z) ? data_out_parellel[data_out_addr] : 0;
    /* --------------  data_z end -------------- */
    assign gru_param_en =   (sdram_rdata_vld) |             // 将sdram参数数据写入gru
                            (wrstate==WRST_READ)|           // 将gru参数读到计算模块
                            (cmdstate==`CMD_LOAD_R) |
                            (cmdstate==`CMD_LOAD_ZMINUS) 
                            ;

    assign gru_param_in =   ((cmdstate==`CMD_LOAD_BIAS)|
                             (cmdstate==`CMD_LOAD_WEIGHT_I)|
                             (cmdstate==`CMD_LOAD_WEIGHT_H)) ?
                                sdram_rdata:
                                    0
                                ;

    assign gru_param_write = sdram_rdata_vld;

    assign gru_sel =    //(cmdstate==`CMD_LOAD_BIAS) ? 
                        ((cmdstate==`CMD_LOAD_BIAS)|(cmdstate==`CMD_LOAD_R)) ? 
                            1 :
                            ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)) ?
                                2 :
                                0;
    
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) gru_addr <= 0;
        // 向sdram请求数据，写gru。
        else if(sdram_rdata_vld & (gru_addr==(ahb_out_ch[12:5]-1)) & (gru_sel==1)) 
            gru_addr <= 0;
        else if(sdram_rdata_vld & (gru_addr==(ahb_in_ch[12:5]-1)) & (gru_sel==2)) 
            gru_addr <= 0;
        else if(sdram_rdata_vld & 
                ((wrstate==WRST_WRITE)|(wrstate==WRST_WAIT)) ) 
            gru_addr <= gru_addr+1;
        // 从gru读出到cal
        else if(gru_param_en&(gru_addr==(ahb_in_ch[12:5]-1))&
            ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
            gru_addr <= 0;
        else if(gru_param_en&
            ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
            gru_addr <= gru_addr+1;
        else if(cmdstate==`CMD_NONE)
            gru_addr <= 0;
    end


    wire [255:0] gru_param_weight_out;
    wire [15:0] gru_param_bias_out;

    gru #(  .DW(16), .BATCH_LENGTH(16), .DW_MEM(256),    
            .INPUT_CHANNEL(288)
    )gru
    (
        .clk                    (clk                    ),
        .rst_n                  (rst_n                  ),

        .en                     (gru_param_en           ),
        .wdata                  (gru_param_in           ),
        .write                  (gru_param_write        ),
        .sel                    (gru_sel                ),
        .addr                   (gru_addr               ),

        .bias_out_addr          (gru_bias_out_addr      ),
        .weight_out             (gru_param_weight_out   ),
        .bias_out               (gru_param_bias_out     )
    );

    assign accum_clr = (calstate==CALST_IDLE) &(calstate_next!=CALST_IDLE); 

    
    assign accum_valid_ok = (accum_valid_cnt==(ahb_in_ch[12:5]-1))&accum_valid;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) accum_valid_cnt <= 0;
        else if(accum_valid & accum_valid_ok) 
            accum_valid_cnt <= 0;
        else if(accum_valid) accum_valid_cnt <= accum_valid_cnt + 1;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) out_ch_store_cnt <= 0;
        else if((out_ch_store_cnt==(ahb_out_ch[12:1]-1)) & accum_valid_ok) 
            out_ch_store_cnt<=0;
        else if(accum_valid_ok & ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
            out_ch_store_cnt <= out_ch_store_cnt+1;
    end

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) out_ch_load_cnt <= 0;
        else if(cmdstate==`CMD_NONE)
            out_ch_load_cnt <= 0;
        else if((load_cnt==(ahb_in_ch-32)) & sdram_ready &
                ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
            out_ch_load_cnt <= out_ch_load_cnt + load_cnt + 32;
    end

    reg embedding_ok, z_embedding_ok;
    

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) data_out_addr <= 0;
        else if(cmdstate==`CMD_NONE) data_out_addr <= 0;
        // embedding
        else if(embedding_ok | z_embedding_ok)
            data_out_addr <= 0;
        else if(sdram_rdata_vld & 
                ((cmdstate==`CMD_LOAD_EMBEDDING)|(cmdstate==`CMD_LOAD_ZEMBEDDING)))
            data_out_addr<=data_out_addr+1;
        // CMD_LOAD_RESULT
        else if((data_out_addr==15)&
                ((cmdstate==`CMD_LOAD_RESULT)|(cmdstate==`CMD_STORE_H)|
                (cmdstate==`CMD_STORE_R)|(cmdstate==`CMD_STORE_Z)|(cmdstate==`CMD_STORE_H)) )
            data_out_addr <= 0;
        else if((cmdstate==`CMD_LOAD_RESULT)|(cmdstate==`CMD_STORE_H)|
                (cmdstate==`CMD_STORE_R)|(cmdstate==`CMD_STORE_Z))
            data_out_addr<=data_out_addr+1;
    end

    
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) embedding_ok <= 0;
        else if((cmdstate==`CMD_LOAD_EMBEDDING)&(cmdstate_next==`CMD_NONE))
            embedding_ok <= 1;
        else embedding_ok <= 0;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) z_embedding_ok <= 0;
        else if((cmdstate==`CMD_LOAD_ZEMBEDDING)&(cmdstate_next==`CMD_NONE))
            z_embedding_ok <= 1;
        else z_embedding_ok <= 0;
    end

    genvar data_out_i;
    generate
        
        for(data_out_i=0;data_out_i<256;data_out_i=data_out_i+1)begin
            if((data_out_i%16)==0) begin
                assign data_out_parellel[data_out_i/16]={
                    data_out[data_out_i+15],data_out[data_out_i+14],data_out[data_out_i+13],data_out[data_out_i+12], 
                    data_out[data_out_i+11],data_out[data_out_i+10],data_out[data_out_i+9], data_out[data_out_i+8], 
                    data_out[data_out_i+7], data_out[data_out_i+6], data_out[data_out_i+5], data_out[data_out_i+4], 
                    data_out[data_out_i+3], data_out[data_out_i+2], data_out[data_out_i+1], data_out[data_out_i+0]
                };
            end

            always @(posedge clk, negedge rst_n) begin
                //if(!rst_n) data_out[data_out_i] <= 16'h0800;
                if(!rst_n) data_out[data_out_i] <= 16'h0000;
                // 得到计算结果
                else if((data_out_i==out_ch_store_cnt) & accum_valid_ok &
                    ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)))
                    data_out[data_out_i] <= accum_sum;
                // 加载embedding
                else if(sdram_rdata_vld & 
                        (data_out_addr==(data_out_i/16)) &
                        ((cmdstate==`CMD_LOAD_EMBEDDING) | (cmdstate==`CMD_LOAD_ZEMBEDDING)))
                    data_out[data_out_i] <= sdram_rdata[(data_out_i%16)*16+:16];
                // 加载(x * wi + bi) + (h * wh + bh)
                else if(add_parellel_valid & ((data_out_i/16)==accum_parellel_cal_cnt))
                //else if((cmdstate==`CMD_ADD) & ((data_out_i/16)==accum_parellel_cal_cnt_d1))
                    data_out[data_out_i] <= add_parellel_sum[(data_out_i%16)*16+:16];
                // 得到sigmoid运算结果
                else if(sigmoid_valid & (data_out_i==sigmoid_vld_cnt))
                    data_out[data_out_i] <= sigmoid_dout;
                // 得到tanh运算结果
                else if(tanh_valid & (data_out_i==tanh_vld_cnt))
                    data_out[data_out_i] <= tanh_dout;
                // 得到exp运算结果(实际上exp_out应该继续往下自动运算，所以这一步没有意义，有空删除)
                else if(exp_valid & (data_out_i==exp_vld_cnt))
                    data_out[data_out_i] <= exp_out;
                // 得到r*nh, (1-z)*h, z*n运算结果
                else if(((data_out_i/16)==cal_product_cnt) &
                        ((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z)|(cmdstate==`CMD_LOAD_ZMINUS)) )
                    data_out[data_out_i] <= cal_product[(data_out_i%16)*16+:16];
            end
        end
    endgenerate
    
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) cal_product_cnt <= 0;
        else if((cal_product_cnt==15) & cal_product_valid &
                ((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z)|(cmdstate==`CMD_LOAD_ZMINUS)))
            cal_product_cnt <= 0;
        else if(cal_product_valid & ((cmdstate==`CMD_LOAD_R)|(cmdstate==`CMD_LOAD_Z)|(cmdstate==`CMD_LOAD_ZMINUS)))
            cal_product_cnt <= cal_product_cnt + 1;
    end

    assign cal_mult_0 = (cmdstate==`CMD_LOAD_WEIGHT_I) ? data_in_rdata  :
                        (cmdstate==`CMD_LOAD_WEIGHT_H) ? data_h_rdata   :
                        (cmdstate==`CMD_LOAD_R) ? data_in_rdata          : 
                        //(cmdstate==`CMD_LOAD_Z) ? data_h_rdata          : 
                        (cmdstate==`CMD_LOAD_Z) ? data_h_rdata_trans          : 
                        (cmdstate==`CMD_LOAD_ZMINUS) ? data_in_rdata          : 
                        0;
    assign cal_mult_1 = ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H)) ? 
                            gru_param_weight_out :
                            (cmdstate==`CMD_LOAD_R) ?
                                data_r_rdata:
                                (cmdstate==`CMD_LOAD_Z) ?
                                    data_z_rdata:
                                    (cmdstate==`CMD_LOAD_ZMINUS) ?
                                        data_z_rdata_minus:
                                        0;
    assign cal_bias = gru_param_bias_out;


    wire [15:0]                   cal_din_dbg;
    wire [15:0]                   cal_weight_dbg;
    wire [15:0]                   p_dbg;

    assign cal_extend_en = (cmdstate==`CMD_LOAD_WEIGHT_I) & (ahb_layer==32);

    cal_top #(
        
    )cal(

        .cal_din_dbg(cal_din_dbg),
        .cal_weight_dbg(cal_weight_dbg),
        .p_dbg(p_dbg),

        .clk            (clk),
        .rst_n          (rst_n),

        .din            (cal_mult_0),
        .weight_in      (cal_mult_1),
        .bias_in        (cal_bias),
        .channel_sum    (cal_sum),

        .en             (cal_en),
        .valid          (cal_valid),
        .extend_en      (cal_extend_en),
        .product        (cal_product),
        .product_valid  (cal_product_valid)
    );

    accum accum(
        .clk        (clk),
        .rst_n      (rst_n),
        .clr        (accum_clr),
        // 我的定点数计算累加后会低大约0.03，这里做补偿。
        .load       (gru_param_bias_out + ahb_compensation),
        //.load       (0),
        .din        (cal_sum),
        .enable     (cal_valid),
        .dout       (accum_sum),
        .valid      (accum_valid)
    );
    
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) accum_parellel_cnt <= 0;
        else if(cmdstate==`CMD_NONE) accum_parellel_cnt <= 0;
        else if((accum_parellel_cnt==15) & accum_valid_ok)
            accum_parellel_cnt <= 0;
        else if(accum_valid_ok & 
                ((cmdstate==`CMD_LOAD_WEIGHT_I)|(cmdstate==`CMD_LOAD_WEIGHT_H))) 
            accum_parellel_cnt <= accum_parellel_cnt + 1;
    end 
    always @(*) begin
        if(accum_valid)
            accum_shift <= {accum_sum, accum_shift_sync[255:16]};
        else accum_shift <= accum_shift_sync;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) accum_shift_sync <= 0;
        else if(accum_valid_ok)
        accum_shift_sync <= accum_shift;
    end
    assign sigmoid_din = (cmdstate==`CMD_LOAD_SIGMOID) ? 
                        data_in_rdata[((sigmoid_addr%16)<<4)+:16] : 0;
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) sigmoid_en <= 0;
        else if(sigmoid_addr==255) sigmoid_en <= 0;
        else if((cmdstate==`CMD_LOAD_SIGMOID)) 
            sigmoid_en <= data_in_en;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) sigmoid_addr <= 0;
        else if((sigmoid_addr==255)) sigmoid_addr <= 0;
        else if(sigmoid_en) sigmoid_addr <= sigmoid_addr+1;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) sigmoid_vld_cnt <= 0;
        else if(sigmoid_valid & (sigmoid_vld_cnt==255)) 
            sigmoid_vld_cnt<=0;
        else if(sigmoid_valid) sigmoid_vld_cnt<=sigmoid_vld_cnt+1;
    end

    assign tanh_din = (cmdstate==`CMD_LOAD_TANH) ? 
                        data_in_rdata[((tanh_addr%16)<<4)+:16] : 0;
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) tanh_en <= 0;
        else if(tanh_addr==255) tanh_en <= 0;
        else if((cmdstate==`CMD_LOAD_TANH)) 
            tanh_en <= data_in_en;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) tanh_addr <= 0;
        else if((tanh_addr==255)) tanh_addr <= 0;
        else if(tanh_en) tanh_addr <= tanh_addr+1;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) tanh_vld_cnt <= 0;
        else if(tanh_valid & (tanh_vld_cnt==255)) 
            tanh_vld_cnt<=0;
        else if(tanh_valid) tanh_vld_cnt<=tanh_vld_cnt+1;
    end

    assign exp_in = (cmdstate==`CMD_EXP) ? 
                        data_in_rdata[((exp_addr%16)<<4)+:16] : 0;
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) exp_en <= 0;
        else if(exp_addr==97) exp_en <= 0;
        else if((cmdstate==`CMD_EXP)) 
            exp_en <= data_in_en;
    end


    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) exp_addr <= 0;
        else if((exp_addr==97)) exp_addr <= 0;
        else if(exp_en) exp_addr <= exp_addr+1;
    end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) exp_vld_cnt <= 0;
        else if(exp_valid & (exp_vld_cnt==97)) 
            exp_vld_cnt<=0;
        else if(exp_valid) exp_vld_cnt<=exp_vld_cnt+1;
    end
    sigmoid sigmoid(
        .clk        (clk),
        .rst_n      (rst_n),
        .din        (sigmoid_din),
        .en         (sigmoid_en),
        .dout       (sigmoid_dout),
        .valid      (sigmoid_valid)
    );
    tanh tanh(
        .clk        (clk),
        .rst_n      (rst_n),
        .din        (tanh_din),
        .en         (tanh_en),
        .dout       (tanh_dout),
        .valid      (tanh_valid)
    );
    exp u_exp(
        .clk        (clk),
        .en         (exp_en),
        .in         (exp_in),    
        .out        (exp_out),
        .valid      (exp_valid)             
    );

    assign add_parellel0 = data_tmp0_rdata;
    assign add_parellel1 = data_tmp1_rdata;
    always @(posedge clk) 
        add_parellel_valid <= data_tmp0_en & (cmdstate==`CMD_ADD);

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) accum_parellel_cal_cnt <= 0;
        else if(cmdstate!=`CMD_ADD) accum_parellel_cal_cnt <= 0;
        else if((accum_parellel_cal_cnt==15)&(cmdstate==`CMD_ADD)) 
            accum_parellel_cal_cnt<=0;
        else if((cmdstate==`CMD_ADD)&data_tmp0_valid)
            accum_parellel_cal_cnt <= accum_parellel_cal_cnt + 1;
    end

    always @(posedge clk)
        accum_parellel_cal_cnt_d1 <= accum_parellel_cal_cnt;

    add_top add_parellel(
        .add0   (add_parellel0),
        .add1   (add_parellel1),
        .sum    (add_parellel_sum)
    );


    
    wire [39:0] multinomial_din;
    wire multinomial_add_en;
    wire [7:0] multinomial_index_dout;
    wire multinomial_valid;
    
    wire multinomial_reload;
    //wire [31:0] multinomial_seed_in;
    reg [31:0] multinomial_seed_in;
    wire multinomial_seed_en;

    wire [39:0] target_dbg;
    wire [39:0] sum_dbg;
    wire [39:0] accum_dbg;
    wire [3:0] state_dbg;
    wire [6:0] accum_cnt_dbg;
    wire [39:0] rdata_dbg;

    assign multinomial_din = exp_out;
    assign multinomial_add_en = exp_valid;
    assign multinomial_reload = multinomial_valid;
    //assign multinomial_seed_in = 1;
    // always @(posedge clk, negedge rst_n) begin
    //     if(!rst_n) multinomial_seed_in <= 1;
    //     else if(multinomial_seed_in==10000) multinomial_seed_in <= 1;
    //     else multinomial_seed_in <= multinomial_seed_in + 1;
    // end
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) multinomial_seed_in <= 1;
        else multinomial_seed_in <= random_seed;
    end
    assign multinomial_seed_en = (ahb_cmd==20);
    wire chord_flag;
    

    multinomial #(.DW(40)) 
    u_multinomial(
        .clk        (clk),
        .rst_n      (rst_n),
        .din        (multinomial_din),
        .add_en     (multinomial_add_en),
        .reload     (multinomial_reload),
        .seed_in    (multinomial_seed_in),
        .seed_en    (multinomial_seed_en),
        .index_dout (multinomial_index_dout),
        .valid      (multinomial_valid),

        .chord_flag (chord_flag),

        .target_dbg (target_dbg),
        .sum_dbg    (sum_dbg),
        .accum_dbg  (accum_dbg),
        .state_dbg  (state_dbg),
        .accum_cnt_dbg(accum_cnt_dbg),
        .rdata_dbg  (rdata_dbg)
    );

    wire [5:0] ahb_layer_ila;
    assign ahb_layer_ila = ahb_layer[5:0];
    reg [11:0] result_cnt_ila;
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) result_cnt_ila <= 0;
        else if(multinomial_valid) result_cnt_ila <= result_cnt_ila + 1;
    end
    assign chord_flag = ((result_cnt_ila%5)==4);

    wire result_wrong_ila;
    assign result_wrong_ila = (result_cnt_ila!=0) & 
            ((((result_cnt_ila%5)==0) & (midi_wave<47))|(((result_cnt_ila%5)!=0) & (midi_wave>48)));

    generate 
        if(LOADPARAMS_ILA_DBG) begin

            wire [4:0] cmd_state_dbg = cmdstate[4:0];
            wire [16:0] sdram_rdata_dbg = sdram_rdata[16:0];
            wire [23:0] sdram_addr_dbg = sdram_addr[23:0];

            // ila_loadparams ila_loadparams(
            //     .clk(clk),

            //     .probe0 (cmd_state_dbg),
            //     .probe1 (midi_wave),
            //     .probe2 (sdram_rdata_dbg),
            //     .probe3 (target_dbg),
            //     .probe4 (sdram_rdata_vld),
            //     .probe5 (result_wrong_ila),
            //     .probe6 (sdram_write),
            //     .probe7 (result_cnt_ila),
            //     .probe8 (sdram_ready),
            //     .probe9 (tanh_dout),
            //     .probe10(sigmoid_dout),
            //     .probe11(cal_en),
            //     .probe12(ahb_layer_ila),
            //     .probe13(accum_sum),
            //     .probe14(accum_valid_ok),
            //     .probe15(gru_bias_out_addr),
            //     .probe16(gru_param_bias_out),
            //     .probe17(sdram_addr_dbg),
            //     .probe18(accum_dbg),
            //     .probe19(NN_result_valid),
            //     .probe20(multinomial_valid),
            //     .probe21(multinomial_index_dout),
            //     .probe22(exp_in),
            //     .probe23(accum_cnt_dbg),

            //     .probe24(gru_param_en),
            //     .probe25(gru_param_write),
            //     .probe26(gru_sel),
            //     .probe27(gru_addr)
            // );

            // ila_loadparams2 ila_loadparams2(
            //     .clk(clk),
                
            //     .probe0 (cmd_state_dbg),
            //     .probe1 (sdram_rdata_dbg),
            //     .probe2 (sdram_rdata_vld),
            //     .probe3 (sdram_write),
            //     .probe4 (result_cnt_ila),
            //     .probe5 (sdram_ready),
            //     .probe6 (cal_en),
            //     .probe7 (ahb_layer_ila),
            //     .probe8 (gru_addr),
            //     .probe9 (gru_bias_out_addr),
            //     .probe10(gru_param_bias_out),
            //     .probe11(sdram_addr_dbg),
            //     .probe12(NN_result_valid),

            //     .probe13(gru_param_en),
            //     .probe14(gru_sel)
            // );
        end
    endgenerate
    
    assign NN_SR = {24'b0, cmdstate};

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) midi_wave <= 0;
        else if(multinomial_valid)
            midi_wave <= multinomial_index_dout;
    end

    reg multinomial_valid_d;
    always @(posedge clk) multinomial_valid_d <= multinomial_valid;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) NN_result_valid <= 0;
        else NN_result_valid = multinomial_valid | multinomial_valid_d;
    end

    // simulation
    always @(posedge clk) if(NN_result_valid) $display("get: %d\n", midi_wave);
    

endmodule


