/**
 ****************************************************************************************
 *
 * @file usr_design.c
 *
 * @brief Product related design.
 *
 * Copyright(C) 2015 NXP Semiconductors N.V.
 * All rights reserved.
 *
 * $Rev: 1.0 $
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup  USR
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "app_env.h"
#include "led.h"
#include "uart.h"
#include "lib.h"
#include "usr_design.h"
#include "gpio.h"
#include "button.h"
#include "sleep.h"
#include "ke_mem.h"

#include "mible_type.h"
#include "mible_beacon.h"
#include "mible_server.h"
#include "mible_api.h"

#include "mible_api.h"
/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

#define LED_ON_DUR_ADV_FAST        2
#define LED_OFF_DUR_ADV_FAST       (uint16_t)((GAP_ADV_FAST_INTV2*0.625)/10)
#define LED_ON_DUR_ADV_SLOW        2
#define LED_OFF_DUR_ADV_SLOW       (uint16_t)((GAP_ADV_SLOW_INTV*0.625)/10)
#define LED_ON_DUR_CON                    0xffff
#define LED_OFF_DUR_CON                   0
#define LED_ON_DUR_IDLE                   0
#define LED_OFF_DUR_IDLE        0xffff


#define EVENT_BUTTON1_PRESS_ID            0

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


struct usr_env_tag usr_env = {LED_ON_DUR_IDLE, LED_OFF_DUR_IDLE};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief   Led1 for BLE status
 ****************************************************************************************
 */
static void usr_led1_set(uint16_t timer_on, uint16_t timer_off)
{
    usr_env.led1_on_dur = timer_on;
    usr_env.led1_off_dur = timer_off;

    if (timer_on == 0 || timer_off == 0)
    {
        if (timer_on == 0)
        {
            led_set(1, LED_OFF);
        }
        if (timer_off == 0)
        {
            led_set(1, LED_ON);
        }
        ke_timer_clear(APP_SYS_LED_1_TIMER, TASK_APP);
    }
    else
    {
        led_set(1, LED_OFF);
        ke_timer_set(APP_SYS_LED_1_TIMER, TASK_APP, timer_off);
    }
}

/**
 ****************************************************************************************
 * @brief   Led 1 flash process
 ****************************************************************************************
 */
static void usr_led1_process(void)
{
    if(led_get(1) == LED_ON)
    {
        led_set(1, LED_OFF);
        ke_timer_set(APP_SYS_LED_1_TIMER, TASK_APP, usr_env.led1_off_dur);
    }
    else
    {
        led_set(1, LED_ON);
        ke_timer_set(APP_SYS_LED_1_TIMER, TASK_APP, usr_env.led1_on_dur);
    }
}

/**
 ****************************************************************************************
 * @brief   Application task message handler
 ****************************************************************************************
 */
extern void mible_Callback (ke_msg_id_t const msgid, void const *param);

void app_task_msg_hdl(ke_msg_id_t const msgid, void const *param)
{
    switch(msgid)
    {
        case GAP_SET_MODE_REQ_CMP_EVT:
            if(APP_IDLE == ke_state_get(TASK_APP))
            {
                usr_led1_set(LED_ON_DUR_ADV_FAST, LED_OFF_DUR_ADV_FAST);
                ke_timer_set(APP_ADV_INTV_UPDATE_TIMER, TASK_APP, 30 * 100);
            }
            else if(APP_ADV == ke_state_get(TASK_APP))
            {
                usr_led1_set(LED_ON_DUR_ADV_SLOW, LED_OFF_DUR_ADV_SLOW);
            }
            break;

        case GAP_ADV_REQ_CMP_EVT:
            usr_led1_set(LED_ON_DUR_IDLE, LED_OFF_DUR_IDLE);
            ke_timer_clear(APP_ADV_INTV_UPDATE_TIMER, TASK_APP);
            break;

        case GAP_DISCON_CMP_EVT:
            usr_led1_set(LED_ON_DUR_IDLE, LED_OFF_DUR_IDLE);

            // start adv
//            app_gap_adv_start_req(GAP_GEN_DISCOVERABLE|GAP_UND_CONNECTABLE,
//                    app_env.adv_data, app_set_adv_data(GAP_GEN_DISCOVERABLE),
//                    app_env.scanrsp_data, app_set_scan_rsp_data(app_get_local_service_flag()),
//                    GAP_ADV_FAST_INTV1, GAP_ADV_FAST_INTV2);
            Mibeacon_Advertise();
            break;

        case GAP_LE_CREATE_CONN_REQ_CMP_EVT:
            if(((struct gap_le_create_conn_req_cmp_evt *)param)->conn_info.status == CO_ERROR_NO_ERROR)
            {
                if(GAP_PERIPHERAL_SLV == app_get_role())
                {
                    ke_timer_clear(APP_ADV_INTV_UPDATE_TIMER, TASK_APP);
                    usr_led1_set(LED_ON_DUR_CON, LED_OFF_DUR_CON);  
                }
            }
            break;

        default:
            break;
    }
    
    mible_Callback(msgid, param);
}

