



#include "load_params.h"




void Loadparams_SD_2_UART(uint32_t sector_begin, uint32_t sector_end, uint32_t remain)
{
	uint8_t rd_arr[512]={0};
	uint32_t i=0, j=0;
	
	for(j=sector_begin;j<=sector_end;j++){
		printf("begin to read sector: %d\r\n",j);
		SD_read_sector(j, rd_arr);
		for(i=0;i<512;i++) printf("%d-%02x\t", i, rd_arr[i]);
	}
}


uint8_t Uart1GetC_Timeout(uint32_t timeout_ms)
{	
	extern uint8_t TimerIntFlag;
	
	CMSDK_TIMER0->CTRL = 0; 					// disable
	TimerIntFlag = 0;
	TimerSetValue(CLK_FREQ/1000*timeout_ms);	// set value
	CMSDK_TIMER0->CTRL = 9;						// enable 
	while(1){
		if(TimerIntFlag){
			TimerIntFlag = 0;
			CMSDK_TIMER0->CTRL = 0; 			// disable
			return 0xff;
		}
		else if(CMSDK_UART1->STATE & 2){
			CMSDK_TIMER0->CTRL = 0; 			// disable
			return CMSDK_UART1->DATA; 
		}
	}
}

/* ********************************************************************* */
//											sector		
// embedding.weight: 98x256x2 = 50176 		0-97	
// 上位机									下位机
// 			 - 0x00 'flag' 0x0a --------->
//			<- "ready" 0x0a -------------
//			 - |each sector data| 0x0a -->
//			......
//			
// 
//

// 通过UART向SD卡写参数。一次通过uart收512byte，然后写一个扇区
void Loadparams_UART_2_SD(uint32_t sector_begin, uint32_t sector_end)
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
			}
		}
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



void Loadparams_SD_2_DDR3(uint32_t sector_begin, uint32_t sector_end, uint32_t sdram_addr_begin)
{
	uint32_t i=0, j=0, k=0;
	uint32_t tmp_arr[8]={0};
	uint8_t rd_arr[512]={0};
	
	for(i=sector_begin;i<=sector_end;i++){
		// 读每扇区，并写到 ddr3 sdram 中
		SD_read_sector(i, rd_arr);
		for(j=0; j<512; j=j+0x20){
			
			// 每32 byte
			for(k=0; k<8; k++){
				// 按byte存储
				//tmp_arr[k] = ((uint32_t)rd_arr[j+(k<<2)] << 0) + ((uint32_t)rd_arr[j+(k<<2)+1] << 8) + ((uint32_t)rd_arr[j+(k<<2)+2] << 16) + ((uint32_t)rd_arr[j+(k<<2)+3] << 24);
				// 按half word存储
				tmp_arr[k] = ((uint32_t)rd_arr[j+(k<<2)] << 24) + ((uint32_t)rd_arr[j+(k<<2)+1] << 16) + ((uint32_t)rd_arr[j+(k<<2)+2] << 8) + ((uint32_t)rd_arr[j+(k<<2)+3] << 0);
				//printf("%08x \r\n",tmp_arr[k]);
			}
			//ddr3_WriteEightWords((i<<9)+j, tmp_arr);
			//ddr3_WriteEightWords(sdram_addr_begin + j, tmp_arr);
			//ddr3_WriteEightWords(i*512 + j , tmp_arr);
			ddr3_WriteEightWords(sdram_addr_begin + (i-sector_begin)*512 + j , tmp_arr);
		}
	}
}



void Loadparams_DDR3_2_UART(uint32_t addr_begin, uint32_t size)
{
	
	uint32_t i=0, j=0;
	uint32_t tmp_arr[8]={0};
	
	
	for(i=addr_begin; i<addr_begin+size; i=i+0x20){
		
		ddr3_ReadEightWords(i, tmp_arr);
		for(j=0; j<8; j++){
			printf("0x%x-%08x\t", i+j*4, tmp_arr[j]);
		}
	}
	
}
	


