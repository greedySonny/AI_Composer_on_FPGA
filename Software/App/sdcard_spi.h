





#include "driver.h"



void SD_Test(void);



void SD_Init(void);
uint8_t SD_Reset(void);
uint8_t SD_Initial(void);

uint8_t SD_write_sector(uint32_t address, uint8_t *Buffer);
uint8_t SD_read_sector(uint32_t address, uint8_t* buffer);

uint8_t SD_Check(void);
void SD_WriteParams(uint32_t sector_begin, uint32_t sector_end, uint32_t remain);




uint8_t Write_Cmd(uint8_t* p);
void SD_CS_Enable(void);
void SD_CS_Disable(void);
void SD_SendBytes(uint8_t* bytes, uint32_t length);
