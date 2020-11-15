#ifndef __SPI_H
#define __SPI_H





#include "CMSDK_CM0.h"



/******************SSP�������ú궨����   CR0\CR1\CPSR�Ĵ���********************/
typedef struct
{
	uint8_t SSP_Mode;    				//SSP����ģʽ
	uint8_t SSP_FRF;					//SSP����֡��ʽ			                            
	uint8_t SSP_CPOL;      				//SSPCLKOUT ����   (��������motprpla SPI)                               
	uint8_t SSP_CPHA;   				//SSPCLKOUT ��λ   (��������motprpla SPI)  
	uint8_t SSP_SlaveOutDisable;		//�ڴ�ģʽ�£�����SSPTXD���		
	uint8_t	SSP_LoopBackMode;			//����ģʽʹ��
	uint8_t SSP_DataSize;  				//SSP֡���ݿ�� (4-32bit)   
	uint8_t SSP_SerialClockRate;		//SSP����ʱ������
	uint8_t SSP_ClockPrescaleDivisor;	//ʱ��Ԥ��Ƶ�ķ�Ƶ����

}SSP_InitTypeDef;

/*SSP����ģʽ����   SSP_Mode*/
#define SSPMode_Master				(uint8_t)0x00
#define SSPMode_Slave				(uint8_t)0x01

/*SSP֡��ʽ����     SSP_FRF*/
#define SSP_MotorolaSPI				(uint8_t)0x00
#define SSP_TISerial				(uint8_t)0x01			 
#define SSP_NationalMicrowire		(uint8_t)0x02

/*SSPCLKOUT��������  SSP_CPOL   ������Motorola SPI֡��ʽ*/
#define SSPCLKOUT_Polarity_High		(uint8_t)0x01		
#define SSPCLKOUT_Polarity_Low		(uint8_t)0x00		

/*SSPCLKOUT��λ����  SSP_CPHA   ������Motorola SPI֡��ʽ*/
#define SSPCLKOUT_Phase_High		(uint8_t)0x01		
#define SSPCLKOUT_Phase_Low			(uint8_t)0x00

/*SSP�ڴ�ģʽ�� SSPTXD�������  SSP_SlaveOutDisable*/
#define SSPSlaveOutputDisable 		(uint8_t)0x01
#define SSPSlaveOutputEnable 		(uint8_t)0x00

/*SSP����ģʽʹ��/����         SSP_LoopBackMode */
#define SSPLBM_Enable		 		(uint8_t)0x01
#define SSPLBM_Disable		 		(uint8_t)0x00


/*********************SSP����/���ͻ�����״̬�궨����   SSPSR�Ĵ���**************************/
#define SSP_BusyFlag				(uint8_t)0x10
#define SSP_ReceiveFIFOfull			(uint8_t)0x08
#define SSP_ReceiveFIFOnotEmpty		(uint8_t)0x04
#define SSP_TransmitFIFOnotfull		(uint8_t)0x02
#define SSP_TransmitFIFOempty		(uint8_t)0x01


//��ȡ�ж�״̬λ ԭ��״̬���������ж�״̬
#define SSP_ITStatusRIS				(uint8_t)0x00
#define SSP_ITStatusMIS				(uint8_t)0x01

/*************�жϺ궨����**************/
#define SSP_IT_TX				(uint8_t)0x08//����FIFO��ջ����ʱ���ж�
#define SSP_IT_RX				(uint8_t)0x04//����FIFO���������ʱ���ж�
#define SSP_IT_RT				(uint8_t)0x02//���ճ�ʱ�ж�
#define SSP_IT_ROR				(uint8_t)0x01//��������ж�

#define SPI_RX_InterruptEnable  0//ʹ���жϽ���




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
