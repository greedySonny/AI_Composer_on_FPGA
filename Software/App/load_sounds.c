




#include "load_sounds.h"
#include "load_params.h"







// 通过UART向SD卡写参数。一次通过uart收512byte，然后写一个扇区
void Loadsounds_UART_2_SD(uint32_t sector_begin, uint32_t sector_end)
{
	uint32_t i, j;
	uint8_t uart_rx_buffer_flag=0;
	uint8_t uart_rxdata=0;
	uint8_t uart_rx_buffer[512]={0};
	uint32_t datalength=0;
	uint8_t sel = 0;
	uint8_t rddata=0;
	
	printf("start to write sector %d - %d\r\n",sector_begin,sector_end);

	while(!uart_rx_buffer_flag){
		//uart_rxdata = Uart1Getc();			// 等待串口数据
		do{
			Uart1Putc('R');
			Uart1Putc('\n');
			rddata = Uart1GetC_Timeout(1000);
		}while(rddata==0xff);
		if(uart_rxdata==0x00){				// 判断读入内容
			uart_rxdata = Uart1Getc();		// 等待并判断第二个字节
			
			if(uart_rxdata=='$'){
				
				uart_rxdata = Uart1Getc();	// 读取datalength首字节
				datalength |= ((uint32_t)uart_rxdata)<<16;
				uart_rxdata = Uart1Getc();	// 读取datalength第二字节
				datalength |= ((uint32_t)uart_rxdata)<<8;
				uart_rxdata = Uart1Getc();	// 读取datalength低字节
				datalength |= (uint32_t)uart_rxdata;
				sel = Uart1Getc();			// 读取写入编号
				uart_rxdata = Uart1Getc();	// 等待结束符
				if(uart_rxdata=='\n'){
					
					uart_rx_buffer_flag = 1;// 识别到上位机。准备进入读数据状态。
					Uart1Putc('r');
					Uart1Putc('\n');
				}
			}else{printf("--%c\r\n",uart_rxdata);}
		}else{printf("-%c\r\n",uart_rxdata);}
	}
	
	printf("header ok\r\n");
	
	
	for(j=sector_begin;j<=sector_end;j++){

		uart_rxdata = 0;
		// receive 512 bytes from uart
		for(i=0;i<256;i++){
			uart_rxdata = Uart1Getc();
			uart_rx_buffer[i<<1] = uart_rxdata;
			uart_rxdata = Uart1Getc();
			uart_rx_buffer[(i<<1)+1] = uart_rxdata;
			uart_rxdata = Uart1Getc();
			if(uart_rxdata!='\n') {
				printf("Error! sector: %d, index: %d, value: %x\r\n", j, i, uart_rxdata);
			}
		}
		printf("sector: %d uart recv ok, preper to write tf card\r\n", j);
		
//		// 读buffer，检查
//		for(i=0;i<512;i++){
//			printf("%d-%x\t", i, uart_rx_buffer[i]);
//		}

		// 写扇区
		SD_write_sector(j, uart_rx_buffer);
		printf("sector %d ok!\r\n", j);
		Uart1Putc(j);
		Uart1Putc('\n');
	}
	
	// ack
	Uart1Putc('v');
	Uart1Putc('\n');
	printf("sector %d - %d write ok\r\n",sector_begin,sector_end);

}	



void Loadsounds_DDR3(void)
{
	uint32_t i;
	for(i=0;i<47;i++)
	{
	/* -------------- 读取参数从SD卡到SDRAM -------------- */
	
		Loadparams_SD_2_DDR3(SECTOR_SOUND_BEGIN(i),				SECTOR_SOUND_END(i),				SDRAM_SOUND_BEGIN(i));		
	}
	
	
}


#define CMD_MIDI_NONE			0
#define CMD_MIDI_RESET 			1
#define CMD_MIDI_LOADSOUND		2





void PlaySound(uint32_t addr, uint32_t note_16th)
{
	uint32_t time=0;
	while(time<=(note_16th*4))  
	{
		if((time>=4*8*4) && (MIDI->SR & 0x10))	// 4(s) x 8(16th) x 4(times)
		{
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),0);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),1);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),2);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),3);
			time++;
		}
		else if((time<4*8*4) && (MIDI->SR & 0x10)){// fifo less than half
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(addr)+time*1024, 0); 
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(addr)+time*1024, 1);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(addr)+time*1024, 2);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(addr)+time*1024, 3);
			time++;
		}
	}		
}


