#ifndef __GPIO_H
#define __GPIO_H



#include "driver.h"

void GPIO_Init(void);


void GPIO_SetBits(CMSDK_GPIO_TypeDef* CMSDK_GPIOx, uint16_t GPIO_Pinn);
void GPIO_ResetBits(CMSDK_GPIO_TypeDef* CMSDK_GPIOx, uint16_t GPIO_Pinn);


uint16_t GPIO_ReadInput(CMSDK_GPIO_TypeDef* CMSDK_GPIOx);


void LEDSet(uint8_t data);
void LEDToggle(void);
uint32_t LEDRead(void);
uint8_t SWRead(void);



#endif

