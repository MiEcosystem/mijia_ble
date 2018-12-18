#ifndef __USR_CONFIG_H
#define __USR_CONFIG_H

#define IC_TYPE_QFN32       0
#define IC_TYPE_QFN48       1
#define IC_TYPE             IC_TYPE_QFN32           //CST92F30选用QFN32封装
#define CFG_SLEEP_MODE      PWR_MODE_SLEEP          //配置系统启用低功耗
#define CFG_DBG_PRINT		1                       //使能打印输出
#define DEFAULT_TX_POWER	RF_PHY_TX_POWER_0DBM    //设置BLE的发射功率为0DBM

#endif

