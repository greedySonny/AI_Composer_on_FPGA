



#include "sdcard_spi.h"

// ����SD��spi��byte�洢������������uint8������uint32
//uint8_t rd_arr[512]={0};

extern uint8_t UartIntFlag;


// 1. CS����״̬�£��ȵȴ�64��cycle�ﵽ����������ѹ���ٵȴ�10��cycle����ͬ��
// 2. ����CS������CMD0={0x40,0x00,0x00,0x00,0x00,0x95}��
//    ����0x40Ϊ�����ţ�0x00,0x00,0x00,0x00Ϊ���������0x95ΪcrcУ��λ+ֹͣλ
// 3. ����д��CMD0��ֱ������0x01����CMD0д��ɹ�
// 4. ��CS���ߣ��ٵȴ�8��cycle
uint8_t SD_Reset()    //SD����λ,����SPIģʽ
{
	uint8_t i;
	uint32_t rddata;
	uint8_t cmd0[6]={0x40,0x00,0x00,0x00,0x00,0x95};

	SD_CS_Disable();
	
	// ��CS�ߵ�״̬�·���16x8��cycle���ȴ�sd���ȶ�
	for(i=0;i<16;i++)				
	{
		SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	}
	
	SD_CS_Enable();
	// ����CMD0
	SSP_SendNumberByte(PL022_SPI0, cmd0, 6);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	}while(rddata!=1);
	SD_CS_Disable();
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
									
	return 0;    					
}



//��ʼ������,ʹ��CMD55+ACMD41����
uint8_t SD_Initial(void)    	
{
	uint8_t rddata_a, rddata_b;
	uint16_t i, j;
	uint8_t cmd55[6]={0x77,0x00,0x00,0x00,0x00,0xff};
	uint8_t acmd41[6]={0x69,0x40,0x00,0x00,0x00,0xff};
	// uint8_t cmd1[6]={0x41,0x00,0x00,0x00,0x00,0xff};
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
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
	}while((rddata_b!=0));		//����ֵΪ0ʱ,��ʾACMD41���ͳɹ�
	
	SD_CS_Disable();

	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	
	return(0);   		 		//����0,��ʾ��ʼ�������ɹ�
}

uint8_t SD_Check(void)
{
	uint8_t rddata, i, times=0;
	uint8_t cmd8[6] = {0x48,0x00,0x00,0x01,0xaa,0x87};		// return 00 00 01 aa
	uint8_t cmd58[6] = {0x7a,0x00,0x00,0x00,0x00,0xff};		// return 00 ff 80 00
	
	SD_CS_Disable();
	SD_CS_Enable();
	
	// ����cmd8������Ƿ�֧��sd2.0. 
	// resp�� [31:12] reserved, [11:8] supply voltage, 
	// 00 00 01 aa
	
	do{
		SSP_SendNumberByte(PL022_SPI0, cmd8, 6);
		do{
			rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
		}while(rddata!=1);
		times++;
	}while((rddata!=1)&&(times<50));
	
	SD_CS_Disable();
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	
	SD_CS_Disable();
	for(i=0;i<8;i++) SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SD_CS_Enable();

	// ����cmd58����ȡOCR, opreating condition register,���������Ĵ�������������һ����ѹ
	// resp: 00 ff 80 00
	SSP_SendNumberByte(PL022_SPI0, cmd58, 6);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	}while(rddata==0xff);
	
	for(i=0;i<8;i++){
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 	// ����4bytes��OCR��Ϣ
	}


	SD_CS_Disable();
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	
	return rddata;
}

uint8_t SD_write_sector(uint32_t address, uint8_t *Buffer)
{
	uint8_t rddata;
	uint32_t i;
	uint8_t cmd24[6]={0x58,0x00,0x00,0x00,0x00,0xff};
    				//CMD24����,��ַ��ʼ��Ϊ0
	address<<=9;    	//��������ַת��Ϊ�ֽڵ�ַ
	cmd24[1]=((address&0xff000000)>>24);
	cmd24[2]=((address&0x00ff0000)>>16);
	cmd24[3]=((address&0x0000ff00)>>8);
	
	SD_CS_Disable();
	SD_CS_Enable();

	// д��cmd24
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ȵȴ�8��cycle
	SSP_SendNumberByte(PL022_SPI0, cmd24, 6);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	}while(rddata!=0);

	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); // �ٵȴ�8��cycle

	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xfe); 		// д�뿪ʼ��־�ֽڡ�0xfe��
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
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); //��miso�϶�ȡ����ֵ
	}while(rddata!=0xff);	// �ж�SD��æ
	
	SD_CS_Disable();
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
	
	return(0);    			// ����0,��ʾ����д�����ɹ�
}

uint8_t SD_read_sector(uint32_t address, uint8_t* buffer) //SD��������
{
	uint8_t rddata; 
	uint32_t i;
	uint8_t cmd17[6]={0x51,0x00,0x00,0x00,0x00,0xff};//CMDl7
	
	address<<=9;    //��������ַת��Ϊ�ֽڵ�ַ
	cmd17[1]=((address & 0xff000000)>>24);    //���ֽڵ�ַд�뵽CMDl7����
	cmd17[2]=((address & 0x00ff0000)>>16);
	cmd17[3]=((address & 0x0000ff00)>>8);
	
	SD_CS_Disable();
	SD_CS_Enable();
	
	//printf("send read sector cmd");
	// ���Ͷ�����ָ��
	do{ 
		rddata = Write_Cmd(cmd17);    //д��CMDl7�����ȡ����ֵ
	}while(rddata!=0);  //����ֵΪ0��ʾ����д��ɹ�
	delay_us(1);
	//printf("send read sector cmd ok");
	// �ȴ�
	do{ 
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	}while(rddata!=0xfe); // ��DO�϶�ȡ����,������Ϊ0xfeʱ,��ʾ�����ֽ���512�ֽڵ�����
	
	for(i=0;i<512;i++)
	{
		*(buffer++) = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	}
	
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);//��ȡ�����ֽڵ�CRCУ����
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	
	SD_CS_Disable();
	SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff);
	
    return 0;
}

void SD_CS_Enable(void)
{
	while((PL022_SPI0->SR & SSP_TransmitFIFOempty)==0); // �ȴ����ͻ�����Ϊ��
	GPIO_ResetBits(CMSDK_GPIO0, 0x1); // ����CS	
	while(CMSDK_GPIO0->DATAOUT & 0x01); // �ȴ�CS����
}

void SD_CS_Disable(void)
{
	while((PL022_SPI0->SR & SSP_TransmitFIFOempty)==0); // �ȴ����ͻ�����Ϊ��
	GPIO_SetBits(CMSDK_GPIO0, 0x1); // ����CS
	while(!(CMSDK_GPIO0->DATAOUT & 0x01)); // �ȴ�CS����
}


uint8_t Write_Cmd(uint8_t* cmd)   							//���������ȡ����ֵ�ĺ���
{
	uint8_t rddata=0xff, time;
	
	time = 0;
	SSP_SendNumberByte(PL022_SPI0, cmd, 6);
	delay_us(1);
	do{
		rddata = SSP_SendAndReceiveOneByte(PL022_SPI0, 0xff); 
		time++;
	}while((rddata==0xff) & (time<50));
	

	return rddata;				//���ض�ȡ��һ���ֽ�����
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









