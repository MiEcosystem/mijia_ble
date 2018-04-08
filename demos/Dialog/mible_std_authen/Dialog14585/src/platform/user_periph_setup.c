/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2012-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "user_periph_setup.h"       // peripheral configuration
#include "user_profiles_config.h"
#include "gpio.h"
#include "uart.h"                    // UART initialization

#include "app_proxr.h"
#include "user_app_authen.h"

#if BLE_BATT_SERVER
#include "app_bass.h"
#endif

#if (USE_RANGE_EXT)
#include "range_ext_api.h"
#include "sky66111.h"
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
 * Application specific GPIOs reservation
 */
#if BLE_PROX_REPORTER
    RESERVE_GPIO( PUSH_BUTTON, GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, PID_GPIO);
    RESERVE_GPIO( GREEN_LED, GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, PID_GPIO);
#endif
#if (BLE_BATT_SERVER && USE_BAT_LEVEL_ALERT)
    //Setup LED GPIO for battery alert
    RESERVE_GPIO( RED_LED, GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, PID_GPIO);
#endif

#if (BLE_SUOTA_RECEIVER || BLE_MIJIA_SERVER)
#ifndef CFG_SUOTAR_SPI_DISABLE
    #ifndef __DA14586__
    RESERVE_GPIO(SPI_EN,  SPI_EN_GPIO_PORT,  SPI_EN_GPIO_PIN,  PID_SPI_EN);
    RESERVE_GPIO(SPI_CLK, SPI_CLK_GPIO_PORT, SPI_CLK_GPIO_PIN, PID_SPI_CLK);
    RESERVE_GPIO(SPI_DO,  SPI_DO_GPIO_PORT,  SPI_DO_GPIO_PIN,  PID_SPI_DO);
    RESERVE_GPIO(SPI_DI,  SPI_DI_GPIO_PORT,  SPI_DI_GPIO_PIN,  PID_SPI_DI);
    #else
        // The DA14586 GPIOs that are dedicated to its internal SPI flash
        // are automatically reserved by the GPIO driver.
    #endif
#endif

#ifndef CFG_SUOTAR_I2C_DISABLE
    // Example GPIO reservations for an I2C EEPROM.
    RESERVE_GPIO(I2C_SCL, I2C_GPIO_PORT, I2C_SCL_PIN, PID_I2C_SCL);
    RESERVE_GPIO(I2C_SDA, I2C_GPIO_PORT, I2C_SDA_PIN, PID_I2C_SDA);
#endif
#endif //BLE_SUOTA_RECEIVER

#if (USE_RANGE_EXT)
    // Reserve GPIOs for the range extender
    range_ext.init_gpio(NULL);
#endif

#ifdef CFG_PRINTF_UART2
    RESERVE_GPIO( UART2_TX, GPIO_UART2_TX_PORT,  GPIO_UART2_TX_PIN, PID_UART2_TX);
    //RESERVE_GPIO( UART2_RX, GPIO_UART2_RX_PORT,  GPIO_UART2_RX_PIN, PID_UART2_RX);
#endif

}
#endif //DEVELOPMENT_DEBUG

/**
 ****************************************************************************************
 * @brief Map port pins. The UART and SPI port pins and GPIO ports
 *        (for debugging) are mapped.
 ****************************************************************************************
 */
void set_pad_functions(void)        // set gpio port function mode
{
#ifdef __DA14586__
    // disallow spontaneous flash wake-up
    GPIO_ConfigurePin(SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN, OUTPUT, PID_GPIO, true);
#endif

#if (BLE_PROX_REPORTER)
    GPIO_ConfigurePin(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, INPUT_PULLUP, PID_GPIO, false); // Push Button
    GPIO_ConfigurePin(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, false); //Alert LED
#endif
#if (BLE_BATT_SERVER && USE_BAT_LEVEL_ALERT)
    GPIO_ConfigurePin(GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, OUTPUT, PID_GPIO, false); //Battery alert LED
#endif

#if (BLE_SUOTA_RECEIVER || BLE_MIJIA_SERVER)
#ifndef CFG_SUOTAR_SPI_DISABLE
    GPIO_ConfigurePin(SPI_EN_GPIO_PORT,  SPI_EN_GPIO_PIN,  OUTPUT, PID_SPI_EN,  true);
    GPIO_ConfigurePin(SPI_CLK_GPIO_PORT, SPI_CLK_GPIO_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(SPI_DO_GPIO_PORT,  SPI_DO_GPIO_PIN,  OUTPUT, PID_SPI_DO,  false);
    GPIO_ConfigurePin(SPI_DI_GPIO_PORT,  SPI_DI_GPIO_PIN,  INPUT,  PID_SPI_DI,  false);
#endif

#ifndef CFG_SUOTAR_I2C_DISABLE
    // Example GPIO configuration for an I2C EEPROM.
    GPIO_ConfigurePin(I2C_GPIO_PORT, I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(I2C_GPIO_PORT, I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);
#endif
#endif //BLE_SUOTA_RECEIVER

#ifdef CFG_PRINTF_UART2
    GPIO_ConfigurePin( GPIO_UART2_TX_PORT, GPIO_UART2_TX_PIN, OUTPUT, PID_UART2_TX, false );
    //GPIO_ConfigurePin( GPIO_UART2_RX_PORT, GPIO_UART2_RX_PIN, INPUT, PID_UART2_RX, false );
#endif


#if (USE_RANGE_EXT)
    range_ext.enable(MAX_POWER, NULL);
#endif
}

void periph_init(void)  // set i2c, spi, uart, uart2 serial clks
{
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP)) ;

    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);

    //rom patch
    patch_func();

    //Init pads
    set_pad_functions();
		
#ifdef CFG_PRINTF_UART2
		SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
		uart2_init(UART_BAUDRATE_115K2, UART_FRAC_BAUDRATE_115K2, 3);
#endif

#if (BLE_PROX_REPORTER)
    app_proxr_port_reinit(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
    app_button_enable();
#endif //BLE_PROX_REPORTER

#if BLE_BATT_SERVER
    app_batt_port_reinit();
#endif //BLE_BATT_SERVER

    // Enable the pads
    SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);
}
