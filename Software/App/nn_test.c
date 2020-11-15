




#include "nn_test.h"
#include "load_params.h"



//
// fpga��cacheֻ����data_in0�� data_in1, data_out, data_tmp, ��data_r, data_z, data_n������sdram��
// 
// 
// ------- r �� z �������� -------
// 1. load bi, 256��16bit�Ĵ���
// 2. load wi�� 288��16bit�Ĵ�����һ��loadһ��channel����Ҫ����288/16=18��
// 3. ���棺R_X_L0_RESULT -> data_in0
// 4. load bh, 256��16bit�Ĵ���
// 5. load wh��256��16bit�Ĵ�����һ��loadһ��channel����Ҫ����256/16=16��
// 6. ���棺R_H_L0_RESULT -> data_in1
// 7. �ӷ�����256��cycle���ꡣdata_out -> data_in0
// 8. r=sigmoid(data_in0)����rд��sdram
//
// ------- n �������� -------
// 1. load bi, 256��16bit�Ĵ���
// 2. load wi�� 288��16bit�Ĵ�����һ��loadһ��channel����Ҫ����288/16=18��
// 3. ���棺N_X_L0_RESULT -> data_in0
// 4. load bh, 256��16bit�Ĵ���
// 5. load wh��256��16bit�Ĵ�����һ��loadһ��channel����Ҫ����256/16=16��
// 6. ���棺N_H_L0_RESULT -> data_in1
// 7. �˷���data_out = data_in0 * data_in1
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
	
	NN_CTRL->LAYER = 0<<4;		// ��ht-1�Ľ����Ϊh
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
	
	NN_CTRL->LAYER = 0<<4;		// ��ht-1�Ľ����Ϊh
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
	NN_LoadWeight(SDRAM_ADDR_N_WI_L0, 288, 256, CMD_LOAD_WEIGHT_I, 0);	// ����Զ����ص�tmp0
	
	NN_CTRL->LAYER = 0<<4;		// ��ht-1�Ľ����Ϊh
	NN_LoadBias(SDRAM_ADDR_N_BH_L0, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WH_L0, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	// ׼������r*h.w + b
	NN_LoadResult();	// ��(h.w + b)�ļ�����ת�Ƶ�data_in[255..0]
	NN_LoadR();			// ��R���������㣬��data_out�� tmp1�Զ�����LoadR�Ľ��
	
	NN_Add();

	NN_LoadResult();
	NN_LoadTanh();	// tanh
	/* -------------- h = (1-z) * n + z * h-------------- */
	// (1-z)*n
	NN_LoadResult();	// ������õ���nת�Ƶ�data_in[255..0]
	NN_LoadZMinus();	// ��Z��gru����ȡ��������õ�(1-z)*h����data_out������tmp0�ݴ�
	
	// z*h
	NN_LoadZ();			// ��Z��gru����������z * h����data_out������tmp1�ݴ�
	NN_Add();			// tmp0 + tmp1 = (1-z)*n + z*h
	NN_StoreH(0);		// ��layer0���������浽h[0], ��Ϊ��һ���x
	
	/* -------------- r1 -------------- */

	NN_LoadH(0);		// ����һ��layer��������Ϊx����
	NN_LoadBias(SDRAM_ADDR_R_BI_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WI_L1, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 1<<4;		// ��ht-1�Ľ����Ϊh
	NN_LoadBias(SDRAM_ADDR_R_BH_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WH_L1, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreR();		// store r.
	
	/* -------------- z1 -------------- */
	
	NN_LoadH(0);		// ����һ��layer��������Ϊx����
	NN_LoadBias(SDRAM_ADDR_Z_BI_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WI_L1, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 1<<4;		// ��ht-1�Ľ����Ϊh
	NN_LoadBias(SDRAM_ADDR_Z_BH_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WH_L1, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreZ();		// store z.
	
	/* -------------- n1 -------------- */
	NN_LoadH(0);		// ����һ��layer��������Ϊx����
	NN_LoadBias(SDRAM_ADDR_N_BI_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WI_L1, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 1<<4;		// ��ht-1�Ľ����Ϊh
	NN_LoadBias(SDRAM_ADDR_N_BH_L1, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WH_L1, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	// ׼������r*h.w + b
	NN_LoadResult();	// ��(h.w + b)�ļ�����ת�Ƶ�data_in[255..0]
	NN_LoadR();			// ��R���������㣬��data_out�� tmp1�Զ�����LoadR�Ľ��
	NN_Add();
	NN_LoadResult();
	NN_LoadTanh();	// tanh
	
	/* -------------- h = (1-z) * n + z * h-------------- */
	// (1-z)*n
	NN_LoadResult();	// ������õ���nת�Ƶ�data_in[255..0]
	NN_LoadZMinus();	// ��Z��gru����ȡ��������õ�(1-z)*h����data_out������tmp0�ݴ�
	
	// z*h
	NN_LoadZ();			// ��Z��gru����������z * h����data_out������tmp1�ݴ�
	NN_Add();			// tmp0 + tmp1 = (1-z)*n + z*h
	NN_StoreH(1);	// ��layer0���������浽tmp
	
	/* -------------- r2 -------------- */

	NN_LoadH(1);		// ����һ��layer��������Ϊx����
	NN_LoadBias(SDRAM_ADDR_R_BI_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WI_L2, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 2<<4;		// ��ht-1�Ľ����Ϊh
	NN_LoadBias(SDRAM_ADDR_R_BH_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_R_WH_L2, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreR();		// store r.
	
	/* -------------- z2 -------------- */
	
	NN_LoadH(1);		// ����һ��layer��������Ϊx����
	NN_LoadBias(SDRAM_ADDR_Z_BI_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WI_L2, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 2<<4;		// ��ht-1�Ľ����Ϊh
	NN_LoadBias(SDRAM_ADDR_Z_BH_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_Z_WH_L2, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	NN_Add();	// (x * r_wi + bi) + (h * r_wh + bh)
	NN_LoadResult();
	NN_LoadSigmoid();	// sigmoid
	NN_StoreZ();		// store z.
	
	/* -------------- n2 -------------- */
	
	NN_LoadH(1);		// ����һ��layer��������Ϊx����
	NN_LoadBias(SDRAM_ADDR_N_BI_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WI_L2, 256, 256, CMD_LOAD_WEIGHT_I, 0);
	
	NN_CTRL->LAYER = 2<<4;		// ��ht-1�Ľ����Ϊh
	NN_LoadBias(SDRAM_ADDR_N_BH_L2, 256);
	NN_LoadWeight(SDRAM_ADDR_N_WH_L2, 256, 256, CMD_LOAD_WEIGHT_H, 0);
	
	// ׼������r*h.w + b
	NN_LoadResult();	// ��(h.w + b)�ļ�����ת�Ƶ�data_in[255..0]
	NN_LoadR();			// ��R���������㣬��data_out�� tmp1�Զ�����LoadR�Ľ��
	NN_Add();
	NN_LoadResult();
	NN_LoadTanh();	// tanh
	
	/* -------------- h = (1-z) * n + z * h-------------- */
	// (1-z)*n
	NN_LoadResult();	// ������õ���nת�Ƶ�data_in[255..0]
	NN_LoadZMinus();	// ��Z��gru����ȡ��������õ�(1-z)*h����data_out������tmp0�ݴ�
	
	// z*h
	NN_LoadZ();			// ��Z��gru����������z * h����data_out������tmp1�ݴ�
	NN_Add();			// tmp0 + tmp1 = (1-z)*n + z*h
	NN_StoreH(2);		// �����μ��������浽H
	
	
	/* -------------- output -------------- */
	NN_LoadZEmbedding(zembedding);	
	NN_LoadH(2);
	
	NN_LoadWeight(SDRAM_ADDR_OUTPUT, 288, 98, CMD_LOAD_WEIGHT_I, 0);
	
	/* -------------- exp -------------- */
	NN_LoadResult();
	NN_Exp();
	
	
}

	

