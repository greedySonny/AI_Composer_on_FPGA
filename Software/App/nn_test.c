




#include "nn_test.h"
#include "load_params.h"



//
// fpga的cache只保留data_in0， data_in1, data_out, data_tmp, 而data_r, data_z, data_n储存在sdram。
// 
// 
// ------- r 和 z 计算流程 -------
// 1. load bi, 256个16bit寄存器
// 2. load wi， 288个16bit寄存器，一次load一个channel，需要计算288/16=18次
// 3. 保存：R_X_L0_RESULT -> data_in0
// 4. load bh, 256个16bit寄存器
// 5. load wh，256个16bit寄存器，一次load一个channel，需要计算256/16=16次
// 6. 保存：R_H_L0_RESULT -> data_in1
// 7. 加法器，256个cycle加完。data_out -> data_in0
// 8. r=sigmoid(data_in0)，将r写入sdram
//
// ------- n 计算流程 -------
// 1. load bi, 256个16bit寄存器
// 2. load wi， 288个16bit寄存器，一次load一个channel，需要计算288/16=18次
// 3. 保存：N_X_L0_RESULT -> data_in0
// 4. load bh, 256个16bit寄存器
// 5. load wh，256个16bit寄存器，一次load一个channel，需要计算256/16=16次
// 6. 保存：N_H_L0_RESULT -> data_in1
// 7. 乘法器data_out = data_in0 * data_in1
// 
//
//
//



