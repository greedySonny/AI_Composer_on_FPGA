



#include "CMSDK_CM0.h"


#define CMD_NONE				0
#define CMD_LOAD_EMBEDDING		1
#define CMD_LOAD_ZEMBEDDING		2
#define CMD_LOAD_BIAS			3
#define CMD_LOAD_WEIGHT_I		4
#define CMD_LOAD_WEIGHT_H		5
#define CMD_LOAD_RESULT			6
#define CMD_LOAD_X_PLUS_RESULT  7
#define CMD_LOAD_SIGMOID		8
#define CMD_LOAD_TANH			9
#define CMD_STORE_R				10
#define CMD_STORE_Z				11
#define CMD_LOAD_R				12
#define CMD_STORE_H				13
#define CMD_STORE_LAYER			14
#define CMD_LOAD_LAYER			15
#define CMD_LOAD_Z				16
#define CMD_LOAD_H				17
#define CMD_LOAD_ZMINUS			18
#define CMD_INIT_H				19
#define CMD_EXP					20
#define CMD_ADD                 22
#define CMD_LOAD_TEST			26

void NN_Init(void);

void NN_LoadEmbedding(uint32_t addr);
void NN_LoadZEmbedding(uint32_t addr);
void NN_LoadBias(uint32_t bias_addr, uint32_t out_ch);
void NN_LoadWeight(uint32_t weight_addr, uint32_t in_ch, uint32_t out_ch, uint8_t sel_i, uint32_t compensation);

void NN_LoadResult(void);
void NN_Add(void);
void NN_StoreR(void);
void NN_StoreZ(void);
void NN_LoadR(void);
void NN_Exp(void);

void NN_LoadZ(void);
void NN_LoadH(uint32_t layer);
void NN_StoreH(uint32_t layer);
void NN_LoadZMinus(void);

void NN_LoadSigmoid(void);
void NN_LoadTanh(void);
void NN_InitH(uint32_t layer);


void NN_SDRAM_Load_test(void);
	




uint32_t NN_LoadData(void);
void NN_RandomSeed(uint32_t seed);


