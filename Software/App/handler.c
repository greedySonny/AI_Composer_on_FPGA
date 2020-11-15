



#include "driver.h"


extern uint8_t TimerIntFlag;
extern uint8_t UartIntFlag;
extern uint8_t NNIntFlag;


void TIMER0_Handler(void)
{	
	CMSDK_TIMER0->INTCLEAR = 1;
	TimerIntFlag = 1;
}



void UART0_Handler(void){
	
	UART_ClearITFlag();
	UartIntFlag = 1;

}


void NN_Handler(void){
	NN_CTRL->INT = 1;
	NNIntFlag = 1;
	//Uart0Putc('i');Uart0Putc('n');Uart0Putc('t');Uart0Putc('\n');
}
