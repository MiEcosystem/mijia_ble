/**
 ****************************************************************************************
 *
 * @file driver_config.h
 *
 * @brief Driver configuration for QN9020.
 *
 * Copyright(C) 2015 NXP Semiconductors N.V.
 * All rights reserved.
 *
 * $Rev: 1.0 $
 *
 ****************************************************************************************
 */
#ifndef _DRIVER_CONFIG_H_
#define _DRIVER_CONFIG_H_

///@cond
/**
 ****************************************************************************************
 * @addtogroup QN_CONFIG QN9020 Configurations
 * @brief
 ****************************************************************************************
 */
///@endcond

/**
 ****************************************************************************************
 * @addtogroup QN_DRIVER_CONFIG Driver Configurations
 * @ingroup QN_CONFIG
 *
 *  Driver Configurations define driver status (enable or disable), realization method (interrupt or polling),
 *  , which driver to use (dirver code or driver in ROM), driver callback status (enable or disable), and
 *  driver work mode (for example, I2C module work at MASTER or SLAVE mode). Users can modify these configurations.
 *
 *  The following is an example of how to configure UART driver:
 *
 *  CONFIG_ENABLE_DRIVER_UART: This macro can be set to TRUE or FALSE, means to enable or disable UART driver.
 *  Only if this macro value is TRUE, the other macros related to UART have meanings.
 *
 *  CONFIG_UART0_TX_DEFAULT_IRQHANDLER: This macro is used to enable or disable UART0 TX default interrupt
 *  request handler. It can be set to TURE or FALSE. If the macro is defined to FALSE, users can rewrite a new
 *  handler to replace the default handler. This macro will be effective under the condition of UART
 *  driver is enabled and UART0 TX interrupt is enabled.
 *
 *  CONFIG_UART0_TX_ENABLE_INTERRUPT: Define this macro to TRUE to enable UART0 TX interruption. Otherwise,
 *  UART0 data will be transmitted via polling.
 *
 *  CONFIG_ENABLE_ROM_DRIVER_UART: This macro set to TRUE means to use driver burned in ROM. All the UART
 *  APIs become to function pointer which point to ROM address and driver configurations are fixed. Otherwise,
 *  the UART source code will be used, and user can modify them.
 *
 *  UART_CALLBACK_EN: This macro means to enable or disable UART callback.
 *
 *  UART_BAUDRATE_TABLE_EN: This macro means to enable or disable UART baud rate parameters table,
 *  If the macro is defined to FALSE, baud rate will be set by formula calculation.
 *
 * @{
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driver_QN9020.h"
#include "fw_func_addr.h"
#include "app_config.h"

#define BLE_PRJ
/// driver configuration
#define __XTAL                                          XTAL_16MHz        /*!< Extrenal frequency */
#define __SYSTEM_CLOCK                                  SYS_EXT_XTAL      /*!< System clock frequency */
#if QN_SECURE_CONNECTIONS
// Improve AHB to speed up the procedure of DHKey generation
#define __AHB_CLK                                       CLK_16M           /*!< AHB clock frequency */
#else
#define __AHB_CLK                                       CLK_8M            /*!< AHB clock frequency */
#endif
#define __APB_CLK                                       CLK_8M            /*!< APB clock frequency */
#define __BLE_CLK                                       CLK_8M            /*!< BLE clock frequency */
#define __TIMER_CLK                                     CLK_8M            /*!< TIMER clock frequency */
#define __USART_CLK                                     CLK_8M            /*!< UART and SPI clock frequency */
#if (QN_32K_RCO)
#define __32K_TYPE                                      RCO_32K           /*!< 32K clock type: XTAL_32K, RCO_32K */
#else
#define __32K_TYPE                                      XTAL_32K          /*!< 32K clock type: XTAL_32K, RCO_32K */
#endif


#define CONFIG_ENABLE_DRIVER_GPIO                       TRUE        /*!< Enable/Disable GPIO Driver */
#define CONFIG_GPIO_DEFAULT_IRQHANDLER                  TRUE        /*!< Enable/Disable GPIO Default IRQ Handler */
#define CONFIG_GPIO_ENABLE_INTERRUPT                    TRUE        /*!< Enable/Disable GPIO Interrupt */

