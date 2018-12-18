#include "cst92f30.h"
#include "global_config.h"
#include "ll.h"
#include "pwrmgr.h"
#include "gpio.h"
#include "ll_debug.h"
#include "ll_hw_drv.h"
#include "rcc.h"
#include "uart.h"
#include "log.h"
#include "flash.h"
#include "cst92f30_it.h"
#include "cst92f30_btlib.h"
#include "usr_config.h"
#include "log.h"

volatile SysClkSrc_t g_system_clk = SYS_CLK_XTAL_16M;

static void HalRfphyInit(void)
{
    //============config the txPower
    g_rfPhyTxPower  = DEFAULT_TX_POWER ;
    //============config BLE_PHY TYPE
    g_rfPhyPktFmt   = PKT_FMT_BLE1M;
    
    //============config RF Frequency Offset
    g_rfPhyFreqOffSet   =RF_PHY_FREQ_FOFF_00KHZ;

    hal_rom_code_init();
    
    //Quick Boot setting and 
     *(volatile uint32_t *) 0x4000f01c = 0x0000004;       //  3'b1xx: 62.5us.  control bits for main digital part reset wait time after power up charge pump. 

    //========= pull all io to gnd by default
    *(volatile uint32_t *) 0x4000f008 = 0x36db6db6;//P00 - P09 pull down
    *(volatile uint32_t *) 0x4000f00c = 0x36db6db6;//P10 - P19 pull down
    *(volatile uint32_t *) 0x4000f010 = 0x36db6db6;//P20 - P29 pull down
    *(volatile uint32_t *) 0x4000f014 = 0xb0c3edb6;//P30 - P34 pull donw
	
	//set uart port as GPIO
	HalGpioPinInit(P9,GPIO_MODE_IN,GPIO_PUPD_DOWN);
	HalGpioPinInit(P10,GPIO_MODE_IN,GPIO_PUPD_DOWN);

    DCDC_CONFIG_SETTING(0x0d);

    NVIC_SetPriority((IRQn_Type)BLE_IRQn, IRQ_PRIO_REALTIME);
    NVIC_SetPriority((IRQn_Type)TIM_IRQn, IRQ_PRIO_HIGH);
}

void HalBoardInit(void)
{
    HalRfphyInit();                             //射频配置初始化
	HalRccSysClkSrcConfig(g_system_clk);        //系统主时钟初始化
    HalRccClk32kSrcConfig(CLK32_XTAL);          //32K时钟初始化

	LOG_INIT();                                 //LOG调试初始化
	LOG("Power up\n");                          //打印上电Log信息

    HalPwrmgrRetSRAMConfig(RET_SRAM0|RET_SRAM1|RET_SRAM2|RET_SRAM3|RET_SRAM4);//配置低功耗时保存数据的sram区域
	
	RegisterISR(UART_IRQn, HalUartIRQHandler);  //注册串口回调函数
    RegisterISR(GPIO_IRQn, HalGpioIRQHandler);  //注册GPIO中断回调函数
    HalGpioInit();                              //初始化GPIO
	HalFlashInit();                             //初始化flash控制器
}

