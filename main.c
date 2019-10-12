/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "boards.h"
//#include "softdevice_handler.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_qwr.h"

#include "app_timer.h"
//#include "fstorage.h"
#include "nrf_fstorage.h"
#include "fds.h"
#include "peer_manager.h"

#include "bsp.h"
#include "bsp_btn_ble.h"
#include "sensorsim.h"
#include "nrf_gpio.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"


#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "mi_psm.h"
//#include "mible_beacon.h"
//#undef  NRF_LOG_MODULE_NAME
//#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log_default_backends.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/*mible_std_authen*/
#include "mible_api.h"
#include "mible_log.h"
#include "mible_server.h"
#include "mible_beacon.h"
#include "mi_config.h"


#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1 

#define RTT_CTRL_CLEAR                  "[2J"
#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     "nRF_Demo"                                  /**< Name of device. Will be included in the advertising data. */
#define APP_ADV_INTERVAL                200                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      0                                           /**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000)//, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)//, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/
//APP_TIMER_DEF(m_poll_timer);
//APP_TIMER_DEF(m_bindconfirm_timer);
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

static void std_authen_event_cb(mible_std_auth_evt_t evt,
        mible_std_auth_evt_param_t* param);

#define REMOTE_CONTROL_DEMO 0

#if REMOTE_CONTROL_DEMO

static mible_op_mode_t MIBLE_FUNCTION = MODE_REMOTE;
#define BLE_GATEWAY_TEST 0
//Push Button2 -- FAST_PAIR Advertising last time -- 5s
#define ADV_FAST_PAIR_TIME 5000
//Push Button3/Button4 -- OBJECTS Advertising last time -- 0.5s
#define ADV_OBJECTS_TIME 500
//Advertising interval time -- 30ms
#define ADV_INTERVAL_TIME 20

static void* button_timer;
static void* fastpair_timer;

extern void mible_gap_address_set(void);

#endif

/*app variable*/
static device_info dev_info = {
        .bonding = WEAK_BONDING,//,STRONG_BONDING // can be modified according to product
        .pid = PRODUCT_ID,  // product id, can be modified according to product
        .version = "0001",  // can be modified according to product
};

/* YOUR_JOB: Declare all services structure your application is using
   static ble_yy_service_t                     m_yys;
 */

// YOUR_JOB: Use UUIDs for service(s) used in your application.

static void advertising_start(void);

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{

    // Initialize timer module.    
    //APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
       uint32_t err_code;
       err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
       APP_ERROR_CHECK(err_code); */
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the YYY Service events.
 * YOUR_JOB implement a service handler function depending on the event the service you are using can generate
 *
 * @details This function will be called for all YY Service events which are passed to
 *          the application.
 *
 * @param[in]   p_yy_service   YY Service structure.
 * @param[in]   p_evt          Event received from the YY Service.
 *
 *
   static void on_yys_evt(ble_yy_service_t     * p_yy_service,
                       ble_yy_service_evt_t * p_evt)
   {
    switch (p_evt->evt_type)
    {
        case BLE_YY_NAME_EVT_WRITE:
            APPL_LOG("[APPL]: charact written with value %s. \r\n", p_evt->params.char_xx.value.p_str);
            break;

        default:
            // No implementation needed.
            break;
    }
   }*/


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    ret_code_t         err_code;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);
    /* YOUR_JOB: Add code to initialize the services used by the application.
       uint32_t                           err_code;
       ble_yys_init_t                     yys_init;

       // Initialize YYY Service.
       memset(&yys_init, 0, sizeof(yys_init));
       yys_init.evt_handler                  = on_yys_evt;
       yys_init.ble_yy_initial_value.counter = 0;

       err_code = ble_yy_service_init(&yys_init, &yy_init);
       APP_ERROR_CHECK(err_code);*/

}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
//static void application_timers_start(void)
//{
//    /* YOUR_JOB: Start your timers. below is an example of how to start a timer.
//       uint32_t err_code;
//       err_code = app_timer_start(m_app_timer_id, TIMER_INTERVAL, NULL);
//       APP_ERROR_CHECK(err_code); */

