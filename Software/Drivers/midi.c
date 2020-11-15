





#include "midi.h"

// CR   [0]: rd en
//      [1]: int en
//      [2]: back_mode en
//      [3]: mute en
//      [5:4] volume
void MIDI_Init(uint8_t mode, uint8_t volume)
{

	MIDI->CR = 0x1;	// normal mode
	//MIDI->CR = 0x4;	// back mode test
	//MIDI->CR |= (MIDI->CR & 0xfff0) | (mode<<0);
	//MIDI->CR = 0x8;	// mute
	
	MIDI->CR = (MIDI->CR & 0xff0f) | (volume<<4);	// volume : 1
	
	MIDI->CMD = 1; // reset
}



void MIDI_LoadNote(void)
{



}

uint32_t MIDI_FIFO_Half(void)
{
	if(MIDI->SR & 0x10)
		return 1;
	else
		return 0;
}




