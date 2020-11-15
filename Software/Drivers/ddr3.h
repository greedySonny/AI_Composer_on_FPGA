#ifndef __DDR3_H
#define __DDR3_H



#include "CMSDK_CM0.h"

#define DDR3_MIDI_CTRL	4
#define DDR3_NN_CTRL	2
#define DDR3_AHB_CTRL	0



void ddr3_init(void);
void ddr3_rdy(void);

void ddr3_WriteBytes(uint32_t addr, uint32_t* buffer, uint32_t length);
void ddr3_ReadBytes(uint32_t addr, uint32_t *buffer, uint32_t length);

void ddr3_WriteEightWords(uint32_t addr, uint32_t* buffer);
void ddr3_ReadEightWords(uint32_t addr, uint32_t* buffer);

void ddr3_Ctrl(uint32_t role);


#endif