// 1s 	8x16th		16000samples	32000Bytes		about 32 times
// 1/8s 1x16th		2000samples		4000Bytes		about 4 times
void PlayChord(chord_type chord, uint32_t note_16th)
{
	uint32_t time=0;
	uint32_t i=0;
	while(time<note_16th*4)  // 
	{
		if((time>= NOTE_PLAY_LIMIT_16TH * 4) && (MIDI->SR & 0x10))	// 3(s) x 8(16th) x 4(times)
		{
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),0);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),1);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),2);
			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST),3);
			//printf("%d\r\n",time);
			time++;
		}
		else if((time< NOTE_PLAY_LIMIT_16TH * 4) && (MIDI->SR & 0x10)){// fifo less than half
			for(i=0;i<4;i++)
			{
				if(chord.note[i]==20) chord.note[i]=19;
				if(chord.position[i]>=NOTE_PLAY_LIMIT_16TH) 
					Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_REST), i); 
				else 
					Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(chord.note[i])+ 4096*chord.position[i]+ time*1024, i); 
			}
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(chord.note[0])+ 4096*chord.position[0]+ time*1024, 0); 
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(chord.note[1])+ 4096*chord.position[1]+ time*1024, 1);
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(chord.note[2])+ 4096*chord.position[2]+ time*1024, 2);
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(chord.note[3])+ 4096*chord.position[3]+ time*1024, 3);
			//printf("%d\r\n",time);
			time++;
		}
	}		
}

void Loadsounds_Play_Sdram(uint32_t addr, uint32_t track_sel)
{
	MIDI->SOUNDADDR = addr;
	MIDI->TRACK_SEL = track_sel;
	MIDI->CMD = CMD_MIDI_LOADSOUND;
	
	while(MIDI->SR & 0xf);	// 等待指令执行完成
}

void Loadsounds_SD(void)
{
	uint32_t i;
	
	for(i=40;i<46;i++)
	{
	/* -------------- 读取参数从UART卡到SD卡 -------------- */
	
		Loadsounds_UART_2_SD(SECTOR_SOUND_BEGIN(i), SECTOR_SOUND_END(i));
	}
	
	Loadsounds_UART_2_SD(SECTOR_SOUND_BEGIN(NOTE_REST), SECTOR_SOUND_END(NOTE_REST)); // rest
	//Loadsounds_UART_2_SD(SECTOR_SOUND_BEGIN(NOTE_G3S), SECTOR_SOUND_END(NOTE_G3S)); // rest
	
	
}

void C_Major_Test(uint8_t speed)
{
	PlaySound(NOTE_C3,speed);
	PlaySound(NOTE_D3,speed);
	PlaySound(NOTE_E3,speed);
	PlaySound(NOTE_F3,speed);
	PlaySound(NOTE_G3,speed);
	PlaySound(NOTE_A3,speed);
	PlaySound(NOTE_B3,speed);
	
}
void Little_Start_Test(uint8_t speed)
{
	PlaySound(NOTE_C3,speed);
	PlaySound(NOTE_C3,speed);
	PlaySound(NOTE_G3,speed);
	PlaySound(NOTE_G3,speed);
	PlaySound(NOTE_A3,speed);
	PlaySound(NOTE_A3,speed);
	PlaySound(NOTE_G3,speed);
	
	PlaySound(NOTE_REST,speed);
	
	PlaySound(NOTE_F3,speed);
	PlaySound(NOTE_F3,speed);
	PlaySound(NOTE_E3,speed);
	PlaySound(NOTE_E3,speed);
	PlaySound(NOTE_D3,speed);
	PlaySound(NOTE_D3,speed);
	PlaySound(NOTE_C3,speed);
	
	PlaySound(NOTE_REST,speed);
}

