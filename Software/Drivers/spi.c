

// xbq修改了Verilog ip，将tx闲置时拉高（好像没什么用，只是满足tf卡时序图而已）
// 可以参考https://www.nxp.com.cn/docs/en/application-note/AN10406.pdf



#include "spi.h"



void SSP_Init(PL022_SPI_TypeDef* SPIx, SSP_InitTypeDef* SSP_InitStruct)
{
	SPIx->CPSR = (SPIx->CPSR&0xFF00)|SSP_InitStruct->SSP_ClockPrescaleDivisor;//CPSR寄存器bit0-bit7有效
	
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

	SSP_InitStructure.SSP_ClockPrescaleDivisor = 2;//时钟分频因子
	//SSP_InitStructure.SSP_ClockPrescaleDivisor = 4;//时钟分频因子
	SSP_InitStructure.SSP_CPHA = SSPCLKOUT_Phase_High;//相位
	SSP_InitStructure.SSP_CPOL = SSPCLKOUT_Polarity_High;//极性
	SSP_InitStructure.SSP_DataSize = 7;//8bit数据帧格式
	SSP_InitStructure.SSP_FRF = SSP_MotorolaSPI;//motorola SPI模式
	SSP_InitStructure.SSP_LoopBackMode = SSPLBM_Disable;//正常接口
	SSP_InitStructure.SSP_Mode = SSPMode_Master;//主模式
	//SSP_InitStructure.SSP_SerialClockRate = 10;//时钟速率
	SSP_InitStructure.SSP_SerialClockRate = 1;//时钟速率
	SSP_InitStructure.SSP_SlaveOutDisable = SSPSlaveOutputDisable;//仅在从机模式下有效
	SSP_Init(PL022_SPI0,&SSP_InitStructure);//初始化SPI1
	
	SSP_CmdEnable(PL022_SPI0,1);//使能SPI1
	
	
#if SPI_RX_InterruptEnable
	
	SSP_ITConfig(PL022_SPI0,SSP_IT_RX,ENABLE);//SSP接收中断使能,发送中断使能
	NVIC_ClearPendingIRQ(SPI0_IRQn);//清除所有全局NVIC 挂起中断bit
	NVIC_EnableIRQ(SPI0_IRQn);//使能中断管理器中的中断
	
#endif
}

void SSP_CmdEnable(PL022_SPI_TypeDef* PL022_SPIx, uint32_t NewState)
{
	if(NewState!=0)
		PL022_SPIx->CR1 |= 0x0002;//使能
	else
		PL022_SPIx->CR1 &= 0xFFFD;//失能
}

uint8_t SSP_SendAndReceiveOneByte(PL022_SPI_TypeDef* PL022_SPIx, uint8_t byte)
{
	while((PL022_SPIx->SR & SSP_TransmitFIFOempty)==0);//等待发送缓冲区为空
	PL022_SPIx->DR = byte;//将发送字节写入发送缓冲区
	
	while((PL022_SPIx->SR & SSP_ReceiveFIFOnotEmpty)==0);//等待接收缓冲区不为空
	return PL022_SPIx->DR;//读出接收缓冲区中的数据
}


void SSP_SendNumberByte(PL022_SPI_TypeDef* PL022_SPIx, uint8_t *pBuff, uint16_t buffNumber)
{
	uint16_t i = 0; 
	uint8_t res = 0;
	
	while(i < buffNumber)
	{     //SPI发送FIFO 8字节深度,只要FIFO不满，就往里面写数据
		res = PL022_SPIx->SR;
		if((res& SSP_TransmitFIFOnotfull) == SSP_TransmitFIFOnotfull)
			PL022_SPIx->DR = pBuff[i++];//将发送字节写入发送缓冲区
	}
}	




