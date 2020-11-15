







module add_top(

    input               clk,
    input               rst_n,

    input   [255:0]     add0,
    input   [255:0]     add1,
    input               en,

    output  [255:0]     sum,
    output              valid


);

    wire [15:0] add0_detach[0:15];
    wire [15:0] add1_detach[0:15];
    wire [15:0] sum_detach[0:15];


    genvar i;
    generate
        for(i=0;i<16;i=i+1) begin
            assign add0_detach[i] = add0[i*16+:16];
            assign add1_detach[i] = add1[i*16+:16];
            assign sum[i*16+:16] = sum_detach[i];

            ip_add_no_latency add(

                .A      (add0_detach[i]),
                .B      (add1_detach[i]),
                .S      (sum_detach[i])

            );
        end

    endgenerate


endmodule
