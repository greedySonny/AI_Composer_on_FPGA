

// xbq�޸���Verilog ip����tx����ʱ���ߣ�����ûʲô�ã�ֻ������tf��ʱ��ͼ���ѣ�
// ���Բο�https://www.nxp.com.cn/docs/en/application-note/AN10406.pdf



#include "spi.h"



void SSP_Init(PL022_SPI_TypeDef* SPIx, SSP_InitTypeDef* SSP_InitStruct)
{
	SPIx->CPSR = (SPIx->CPSR&0xFF00)|SSP_InitStruct->SSP_ClockPrescaleDivisor;//CPSR�Ĵ���bit0-bit7��Ч
	
	SPIx->CR0 = (SSP_InitStruct->SSP_SerialClockRate<<8)|
				(SSP_InitStruct->SSP_CPHA<<7)|
				(SSP_InitStruct->SSP_CPOL<<6)|
				(SSP_InitStruct->SSP_FRF<<4)|
				(SSP_InitStruct->SSP_DataSize<<0);  //CR0[3:0] Data Size Select:
	
	SPIx->CR1 = (SPIx->CR1&0xFFF0)|
				(SSP_InitStruct->SSP_SlaveOutDisable<<3)|
				(SSP_InitStruct->SSP_Mode<<2)|
				(SSP_InitStruct->SSP_LoopBackMode<<0);
}

void SSP0_Init(void)
{
	SSP_InitTypeDef SSP_InitStructure;

	SSP_InitStructure.SSP_ClockPrescaleDivisor = 2;//ʱ�ӷ�Ƶ����
	//SSP_InitStructure.SSP_ClockPrescaleDivisor = 4;//ʱ�ӷ�Ƶ����
	SSP_InitStructure.SSP_CPHA = SSPCLKOUT_Phase_High;//��λ
	SSP_InitStructure.SSP_CPOL = SSPCLKOUT_Polarity_High;//����
	SSP_InitStructure.SSP_DataSize = 7;//8bit����֡��ʽ
	SSP_InitStructure.SSP_FRF = SSP_MotorolaSPI;//motorola SPIģʽ
	SSP_InitStructure.SSP_LoopBackMode = SSPLBM_Disable;//�����ӿ�
	SSP_InitStructure.SSP_Mode = SSPMode_Master;//��ģʽ
	//SSP_InitStructure.SSP_SerialClockRate = 10;//ʱ������
	SSP_InitStructure.SSP_SerialClockRate = 1;//ʱ������
	SSP_InitStructure.SSP_SlaveOutDisable = SSPSlaveOutputDisable;//���ڴӻ�ģʽ����Ч
	SSP_Init(PL022_SPI0,&SSP_InitStructure);//��ʼ��SPI1
	
	SSP_CmdEnable(PL022_SPI0,1);//ʹ��SPI1
	
	
#if SPI_RX_InterruptEnable
	
	SSP_ITConfig(PL022_SPI0,SSP_IT_RX,ENABLE);//SSP�����ж�ʹ��,�����ж�ʹ��
	NVIC_ClearPendingIRQ(SPI0_IRQn);//�������ȫ��NVIC �����ж�bit
	NVIC_EnableIRQ(SPI0_IRQn);//ʹ���жϹ������е��ж�
	
#endif
}

void SSP_CmdEnable(PL022_SPI_TypeDef* PL022_SPIx, uint32_t NewState)
{
	if(NewState!=0)
		PL022_SPIx->CR1 |= 0x0002;//ʹ��
	else
		PL022_SPIx->CR1 &= 0xFFFD;//ʧ��
}

uint8_t SSP_SendAndReceiveOneByte(PL022_SPI_TypeDef* PL022_SPIx, uint8_t byte)
{
	while((PL022_SPIx->SR & SSP_TransmitFIFOempty)==0);//�ȴ����ͻ�����Ϊ��
	PL022_SPIx->DR = byte;//�������ֽ�д�뷢�ͻ�����
	
	while((PL022_SPIx->SR & SSP_ReceiveFIFOnotEmpty)==0);//�ȴ����ջ�������Ϊ��
	return PL022_SPIx->DR;//�������ջ������е�����
}


void SSP_SendNumberByte(PL022_SPI_TypeDef* PL022_SPIx, uint8_t *pBuff, uint16_t buffNumber)
{
	uint16_t i = 0; 
	uint8_t res = 0;
	
	while(i < buffNumber)
	{     //SPI����FIFO 8�ֽ����,ֻҪFIFO��������������д����
		res = PL022_SPIx->SR;
		if((res& SSP_TransmitFIFOnotfull) == SSP_TransmitFIFOnotfull)
			PL022_SPIx->DR = pBuff[i++];//�������ֽ�д�뷢�ͻ�����
	}
}	




