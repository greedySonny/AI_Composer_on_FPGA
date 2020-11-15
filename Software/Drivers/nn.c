#ifndef __NN_H
#define __NN_H



#include "nn.h"

#include "driver.h"


uint8_t NNIntFlag = 0;



void NN_SDRAM_Load_test(void)
{
	NN_CTRL->BIAS_ADDR = 0;
	NN_CTRL->OUT_CH = 8192;
	NN_CTRL->CMD = CMD_LOAD_TEST;
	while(NN_CTRL->SR != 0);
}


void NN_Init(void)
{

	NN_CTRL->CR = 0x2;		// 开中断
	
	NVIC_ClearPendingIRQ(NN_IRQn);
	NVIC_EnableIRQ(NN_IRQn);
}


void NN_InitH(uint32_t layer)
{
	NN_CTRL->LAYER = layer<<4;
	NN_CTRL->CMD = CMD_INIT_H;
	
	while(NN_CTRL->SR != 0);
}

void NN_LoadEmbedding(uint32_t addr)
{	
	
	NN_CTRL->OUT_CH = 256*2;
	NN_CTRL->BIAS_ADDR = addr;
	NN_CTRL->CMD = CMD_LOAD_EMBEDDING;
	
	while(NN_CTRL->SR != 0);
}
	
void NN_LoadZEmbedding(uint32_t addr)
{
	
	NN_CTRL->OUT_CH = 32*2;
	NN_CTRL->BIAS_ADDR = addr;
	NN_CTRL->CMD = CMD_LOAD_ZEMBEDDING;
	
	while(NN_CTRL->SR != 0);
}

void NN_LoadBias(uint32_t bias_addr, uint32_t out_ch)
{
	
	NN_CTRL->BIAS_ADDR = bias_addr;
	NN_CTRL->OUT_CH = out_ch<<1;
	NN_CTRL->CMD = CMD_LOAD_BIAS;
	while(NN_CTRL->SR != 0);
}

void NN_LoadWeight(uint32_t weight_addr, uint32_t in_ch, uint32_t out_ch, uint8_t sel_i, uint32_t compensation)
{
	NN_CTRL->IN_CH = in_ch<<1;
	NN_CTRL->OUT_CH = out_ch<<1;
	NN_CTRL->WEIGHT_ADDR = weight_addr;
	NN_CTRL->COMPENSATION = compensation;
	
	NN_CTRL->CMD = sel_i;
		
	while(NN_CTRL->SR != 0);
}

void NN_LoadResult(void)
{
	NN_CTRL->CMD = CMD_LOAD_RESULT;
	while(NN_CTRL->SR != 0);
}

void NN_LoadXPlusResult(void)
{
	NN_CTRL->CMD = CMD_LOAD_X_PLUS_RESULT;
	while(NN_CTRL->SR != 0);
}	

void NN_Add(void)
{
	NN_CTRL->CMD = CMD_ADD;
	while(NN_CTRL->SR != 0);
}	

void NN_StoreR(void)
{
	NN_CTRL->CMD = CMD_STORE_R;
	while(NN_CTRL->SR != 0);
}

void NN_StoreZ(void)
{
	NN_CTRL->CMD = CMD_STORE_Z;
	while(NN_CTRL->SR != 0);
}

void NN_LoadR(void)
{
	NN_CTRL->CMD = CMD_LOAD_R;
	while(NN_CTRL->SR != 0);
}

void NN_StoreHLayer(uint32_t layer)
{
	NN_CTRL->LAYER = layer<<4;
	NN_CTRL->CMD = CMD_STORE_LAYER;
	while(NN_CTRL->SR != 0);
}

void NN_LoadHLayer(uint32_t layer)
{
	NN_CTRL->LAYER = layer<<4;
	NN_CTRL->CMD = CMD_LOAD_LAYER;
	while(NN_CTRL->SR != 0);
}

void NN_LoadZ(void)
{
	NN_CTRL->CMD = CMD_LOAD_Z;
	while(NN_CTRL->SR != 0);
}

void NN_LoadZMinus(void)
{
	NN_CTRL->CMD = CMD_LOAD_ZMINUS;
	while(NN_CTRL->SR != 0);
}

void NN_LoadH(uint32_t layer)
{
	NN_CTRL->LAYER = layer<<4;
	NN_CTRL->CMD = CMD_LOAD_H;
	while(NN_CTRL->SR != 0);
}

void NN_StoreH(uint32_t layer)
{
	NN_CTRL->LAYER = layer<<4;
	NN_CTRL->CMD = CMD_STORE_H;
	while(NN_CTRL->SR != 0);
}

void NN_LoadSigmoid(void)
{
	NN_CTRL->CMD = CMD_LOAD_SIGMOID;
	while(NN_CTRL->SR != 0);
}

void NN_LoadTanh(void)
{
	NN_CTRL->CMD = CMD_LOAD_TANH;
	while(NN_CTRL->SR != 0);
}

void NN_Exp(void)
{
	NN_CTRL->CMD = CMD_EXP;
	while(NN_CTRL->SR != 0);
}

uint32_t NN_LoadData(void)
{
	return NN_CTRL->DATA;
}

void NN_RandomSeed(uint32_t seed)
{
	NN_CTRL->DATA = seed;

}
	
#endif
