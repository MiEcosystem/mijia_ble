/**
 ****************************************************************************************
 *
 * @file usr_config.h
 *
 * @brief User configuration file.
 *
 * Copyright(C) 2015 NXP Semiconductors N.V.
 * All rights reserved.
 *
 * $Rev: 1.0 $
 *
 ****************************************************************************************
 */

#ifndef USR_CONFIG_H_
#define USR_CONFIG_H_

// User configuration

/// Software verion: CFG_SW_DEBUG, CFG_SW_RELEASE
// In the debug version the program will be in the infinite loop if the hardfault exception is triggered.
// In the release version the program will execute a system reboot if the hardfault exception is triggered.
#define CFG_SW_DEBUG

/// Chip version: CFG_9020_B2, CFG_9020_B4
// Select corresponding library file according to the chip version.
// The last marking letter of chip is 'E' for QN902x B4 and 'D' for QN902x B2.
#define CFG_9020_B4

/// High operating ambient temperature support
// Define this macro if using T version chip (QN9022T) which supports work at 125 degrees Celsius.
// #define CFG_T_VERSION

/// Evaluation board indication
// The GPIOs used for QN9021/QN9022 miniDK's LED and button are different from QN9020 miniDK.
// If the QN9021 or QN9022 miniDK is used, the following macro shall be defined.
// #define CFG_9021_MINIDK
// #define CFG_9022_MINIDK_V1_1

/// External flash
// QN9022 uses external flash. BOND_EN should be set correctly in this case
// to prevent extra power consumption in SLEEP mode.
// #define CFG_EXT_FLASH

/// No external flash
// If QN9022 does not use external flash, define this macro to prevent infinite loop
// in the flash driver.
// #define CFG_NO_FLASH

/// Work mode: CFG_WM_SOC, CFG_WM_NP, CFG_WM_HCI
#define CFG_WM_SOC

/// Easy ACI
// #define CFG_EACI

/// Easy API
// #define CFG_EAPI

#if ((defined(CFG_EACI) || defined(CFG_EAPI)) && !defined(CFG_WM_SOC))
    #error "CFG_WM_SOC shall be defined when using EACI or EAPI"
#endif

/// Local name
// The local name defined by this macro is only used when the device name tag 
// is not availiable in the NVDS. Generally the local name in the advertising
// packet is obtained from device name in the NVDS.
#define CFG_LOCAL_NAME   "NXP MIBLE"

/// DC-DC enable
#define CFG_DC_DC

/// 32k RCO
// #define CFG_32K_RCO

/// NVDS WRTIE SUPPORT
#define CFG_NVDS_WRITE    //chx

/// Test mode controll pin
//#define CFG_TEST_CTRL_PIN GPIO_P31

/// Memory retention
// There are 8 banks of SRAM, and each bank is 8k bytes. Bank0 is always power on.
// The power of bank 1~7 can be configured by developer. If the memory bank is not
// used by application, it can be off to save power.
// The BLE stack stores its data in the bank 6 & 7. If the BLE is used, bank 6 & 7
// should be power on.
// The develop can configure this macro based on memory usage to optimize power
// consumption.
#define CFG_MEM_RETENTION   (MEM_BLOCK1 | MEM_BLOCK2 | MEM_BLOCK3 | MEM_BLOCK4 | MEM_BLOCK5 | MEM_BLOCK6 | MEM_BLOCK7)

/// Deep sleep support
//#define CFG_DEEP_SLEEP

/// BLE Sleep
//#define CFG_BLE_SLEEP

/// BLE MAX sleep duration
#define CFG_BLE_MAX_SLEEP_DUR   0x3e80  // 625us * 0x3e80 = 10s

/// Transport layer UART interface used in network processor mode and controller mode
#define CFG_HCI_UART                    QN_UART0
/// Transport layer SPI interface used in network processor mode and controller mode
//#define CFG_HCI_SPI                     QN_SPI0
/// SPI write ready, notify host to read from SPI, output
//#define CFG_HCI_SPI_WR_CTRL_PIN         GPIO_P26

/// Debug information output interface
#define CFG_DEBUG_UART  QN_UART0

/// Debug print option
#define CFG_DBG_PRINT

/// Debug information
#define CFG_DBG_INFO

/// Maximum connection number
#define CFG_CON 1

/// GAP role: CFG_CENTRAL, CFG_PERIPHERAL, CFG_OBSERVER, CFG_BROADCASTER, CFG_ALLROLES
#define CFG_PERIPHERAL

