


#include "driver.h"
#include "sdcard_spi.h"

#define SECTOR_EMBEDDING_WEIGHT_BEGIN				0
#define SECTOR_EMBEDDING_WEIGHT_END					97
#define SECTOR_Z_EMBEDDING_WEIGHT_BEGIN				98
#define SECTOR_Z_EMBEDDING_WEIGHT_END				107
#define SECTOR_R_WI_L0_BEGIN						108
#define SECTOR_R_WI_L0_END							395
#define SECTOR_R_BI_L0_BEGIN						396
#define SECTOR_R_BI_L0_END							396
#define SECTOR_R_WH_L0_BEGIN						397
#define SECTOR_R_WH_L0_END							652
#define SECTOR_R_BH_L0_BEGIN						653
#define SECTOR_R_BH_L0_END							653
#define SECTOR_Z_WI_L0_BEGIN						654
#define SECTOR_Z_WI_L0_END							941
#define SECTOR_Z_BI_L0_BEGIN						942
#define SECTOR_Z_BI_L0_END							942
#define SECTOR_Z_WH_L0_BEGIN						943
#define SECTOR_Z_WH_L0_END							1198
#define SECTOR_Z_BH_L0_BEGIN						1199
#define SECTOR_Z_BH_L0_END							1199
#define SECTOR_N_WI_L0_BEGIN						1200
#define SECTOR_N_WI_L0_END							1487
#define SECTOR_N_BI_L0_BEGIN						1488
#define SECTOR_N_BI_L0_END							1488
#define SECTOR_N_WH_L0_BEGIN						1489
#define SECTOR_N_WH_L0_END							1744
#define SECTOR_N_BH_L0_BEGIN						1745
#define SECTOR_N_BH_L0_END							1745
#define SECTOR_R_WI_L1_BEGIN						1746
#define SECTOR_R_WI_L1_END							2001
#define SECTOR_R_BI_L1_BEGIN						2002
#define SECTOR_R_BI_L1_END							2002
#define SECTOR_R_WH_L1_BEGIN						2003
#define SECTOR_R_WH_L1_END							2258
#define SECTOR_R_BH_L1_BEGIN						2259
#define SECTOR_R_BH_L1_END							2259
#define SECTOR_Z_WI_L1_BEGIN						2260
#define SECTOR_Z_WI_L1_END							2515
#define SECTOR_Z_BI_L1_BEGIN						2516
#define SECTOR_Z_BI_L1_END							2516
#define SECTOR_Z_WH_L1_BEGIN						2517
#define SECTOR_Z_WH_L1_END							2772
#define SECTOR_Z_BH_L1_BEGIN						2773
#define SECTOR_Z_BH_L1_END							2773
#define SECTOR_N_WI_L1_BEGIN						2774
#define SECTOR_N_WI_L1_END							3029
#define SECTOR_N_BI_L1_BEGIN						3030
#define SECTOR_N_BI_L1_END							3030
#define SECTOR_N_WH_L1_BEGIN						3031
#define SECTOR_N_WH_L1_END							3286
#define SECTOR_N_BH_L1_BEGIN						3287
#define SECTOR_N_BH_L1_END							3287
#define SECTOR_R_WI_L2_BEGIN						3288
#define SECTOR_R_WI_L2_END							3543
#define SECTOR_R_BI_L2_BEGIN						3544
#define SECTOR_R_BI_L2_END							3544
#define SECTOR_R_WH_L2_BEGIN						3545
#define SECTOR_R_WH_L2_END							3800
#define SECTOR_R_BH_L2_BEGIN						3801
#define SECTOR_R_BH_L2_END							3801
#define SECTOR_Z_WI_L2_BEGIN						3802
#define SECTOR_Z_WI_L2_END							4057
#define SECTOR_Z_BI_L2_BEGIN						4058
#define SECTOR_Z_BI_L2_END							4058
#define SECTOR_Z_WH_L2_BEGIN						4059
#define SECTOR_Z_WH_L2_END							4314
#define SECTOR_Z_BH_L2_BEGIN						4315
#define SECTOR_Z_BH_L2_END							4315
#define SECTOR_N_WI_L2_BEGIN						4316
#define SECTOR_N_WI_L2_END							4571
#define SECTOR_N_BI_L2_BEGIN						4572
#define SECTOR_N_BI_L2_END							4572
#define SECTOR_N_WH_L2_BEGIN						4573
#define SECTOR_N_WH_L2_END							4828
#define SECTOR_N_BH_L2_BEGIN						4829
#define SECTOR_N_BH_L2_END							4829
#define SECTOR_OUTPUT_BEGIN							4830
#define SECTOR_OUTPUT_END							4940