#define CONFIG_ENABLE_DRIVER_SPI0                       TRUE        /*!< Enable/Disable SPI Driver */
#define CONFIG_SPI0_DEFAULT_IRQHANDLER                  TRUE        /*!< Enable/Disable SPI0 Default IRQ Handler */
#define CONFIG_SPI0_TX_ENABLE_INTERRUPT                 TRUE        /*!< Enable/Disable(Polling) SPI0 TX Interrupt */
#define CONFIG_SPI0_RX_ENABLE_INTERRUPT                 TRUE        /*!< Enable/Disable(Polling) SPI0 RX Interrupt */
#define CONFIG_ENABLE_DRIVER_SPI1                       TRUE        /*!< Enable/Disable SPI Driver */
#define CONFIG_SPI1_DEFAULT_IRQHANDLER                  FALSE       /*!< Enable/Disable SPI1 Default IRQ Handler */
#define CONFIG_SPI1_TX_ENABLE_INTERRUPT                 FALSE       /*!< Enable/Disable(Polling) SPI1 TX Interrupt */
#define CONFIG_SPI1_RX_ENABLE_INTERRUPT                 FALSE       /*!< Enable/Disable(Polling) SPI1 RX Interrupt */

#define CONFIG_ENABLE_DRIVER_UART0                      TRUE        /*!< Enable/Disable UART Driver */
#define CONFIG_UART0_TX_DEFAULT_IRQHANDLER              TRUE        /*!< Enable/Disable UART0 TX Default IRQ Handler */
#define CONFIG_UART0_RX_DEFAULT_IRQHANDLER              TRUE        /*!< Enable/Disable UART0 RX Default IRQ Handler */
#define CONFIG_UART0_TX_ENABLE_INTERRUPT                TRUE        /*!< Enable/Disable(Polling) UART0 TX Interrupt */
#define CONFIG_UART0_RX_ENABLE_INTERRUPT                TRUE        /*!< Enable/Disable(Polling) UART0 RX Interrupt */
#define CONFIG_ENABLE_DRIVER_UART1                      TRUE        /*!< Enable/Disable UART Driver */
#define CONFIG_UART1_TX_DEFAULT_IRQHANDLER              FALSE       /*!< Enable/Disable UART1 TX Default IRQ Handler */
#define CONFIG_UART1_RX_DEFAULT_IRQHANDLER              FALSE       /*!< Enable/Disable UART1 RX Default IRQ Handler */
#define CONFIG_UART1_TX_ENABLE_INTERRUPT                FALSE       /*!< Enable/Disable(Polling) UART1 TX Interrupt */
#define CONFIG_UART1_RX_ENABLE_INTERRUPT                FALSE       /*!< Enable/Disable(Polling) UART1 RX Interrupt */

#define CONFIG_ENABLE_DRIVER_SERIAL_FLASH               TRUE        /*!< Enable/Disable Serial Flash Driver */

#define CONFIG_ENABLE_DRIVER_I2C                        TRUE        /*!< Enable/Disable I2C Driver */
#define CONFIG_I2C_DEFAULT_IRQHANDLER                   FALSE       /*!< Enable/Disable I2C Default IRQ Handler */
#define CONFIG_I2C_ENABLE_INTERRUPT                     FALSE       /*!< Enable/Disable(Polling) I2C Interrupt */

#define CONFIG_ENABLE_DRIVER_TIMER0                     TRUE        /*!< Enable/Disable TIMER Driver */
#define CONFIG_TIMER0_DEFAULT_IRQHANDLER                TRUE        /*!< Enable/Disable TIMER0 Default IRQ Handler */
#define CONFIG_TIMER0_ENABLE_INTERRUPT                  TRUE        /*!< Enable/Disable TIMER0 Interrupt */
#define CONFIG_ENABLE_DRIVER_TIMER1                     TRUE        /*!< Enable/Disable TIMER Driver */
#define CONFIG_TIMER1_DEFAULT_IRQHANDLER                TRUE        /*!< Enable/Disable TIMER1 Default IRQ Handler */
#define CONFIG_TIMER1_ENABLE_INTERRUPT                  TRUE        /*!< Enable/Disable TIMER1 Interrupt */
#define CONFIG_ENABLE_DRIVER_TIMER2                     TRUE        /*!< Enable/Disable TIMER Driver */
#define CONFIG_TIMER2_DEFAULT_IRQHANDLER                TRUE        /*!< Enable/Disable TIMER2 Default IRQ Handler */
#define CONFIG_TIMER2_ENABLE_INTERRUPT                  TRUE        /*!< Enable/Disable TIMER2 Interrupt */
#define CONFIG_ENABLE_DRIVER_TIMER3                     TRUE        /*!< Enable/Disable TIMER Driver */
#define CONFIG_TIMER3_DEFAULT_IRQHANDLER                TRUE        /*!< Enable/Disable TIMER3 Default IRQ Handler */
#define CONFIG_TIMER3_ENABLE_INTERRUPT                  TRUE        /*!< Enable/Disable TIMER3 Interrupt */

