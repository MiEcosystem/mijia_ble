/**
 ****************************************************************************************
 *
 * @file user_app_authen.c
 *
 * @brief Proximity reporter project source code.
 *
 * Copyright (C) 2015-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "gapc_task.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "app_easy_msg_utils.h"
#include "gpio.h"
#include "app_security.h"
#include "user_app_authen.h"
#include "arch_api.h"
#include "app_task.h"
#include "app_proxr.h"
#include "arch_console.h"
#include "mible_type.h"
#include "mible_api.h"

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#if (!SUOTAR_SPI_DISABLE)
#include "spi_flash.h"
#endif
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */



void app_button_enable(void)
{
#if (BLE_PROX_REPORTER)
    app_easy_wakeup_set(app_wakeup_cb);
    wkupct_register_callback(app_button_press_cb);

    if (GPIO_GetPinStatus(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN))
    {
        wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), // select pin (GPIO_BUTTON_PORT, GPIO_BUTTON_PIN)
                          WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, WKUPCT_PIN_POLARITY_LOW), // polarity low
                          1, // 1 event
                          0); // debouncing time = 0
    }
#endif
}

#if (BLE_SUOTA_RECEIVER)
void on_suotar_status_change(const uint8_t suotar_event)
{
#if (!SUOTAR_SPI_DISABLE)
    int8_t man_dev_id = 0;

    man_dev_id = spi_flash_enable(SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN);
    if (man_dev_id == SPI_FLASH_AUTO_DETECT_NOT_DETECTED)
    {
        // The device was not identified. The default parameters are used.
        // Alternatively, an error can be asserted here.
        spi_flash_init(SPI_FLASH_DEFAULT_SIZE, SPI_FLASH_DEFAULT_PAGE);
    }

    if (suotar_event == SUOTAR_END)
    {
        // Power down SPI Flash
        spi_flash_power_down();
    }
#endif
}
#endif

void user_app_on_disconnect(struct gapc_disconnect_ind const *param)
{
    //default_app_on_disconnect(NULL);
		ke_state_set(TASK_APP, APP_CONNECTED);
		arch_printf("reason:%x\n",param->reason);
#if (BLE_MIJIA_SERVER)
		mible_gap_evt_t evt;
		evt = MIBLE_GAP_EVT_DISCONNET;
		mible_gap_evt_param_t mi_param;
		memset(&mi_param,0,sizeof(param));
		mi_param.conn_handle = param->conhdl;
		mi_param.disconnect.reason = (mible_gap_disconnect_reason_t)param->reason;
		mible_gap_event_callback(evt,&mi_param);
#endif

}

/**
 ****************************************************************************************
 * @brief Put the system into deep sleep mode. It demonstrates the deep sleep mode usage
 *        and how the system can wake up from it. Once the system enters deep sleep state
 *        it retains NO RAM blocks. The exit from the deep sleep state causes a system
 *        reboot.
 * @note  The system can wake up from deep sleep by:
 *          - external wake up interrupt, caused e.g. by button press (properly configured GPIO pin)
 *          - power on reset, caused e.g. by button press (properly configured GPIO pin)
 *          - H/W reset button press or power cycle (at any time)
 *        When the system exits deep sleep state, the boot process is triggered.
 *        The application code has to be programmed in an external memory resource or
 *        in the OTP memory, in order for the system to reboot properly.
 * @return void
 ****************************************************************************************
*/
static void put_system_into_deep_sleep(void)
{
#if defined (CFG_EXT_INT_WAKEUP_DEEP_SLEEP) && !defined (CFG_POR_WAKEUP_DEEP_SLEEP)
    // configure button for wake-up interrupt
    app_button_enable();

    // set deep sleep - external interrupt wake up
    arch_set_deep_sleep(true);

#elif defined (CFG_POR_WAKEUP_DEEP_SLEEP) && !defined (CFG_EXT_INT_WAKEUP_DEEP_SLEEP)
    // configure button for POR
    GPIO_EnablePorPin(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, GPIO_POR_PIN_POLARITY_LOW, GPIO_GetPorTime());

    // set deep sleep - POR wake up
    arch_set_deep_sleep(false);

#else
    // do nothing.
    // The system will eventually enter the selected Extended sleep state.
    // A button press will wake up the system if the respective GPIO is configured as a wake up interrupt.
#endif
}

void app_advertise_complete(const uint8_t status)
{
#if (BLE_PROX_REPORTER)
    app_proxr_alert_stop();
#endif

    // Disable wakeup for BLE and timer events. Only external (GPIO) wakeup events can wakeup processor.
    if (status == GAP_ERR_CANCELED)
    {
        arch_ble_ext_wakeup_on();
    }

    // configure button to trigger wake-up interrupt
    app_button_enable();

    // Put system into deep sleep
    put_system_into_deep_sleep();
}

/// @} APP
