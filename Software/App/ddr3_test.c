



#include "ddr3_test.h"

#define DDR3_TEST_DEEPTH 		256
#define DDR3_TEST_ADDR_BEGIN 	0


void ddr3_test(void)
{
	// 如果在声明时初始化，则在进入函数时要一个一个写入0，耗时严重。
//	uint32_t ddr3_wrdata[DDR3_TEST_DEEPTH]={0};
//	uint32_t ddr3_rddata[DDR3_TEST_DEEPTH]={0};
	uint32_t ddr3_wrdata[DDR3_TEST_DEEPTH];
	uint32_t ddr3_rddata[DDR3_TEST_DEEPTH];
	uint32_t i = 0;
	
	//DDR3CTRL->CR = 0x55;

	// 写
	for(i=0;i<DDR3_TEST_DEEPTH;i++) ddr3_wrdata[i]=i;
	ddr3_WriteBytes(DDR3_TEST_ADDR_BEGIN, ddr3_wrdata, DDR3_TEST_DEEPTH);
	
	// 读
	ddr3_ReadBytes(DDR3_TEST_ADDR_BEGIN, ddr3_rddata, DDR3_TEST_DEEPTH);
	for(i=0;i<DDR3_TEST_DEEPTH;i++) {
	#ifndef SIMULATION
		printf("%x-%x\t", i, ddr3_rddata[i]);
		if(i!=ddr3_rddata[i]) printf("wrong!-%x\r\n",i);
	#endif
		LEDSet(ddr3_rddata[i]);
	}
	
	
	// 切换sdram控制权
	DDR3CTRL->CR = 0x2; 
}