/**
 ****************************************************************************************
 * @brief Handles LED status timer.
 *
 * @param[in] msgid      APP_SYS_UART_DATA_IND
 * @param[in] param      Pointer to struct app_uart_data_ind
 * @param[in] dest_id    TASK_APP
 * @param[in] src_id     TASK_APP
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_led_timer_handler(ke_msg_id_t const msgid, void const *param,
                               ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    if(msgid == APP_SYS_LED_1_TIMER)
    {
        usr_led1_process();
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles advertising mode timer event.
 *
 * @param[in] msgid     APP_ADV_INTV_UPDATE_TIMER
 * @param[in] param     None
 * @param[in] dest_id   TASK_APP
 * @param[in] src_id    TASK_APP
 *
 * @return If the message was consumed or not.
 * @description
 *
 * This handler is used to inform the application that first phase of adversting mode is timeout.
 ****************************************************************************************
 */
int app_gap_adv_intv_update_timer_handler(ke_msg_id_t const msgid, void const *param,
                                          ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    if(APP_ADV == ke_state_get(TASK_APP))
    {
        usr_led1_set(LED_ON_DUR_IDLE, LED_OFF_DUR_IDLE);

        // Update Advertising Parameters
//        app_gap_adv_start_req(GAP_GEN_DISCOVERABLE|GAP_UND_CONNECTABLE, 
//                                app_env.adv_data, app_set_adv_data(GAP_GEN_DISCOVERABLE), 
//                                app_env.scanrsp_data, app_set_scan_rsp_data(app_get_local_service_flag()),
//                                GAP_ADV_SLOW_INTV, GAP_ADV_SLOW_INTV);
        Mibeacon_Advertise();
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief   Restore peripheral setting after wakeup
 ****************************************************************************************
 */
void usr_sleep_restore(void)
{
#if QN_DBG_PRINT
    uart_init(QN_DEBUG_UART, USARTx_CLK(0), UART_9600);
    uart_tx_enable(QN_DEBUG_UART, MASK_ENABLE);
    uart_rx_enable(QN_DEBUG_UART, MASK_ENABLE);
#endif
}

/**
 ****************************************************************************************
 * @brief Handles button press after cancel the jitter.
 *
 * @param[in] msgid     Id of the message received
 * @param[in] param     None
 * @param[in] dest_id   TASK_APP
 * @param[in] src_id    TASK_APP
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
int app_button_timer_handler(ke_msg_id_t const msgid, void const *param,
                               ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case APP_SYS_BUTTON_1_TIMER:
            // make sure the button is pressed
            if(gpio_read_pin(BUTTON1_PIN) == GPIO_LOW)
            {
                if(APP_IDLE == ke_state_get(TASK_APP))
                {
                    struct app_mis_env_tag *app_mis_env = &app_env.mis_ev;
                    if(!app_mis_env->enabled)
                    {
                        // start adv
//                        app_gap_adv_start_req(GAP_GEN_DISCOVERABLE|GAP_UND_CONNECTABLE,
//                                app_env.adv_data, app_set_adv_data(GAP_GEN_DISCOVERABLE),
//                                app_env.scanrsp_data, app_set_scan_rsp_data(app_get_local_service_flag()),
//                                GAP_ADV_FAST_INTV1, GAP_ADV_FAST_INTV2);
                        Mibeacon_Advertise();

#if (QN_DEEP_SLEEP_EN)
                        // prevent entering into deep sleep mode
                        sleep_set_pm(PM_SLEEP);
#endif
                    }
                }
                else if(APP_ADV == ke_state_get(TASK_APP))
                {
                    // stop adv
                    //app_gap_adv_stop_req();
                    mible_gap_adv_stop();

#if (QN_DEEP_SLEEP_EN)
                    // allow entering into deep sleep mode
                    sleep_set_pm(PM_DEEP_SLEEP);
#endif
                }
            }
            break;

        default:
            ASSERT_ERR(0);
            break;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles button press before key debounce.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
void app_event_button1_press_handler(void)
{
#if ((QN_DEEP_SLEEP_EN) && (!QN_32K_RCO))
    if (sleep_env.deep_sleep) 
    {
        sleep_env.deep_sleep = false;
        // start 32k xtal wakeup timer
        wakeup_32k_xtal_start_timer();
    }
#endif

    // delay 20ms to debounce
    ke_timer_set(APP_SYS_BUTTON_1_TIMER, TASK_APP, 2);
    ke_evt_clear(1UL << EVENT_BUTTON1_PRESS_ID);
}

/**
 ****************************************************************************************
 * @brief   Button 1 click callback
 * @description
 *  Button 1 is used to enter adv mode.
 ****************************************************************************************
 */
void usr_button1_cb(void)
{
    // If BLE is in the sleep mode, wakeup it.
    if(ble_ext_wakeup_allow())
    {
#if ((QN_DEEP_SLEEP_EN) && (!QN_32K_RCO))
        if (sleep_env.deep_sleep)
        {
            wakeup_32k_xtal_switch_clk();
        }
#endif

        sw_wakeup_ble_hw();

// #if (QN_DEEP_SLEEP_EN)
//         // prevent deep sleep
//         if(sleep_get_pm() == PM_DEEP_SLEEP)
//         {
//             sleep_set_pm(PM_SLEEP);
//         }
// #endif
    }

    // key debounce:
    // We can set a soft timer to debounce.
    // After wakeup BLE, the timer is not calibrated immediately and it is not precise.
    // So We set a event, in the event handle, set the soft timer.
    ke_evt_set(1UL << EVENT_BUTTON1_PRESS_ID);
}

/**
 ****************************************************************************************
 * @brief   All GPIO interrupt callback
 ****************************************************************************************
 */
void gpio_interrupt_callback(enum gpio_pin pin)
{
    switch(pin)
    {
        case BUTTON1_PIN:
            //Button 1 is used to enter adv mode.
            usr_button1_cb();
            break;

#if (defined(QN_TEST_CTRL_PIN))
        case QN_TEST_CTRL_PIN:
            //When test controll pin is changed to low level, this function will reboot system.
            gpio_disable_interrupt(QN_TEST_CTRL_PIN);
            syscon_SetCRSS(QN_SYSCON, SYSCON_MASK_REBOOT_SYS);
            break;
#endif

        default:
            break;
    }
}

/*app variable*/
device_info dev_info = {
    .bonding = WEAK_BONDING,
    .pid = 0x9c,
    .version = "0000",
};

/**
 ****************************************************************************************
 * @brief   User initialize
 ****************************************************************************************
 */
void usr_init(void)
{
    if(KE_EVENT_OK != ke_evt_callback_set(EVENT_BUTTON1_PRESS_ID, 
                                            app_event_button1_press_handler))
    {
        ASSERT_ERR(0);
    }

}

void mible_init()
{
    mible_server_info_init(&dev_info);
    mible_server_miservice_init();
    Mibeacon_Set_AdvData();
}

void Mibeacon_Advertise(void)
{
    mible_gap_adv_param_t adv_param =(mible_gap_adv_param_t){
        .adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .adv_interval_min = 0x0020,
        .adv_interval_max = 0x0020,
        .ch_mask = {0},
    };
    if(MI_SUCCESS != mible_gap_adv_start(&adv_param)){
        MI_LOG_ERROR("mible_gap_adv_start failed. \r\n");
        return;
    }
}


void Mibeacon_Set_AdvData(void)
{
    MI_LOG_INFO("advertising init...\n\r");
    mibeacon_frame_ctrl_t frame_ctrl = {
    .time_protocol = 0,
    .is_encrypt = 0,
    .mac_include = 1,
    .cap_include = 1,
    .obj_include = 0,
    .bond_confirm = 0,
    .version = 0x03,
    };
    mibeacon_capability_t cap = {.connectable = 1,
                                .encryptable = 1,
                                .bondAbility = 1};

    mible_addr_t dev_mac;
    mible_gap_address_get(dev_mac);
    
    mibeacon_config_t mibeacon_cfg = {
    .frame_ctrl = frame_ctrl,
    .pid =dev_info.pid,
    .p_mac = (mible_addr_t*)dev_mac, 
    .p_capability = &cap,
    .p_obj = NULL,
    };
    
    uint8_t service_data[31];
    uint8_t service_data_len=0;
    
    if(MI_SUCCESS != mible_service_data_set(&mibeacon_cfg, service_data, &service_data_len)){
        MI_LOG_ERROR("mibeacon_data_set failed. \r\n");
        return;
    }
    
    uint8_t adv_data[23]={0};
    uint8_t adv_len=0;
    // add flags
    adv_data[0] = 0x02;
    adv_data[1] = 0x01;
    adv_data[2] = 0x06;
    
    memcpy(adv_data+3, service_data, service_data_len);
    adv_len = service_data_len + 3;
    
    mible_gap_adv_data_set(adv_data,adv_len,NULL,0);
    
    MI_LOG_INFO("adv mi service data:");
    MI_LOG_HEXDUMP(adv_data, adv_len);
    MI_LOG_PRINTF("\r\n");
    return;
}




/// @} USR