void NN_Run(uint32_t zembedding, uint32_t embedding)
{
	
	NN_CTRL->LAYER = 0<<4;
	/* -------------- r0 -------------- */
	NN_LoadZEmbedding(zembedding);	
	NN_LoadEmbedding(embedding);		
	NN_LoadBias(SDRAM_ADDR_R_BI_L0, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WI_L0, 288, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 0<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_R_BH_L0, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WH_L0, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();
	NN_LoadResult();		// data_in = data_out
	NN_LoadSigmoid();		// data_out = sigmoid(data_in)
	NN_StoreR();			// store r.
	
	/* -------------- z0 -------------- */
	NN_LoadZEmbedding(zembedding);	
	NN_LoadEmbedding(embedding);	
	NN_LoadBias(SDRAM_ADDR_Z_BI_L0, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WI_L0, 288, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 0<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_Z_BH_L0, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WH_L0, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();
	
	NN_LoadResult();		// data_in = data_out
	NN_LoadSigmoid();		// data_out = sigmoid(data_in)
	NN_StoreZ();			// store z.

	/* -------------- n0 -------------- */
	NN_LoadZEmbedding(zembedding);	
	NN_LoadEmbedding(embedding);	
	NN_LoadBias(SDRAM_ADDR_N_BI_L0, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WI_L0, 288, 256, CMD_LOAD_WEIGHT_I, 0);	// 结果自动加载到tmp0
	
	NN_CTRL->LAYER = 0<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_N_BH_L0, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WH_L0, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	// 准备计算r*h.w + b
	NN_LoadResult();	// 将(h.w + b)的计算结果转移到data_in[255..0]
	NN_LoadR();			// 将R读出并计算，到data_out， tmp1自动加载LoadR的结果
	
	NN_Add();

	NN_LoadResult();
	NN_LoadTanh();	// tanh
	/* -------------- h = (1-z) * n + z * h-------------- */
	// (1-z)*n
	NN_LoadResult();	// 将计算得到的n转移到data_in[255..0]
	NN_LoadZMinus();	// 将Z由gru读出取负并计算得到(1-z)*h，到data_out，并到tmp0暂存
	
	// z*h
	NN_LoadZ();			// 将Z由gru读出并计算z * h，到data_out，并到tmp1暂存
	NN_Add();			// tmp0 + tmp1 = (1-z)*n + z*h
	NN_StoreH(0);		// 将layer0计算结果保存到h[0], 作为下一层的x
	
	/* -------------- r1 -------------- */

	NN_LoadH(0);		// 将上一层layer计算结果作为x加载
	NN_LoadBias(SDRAM_ADDR_R_BI_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WI_L1, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 1<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_R_BH_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WH_L1, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreR();		// store r.
	
	/* -------------- z1 -------------- */
	
	NN_LoadH(0);		// 将上一层layer计算结果作为x加载
	NN_LoadBias(SDRAM_ADDR_Z_BI_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WI_L1, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 1<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_Z_BH_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WH_L1, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreZ();		// store z.
	
	/* -------------- n1 -------------- */
	NN_LoadH(0);		// 将上一层layer计算结果作为x加载
	NN_LoadBias(SDRAM_ADDR_N_BI_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WI_L1, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 1<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_N_BH_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WH_L1, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	// 准备计算r*h.w + b
	NN_LoadResult();	// 将(h.w + b)的计算结果转移到data_in[255..0]
	NN_LoadR();			// 将R读出并计算，到data_out， tmp1自动加载LoadR的结果
	NN_Add();
	NN_LoadResult();
	NN_LoadTanh();	// tanh
	
	/* -------------- h = (1-z) * n + z * h-------------- */
	// (1-z)*n
	NN_LoadResult();	// 将计算得到的n转移到data_in[255..0]
	NN_LoadZMinus();	// 将Z由gru读出取负并计算得到(1-z)*h，到data_out，并到tmp0暂存
	
	// z*h
	NN_LoadZ();			// 将Z由gru读出并计算z * h，到data_out，并到tmp1暂存
	NN_Add();			// tmp0 + tmp1 = (1-z)*n + z*h
	NN_StoreH(1);	// 将layer0计算结果保存到tmp
	
	/* -------------- r2 -------------- */

	NN_LoadH(1);		// 将上一层layer计算结果作为x加载
	NN_LoadBias(SDRAM_ADDR_R_BI_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WI_L2, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 2<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_R_BH_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WH_L2, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreR();		// store r.
	
	/* -------------- z2 -------------- */
	
	NN_LoadH(1);		// 将上一层layer计算结果作为x加载
	NN_LoadBias(SDRAM_ADDR_Z_BI_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WI_L2, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 2<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_Z_BH_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WH_L2, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreZ();		// store z.
	
	/* -------------- n2 -------------- */
	
	NN_LoadH(1);		// 将上一层layer计算结果作为x加载
	NN_LoadBias(SDRAM_ADDR_N_BI_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WI_L2, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 2<<4;		// 将ht-1的结果作为h
	NN_LoadBias(SDRAM_ADDR_N_BH_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WH_L2, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	// 准备计算r*h.w + b
	NN_LoadResult();	// 将(h.w + b)的计算结果转移到data_in[255..0]
	NN_LoadR();			// 将R读出并计算，到data_out， tmp1自动加载LoadR的结果
	NN_Add();
	NN_LoadResult();
	NN_LoadTanh();	// tanh
	
	/* -------------- h = (1-z) * n + z * h-------------- */
	// (1-z)*n
	NN_LoadResult();	// 将计算得到的n转移到data_in[255..0]
	NN_LoadZMinus();	// 将Z由gru读出取负并计算得到(1-z)*h，到data_out，并到tmp0暂存
	
	// z*h
	NN_LoadZ();			// 将Z由gru读出并计算z * h，到data_out，并到tmp1暂存
	NN_Add();			// tmp0 + tmp1 = (1-z)*n + z*h
	NN_StoreH(2);		// 将本次计算结果保存到H
	
	
	/* -------------- output -------------- */
	NN_LoadZEmbedding(zembedding);	
	NN_LoadH(2);
	
	NN_LoadWeight(SDRAM_ADDR_OUTPUT, 288, 98, CMD_LOAD_WEIGHT_I, 0);
	
	/* -------------- exp -------------- */
	NN_LoadResult();
	NN_Exp();
	
	
}

	