#define SDRAM_ADDR_EMBEDDING_WEIGHT					0x0
#define SDRAM_ADDR_Z_EMBEDDING_WEIGHT				0xC400
#define SDRAM_ADDR_R_WI_L0							0xD800		
#define SDRAM_ADDR_R_BI_L0							0x31800
#define SDRAM_ADDR_R_WH_L0							0x31A00		
#define SDRAM_ADDR_R_BH_L0							0x51A00
#define SDRAM_ADDR_Z_WI_L0 							0x51C00
#define SDRAM_ADDR_Z_BI_L0 							0x75C00
#define SDRAM_ADDR_Z_WH_L0 							0x75E00
#define SDRAM_ADDR_Z_BH_L0 							0x95E00
#define SDRAM_ADDR_N_WI_L0 							0x96000
#define SDRAM_ADDR_N_BI_L0 							0xBA000
#define SDRAM_ADDR_N_WH_L0 							0xBA200
#define SDRAM_ADDR_N_BH_L0 							0xDA200
#define SDRAM_ADDR_R_WI_L1							0xDA400
#define SDRAM_ADDR_R_BI_L1							0xFA400
#define SDRAM_ADDR_R_WH_L1							0xFA600
#define SDRAM_ADDR_R_BH_L1							0x11A600
#define SDRAM_ADDR_Z_WI_L1							0x11A800
#define SDRAM_ADDR_Z_BI_L1							0x13A800
#define SDRAM_ADDR_Z_WH_L1							0x13AA00
#define SDRAM_ADDR_Z_BH_L1							0x15AA00
#define SDRAM_ADDR_N_WI_L1							0x15AC00
#define SDRAM_ADDR_N_BI_L1							0x17AC00
#define SDRAM_ADDR_N_WH_L1							0x17AE00
#define SDRAM_ADDR_N_BH_L1							0x19AE00
#define SDRAM_ADDR_R_WI_L2							0x19B000
#define SDRAM_ADDR_R_BI_L2 							0x1BB000
#define SDRAM_ADDR_R_WH_L2 							0x1BB200
#define SDRAM_ADDR_R_BH_L2 							0x1DB200
#define SDRAM_ADDR_Z_WI_L2 							0x1DB400
#define SDRAM_ADDR_Z_BI_L2 							0x1FB400
#define SDRAM_ADDR_Z_WH_L2 							0x1FB600
#define SDRAM_ADDR_Z_BH_L2 							0x21B600
#define SDRAM_ADDR_N_WI_L2 							0x21B800
#define SDRAM_ADDR_N_BI_L2 							0x23B800
#define SDRAM_ADDR_N_WH_L2 							0x23BA00
#define SDRAM_ADDR_N_BH_L2 							0x25BA00	
#define SDRAM_ADDR_OUTPUT							0x25BC00


void Loadparams_SD_2_UART(uint32_t sector_begin, uint32_t sector_end, uint32_t remain);
void Loadparams_UART_2_SD(uint32_t sector_begin, uint32_t sector_end);
void Loadparams_SD_2_DDR3(uint32_t sector_begin, uint32_t sector_end, uint32_t sdram_addr_begin);
void Loadparams_DDR3_2_UART(uint32_t addr_begin, uint32_t size);

void Loadparams_Test(void);
void Loadparams_SD(void);
void Loadparams_DDR3(void);

uint8_t Uart1GetC_Timeout(uint32_t timeout_ms);