//}


void mible_on_ble_evt(ble_evt_t const *p_ble_evt);


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    
    ret_code_t err_code = NRF_SUCCESS;
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");
            // LED indication will be changed when advertising starts.
            break;

        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }

    mible_on_ble_evt(p_ble_evt);

}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

#if REMOTE_CONTROL_DEMO
    // This is a demo. A real product must has a valid public address.
    //mible_gap_address_set();
#endif
}

#if REMOTE_CONTROL_DEMO
static void push_key_mibeacon(uint8_t key)
{
    mible_timer_stop(button_timer);
    
    mibeacon_obj_t pushObj = {.type = MI_STA_BUTTON,
                              .len = 3,
                              .val[0] = 0x00,
                              .val[1] = 0x00,
                              .val[2] = key,};
            
    uint32_t errno;

    uint8_t adv_data[31];
    uint8_t adv_dlen = 0;

    mibeacon_config_t beacon_cfg = {
            .frame_ctrl.version = 3,
            .frame_ctrl.is_encrypt = 1,
            .pid = dev_info.pid, //156, //beacon_nonce.pid,
            .p_obj = &pushObj,
            .obj_num = 1,
        };

    //mible_manu_data_set(&beacon_cfg, adv_data, &adv_dlen);
    mible_service_data_set(&beacon_cfg, adv_data, &adv_dlen);
    MI_LOG_HEXDUMP(adv_data, adv_dlen);
    MI_LOG_INFO("mibeacon event adv ...\n");
    errno = mible_gap_adv_data_set( adv_data, adv_dlen, NULL, 0);
    MI_ERR_CHECK(errno);
    
    mible_timer_start(button_timer, ADV_OBJECTS_TIME, NULL);
}

