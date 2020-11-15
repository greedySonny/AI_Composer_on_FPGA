



#include "sdcard_spi.h"

// 由于SD卡spi按byte存储，所以这里设uint8而不是uint32
//uint8_t rd_arr[512]={0};

extern uint8_t UartIntFlag;


// 1. CS拉高状态下，先等待64个cycle达到正常工作电压，再等待10个cycle进行同步
// 2. 拉低CS，发送CMD0={0x40,0x00,0x00,0x00,0x00,0x95}，
//    其中0x40为命令编号，0x00,0x00,0x00,0x00为命令参数，0x95为crc校验位+停止位
// 3. 持续写入CMD0，直到读到0x01表明CMD0写入成功
// 4. 将CS拉高，再等待8个cycle
uint8_t SD_Reset()    //SD卡复位,进入SPI模式
{
	uint8_t i;
	uint32_t rddata;
	uint8_t cmd0[6]={0x40,0x00,0x00,0x00,0x00,0x95};

	SD_CS_Disable();
	
	// 在CS高的状态下发送16x8个cycle，等待sd卡稳定
	for(i=0;i<16;i++)				
	{
		SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	}
	
	SD_CS_Enable();
	// 发送CMD0
	SSP_SendNumberByte(PL022_SPI0, cmd0, 6);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	}while(rddata!=1);
	SD_CS_Disable();
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
									
	return 0;    					
}



//初始化函数,使用CMD55+ACMD41命令
uint8_t SD_Initial(void)    	
{
	uint8_t rddata_a, rddata_b;
	uint16_t i, j;
	uint8_t cmd55[6]={0x77,0x00,0x00,0x00,0x00,0xff};
	uint8_t acmd41[6]={0x69,0x40,0x00,0x00,0x00,0xff};
	// uint8_t cmd1[6]={0x41,0x00,0x00,0x00,0x00,0xff};
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SD_CS_Disable();
	SD_CS_Enable();
	do{
		i=0;
		SSP_SendNumberByte(PL022_SPI0, cmd55, 6);
		do{
			rddata_a = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
			i++;
		}while((rddata_a!=1)&&(i<50));

		i=0;
		SSP_SendNumberByte(PL022_SPI0, acmd41, 6);
		do{
			rddata_b = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
			i++;
		}while((rddata_b!=0)&&(i<50));
		j++;
	}while((rddata_b!=0));		//返回值为0时,表示ACMD41发送成功
	
	SD_CS_Disable();

	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	
	return(0);   		 		//返回0,表示初始化操作成功
}

uint8_t SD_Check(void)
{
	uint8_t rddata, i, times=0;
	uint8_t cmd8[6] = {0x48,0x00,0x00,0x01,0xaa,0x87};		// return 00 00 01 aa
	uint8_t cmd58[6] = {0x7a,0x00,0x00,0x00,0x00,0xff};		// return 00 ff 80 00
	
	SD_CS_Disable();
	SD_CS_Enable();
	
	// 发送cmd8，检查是否支持sd2.0. 
	// resp： [31:12] reserved, [11:8] supply voltage, 
	// 00 00 01 aa
	
	do{
		SSP_SendNumberByte(PL022_SPI0, cmd8, 6);
		do{
			rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
		}while(rddata!=1);
		times++;
	}while((rddata!=1)&&(times<50));
	
	SD_CS_Disable();
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	
	SD_CS_Disable();
	for(i=0;i<8;i++) SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SD_CS_Enable();

	// 发送cmd58，读取OCR, opreating condition register,操作条件寄存器，用来返回一个电压
	// resp: 00 ff 80 00
	SSP_SendNumberByte(PL022_SPI0, cmd58, 6);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	}while(rddata==0xff);
	
	for(i=0;i<8;i++){
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 	// 返回4bytes的OCR信息
	}


	SD_CS_Disable();
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	
	return rddata;
}

