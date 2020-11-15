



#include "timer.h"




// 
// 
//


uint8_t TimerIntFlag=0;

void TimerInit(void)
{
	// disable
	CMSDK_TIMER0->CTRL = 0;
	CMSDK_TIMER1->CTRL = 0;
	
	CMSDK_TIMER0->RELOAD = 400000;//1s
	CMSDK_TIMER1->RELOAD = 400000;
	
	// enable
	CMSDK_TIMER0->CTRL = 	//CMSDK_TIMER_CTRL_IRQEN_Msk		|
							//CMSDK_TIMER_CTRL_SELEXTCLK_Msk	|
							//CMSDK_TIMER_CTRL_SELEXTEN_Pos		|
							CMSDK_TIMER_CTRL_EN_Msk				;
	
	//CMSDK_TIMER1->CTRL = CMSDK_TIMER_CTRL_EN_Msk;
	
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void TimerEnable(void)
{
	uint32_t reg=0;
	
	reg = CMSDK_TIMER0->CTRL | 0x1;
	CMSDK_TIMER0->CTRL = reg;
}

void TimerDisable(void)
{
	uint32_t reg=0;
	
	reg = CMSDK_TIMER0->CTRL & 0xfffe;
	CMSDK_TIMER0->CTRL = reg;
}

void TimerIntEnable(void)
{
	uint32_t reg=0;
	
	reg = CMSDK_TIMER0->CTRL | 0x8;
	CMSDK_TIMER0->CTRL = reg;
}

void TimerIntDisable(void)
{
	uint32_t reg=0;
	
	reg = CMSDK_TIMER0->CTRL & 0xfff7;
	CMSDK_TIMER0->CTRL = reg;
}

void TimerSetValue(uint32_t value)
{
	CMSDK_TIMER0->RELOAD = value;
}

uint32_t TimerValueGet(void)
{
	return CMSDK_TIMER0->VALUE;
}

uint32_t Timer1ValueGet(void)
{
	return CMSDK_TIMER1->VALUE;
}

void delay_us(uint32_t n_us)
{
	CMSDK_TIMER0->CTRL = 0; 				// disable
	TimerIntFlag = 0;
	TimerSetValue(CLK_FREQ/1000000*n_us);		// set value
	CMSDK_TIMER0->CTRL = 9;					// enable 
	while(!TimerIntFlag);					// wait
	TimerIntFlag = 0;
	CMSDK_TIMER0->CTRL = 0; 				// disable
}

void delay_ms(uint32_t n_ms)
{
	CMSDK_TIMER0->CTRL = 0; 				// disable
	TimerIntFlag = 0;
	TimerSetValue(CLK_FREQ/1000*n_ms);		// set value
	CMSDK_TIMER0->CTRL = 9;					// enable 
	while(!TimerIntFlag);					// wait
	TimerIntFlag = 0;
	CMSDK_TIMER0->CTRL = 0; 				// disable
}