static void ble_fastpair_event(void)
{
    mible_timer_stop(fastpair_timer);
    
    MI_LOG_INFO("ble_fastpair advertising init...\n");
    mibeacon_frame_ctrl_t frame_ctrl = {
        .is_encrypt = 0,
        .mac_include = 1,
        .cap_include = 1,
        .obj_include = 1,
        .bond_confirm = 0,
        .version = 0x04,
    };
    mibeacon_capability_t cap = {.connectable = 1,
                                 .encryptable = 1,
                                 .bondAbility = 1};

    mible_addr_t dev_mac;
    mible_gap_address_get(dev_mac);
                                                                
    mibeacon_obj_t fastpair_obj = {.type = MI_EVT_SIMPLE_PAIR,
                                   .len = 2,
                                   .val[0] = 0x01,
                                   .val[1] = 0x10,};
    
    mibeacon_config_t mibeacon_cfg = {
        .frame_ctrl = frame_ctrl,
        .pid =dev_info.pid,
        .p_mac = (mible_addr_t*)dev_mac, 
        .p_capability = &cap,
        .p_obj = (mibeacon_obj_t*)&fastpair_obj,
        .obj_num = 1,
    };
    
    uint8_t service_data[31];
    uint8_t service_data_len = 0;
    
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
    
    MI_LOG_INFO("fastpair adv data:");
    MI_LOG_HEXDUMP(adv_data, adv_len);
    MI_PRINTF("\r\n");
        
    mible_timer_start(fastpair_timer, ADV_FAST_PAIR_TIME, NULL);
        
    return;
}
#endif

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
static void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    static uint8_t battery;
    switch (event)
    {
        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break; // BSP_EVENT_DISCONNECT



#if REMOTE_CONTROL_DEMO				
        case BSP_EVENT_KEY_0:
                MI_LOG_INFO("\nBSP_EVENT_KEY_0\n");
                break;
        case BSP_EVENT_KEY_1:
                MI_LOG_INFO("\nBSP_EVENT_KEY_1\n");    
                ble_fastpair_event();
                break;
        case BSP_EVENT_KEY_2:            
                MI_LOG_INFO("\nBSP_EVENT_KEY_2\n");
                push_key_mibeacon(2);
                break;
        case BSP_EVENT_KEY_3:
                MI_LOG_INFO("\nBSP_EVENT_KEY_3\n");
                push_key_mibeacon(3);
                break;
#else
        case BSP_EVENT_KEY_0:
            MI_LOG_WARNING("mibeacon seqnum reset\n");
            test_mibeacon_sn_reset();
            break;

        case BSP_EVENT_KEY_1:
            battery = battery < 100 ? battery + 1 : 0;
            test_mibeacon_obj_enque(MI_STA_BATTERY, sizeof(battery), &battery, 4);
            MI_LOG_WARNING("enque cipher mibeacon v4 contains battery state.\n");
            break;

        case BSP_EVENT_KEY_2:
            battery = battery < 100 ? battery + 1 : 0;
            test_mibeacon_obj_enque(MI_STA_BATTERY, sizeof(battery), &battery, 5);
            MI_LOG_WARNING("enque cipher mibeacon v5 contains battery state.\n");
            break;

        case BSP_EVENT_KEY_3:
            battery = battery < 100 ? battery + 1 : 0;
            test_mibeacon_plain_obj_enque(MI_STA_BATTERY, sizeof(battery), &battery);
            MI_LOG_WARNING("enque plain mibeacon v5 contains battery state.\n");
            break;
#endif
        default:
            break;
    }
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    MI_LOG_INFO("advertising init...\n");
    mibeacon_frame_ctrl_t frame_ctrl = {
    //.time_protocol = 0,
    .is_encrypt = 0,
    .mac_include = 1,
    .cap_include = 1,
    .obj_include = 0,
    .bond_confirm = 0,
    .version = 0x04,
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
    MI_PRINTF("\r\n");
    return;
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    uint32_t err_code;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, NULL);
    APP_ERROR_CHECK(err_code);

    /* assign BUTTON 1 to reset, for more details to check bsp_event_handler()*/
    err_code = bsp_event_to_button_action_assign(0,
                                             BSP_BUTTON_ACTION_PUSH,
                                             BSP_EVENT_KEY_0);
    APP_ERROR_CHECK(err_code);

    /* assign BUTTON 2 to invoke mibeacon v4, for more details to check bsp_event_handler()*/
    err_code = bsp_event_to_button_action_assign(1,
                                             BSP_BUTTON_ACTION_PUSH,
                                             BSP_EVENT_KEY_1);
    APP_ERROR_CHECK(err_code);

    /* assign BUTTON 3 to invoke mibeacon v5, for more details to check bsp_event_handler()*/
    err_code = bsp_event_to_button_action_assign(2,
                                             BSP_BUTTON_ACTION_PUSH,
                                             BSP_EVENT_KEY_2);
    APP_ERROR_CHECK(err_code);

    /* assign BUTTON 4 to invoke mibeacon v5 encrypt, for more details to check bsp_event_handler()*/
    err_code = bsp_event_to_button_action_assign(3,
                                             BSP_BUTTON_ACTION_PUSH,
                                             BSP_EVENT_KEY_3);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Power manager.
*/ 
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
void advertising_start(void){
    mible_gap_adv_param_t adv_param =(mible_gap_adv_param_t){
        .adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
#if REMOTE_CONTROL_DEMO
        //Advertising interval in 625 us units.
        .adv_interval_min = ADV_INTERVAL_TIME/0.625, //MSEC_TO_UNITS(100, UNIT_0_625_MS),
        .adv_interval_max = 50/0.625,                //MSEC_TO_UNITS(200, UNIT_0_625_MS),
#else
        .adv_interval_min = MSEC_TO_UNITS(100, UNIT_0_625_MS),
        .adv_interval_max = MSEC_TO_UNITS(200, UNIT_0_625_MS),
#endif
        .ch_mask = {0},
    };
    if(MI_SUCCESS != mible_gap_adv_start(&adv_param)){
        MI_LOG_ERROR("mible_gap_adv_start failed. \r\n");
        return;
    }
}

