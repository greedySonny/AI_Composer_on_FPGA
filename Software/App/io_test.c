



#include "io_test.h"





void GPIO_Test(void)
{
	uint32_t read;
	
	LEDSet(0x02);
	LEDSet(0x01);
	
	read = SWRead();
	LEDSet(read);
}


