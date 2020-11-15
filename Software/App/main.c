


#include "driver.h"
#include "ddr3_test.h"
#include "sdcard_spi.h"
#include "load_params.h"
#include "io_test.h"
#include "nn_test.h"
#include "load_sounds.h"

//#define STORE_PARAMS_TO_TF
//#define STORE_SOUNDS_TO_TF
#define STORE_PARAMS_TO_SDRAM
#define STORE_SOUNDS_TO_SDRAM


extern uint8_t NNIntFlag;

typedef struct
{
	uint32_t note;
	uint32_t tone;
}token_type;
token_type token;

int main()
{
	uint32_t pos_dict[5] = {0,0,0,0,0};
	uint32_t inst_conv_dict[5] = {0,1,4,2,3};
	uint8_t nn_run_flag=0;
	chord_type chord = {{NOTE_REST,NOTE_REST,NOTE_REST,NOTE_REST},
						{0,0,0,0}};
	token_type current_token_dict[5] = {{0,7},{0,7},{0,7},{0,7},{0,7}};
	uint32_t qual[6]={0,1,2,3,4,5}; // major, minor, diminished, augmented, other, none
	uint32_t inst=0, next_inst=0;
	uint32_t index=0, z=0, x = 0;
	uint32_t i=0;
	
	/* -------------- 初始化 -------------- */
	UartStdOutInit();
	TimerInit();
	GPIO_Init();
	SSP0_Init();
	GPIO_Test();
	MIDI_Init(0x4,0x3); // back mode
	//MIDI_Init(0x1,0x1); // normal mode

	printf("Hi\r\n");
	//Uart1Putc('-');Uart1Putc('\n');
	
	ddr3_init(); // 等待校准
	ddr3_rdy();
	//ddr3_test();	// ahb ddr3 test
	//DDR3CTRL->CR = 0x2; // 控制权给nn
	//NN_SDRAM_Load_test();
	DDR3CTRL->CR = 0x0; // 控制权给ahb

///* -------------- 读取参数从UART卡到SD卡 -------------- */
//#ifdef STORE_PARAMS_TO_TF
//#ifndef SIMULATION
//	SD_Init();
//	
//	//Loadparams_SD();
//	//printf("Loadparams_SD ok!\r\n");
//#endif
//#endif
/* -------------- 读取声音从UART卡到SD卡 -------------- */
#ifdef STORE_SOUNDS_TO_TF
#ifndef SIMULATION
	SD_Init();
	Loadsounds_SD();
	//Loadsounds_UART_2_SD(SECTOR_SOUND_BEGIN(0), SECTOR_SOUND_END(0));
	printf("Loadsounds_SD ok!\r\n");
#endif
#endif
/* -------------- 读取参数从SD卡到DDR3 SDRAM -------------- */
#ifdef STORE_PARAMS_TO_SDRAM
#ifndef SIMULATION
	SD_Init(); // 不重新初始化的话，写完再读会卡住。不知道什么原因。
	//Loadparams_SD_2_UART(SECTOR_EMBEDDING_WEIGHT_BEGIN, SECTOR_EMBEDDING_WEIGHT_BEGIN, 0); // 读扇区，验证
	Loadparams_DDR3();
	printf("Loadparams_DDR3 ok!\r\n");
	//Loadparams_SD_2_DDR3(SECTOR_EMBEDDING_WEIGHT_BEGIN, SECTOR_EMBEDDING_WEIGHT_END, SDRAM_ADDR_EMBEDDING_WEIGHT);	
	// 读DDR3，验证
	//Loadparams_DDR3_2_UART(SDRAM_ADDR_EMBEDDING_WEIGHT,512);
#endif
#endif
/* -------------- 读取声音从SD卡到DDR3 SDRAM -------------- */
#ifdef STORE_SOUNDS_TO_SDRAM
#ifndef SIMULATION
	SD_Init(); 
	//Loadparams_SD_2_UART(SECTOR_SOUND_BEGIN(NOTE_G3S), SECTOR_SOUND_BEGIN(NOTE_G3S), 0); // 读扇区，验证
	Loadsounds_DDR3();
	//Loadparams_SD_2_DDR3(SECTOR_SOUND_BEGIN(0), SECTOR_SOUND_END(0), SDRAM_SOUND_BEGIN(0));	
	printf("Loadsounds_DDR3 ok!\r\n");
	
	// 读DDR3，验证
	//Loadparams_DDR3_2_UART(SDRAM_SOUND_BEGIN(NOTE_G3S),512);
#endif
#endif


	NN_Init();
	
	while(1){
		
		if(!SWRead()) continue;
		
		NN_InitH(0); NN_InitH(1); NN_InitH(2);
		printf("start to run...\r\n");
		ddr3_Ctrl(DDR3_NN_CTRL);
		NN_RandomSeed(Timer1ValueGet()+SWRead());	// 加载随机种子
		x = 52; // 初始化。可使用随机值48~72
		i=0;
		while(1)
		{
			if(MIDI_FIFO_Half()) nn_run_flag = 1;
			else continue;

			inst = inst_conv_dict[i%5];
			z = pos_dict[inst];
			//printf("i: %d, z: %d, x: %d\r\n", i, z, x);
			printf(" %d,", x);
			if(i%5==0) Uart0Putc('\r');Uart0Putc('\n');

			NN_Run(SDRAM_ADDR_Z_EMBEDDING_WEIGHT + 64*z, SDRAM_ADDR_EMBEDDING_WEIGHT+512*x);
			while(!NNIntFlag); // 等待计算完成。
			NNIntFlag = 0;
			x = NN_LoadData();
			
			if((i!=0)&(x==0)) 	// 计算结果为0时，结束。
			{
				printf("i=%d, y=%d, finish!\r\n",i,x);
				break;
			}
			if(i>2000)
			{
				printf("i==2000, finish!\r\n");
				break;
			}
			
			// note
			if(x <= 47){
				token.note = x + 35;
				token.tone = 7;
				if(chord.note[i%5]==x)
				{
					chord.position[i%5]+=2;
				}
				else
				{
					// 常规模式
					chord.position[i%5] = 0;
					chord.note[i%5] = x;

				}
				//printf("z: %d, %d, %d, %d, %d\r\n",pos_dict[0],pos_dict[1],pos_dict[2],pos_dict[3],pos_dict[4]);
			}
			// chord
			else{
				token.note = x % 12;
				token.tone = qual[(x-48)/12];
				//printf("i = %d,y = %d, except token = %d\r\n",i,x,token.note);
				
				ddr3_Ctrl(DDR3_MIDI_CTRL);
				//printf("i:%d,\tnote[\t%d\t%d\t%d\t%d]\r\n",i,chord.note[0], chord.note[1], chord.note[2], chord.note[3]);
				//printf("i:%d,\tposi[\t%d\t%d\t%d\t%d]\r\n\r\n",i,chord.position[0], chord.position[1], chord.position[2], chord.position[3]);
				PlayChord(chord, 2);
				ddr3_Ctrl(DDR3_NN_CTRL);
				//printf("play: %d, %d, %d, %d\r\n",chord.note[0], chord.note[1], chord.note[2], chord.note[3]);
			}
			
			next_inst = inst_conv_dict[(i+1)%5];
			//printf("i is %d, token is %d/%d, current_token_dict is: %d/%d, %d, %d, %d, %d\r\n", i, token.note, token.tone,
			//		current_token_dict[0].note,current_token_dict[0].tone,current_token_dict[1].note,current_token_dict[2].note,current_token_dict[3].note,current_token_dict[4].note);
			
			if((current_token_dict[next_inst].note==token.note)&&(current_token_dict[next_inst].tone==token.tone))
			{
				pos_dict[next_inst] += 1;
				//printf("if %d/%d, pos_dict is %d, %d, %d, %d, %d\r\n",current_token_dict[next_inst].note,current_token_dict[next_inst].tone,pos_dict[0],pos_dict[1],pos_dict[2],pos_dict[3],pos_dict[4]);
			}
			else
			{
				//printf("not equal. note %d/%d, chord %d/%d\r\n", current_token_dict[next_inst].note, token.note, current_token_dict[next_inst].tone, token.tone);
				current_token_dict[next_inst] = token;
				pos_dict[next_inst] = 0;
			}
			if(nn_run_flag){
				nn_run_flag = 0;
				i++;
			}
		}
	}
//	printf("start to test sound.\r\n");
//	DDR3CTRL->CR = 0x4; 	// 控制权给midi
//	index=0;
//	while(1){
//		if(MIDI->SR & 0x10){// fifo less than half
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(20)+i*1024,0);
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_C3)+i*1024,1);
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_C3)+i*1024,2);
//			Loadsounds_Play_Sdram(SDRAM_SOUND_BEGIN(NOTE_C3)+i*1024,3);
//			i++;
//		}
//		if(i>=60){
//			printf("next\r\n");
//			index++;
//			i=0;
//		}
//		if(index>7)
//			break;
//	}	
	
	printf("finish.\r\n");
	delay_ms(5000);
	while(1){
		ddr3_Ctrl(DDR3_MIDI_CTRL); 	// 控制权给midi
		LEDToggle();
		//PlaySound(NOTE_G3S, 4);
		//Little_Start_Test(8);
		PlaySound(NOTE_C3,16);
		//All_Sound_Test();
		//Canon_Chord_Test(0,32);
		//Canon_Chord_Test(8,4);
		//C_Major_Test(2);
		Uart0Putc('v');
		delay_ms(1000);
	}

}


