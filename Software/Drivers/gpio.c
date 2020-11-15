

#include "gpio.h"


void GPIO_Init(void)
{
	
	CMSDK_GPIO0->ALTFUNCCLR = ((uint32_t)0xffff);	// 清除对应的复用功能
	//CMSDK_GPIO0->ALTFUNCSET |= (uint32_t)0xc0;	// 设置复用UART1
	CMSDK_GPIO0->OUTENABLESET = (uint32_t)0xff00; // [15:8] LED, [7:0] SW
	
	
//	// 设置拨码开关上升沿触发中断
//	CMSDK_GPIO0->INTENCLR = ((uint32_t)0xffff);
//	CMSDK_GPIO0->INTENSET = ((uint32_t)0xf000);	
//	CMSDK_GPIO0->INTTYPESET |= ((uint32_t)0xf000);	
//	CMSDK_GPIO0->INTPOLSET |= ((uint32_t)0xf000);	
//	
//	NVIC_ClearPendingIRQ(SW11_IRQn);
//	NVIC_EnableIRQ(SW11_IRQn);
//	NVIC_ClearPendingIRQ(SW10_IRQn);
//	NVIC_EnableIRQ(SW10_IRQn);
//	NVIC_ClearPendingIRQ(SW9_IRQn);
//	NVIC_EnableIRQ(SW9_IRQn);
//	NVIC_ClearPendingIRQ(SW8_IRQn);
//	NVIC_EnableIRQ(SW8_IRQn);
}



void GPIO_SetBits(CMSDK_GPIO_TypeDef* CMSDK_GPIOx, uint16_t GPIO_Pinn)
{
	CMSDK_GPIOx->DATAOUT |= GPIO_Pinn;
}

void GPIO_ResetBits(CMSDK_GPIO_TypeDef* CMSDK_GPIOx, uint16_t GPIO_Pinn)
{
	CMSDK_GPIOx->DATAOUT &= (~GPIO_Pinn);
}


uint16_t GPIO_ReadInput(CMSDK_GPIO_TypeDef* CMSDK_GPIOx)
{
	return CMSDK_GPIOx->DATA;
}

void LEDSet(uint8_t data)
{
	uint32_t led = 0;
	
	led = (data<<8)&0xff00;
	CMSDK_GPIO0->DATAOUT = ((CMSDK_GPIO0->DATAOUT) & 0xff) | led;
}

void LEDToggle(void)
{
	uint32_t led=0;
	
	led = CMSDK_GPIO0->DATA;
	
	CMSDK_GPIO0->DATAOUT = ((~led)&0xff00) | (led&0xff) ;
}

uint32_t LEDRead(void)
{
	uint32_t rdata;
	
	rdata = CMSDK_LED->LEDOUT;
	return rdata;
}

uint8_t SWRead(void)
{
	return CMSDK_GPIO0->DATA;
}