void All_Sound_Test(void)
{
	uint8_t i;
	
//	for(i=NOTE_C2;i<=NOTE_B4;i++)
//	{
//		printf("prepare: %d\r\n", i);
//		PlaySound(NOTE_REST,4);
//		PlaySound(i,1);
//		PlaySound(i,1);
//		PlaySound(NOTE_REST,1);
//		PlaySound(i,1);
//		PlaySound(NOTE_REST,1);
//	}
	
	chord_type chord = {{NOTE_C2,NOTE_C2,NOTE_C2,NOTE_C2},{0,0,0,0}};
	for(i=NOTE_C2;i<=NOTE_A5;i++)
	{
		printf("prepare: %d\r\n", i);
		PlaySound(NOTE_REST,4);
		PlayChord(chord,8);
		PlaySound(NOTE_REST,4);
		PlayChord(chord,4);
		PlaySound(NOTE_REST,4);
		PlayChord(chord,16);
		chord.note[0]++;
		chord.note[1]++;
		chord.note[2]++;
		chord.note[3]++;
	}
}

void Canon_Chord_Test(uint8_t position, uint8_t length)
{
	chord_type chord[8] = {
		
//		{NOTE_F3S, position,	NOTE_D3,  position,		NOTE_B2,  position,		NOTE_REST, position},
//		{NOTE_F3S, position,	NOTE_C3S, position,		NOTE_A2,  position,		NOTE_REST, position},
//		{NOTE_D3,  position,	NOTE_B2,  position,		NOTE_G2,  position,		NOTE_REST, position},
//		{NOTE_D3,  position,	NOTE_B2,  position,		NOTE_F2S, position,		NOTE_REST, position},
//		{NOTE_B2,  position,	NOTE_A2,  position,		NOTE_E2,  position,		NOTE_REST, position},
//		{NOTE_B2,  position,	NOTE_G2,  position,		NOTE_D2,  position,		NOTE_REST, position},
//		{NOTE_G2,  position,	NOTE_E2,  position,		NOTE_C2S, position,		NOTE_REST, position},
//		{NOTE_C3S, position,	NOTE_A2,  position,		NOTE_F2S, position,		NOTE_REST, position}

		{{NOTE_F3S, NOTE_D3,  NOTE_B2,  NOTE_REST}, {position,position,position,position}},
		{{NOTE_F3S, NOTE_C3S, NOTE_A2,  NOTE_REST}, {position,position,position,position}},
		{{NOTE_D3,  NOTE_B2,  NOTE_G2,  NOTE_REST}, {position,position,position,position}},
		{{NOTE_D3,  NOTE_B2,  NOTE_F2S, NOTE_REST}, {position,position,position,position}},
		{{NOTE_B2,  NOTE_A2,  NOTE_E2,  NOTE_REST}, {position,position,position,position}},
		{{NOTE_B2,  NOTE_G2,  NOTE_D2,  NOTE_REST}, {position,position,position,position}},
		{{NOTE_G2,  NOTE_E2,  NOTE_C2S, NOTE_REST}, {position,position,position,position}},
		{{NOTE_C3S, NOTE_A2,  NOTE_F2S, NOTE_REST}, {position,position,position,position}}
	};

//	PlayChord(NOTE_F3S, 	NOTE_D3, 	NOTE_B2, 	NOTE_REST, 	2);
//	PlayChord(NOTE_F3S, 	NOTE_C3S, 	NOTE_A2, 	NOTE_REST, 	2);
//	PlayChord(NOTE_D3, 		NOTE_B2, 	NOTE_G2, 	NOTE_REST, 	2);
//	PlayChord(NOTE_D3, 		NOTE_B2, 	NOTE_F2S, 	NOTE_REST, 	2);
//	PlayChord(NOTE_B2, 		NOTE_A2, 	NOTE_E2, 	NOTE_REST, 	2);
//	PlayChord(NOTE_B2, 		NOTE_G2, 	NOTE_D2, 	NOTE_REST, 	2);
//	PlayChord(NOTE_G2, 		NOTE_E2, 	NOTE_C2S, 	NOTE_REST, 	2);
//	PlayChord(NOTE_C3S, 	NOTE_A2, 	NOTE_F2S, 	NOTE_REST, 	2);
	PlayChord(chord[0], 	length);
	PlayChord(chord[1], 	length);
	PlayChord(chord[2], 	length);
	PlayChord(chord[3], 	length);
	PlayChord(chord[4], 	length);
	PlayChord(chord[5], 	length);
	PlayChord(chord[6], 	length);
	PlayChord(chord[7], 	length);
}

