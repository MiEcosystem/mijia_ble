/**
 ****************************************************************************************
 *
 * @file user_app_authen.h
 *
 * @brief Proximity Project application header file.
 *
 * Copyright (C) 2015-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
 */

#ifndef _USER_APP_AUTHEN_H__
#define _USER_APP_AUTHEN_H__

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief User Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "gapc_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/***************************************************************************************/
/* Selects the trigger mechanism to wake up from deep sleep.                           */
/*  - CFG_EXT_INT_WAKEUP_DEEP_SLEEP - external wake up interrupt caused by button press*/
/*  - CFG_POR_WAKEUP_DEEP_SLEEP     - power on reset caused by button press            */
/* NOTE:                                                                               */
/*  A hardware reset or power cycle will always wake up the system from deep sleep.    */
/***************************************************************************************/
#undef CFG_POR_WAKEUP_DEEP_SLEEP
#undef CFG_EXT_INT_WAKEUP_DEEP_SLEEP

#if defined(CFG_EXT_INT_WAKEUP_DEEP_SLEEP) && defined(CFG_POR_WAKEUP_DEEP_SLEEP)
    #error "Config error: Both CFG_EXT_INT_WAKEUP_DEEP_SLEEP and CFG_POR_WAKEUP_DEEP_SLEEP were defined."
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable push button. Register callback function for button press event.
 *        Must be called in periph_init().
 * @return void
 ****************************************************************************************
 */
void app_button_enable(void);

/**
 ****************************************************************************************
 * @brief Function to be called on the advertising completion event.
 * @param[in] status GAP Error code
 * @return void
 ****************************************************************************************
 */
void app_advertise_complete(const uint8_t status);

#if (BLE_SUOTA_RECEIVER)
/**
 ****************************************************************************************
 * @brief Function called when the SUOTAR status changes.
 * @param[in] suotar_event SUOTAR_START or SUOTAR_STOP
 * @return void
 ****************************************************************************************
 */
void on_suotar_status_change(const uint8_t suotar_event);
#endif // BLE_SUOTA_RECEIVER

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 * @return void
 ****************************************************************************************
*/
void user_app_on_disconnect(struct gapc_disconnect_ind const *param);

/// @} APP

#endif // _USER_PROXR_H_