void time_init(struct tm * time_ptr);

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for application main entry.
 */
int main(void)
{
    bool erase_bonds;

    // Initialize.
    log_init();
    timers_init();
    MI_LOG_INFO(RTT_CTRL_CLEAR"Compiled  %s %s\n", (uint32_t)__DATE__, (uint32_t)__TIME__);
        
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    time_init(NULL);
    buttons_leds_init(&erase_bonds);  
    conn_params_init();
    
    mible_std_auth_evt_register(std_authen_event_cb);

    /* <!> mible_record_create() must be called after ble_stack_init(). */
    mible_record_create(0xBEEF, 0);
    services_init();
	
    int is_registered = mible_server_info_init(&dev_info, MODE_STANDARD);
    mible_server_miservice_init();
    
    // Start execution.
    //application_timers_start();
    advertising_init();
    advertising_start();
		
#if REMOTE_CONTROL_DEMO
    mible_status_t ret = mible_timer_create(&button_timer, (mible_handler_t)advertising_init, MIBLE_TIMER_SINGLE_SHOT);
    if(ret != MI_SUCCESS){
        MI_LOG_ERROR("button_timer_create failed. code = %x .\r\n",ret);
    }else{
        MI_LOG_DEBUG("button_timer_create success. \r\n");
    }      		
		
    ret = mible_timer_create(&fastpair_timer, (mible_handler_t)advertising_init, MIBLE_TIMER_SINGLE_SHOT);
    if(ret != MI_SUCCESS){
        MI_LOG_ERROR("fastpair_timer_create failed. code = %x .\r\n",ret);
    }else{
        MI_LOG_DEBUG("fastpair_timer_create success. \r\n");
		}
#endif

    // Enter main loop.
    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();            
            mible_tasks_exec();
        }
    }
}

void mible_service_init_cmp(void)
{
    MI_LOG_INFO("mible_service_init_cmp\r\n");
}

void mible_connected(void)
{
    MI_LOG_INFO("mible_connected \r\n");
}

void mible_disconnected(void)
{
    MI_LOG_INFO("mible_disconnected \r\n");
    //advertising_init();
    advertising_start();
}

void mible_bonding_evt_callback(mible_bonding_state state)
{
    if(state == BONDING_FAIL){
        MI_LOG_INFO("BONDING_FAIL\r\n");
        mible_gap_disconnect(mible_server_connection_handle);
    }else if(state == BONDING_SUCC){
        MI_LOG_INFO("BONDING_SUCC\r\n");
    }else if(state == LOGIN_FAIL){
        MI_LOG_INFO("LOGIN_FAIL\r\n");
        mible_gap_disconnect(mible_server_connection_handle);
    }else{
        MI_LOG_INFO("LOGIN_SUCC\r\n");
    }
}

void std_authen_event_cb(mible_std_auth_evt_t evt,
        mible_std_auth_evt_param_t* p_param)
{
    switch(evt){
    case MIBLE_STD_AUTH_EVT_SERVICE_INIT_CMP:
        mible_service_init_cmp();
        break;
    case MIBLE_STD_AUTH_EVT_CONNECT:
        mible_gap_adv_stop();
        mible_connected();
        break;
    case MIBLE_STD_AUTH_EVT_DISCONNECT:
        mible_disconnected();
        break;
    case MIBLE_STD_AUTH_EVT_RESULT:
        mible_bonding_evt_callback(p_param->result.state);
        break;
    default:
        MI_LOG_ERROR("Unkown std authen event\r\n");
        break;
    }
}
/**
 * @}
 */
