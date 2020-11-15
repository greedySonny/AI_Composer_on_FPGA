


module gru
#(
    parameter DW                    = 16,
    parameter BATCH_LENGTH          = 16,
    parameter DW_MEM                = 256,

    parameter INPUT_CHANNEL         = 288,
    parameter OUTPUT_CHANNEL        = 256
)(

    input                               clk,
    input                               rst_n,

    // 256 sdram data in， 16x32 cal data out， 16x288 block data store
    // 写数据
    input                               en,
    input       [DW_MEM-1:0]            wdata,
    input                               write,
    input       [3:0]                   sel,
    input       [9:0]                   addr,

    // 读数据
    output  reg [DW*BATCH_LENGTH-1:0]   weight_out,
    input       [9:0]                   bias_out_addr,
    output  reg [DW-1:0]                bias_out,
    output      [DW*BATCH_LENGTH-1:0]   data_out

);

    localparam  SEL_NONE        = 0,
                SEL_BIAS        = 1,
                SEL_WEIGHT      = 2;


    // 每个output，认为有288个16-bit的data
    (* ram_style="block" *)reg     [DW_MEM-1:0]            param_weight_mem[0:(INPUT_CHANNEL*DW/DW_MEM)-1];
    (* ram_style="block" *)reg     [DW_MEM-1:0]            param_bias_mem[0:(OUTPUT_CHANNEL*DW/DW_MEM)-1];


    // 写bias
    // genvar bias_i;
    // generate 
    //     // in: 256bit, out: 16x256bit
    //     for(bias_i=0; bias_i<(OUTPUT_CHANNEL*DW/DW_MEM); bias_i=bias_i+1)begin
    //         always @(posedge clk) 
    //             if(en & write & (sel==SEL_BIAS) & (addr==bias_i)) 
    //                 param_bias_mem[bias_i] <= wdata;
    //     end
    // endgenerate


        // in: 256bit, out: 16x256bit
            always @(posedge clk) 
                if(en & write & (sel==SEL_BIAS)) 
                    param_bias_mem[addr] <= wdata;


    // 读bias
    // always @(posedge clk, negedge rst_n) begin
    //     if(!rst_n) bias_out <= 0;
    //     else
    //         bias_out <= param_bias_mem[bias_out_addr[9:4]][(15-bias_out_addr[3:0])*16+:16];
    // end
    always @(posedge clk) 
        bias_out <= param_bias_mem[bias_out_addr[9:4]][(15-bias_out_addr[3:0])*16+:16];



    // 写weight
    // genvar weight_i;
    // generate 
    //     // in: 256bit, out: 16x288bit
    //     for(weight_i=0; weight_i<(INPUT_CHANNEL*DW/DW_MEM); weight_i=weight_i+1)begin
    //         always @(posedge clk) 
    //             if(en & write & (sel==SEL_WEIGHT) & (addr==weight_i)) 
    //                 param_weight_mem[weight_i] <= wdata;
    //     end
    // endgenerate


        // in: 256bit, out: 16x288bit
        always @(posedge clk) 
            if(en & write & (sel==SEL_WEIGHT)) 
                param_weight_mem[addr] <= wdata;



    // 读weight
    always @(posedge clk) begin
        if(sel==SEL_WEIGHT) weight_out <= param_weight_mem[addr];
        else weight_out <= 0;
    end



endmodule



