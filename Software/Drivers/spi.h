#ifndef __SPI_H
#define __SPI_H





#include "CMSDK_CM0.h"



/******************SSP参数配置宏定义区   CR0\CR1\CPSR寄存器********************/
typedef struct
{
	uint8_t SSP_Mode;    				//SSP主从模式
	uint8_t SSP_FRF;					//SSP数据帧格式			                            
	uint8_t SSP_CPOL;      				//SSPCLKOUT 极性   (仅适用于motprpla SPI)                               
	uint8_t SSP_CPHA;   				//SSPCLKOUT 相位   (仅适用于motprpla SPI)  
	uint8_t SSP_SlaveOutDisable;		//在从模式下，禁用SSPTXD输出		
	uint8_t	SSP_LoopBackMode;			//环回模式使能
	uint8_t SSP_DataSize;  				//SSP帧数据宽度 (4-32bit)   
	uint8_t SSP_SerialClockRate;		//SSP串行时钟速率
	uint8_t SSP_ClockPrescaleDivisor;	//时钟预分频的分频因子

}SSP_InitTypeDef;

/*SSP主从模式定义   SSP_Mode*/
#define SSPMode_Master				(uint8_t)0x00
#define SSPMode_Slave				(uint8_t)0x01

/*SSP帧格式定义     SSP_FRF*/
#define SSP_MotorolaSPI				(uint8_t)0x00
#define SSP_TISerial				(uint8_t)0x01			 
#define SSP_NationalMicrowire		(uint8_t)0x02

/*SSPCLKOUT极性配置  SSP_CPOL   仅用于Motorola SPI帧格式*/
#define SSPCLKOUT_Polarity_High		(uint8_t)0x01		
#define SSPCLKOUT_Polarity_Low		(uint8_t)0x00		

/*SSPCLKOUT相位配置  SSP_CPHA   仅用于Motorola SPI帧格式*/
#define SSPCLKOUT_Phase_High		(uint8_t)0x01		
#define SSPCLKOUT_Phase_Low			(uint8_t)0x00

/*SSP在从模式下 SSPTXD输出禁用  SSP_SlaveOutDisable*/
#define SSPSlaveOutputDisable 		(uint8_t)0x01
#define SSPSlaveOutputEnable 		(uint8_t)0x00

/*SSP环回模式使能/禁用         SSP_LoopBackMode */
#define SSPLBM_Enable		 		(uint8_t)0x01
#define SSPLBM_Disable		 		(uint8_t)0x00


/*********************SSP接收/发送缓冲区状态宏定义区   SSPSR寄存器**************************/
#define SSP_BusyFlag				(uint8_t)0x10
#define SSP_ReceiveFIFOfull			(uint8_t)0x08
#define SSP_ReceiveFIFOnotEmpty		(uint8_t)0x04
#define SSP_TransmitFIFOnotfull		(uint8_t)0x02
#define SSP_TransmitFIFOempty		(uint8_t)0x01


//获取中断状态位 原生状态还是屏蔽中断状态
#define SSP_ITStatusRIS				(uint8_t)0x00
#define SSP_ITStatusMIS				(uint8_t)0x01

/*************中断宏定义区**************/
#define SSP_IT_TX				(uint8_t)0x08//发送FIFO半空或更少时的中断
#define SSP_IT_RX				(uint8_t)0x04//接收FIFO半满或更少时的中断
#define SSP_IT_RT				(uint8_t)0x02//接收超时中断
#define SSP_IT_ROR				(uint8_t)0x01//接收溢出中断

#define SPI_RX_InterruptEnable  0//使能中断接收




extern volatile uint8_t SPI_RX_BUFF[200];
extern volatile uint16_t SPI_RX_STA;

extern uint8_t SPI_TX_BUFF[200];
extern uint8_t SPI_TX_Length;


void SSP_Init(PL022_SPI_TypeDef* SPIx, SSP_InitTypeDef* SSP_InitStruct);
void SSP0_Init(void);

void SSP_CmdEnable(PL022_SPI_TypeDef* PL022_SPIx, uint32_t NewState);
uint8_t SSP_SendAndReceiveOneByte(PL022_SPI_TypeDef* PL022_SPIx, uint8_t byte);
void SSP_SendNumberByte(PL022_SPI_TypeDef* PL022_SPIx, uint8_t *pBuff, uint16_t buffNumber);


#endif