/// Local address type: CFG_ADDR_PUBLIC, CFG_ADDR_RAND
#define CFG_ADDR_PUBLIC

/// Privacy enable
// #define CFG_PRIV_EN

/// Set a true random seed to the ROM code 
// #define CFG_FW_SRAND

/// Support service discovery
// #define CFG_SVC_DISC

/// Support white list
// #define CFG_WL_SUPPORT

/// Link infomation option
// #define CFG_LINK_INFO

/// CSRK support
// #define CFG_CSRK_SUPPORT

/// Security
//#define CFG_SECURITY_ON

//Secure Connection
//#define CFG_SEC_CON

/// ATT parts
//#define CFG_ATTC
#define CFG_ATTS

/// BLE new features
// #define CFG_MULTI_NOTIFICATION_IN_ONE_EVENT
// #define CFG_READ_INDICATION
// #define CFG_SLAVE_LATENCY_IMPROVEMENT

/// Profiles and services

///Health Thermometer Profile Collector Role
// #define CFG_PRF_HTPC
// #define CFG_TASK_HTPC   TASK_PRF1

///Health Thermometer Profile Thermometer Role
// #define CFG_PRF_HTPT
// #define CFG_TASK_HTPT   TASK_PRF1

/// Proximity Profile Monitor Role
// #define CFG_PRF_PXPM
// #define CFG_TASK_PXPM   TASK_PRF1

/// Proximity Profile Reporter Role
// #define CFG_PRF_PXPR
// #define CFG_TASK_PXPR   TASK_PRF1

///Find Me Profile Locator role
// #define CFG_PRF_FMPL
// #define CFG_TASK_FMPL   TASK_PRF1

///Find Me Profile Target role
// #define CFG_PRF_FMPT
// #define CFG_TASK_FMPT   TASK_PRF1

///Device Information Service Client Role
// #define CFG_PRF_DISC
// #define CFG_TASK_DISC   TASK_PRF1

///Device Information Service Server Role
//#define CFG_PRF_DISS
//#define CFG_TASK_DISS   TASK_PRF1

///Blood Pressure Profile Collector Role
// #define CFG_PRF_BLPC
// #define CFG_TASK_BLPC   TASK_PRF1

///Blood Pressure Profile Sensor Role
// #define CFG_PRF_BLPS
// #define CFG_TASK_BLPS   TASK_PRF2

///Time Profile Client Role
// #define CFG_PRF_TIPC
// #define CFG_TASK_TIPC   TASK_PRF1

///Time Profile Server Role
// #define CFG_PRF_TIPS
// #define CFG_TASK_TIPS   TASK_PRF1

///Heart Rate Profile Collector Role
// #define CFG_PRF_HRPC
// #define CFG_TASK_HRPC   TASK_PRF1

///Heart Rate Profile Sensor Role
// #define CFG_PRF_HRPS
// #define CFG_TASK_HRPS   TASK_PRF1

///Glucose Profile Client Role
// #define CFG_PRF_GLPC
// #define CFG_TASK_GLPC   TASK_PRF1

///Glucose Profile Server Role
//#define CFG_PRF_GLPS
//#define CFG_TASK_GLPS   TASK_PRF2

///HID Profile Boot Host Role
// #define CFG_PRF_HOGPBH
// #define CFG_TASK_HOGPBH   TASK_PRF1

///HID Profile Report Host Role
// #define CFG_PRF_HOGPRH
// #define CFG_TASK_HOGPRH   TASK_PRF2

///HID Profile Device Role
// #define CFG_PRF_HOGPD
// #define CFG_TASK_HOGPD   TASK_PRF1

///Scan Parameter Profile Client Role
// #define CFG_PRF_SCPPC
// #define CFG_TASK_SCPPC   TASK_PRF1

///Scan Parameter Profile Server Role
// #define CFG_PRF_SCPPS
// #define CFG_TASK_SCPPS   TASK_PRF1

///Battery Service Client Role
// #define CFG_PRF_BASC
// #define CFG_TASK_BASC   TASK_PRF1

///Battery Service Server Role
// #define CFG_PRF_BASS
// #define CFG_TASK_BASS   TASK_PRF1

///Alert Notification Client Role
//#define CFG_PRF_ANPC
//#define CFG_TASK_ANPC   TASK_PRF2

///Alert Notification Server Role
//#define CFG_PRF_ANPS
//#define CFG_TASK_ANPS   TASK_PRF1

///Mible Server Role
#define CFG_PRF_MIS
#define CFG_TASK_MIS   TASK_PRF1


#endif

