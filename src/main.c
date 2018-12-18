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
#include "board.h"

#define DEFAULT_UART_BAUD   115200

extern int app_main(void);

void printfFuncHex(const char *fun, int line, char *str, uint8_t *dat, int len)
{
	LOG("%s(%d):%s:", fun, line, str);
	for (int ii = 0; ii < len; ii++)
	{
		LOG("%02X ", dat[ii]);
	}
	LOG("\n");
}

void printfFuncChar(const char *fun, int line, char *str, uint8_t *dat, int len)
{
	LOG("%s(%d):%s:", fun, line, str);
	for (int ii = 0; ii < len; ii++)
	{
		LOG("%c", dat[ii]);
	}
	LOG("\n");
}

#include "flash.h"
void sample_flash(void)
{
	uint8_t tempDeviceId[4] = {0};
	memset(tempDeviceId, 0, sizeof(tempDeviceId));
	HalFlashDeviceIdRead(tempDeviceId);
	printfFuncHex(__func__, __LINE__, "Device ID", tempDeviceId, 3);

	uint8_t tempUniqueId[17] = {0};
	memset(tempUniqueId, 0, sizeof(tempUniqueId));
	HalFlashUniqueIdRead(tempUniqueId);
	printfFuncHex(__func__, __LINE__, "Unique ID", tempUniqueId, 16);

	char *str = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ++++1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ----1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ****1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ%%%%1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ####1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ@@@@1234567890abcdef";
	uint8_t tempStr[257] = {0};

	HalFlashErase(0x00005);
	HalFlashErase(0x00006);

	memset(tempStr, 0, sizeof(tempStr));
	HalFlashRead(0x00005F80, tempStr, strlen(str));
	printfFuncHex(__func__, __LINE__, "READ After Erase", tempStr, strlen(str));

	HalFlashWrite(0x00005F80, (uint8_t *)str, (strlen(str) / 2));
	HalFlashWrite(0x00006000, (uint8_t *)(str + (strlen(str) / 2)), (strlen(str) / 2));

	memset(tempStr, 0, sizeof(tempStr));
	HalFlashRead(0x00005F80, tempStr, strlen(str));
	printfFuncChar(__func__, __LINE__, "READ After Write", tempStr, strlen(str));

	HalFlashErase(0x00005);
	HalFlashErase(0x00006);

	memset(tempStr, 0, sizeof(tempStr));
	HalFlashRead(0x00005F80, tempStr, strlen(str));
	printfFuncHex(__func__, __LINE__, "READ After Erase", tempStr, strlen(str));

	LOG("%s(%d):%s:%d\n", __func__, __LINE__, "Write overflow", HalFlashWrite(0x00005F80, (uint8_t *)str, strlen(str)));
}

int  main(void)  
{     
	HalSystemInit();	//底层系统初始化
	HalBoardInit();		//板子硬件初始化

	//sample_flash();		//测试flash
    app_main();			//启动系统调度
    
}


///////////////////////////////////  end  ///////////////////////////////////////