void Loadparams_SD(void)
{
	/* -------------- 读取参数从UART卡到SD卡 -------------- */
//	Loadparams_UART_2_SD(SECTOR_EMBEDDING_WEIGHT_BEGIN, 	SECTOR_EMBEDDING_WEIGHT_END		);	// embedding.weight
//	Loadparams_UART_2_SD(SECTOR_Z_EMBEDDING_WEIGHT_BEGIN, 	SECTOR_Z_EMBEDDING_WEIGHT_END	);	// z_embedding.weight
	
//	Loadparams_UART_2_SD(SECTOR_R_WI_L0_BEGIN,				SECTOR_R_WI_L0_END				);	// r_wi_l0
//	Loadparams_UART_2_SD(SECTOR_R_BI_L0_BEGIN,				SECTOR_R_BI_L0_END				);	// r_bi_l0
//	Loadparams_UART_2_SD(SECTOR_R_WH_L0_BEGIN,				SECTOR_R_WH_L0_END				);	// r_wh_l0	
//	Loadparams_UART_2_SD(SECTOR_R_BH_L0_BEGIN,				SECTOR_R_BH_L0_END				); 	// r_bh_l0
//	Loadparams_UART_2_SD(SECTOR_Z_WI_L0_BEGIN,				SECTOR_Z_WI_L0_END				);	// z_wi_l0
//	Loadparams_UART_2_SD(SECTOR_Z_BI_L0_BEGIN,				SECTOR_Z_BI_L0_END				);	// z_bi_l0
//	Loadparams_UART_2_SD(SECTOR_Z_WH_L0_BEGIN,				SECTOR_Z_WH_L0_END				);	// z_wh_l0
//	Loadparams_UART_2_SD(SECTOR_Z_BH_L0_BEGIN,				SECTOR_Z_BH_L0_END				);	// z_bh_l0
//	Loadparams_UART_2_SD(SECTOR_N_WI_L0_BEGIN,				SECTOR_N_WI_L0_END				);	// n_wi_l0
//	Loadparams_UART_2_SD(SECTOR_N_BI_L0_BEGIN,				SECTOR_N_BI_L0_END				);	// n_bi_l0
//	Loadparams_UART_2_SD(SECTOR_N_WH_L0_BEGIN,				SECTOR_N_WH_L0_END				);	// n_wh_l0
//	Loadparams_UART_2_SD(SECTOR_N_BH_L0_BEGIN,				SECTOR_N_BH_L0_END				);	// n_bh_l0

//	Loadparams_UART_2_SD(SECTOR_R_WI_L1_BEGIN,				SECTOR_R_WI_L1_END				);	// r_wi_l1
//	Loadparams_UART_2_SD(SECTOR_R_BI_L1_BEGIN,				SECTOR_R_BI_L1_END				);	// r_bi_l1
//	Loadparams_UART_2_SD(SECTOR_R_WH_L1_BEGIN,				SECTOR_R_WH_L1_END				);	// r_wh_l1	
//	Loadparams_UART_2_SD(SECTOR_R_BH_L1_BEGIN,				SECTOR_R_BH_L1_END				); 	// r_bh_l1
//	Loadparams_UART_2_SD(SECTOR_Z_WI_L1_BEGIN,				SECTOR_Z_WI_L1_END				);	// z_wi_l1
//	Loadparams_UART_2_SD(SECTOR_Z_BI_L1_BEGIN,				SECTOR_Z_BI_L1_END				);	// z_bi_l1
//	Loadparams_UART_2_SD(SECTOR_Z_WH_L1_BEGIN,				SECTOR_Z_WH_L1_END				);	// z_wh_l1
//	Loadparams_UART_2_SD(SECTOR_Z_BH_L1_BEGIN,				SECTOR_Z_BH_L1_END				);	// z_bh_l1
//	Loadparams_UART_2_SD(SECTOR_N_WI_L1_BEGIN,				SECTOR_N_WI_L1_END				);	// n_wi_l1
//	Loadparams_UART_2_SD(SECTOR_N_BI_L1_BEGIN,				SECTOR_N_BI_L1_END				);	// n_bi_l1
//	Loadparams_UART_2_SD(SECTOR_N_WH_L1_BEGIN,				SECTOR_N_WH_L1_END				);	// n_wh_l1
//	Loadparams_UART_2_SD(SECTOR_N_BH_L1_BEGIN,				SECTOR_N_BH_L1_END				);	// n_bh_l1

//	Loadparams_UART_2_SD(SECTOR_R_WI_L2_BEGIN,				SECTOR_R_WI_L2_END				);	// r_wi_l2
//	Loadparams_UART_2_SD(SECTOR_R_BI_L2_BEGIN,				SECTOR_R_BI_L2_END				);	// r_bi_l2
//	Loadparams_UART_2_SD(SECTOR_R_WH_L2_BEGIN,				SECTOR_R_WH_L2_END				);	// r_wh_l2	
//	Loadparams_UART_2_SD(SECTOR_R_BH_L2_BEGIN,				SECTOR_R_BH_L2_END				); 	// r_bh_l2
//	Loadparams_UART_2_SD(SECTOR_Z_WI_L2_BEGIN,				SECTOR_Z_WI_L2_END				);	// z_wi_l2
//	Loadparams_UART_2_SD(SECTOR_Z_BI_L2_BEGIN,				SECTOR_Z_BI_L2_END				);	// z_bi_l2
//	Loadparams_UART_2_SD(SECTOR_Z_WH_L2_BEGIN,				SECTOR_Z_WH_L2_END				);	// z_wh_l2
//	Loadparams_UART_2_SD(SECTOR_Z_BH_L2_BEGIN,				SECTOR_Z_BH_L2_END				);	// z_bh_l2
//	Loadparams_UART_2_SD(SECTOR_N_WI_L2_BEGIN,				SECTOR_N_WI_L2_END				);	// n_wi_l2
//	Loadparams_UART_2_SD(SECTOR_N_BI_L2_BEGIN,				SECTOR_N_BI_L2_END				);	// n_bi_l2
//	Loadparams_UART_2_SD(SECTOR_N_WH_L2_BEGIN,				SECTOR_N_WH_L2_END				);	// n_wh_l2
//	Loadparams_UART_2_SD(SECTOR_N_BH_L2_BEGIN,				SECTOR_N_BH_L2_END				);	// n_bh_l2

	Loadparams_UART_2_SD(SECTOR_OUTPUT_BEGIN,				SECTOR_OUTPUT_END				);	// n_bh_l2
}