#define CONFIG_ENABLE_DRIVER_PWM0                       TRUE        /*!< Enable/Disable PWM Driver */
#define CONFIG_PWM0_DEFAULT_IRQHANDLER                  FALSE       /*!< Enable/Disable PWM0 Default IRQ Handler */
#define CONFIG_PWM0_ENABLE_INTERRUPT                    FALSE       /*!< Enable/Disable PWM0 Default IRQ Handler */
#define CONFIG_ENABLE_DRIVER_PWM1                       TRUE        /*!< Enable/Disable PWM Driver */
#define CONFIG_PWM1_DEFAULT_IRQHANDLER                  FALSE       /*!< Enable/Disable PWM0 Interrupt */
#define CONFIG_PWM1_ENABLE_INTERRUPT                    FALSE       /*!< Enable/Disable PWM1 Default IRQ Handler */

#define CONFIG_ENABLE_DRIVER_WDT                        TRUE        /*!< Enable/Disable WDT Driver */
#define CONFIG_WDT_DEFAULT_IRQHANDLER                   TRUE        /*!< Enable/Disable WDT Default IRQ Handler */
#define CONFIG_WDT_ENABLE_INTERRUPT                     TRUE        /*!< Enable/Disable WDT Interrupt */

#define CONFIG_ENABLE_DRIVER_DMA                        TRUE        /*!< Enable/Disable DMA Driver */
#define CONFIG_DMA_DEFAULT_IRQHANDLER                   TRUE        /*!< Enable/Disable DMA Default IRQ Handler */
#define CONFIG_DMA_ENABLE_INTERRUPT                     TRUE        /*!< Enable/Disable DMA Interrupt */

#define CONFIG_ENABLE_DRIVER_RTC                        FALSE       /*!< Enable/Disable RTC Driver */
#define CONFIG_RTC_DEFAULT_IRQHANDLER                   TRUE        /*!< Enable/Disable RTC Default IRQ Handler */
#define CONFIG_RTC_ENABLE_INTERRUPT                     TRUE        /*!< Enable/Disable RTC Interrupt */

#define CONFIG_ENABLE_DRIVER_RTC_CAP                    TRUE        /*!< Enable/Disable RTC Captrue Driver */
#define CONFIG_RTC_CAP_DEFAULT_IRQHANDLER               TRUE        /*!< Enable/Disable RTC Captrue Default IRQ Handler */
#define CONFIG_RTC_CAP_ENABLE_INTERRUPT                 TRUE        /*!< Enable/Disable RTC Captrue Interrupt */

#define CONFIG_ENABLE_DRIVER_BLE_DP                     TRUE        /*!< Enable/Disable BLE datapath Driver */

#define CONFIG_ENABLE_DRIVER_CALIB                      TRUE        /*!< Enable/Disable Calibration Driver */
#define CONFIG_CALIB_DEFAULT_IRQHANDLER                 FALSE       /*!< Enable/Disable Calibration Default IRQ Handler */
#define CONFIG_CALIB_ENABLE_INTERRUPT                   FALSE       /*!< Enable/Disable Calibration Interrupt */

#define CONFIG_ENABLE_DRIVER_ADC                        TRUE        /*!< Enable/Disable ADC Driver */
#define CONFIG_ADC_DEFAULT_IRQHANDLER                   FALSE       /*!< Enable/Disable ADC Default IRQ Handler */
#define CONFIG_ADC_ENABLE_INTERRUPT                     FALSE       /*!< Enable/Disable ADC Interrupt */

#define CONFIG_ENABLE_DRIVER_RNG                        TRUE        /*!< Enable/Disable RNG Driver */

#define CONFIG_ENABLE_DRIVER_ANALOG                     TRUE        /*!< Enable/Disable Analog Driver */
#define CONFIG_ENABLE_DRIVER_ACMP0                      TRUE        /*!< Enable/Disable Analog Driver */
#define CONFIG_ACMP0_DEFAULT_IRQHANDLER                 TRUE        /*!< Enable/Disable Analog Comparator Default IRQ Handler */
#define CONFIG_ACMP0_ENABLE_INTERRUPT                   TRUE        /*!< Enable/Disable Analog Comparator Interrupt */
#define CONFIG_ENABLE_DRIVER_ACMP1                      TRUE        /*!< Enable/Disable Analog Driver */
#define CONFIG_ACMP1_DEFAULT_IRQHANDLER                 TRUE        /*!< Enable/Disable Analog Comparator Default IRQ Handler */
#define CONFIG_ACMP1_ENABLE_INTERRUPT                   TRUE        /*!< Enable/Disable Analog Comparator Interrupt */

