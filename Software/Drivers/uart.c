







#include "uart.h"


uint8_t UartIntFlag = 0;


void UartStdOutInit(void)
{
	CMSDK_UART0->BAUDDIV = 347; //40MHz
	CMSDK_UART1->BAUDDIV = 347; //40MHz
//	CMSDK_UART0->BAUDDIV = 336; //38.75MHz
//	CMSDK_UART1->BAUDDIV = 336; //38.75MHz
#ifdef SIMULATION
	CMSDK_UART0->BAUDDIV = 1;
	CMSDK_UART1->BAUDDIV = 1;
#endif
	  
//  CMSDK_UART0->CTRL    = 0x41; // High speed test mode, TX only

	CMSDK_UART0->CTRL    =  //CMSDK_UART_CTRL_HSTM_Msk		|
							CMSDK_UART_CTRL_RXORIRQEN_Msk	|
							//CMSDK_UART_CTRL_TXORIRQEN_Msk	|
							CMSDK_UART_CTRL_RXIRQEN_Msk 	|
							//CMSDK_UART_CTRL_TXIRQEN_Msk	|
							CMSDK_UART_CTRL_RXEN_Msk		|
							CMSDK_UART_CTRL_TXEN_Msk;
	
	CMSDK_UART1->CTRL = 0x3;
	
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
	
	
	
}

void UART_ClearITFlag(void)
{
	CMSDK_UART0->INTCLEAR = 0x02;
}


// Output a character
unsigned char Uart0Putc(unsigned char my_ch)
{
  while ((CMSDK_UART0->STATE & 1)); // Wait if Transmit Holding register is full
  CMSDK_UART0->DATA = my_ch; // write to transmit holding register
  return (my_ch);
}

unsigned char Uart1Putc(unsigned char my_ch)
{
  while ((CMSDK_UART1->STATE & 1)); // Wait if Transmit Holding register is full
  CMSDK_UART1->DATA = my_ch; // write to transmit holding register
  return (my_ch);
}

// Get a character
unsigned char Uart0Getc(void)
{
  while ((CMSDK_UART0->STATE & 2)==0); // Wait if Receive Holding register is empty
  return (CMSDK_UART0->DATA);
}

// Get a character
unsigned char Uart1Getc(void)
{
  while ((CMSDK_UART1->STATE & 2)==0); // Wait if Receive Holding register is empty
  return (CMSDK_UART1->DATA);
}


#pragma import(__use_no_semihosting_swi)

void _sys_exit(int x) 
{ 
	x = x; 
} 

struct __FILE
{
	int handle;
};

FILE __stdout;
FILE __stdin;


int fputc(int ch, FILE *f)
{
	return (Uart0Putc(ch));
}


int fgetc(FILE *f)
{
  return Uart0Getc();
}

int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
  Uart0Putc (ch);
}