uint8_t SD_write_sector(uint32_t address, uint8_t *Buffer)
{
	uint8_t rddata;
	uint32_t i;
	uint8_t cmd24[6]={0x58,0x00,0x00,0x00,0x00,0xff};
    				//CMD24命令,地址初始化为0
	address<<=9;    	//将扇区地址转化为字节地址
	cmd24[1]=((address&0xff000000)>>24);
	cmd24[2]=((address&0x00ff0000)>>16);
	cmd24[3]=((address&0x0000ff00)>>8);
	
	SD_CS_Disable();
	SD_CS_Enable();

	// 写入cmd24
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 先等待8个cycle
	SSP_SendNumberByte(PL022_SPI0, cmd24, 6);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	}while(rddata!=0);

	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // 再等待8个cycle

	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xfe); 		// 写入开始标志字节”0xfe”
	for(i=0;i<512;i++)
	{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, Buffer[i]);
	}
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);		// 2 bytes CRC
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	
	i=0;
	do
	{
		i++;
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); //从miso上读取返回值
	}while(rddata!=0xff);	// 判断SD卡忙
	
	SD_CS_Disable();
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	
	return(0);    			// 返回0,表示扇区写操作成功
}

uint8_t SD_read_sector(uint32_t address, uint8_t* buffer) //SD卡读函数
{
	uint8_t rddata; 
	uint32_t i;
	uint8_t cmd17[6]={0x51,0x00,0x00,0x00,0x00,0xff};//CMDl7
	
	address<<=9;    //将扇区地址转化为字节地址
	cmd17[1]=((address & 0xff000000)>>24);    //将字节地址写入到CMDl7当中
	cmd17[2]=((address & 0x00ff0000)>>16);
	cmd17[3]=((address & 0x0000ff00)>>8);
	
	SD_CS_Disable();
	SD_CS_Enable();
	
	//printf("send read sector cmd");
	// 发送读扇区指令
	do{ 
		rddata = Write_Cmd(cmd17);    //写入CMDl7命令并读取返回值
	}while(rddata!=0);  //返回值为0表示命令写入成功
	delay_us(1);
	//printf("send read sector cmd ok");
	// 等待
	do{ 
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	}while(rddata!=0xfe); // 从DO上读取数据,当数据为0xfe时,表示后续字节是512字节的数据
	
	for(i=0;i<512;i++)
	{
		*(buffer++) = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	}
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);//读取两个字节的CRC校验码
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	
	SD_CS_Disable();
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	
    return 0;
}

void SD_CS_Enable(void)
{
	while((PL022_SPI0->SR & SSP_TransmitFIFOempty)==0); // 等待发送缓冲区为空
	GPIO_ResetBits(CMSDK_GPIO0, 0x1); // 拉低CS	
	while(CMSDK_GPIO0->DATAOUT & 0x01); // 等待CS拉低
}

void SD_CS_Disable(void)
{
	while((PL022_SPI0->SR & SSP_TransmitFIFOempty)==0); // 等待发送缓冲区为空
	GPIO_SetBits(CMSDK_GPIO0, 0x1); // 拉高CS
	while(!(CMSDK_GPIO0->DATAOUT & 0x01)); // 等待CS拉高
}


uint8_t Write_Cmd(uint8_t* cmd)   							//发送命令并读取返回值的函数
{
	uint8_t rddata=0xff, time;
	
	time = 0;
	SSP_SendNumberByte(PL022_SPI0, cmd, 6);
	delay_us(1);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
		time++;
	}while((rddata==0xff) & (time<50));
	

	return rddata;				//返回读取的一个字节数据
}

void SD_Init(void)
{

	SD_Reset();
	printf("R\t");

	SD_Check();
	printf("C\t");
	
	SD_Initial();
	printf("I\t");
	
}

void SD_Test(void)
{
	uint8_t wr_arr[512]={0};
	uint8_t rd_arr[512]={0};
	uint32_t i=0;
	
	for(i=0;i<512;i++) wr_arr[i] = i;
	SD_write_sector(0, wr_arr);
	printf("SD write ok!\r\n");

	SD_read_sector(0, rd_arr);
	for(i=0;i<512;i++) printf("%d-%x\t", i, rd_arr[i]);
	printf("SD read ok!\r\n");
}









