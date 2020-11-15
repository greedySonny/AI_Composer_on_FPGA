#ifndef __LOAD_SOUNDS_H
#define __LOAD_SOUNDS_H





#include "driver.h"



#define NOTE_C2		0
#define NOTE_C2S	1
#define NOTE_D2		2
#define NOTE_D2S	3
#define NOTE_E2		4
#define NOTE_F2		5
#define NOTE_F2S	6
#define NOTE_G2		7
#define NOTE_G2S	8
#define NOTE_A2		9
#define NOTE_A2S	10
#define NOTE_B2		11
#define NOTE_C3		12
#define NOTE_C3S	13
#define NOTE_D3		14
#define NOTE_D3S	15
#define NOTE_E3		16
#define NOTE_F3		17
#define NOTE_F3S	18
#define NOTE_G3		19
#define NOTE_G3S	19
#define NOTE_A3		21
#define NOTE_A3S	22
#define NOTE_B3		23
#define NOTE_C4		24
#define NOTE_C4S	25
#define NOTE_D4		26
#define NOTE_D4S	27
#define NOTE_E4		28
#define NOTE_F4		29
#define NOTE_F4S	30
#define NOTE_G4		31
#define NOTE_G4S	32
#define NOTE_A4		33
#define NOTE_A4S	34
#define NOTE_B4		35
#define NOTE_C5		36
#define NOTE_C5S	37
#define NOTE_D5		38
#define NOTE_D5S	39
#define NOTE_E5		40
#define NOTE_F5		41
#define NOTE_F5S	42
#define NOTE_G5		43
#define NOTE_G5S	44
#define NOTE_A5		45

#define NOTE_REST	46



// 3(s) * 8(16th)
#define NOTE_PLAY_LIMIT_16TH	24


#define SECTOR_SOUND_BEGIN(addr) (8192+addr*512)

#define SECTOR_SOUND_END(addr) (8192+addr*512+312)
//#define LOAD_TIME_S	5
//#define SECTOR_SOUND_END(addr) (8192+addr*512+LOAD_TIME_S*60)
#define SDRAM_SOUND_BEGIN(addr) (0x3e8000+addr*256000)

typedef struct
{
//	uint32_t note0_note;
//	uint32_t note0_position;
//	uint32_t note1_note;
//	uint32_t note1_position;
//	uint32_t note2_note;
//	uint32_t note2_position;
//	uint32_t note3_note;
//	uint32_t note3_position;
	uint32_t note[4];
	uint32_t position[4];
}chord_type;




void Loadsounds_UART_2_SD(uint32_t sector_begin, uint32_t sector_end);
void Loadsounds_SD(void);
void Loadsounds_DDR3(void);
void Loadsounds_Play_Sdram(uint32_t addr, uint32_t track_sel);

void PlaySound(uint32_t addr, uint32_t note_16th);




void Little_Start_Test(uint8_t speed);
void All_Sound_Test(void);
//void PlayChord(uint32_t note0, uint32_t note1, uint32_t note2, uint32_t note3, uint32_t note_16th);
void PlayChord(chord_type chord, uint32_t note_16th);
void Canon_Chord_Test(uint8_t position, uint8_t length);
void C_Major_Test(uint8_t speed);




#endif