void Loadparams_DDR3(void)
{
	/* -------------- 读取参数从SD卡到DDR3 SDRAM -------------- */
	Loadparams_SD_2_DDR3(SECTOR_EMBEDDING_WEIGHT_BEGIN, 	SECTOR_EMBEDDING_WEIGHT_END, 	SDRAM_ADDR_EMBEDDING_WEIGHT);	// embedding.weight
	Loadparams_SD_2_DDR3(SECTOR_Z_EMBEDDING_WEIGHT_BEGIN,	SECTOR_Z_EMBEDDING_WEIGHT_END,	SDRAM_ADDR_Z_EMBEDDING_WEIGHT);	// z_embedding.weight
	
	Loadparams_SD_2_DDR3(SECTOR_R_WI_L0_BEGIN,				SECTOR_R_WI_L0_END,				SDRAM_ADDR_R_WI_L0);			// r_wi_l0
	Loadparams_SD_2_DDR3(SECTOR_R_BI_L0_BEGIN,				SECTOR_R_BI_L0_END,				SDRAM_ADDR_R_BI_L0);			// r_bi_l0
	Loadparams_SD_2_DDR3(SECTOR_R_WH_L0_BEGIN,				SECTOR_R_WH_L0_END,				SDRAM_ADDR_R_WH_L0);			// r_wh_l0
	Loadparams_SD_2_DDR3(SECTOR_R_BH_L0_BEGIN,				SECTOR_R_BH_L0_END,				SDRAM_ADDR_R_BH_L0);			// r_bh_l0
	Loadparams_SD_2_DDR3(SECTOR_Z_WI_L0_BEGIN,				SECTOR_Z_WI_L0_END,				SDRAM_ADDR_Z_WI_L0);			// z_wi_l0
	Loadparams_SD_2_DDR3(SECTOR_Z_BI_L0_BEGIN,				SECTOR_Z_BI_L0_END,				SDRAM_ADDR_Z_BI_L0);			// z_bi_l0
	Loadparams_SD_2_DDR3(SECTOR_Z_WH_L0_BEGIN,				SECTOR_Z_WH_L0_END,				SDRAM_ADDR_Z_WH_L0);			// z_wh_l0
	Loadparams_SD_2_DDR3(SECTOR_Z_BH_L0_BEGIN,				SECTOR_Z_BH_L0_END,				SDRAM_ADDR_Z_BH_L0);			// z_bh_l0
	Loadparams_SD_2_DDR3(SECTOR_N_WI_L0_BEGIN,				SECTOR_N_WI_L0_END,				SDRAM_ADDR_N_WI_L0);			// n_wi_l0
	Loadparams_SD_2_DDR3(SECTOR_N_BI_L0_BEGIN,				SECTOR_N_BI_L0_END,				SDRAM_ADDR_N_BI_L0);			// n_bi_l0
	Loadparams_SD_2_DDR3(SECTOR_N_WH_L0_BEGIN,				SECTOR_N_WH_L0_END,				SDRAM_ADDR_N_WH_L0);			// n_wh_l0
	Loadparams_SD_2_DDR3(SECTOR_N_BH_L0_BEGIN,				SECTOR_N_BH_L0_END,				SDRAM_ADDR_N_BH_L0);			// n_bh_l0
	
	Loadparams_SD_2_DDR3(SECTOR_R_WI_L1_BEGIN,				SECTOR_R_WI_L1_END,				SDRAM_ADDR_R_WI_L1);			// r_wi_l1
	Loadparams_SD_2_DDR3(SECTOR_R_BI_L1_BEGIN,				SECTOR_R_BI_L1_END,				SDRAM_ADDR_R_BI_L1);			// r_bi_l1
	Loadparams_SD_2_DDR3(SECTOR_R_WH_L1_BEGIN,				SECTOR_R_WH_L1_END,				SDRAM_ADDR_R_WH_L1);			// r_wh_l1
	Loadparams_SD_2_DDR3(SECTOR_R_BH_L1_BEGIN,				SECTOR_R_BH_L1_END,				SDRAM_ADDR_R_BH_L1);			// r_bh_l1
	Loadparams_SD_2_DDR3(SECTOR_Z_WI_L1_BEGIN,				SECTOR_Z_WI_L1_END,				SDRAM_ADDR_Z_WI_L1);			// z_wi_l1
	Loadparams_SD_2_DDR3(SECTOR_Z_BI_L1_BEGIN,				SECTOR_Z_BI_L1_END,				SDRAM_ADDR_Z_BI_L1);			// z_bi_l1
	Loadparams_SD_2_DDR3(SECTOR_Z_WH_L1_BEGIN,				SECTOR_Z_WH_L1_END,				SDRAM_ADDR_Z_WH_L1);			// z_wh_l1
	Loadparams_SD_2_DDR3(SECTOR_Z_BH_L1_BEGIN,				SECTOR_Z_BH_L1_END,				SDRAM_ADDR_Z_BH_L1);			// z_bh_l1
	Loadparams_SD_2_DDR3(SECTOR_N_WI_L1_BEGIN,				SECTOR_N_WI_L1_END,				SDRAM_ADDR_N_WI_L1);			// n_wi_l1
	Loadparams_SD_2_DDR3(SECTOR_N_BI_L1_BEGIN,				SECTOR_N_BI_L1_END,				SDRAM_ADDR_N_BI_L1);			// n_bi_l1
	Loadparams_SD_2_DDR3(SECTOR_N_WH_L1_BEGIN,				SECTOR_N_WH_L1_END,				SDRAM_ADDR_N_WH_L1);			// n_wh_l1
	Loadparams_SD_2_DDR3(SECTOR_N_BH_L1_BEGIN,				SECTOR_N_BH_L1_END,				SDRAM_ADDR_N_BH_L1);			// n_bh_l1
	
	Loadparams_SD_2_DDR3(SECTOR_R_WI_L2_BEGIN,				SECTOR_R_WI_L2_END,				SDRAM_ADDR_R_WI_L2);			// r_wi_l2
	Loadparams_SD_2_DDR3(SECTOR_R_BI_L2_BEGIN,				SECTOR_R_BI_L2_END,				SDRAM_ADDR_R_BI_L2);			// r_bi_l2
	Loadparams_SD_2_DDR3(SECTOR_R_WH_L2_BEGIN,				SECTOR_R_WH_L2_END,				SDRAM_ADDR_R_WH_L2);			// r_wh_l2
	Loadparams_SD_2_DDR3(SECTOR_R_BH_L2_BEGIN,				SECTOR_R_BH_L2_END,				SDRAM_ADDR_R_BH_L2);			// r_bh_l2
	Loadparams_SD_2_DDR3(SECTOR_Z_WI_L2_BEGIN,				SECTOR_Z_WI_L2_END,				SDRAM_ADDR_Z_WI_L2);			// z_wi_l2
	Loadparams_SD_2_DDR3(SECTOR_Z_BI_L2_BEGIN,				SECTOR_Z_BI_L2_END,				SDRAM_ADDR_Z_BI_L2);			// z_bi_l2
	Loadparams_SD_2_DDR3(SECTOR_Z_WH_L2_BEGIN,				SECTOR_Z_WH_L2_END,				SDRAM_ADDR_Z_WH_L2);			// z_wh_l2
	Loadparams_SD_2_DDR3(SECTOR_Z_BH_L2_BEGIN,				SECTOR_Z_BH_L2_END,				SDRAM_ADDR_Z_BH_L2);			// z_bh_l2
	Loadparams_SD_2_DDR3(SECTOR_N_WI_L2_BEGIN,				SECTOR_N_WI_L2_END,				SDRAM_ADDR_N_WI_L2);			// n_wi_l2
	Loadparams_SD_2_DDR3(SECTOR_N_BI_L2_BEGIN,				SECTOR_N_BI_L2_END,				SDRAM_ADDR_N_BI_L2);			// n_bi_l2
	Loadparams_SD_2_DDR3(SECTOR_N_WH_L2_BEGIN,				SECTOR_N_WH_L2_END,				SDRAM_ADDR_N_WH_L2);			// n_wh_l2
	Loadparams_SD_2_DDR3(SECTOR_N_BH_L2_BEGIN,				SECTOR_N_BH_L2_END,				SDRAM_ADDR_N_BH_L2);			// n_bh_l2
	
	Loadparams_SD_2_DDR3(SECTOR_OUTPUT_BEGIN,				SECTOR_OUTPUT_END,				SDRAM_ADDR_OUTPUT);			// output
}

void Loadparams_Test(void)
{

	/* -------------- 读取参数从UART卡到SD卡 -------------- */
	Loadparams_UART_2_SD(0,97);				// embedding.weight
	delay_ms(1000);
	Loadparams_UART_2_SD(98,107);			// z_embedding.weight
	printf("UART_2_SD ok!\r\n");
	
	SD_Init(); // 不重新初始化的话，写完再读会卡住。不知道什么原因。
	

	// 读扇区，验证
	//Loadparams_SD_2_UART(0, test_sectors-1, 0);
	
	/* -------------- 读取参数从SD卡到DDR3 SDRAM -------------- */
	Loadparams_SD_2_DDR3(0,(98+10),0);
	printf("SD_2_DDR3 ok!\r\n");
	
	// 读DDR3，验证
	Loadparams_DDR3_2_UART(0,512*(98+10));
	
}
	