#define CONFIG_ENABLE_DRIVER_QNRF                       TRUE        /*!< Enable/Disable RF Driver */
#define CONFIG_ENABLE_DRIVER_SLEEP                      TRUE        /*!< Enable/Disable Sleep Driver */
#define CONFIG_ENABLE_DRIVER_SYSCON                     TRUE        /*!< Enable/Disable System Controller Driver */

#define CONFIG_ENABLE_ROM_DRIVER_CALIB                  TRUE        /*!< Enable/Disable Calibration ROM Driver */

/// target configuration
#define GPIO_CALLBACK_EN                                TRUE        /*!< Enable/Disable GPIO Driver Callback */

#define UART_DMA_EN                                     FALSE       /*!< Enable/Disable UART DMA function */
#define UART_CALLBACK_EN                                TRUE        /*!< Enable/Disable UART Driver Callback */
#define UART_BAUDRATE_TABLE_EN                          TRUE        /*!< Enable/Disable UART Baudrate table */

#define SPI_DMA_EN                                      FALSE       /*!< Enable/Disable SPI DMA function */
#define SPI_CALLBACK_EN                                 TRUE        /*!< Enable/Disable SPI Driver Callback */

#define I2C_MODE                                        I2C_MASTER  /*!< Config I2C Mode: Master or Slave */
#define I2C_CALLBACK_EN                                 TRUE        /*!< Enable/Disable I2C Driver Callback */

#define TIMER0_CAP_MODE                                 INCAP_EVENT_MOD     /*!< Config Timer0 Capture Mode: Input Capture timer/event/counter mode */
#define TIMER1_CAP_MODE                                 INCAP_TIMER_MOD     /*!< Config Timer1 Capture Mode: Input Capture timer/event/counter mode */
#define TIMER2_CAP_MODE                                 INCAP_EVENT_MOD     /*!< Config Timer2 Capture Mode: Input Capture timer/event/counter mode */
#define TIMER3_CAP_MODE                                 INCAP_COUNTER_MOD   /*!< Config Timer3 Capture Mode: Input Capture timer/event/counter mode */

#define TIMER0_CALLBACK_EN                              TRUE        /*!< Enable/Disable Timer0 Driver Callback */
#define TIMER1_CALLBACK_EN                              TRUE        /*!< Enable/Disable Timer1 Driver Callback */
#define TIMER2_CALLBACK_EN                              TRUE        /*!< Enable/Disable Timer2 Driver Callback */
#define TIMER3_CALLBACK_EN                              TRUE        /*!< Enable/Disable Timer3 Driver Callback */

#define RTC_CALLBACK_EN                                 TRUE        /*!< Enable/Disable RTC Driver Callback */
#define RTC_CAP_CALLBACK_EN                             TRUE        /*!< Enable/Disable RTC Capture Driver Callback */
#define USE_STD_C_LIB_TIME                              TRUE        /*!< Enable/Disable Standard C library function to parse date and time */

#define DMA_CALLBACK_EN                                 TRUE        /*!< Enable/Disable DMA Driver Callback */

#define ADC_DMA_EN                                      FALSE       /*!< Enable/Disable ADC DMA function */
#define ADC_CALLBACK_EN                                 TRUE        /*!< Enable/Disable ADC Driver Callback */
#define ADC_WCMP_CALLBACK_EN                            TRUE        /*!< Enable/Disable ADC WCMP Callback */

#define ACMP_CALLBACK_EN                                TRUE        /*!< Enable/Disable Analog Comparator Driver Callback */

#define CALIB_CALLBACK_EN                               FALSE       /*!< Enable/Disable Calibration Driver Callback */

#define SLEEP_CALLBACK_EN                               TRUE        /*!< Enable/Disable Sleep Wakeup Callback */
#define SLEEP_CONFIG_EN                                 TRUE        /*!< Enable/Disable User Config Before Enter Sleep */
#define ACMP_WAKEUP_EN                                  FALSE       /*!< Enable/Disable Analog comparator wakeup MCU */
#define GPIO_WAKEUP_EN                                  TRUE        /*!< Enable/Disable GPIO wakeup MCU */
#define SLEEP_TIMER_WAKEUP_EN                           TRUE        /*!< Enable/Disable Sleep timer wakeup MCU */
#define QN_32K_LOW_POWER_MODE_EN                        FALSE       /*!< Enable/Disable Low power mode */


#define UART_RX_ACTIVE_BIT_EN                           FALSE       /*!< Enable/Disable uart rx active bit set */
#define SPI_RX_ACTIVE_BIT_EN                            FALSE       /*!< Enable/Disable spi rx active bit set */

/// @}QN_DRIVER_CONFIG

#endif /* _DRIVER_CONFIG_H_ */
