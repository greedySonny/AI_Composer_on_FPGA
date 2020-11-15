


#include "ddr3.h"




void ddr3_init(void)
{
	
	while(!(DDR3CTRL->SR & 0x1));
}

void ddr3_rdy(void)
{
	while(!(DDR3CTRL->SR & 0x2));
}

void ddr3_WriteEightWords(uint32_t addr, uint32_t* buffer)
{
	uint32_t* data = (uint32_t*) (DDR3_BASE+addr);

	*data = *buffer; buffer++; 
	*data = *buffer; buffer++; 
	*data = *buffer; buffer++; 
	*data = *buffer; buffer++; 
	*data = *buffer; buffer++; 
	*data = *buffer; buffer++;
	*data = *buffer; buffer++; 
	*data = *buffer; 
}

void ddr3_ReadEightWords(uint32_t addr, uint32_t* buffer)
{
	
	uint32_t* data = (uint32_t*) (DDR3_BASE+addr);

	*buffer = *data; buffer++;
	*buffer = *data; buffer++;
	*buffer = *data; buffer++;
	*buffer = *data; buffer++;
	*buffer = *data; buffer++;
	*buffer = *data; buffer++;
	*buffer = *data; buffer++;
	*buffer = *data; 
}

// at least 8 bytes for once writing
void ddr3_WriteBytes(uint32_t addr, uint32_t* buffer, uint32_t length)
{
	uint32_t i=0;
	
	for(i=0;i<(length/8);i++){
		ddr3_WriteEightWords(addr+i*32, buffer+i*8);
	}
}

// at least 8 bytes for once reading
void ddr3_ReadBytes(uint32_t addr, uint32_t *buffer, uint32_t length)
{
	uint32_t i=0;

	
	for(i=0;i<(length/8);i++){
		ddr3_ReadEightWords(addr+i*32, buffer+i*8);
		
	}
}

void ddr3_Ctrl(uint32_t role)
{
	DDR3CTRL->CR = role;
}

