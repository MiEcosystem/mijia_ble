/*
 * Copyright 2018, Cypress Semiconductor Corporation or a subsidiary of Cypress Semiconductor 
 *  Corporation. All rights reserved. This software, including source code, documentation and  related 
 * materials ("Software"), is owned by Cypress Semiconductor  Corporation or one of its 
 *  subsidiaries ("Cypress") and is protected by and subject to worldwide patent protection  
 * (United States and foreign), United States copyright laws and international treaty provisions. 
 * Therefore, you may use this Software only as provided in the license agreement accompanying the 
 * software package from which you obtained this Software ("EULA"). If no EULA applies, Cypress 
 * hereby grants you a personal, nonexclusive, non-transferable license to  copy, modify, and 
 * compile the Software source code solely for use in connection with Cypress's  integrated circuit 
 * products. Any reproduction, modification, translation, compilation,  or representation of this 
 * Software except as specified above is prohibited without the express written permission of 
 * Cypress. Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO  WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING,  BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE. Cypress reserves the right to make changes to 
 * the Software without notice. Cypress does not assume any liability arising out of the application 
 * or use of the Software or any product or circuit  described in the Software. Cypress does 
 * not authorize its products for use in any products where a malfunction or failure of the 
 * Cypress product may reasonably be expected to result  in significant property damage, injury 
 * or death ("High Risk Product"). By including Cypress's product in a High Risk Product, the 
 *  manufacturer of such system or application assumes  all risk of such use and in doing so agrees 
 * to indemnify Cypress against all liability.
 */

/** @file
 *
 * BLE Vendor Specific Device
 *
 * During initialization the app registers with LE stack to receive variousdev_info
 * notifications including bonding complete, connection status change and
 * peer write.  When device is successfully bonded, application saves
 * peer's Bluetooth Device address to the NVRAM.  Bonded device can also
 * write in to client configuration descriptor of the notification
 * characteristic.  That is also saved in the NVRAM.  When user pushes the
 * button, notification/indication is sent to the bonded and registered host.
 * User can also write to the characteristic configuration value. Number
 * of LED blinks indicates the value written
 *
 *
 * Features demonstrated
 *  - GATT database and Device configuration initialization
 *  - Registration with LE stack for various events
 *  - NVRAM read/write operation
 *  - Sending data to the client
 *  - Processing write requests from the client
 *
 * To demonstrate the app, work through the following steps.
 * 1. Plug the WICED eval board into your computer
 * 2. Build and download the application (to the WICED board)
 * 3. Pair with client.
 * 4. On the client side register for notifications
 * 5. Push a button on the tag to send notifications to the client
 * 6. Write the hello sensor characteristic configuration value from client
 * 7. Number of LED blinks on hello sensor indicates value written by client
 */
 
#include "sparcommon.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_cfg.h"
#include "wiced_hal_gpio.h"
#include "wiced_bt_uuid.h"
#include "wiced_gki.h"
#include "wiced_bt_app_common.h"
#include "wiced_bt_app_hal_common.h"
#include "wiced_result.h"
#include "wiced_hal_platform.h"
#include "wiced_memory.h"
#include "wiced_transport.h"
#include "wiced_bt_stack.h"
#include "mible_std_authen.h"

#ifdef  WICED_BT_TRACE_ENABLE
#include "wiced_bt_trace.h"
#endif

#include "mible_api.h"
#include "mible_wiced.h"


/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define HELLO_SENSOR_GATTS_MAX_CONN     1


/******************************************************************************
 *                                Structures
 ******************************************************************************/
typedef struct
{
    BD_ADDR   remote_addr;              // remote peer device address
    uint32_t  timer_count;              // timer count
    uint32_t  fine_timer_count;         // fine timer count
    uint16_t  conn_id;                  // connection ID referenced by the stack
    uint16_t  peer_mtu;                 // peer MTU
    uint8_t   num_to_write;             // num msgs to send, incr on each button intr
    uint8_t   flag_indication_sent;     // indicates waiting for ack/cfm
    uint8_t   flag_stay_connected;      // stay connected or disconnect after all messages are sent
    uint8_t   battery_level;            // dummy battery level
} hello_sensor_state_t;

#pragma pack(1)
/* Host information saved in  NVRAM */
typedef PACKED struct
{
    BD_ADDR  bdaddr;                                /* BD address of the bonded host */
    uint16_t  characteristic_client_configuration;  /* Current value of the client configuration descriptor */
    uint8_t   number_of_blinks;                     /* Sensor config, number of times to blink the LEd when button is pushed. */
} host_info_t;
#pragma pack()

typedef struct
{
    uint16_t handle;
    uint16_t attr_len;
    void     *p_attr;
} attribute_t;

extern const wiced_bt_cfg_settings_t wiced_bt_cfg_settings;
extern const wiced_bt_cfg_buf_pool_t wiced_bt_cfg_buf_pools[];


/******************************************************************************
 *                                Variables Definitions
 ******************************************************************************/
/*
 * This is the GATT database for the Hello Sensor application.  It defines
 * services, characteristics and descriptors supported by the sensor.  Each
 * attribute in the database has a handle, (characteristic has two, one for
 * characteristic itself, another for the value).  The handles are used by
 * the peer to access attributes, and can be used locally by application for
 * example to retrieve data written by the peer.  Definition of characteristics
 * and descriptors has GATT Properties (read, write, notify...) but also has
 * permissions which identify if and how peer is allowed to read or write
 * into it.  All handles do not need to be sequential, but need to be in
 * ascending order.
 */
const uint8_t hello_sensor_gatt_database[]=
{
    // Declare mandatory GATT service
    PRIMARY_SERVICE_UUID16( HANDLE_HSENS_GATT_SERVICE, UUID_SERVICE_GATT ),

    // Declare mandatory GAP service. Device Name and Appearance are mandatory
    // characteristics of GAP service
    PRIMARY_SERVICE_UUID16( HANDLE_HSENS_GAP_SERVICE, UUID_SERVICE_GAP ),

        // Declare mandatory GAP service characteristic: Dev Name
        CHARACTERISTIC_UUID16( HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_NAME, HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_NAME_VAL,
            UUID_CHARACTERISTIC_DEVICE_NAME, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

        // Declare mandatory GAP service characteristic: Appearance
        CHARACTERISTIC_UUID16( HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_APPEARANCE, HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_APPEARANCE_VAL,
            UUID_CHARACTERISTIC_APPEARANCE, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    // Declare proprietary Hello Service with 128 byte UUID
    PRIMARY_SERVICE_UUID128( HANDLE_HSENS_SERVICE, UUID_HELLO_SERVICE ),

        // Declare characteristic used to notify/indicate change
        CHARACTERISTIC_UUID128( HANDLE_HSENS_SERVICE_CHAR_NOTIFY, HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL,
            UUID_HELLO_CHARACTERISTIC_NOTIFY, LEGATTDB_CHAR_PROP_READ | LEGATTDB_CHAR_PROP_NOTIFY | LEGATTDB_CHAR_PROP_INDICATE, LEGATTDB_PERM_READABLE ),

            // Declare client characteristic configuration descriptor
            // Value of the descriptor can be modified by the client
            // Value modified shall be retained during connection and across connection
            // for bonded devices.  Setting value to 1 tells this application to send notification
            // when value of the characteristic changes.  Value 2 is to allow indications.
            CHAR_DESCRIPTOR_UUID16_WRITABLE( HANDLE_HSENS_SERVICE_CHAR_CFG_DESC, UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
            LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_REQ | LEGATTDB_PERM_AUTH_READABLE | LEGATTDB_PERM_AUTH_WRITABLE),

        // Declare characteristic Hello Configuration
        // The configuration consists of 1 bytes which indicates how many times to
        // blink the LED when user pushes the button.
        CHARACTERISTIC_UUID128_WRITABLE( HANDLE_HSENS_SERVICE_CHAR_BLINK, HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL,
            UUID_HELLO_CHARACTERISTIC_CONFIG, LEGATTDB_CHAR_PROP_READ | LEGATTDB_CHAR_PROP_WRITE,
            LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ ),

    // Declare Device info service
    PRIMARY_SERVICE_UUID16( HANDLE_HSENS_DEV_INFO_SERVICE, UUID_SERVICE_DEVICE_INFORMATION ),

        // Handle 0x4e: characteristic Manufacturer Name, handle 0x4f characteristic value
        CHARACTERISTIC_UUID16( HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MFR_NAME, HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MFR_NAME_VAL,
            UUID_CHARACTERISTIC_MANUFACTURER_NAME_STRING, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

        // Handle 0x50: characteristic Model Number, handle 0x51 characteristic value
        CHARACTERISTIC_UUID16( HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MODEL_NUM, HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MODEL_NUM_VAL,
            UUID_CHARACTERISTIC_MODEL_NUMBER_STRING, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

        // Handle 0x52: characteristic System ID, handle 0x53 characteristic value
        CHARACTERISTIC_UUID16( HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_SYSTEM_ID, HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_SYSTEM_ID_VAL,
            UUID_CHARACTERISTIC_SYSTEM_ID, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE ),

    // Declare Battery service
    PRIMARY_SERVICE_UUID16( HANDLE_HSENS_BATTERY_SERVICE, 0x180F ),

        // Handle 0x62: characteristic Battery Level, handle 0x63 characteristic value
        CHARACTERISTIC_UUID16( HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL, HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL_VAL,
            UUID_CHARACTERISTIC_BATTERY_LEVEL, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE),

	
    //xiao mi gatt database
	PRIMARY_SERVICE_UUID16(SQUIRREL_IDX_SVC, UUID_SERVICE_SQUIRREL),


   //TOKEN UUID
   CHARACTERISTIC_UUID16_WRITABLE(MI_IDX_TOKEN_CHAR, MI_IDX_TOKEN_VAL, UUID_CHARACTERISTIC_TOKEN,
								LEGATTDB_CHAR_PROP_NOTIFY|LEGATTDB_CHAR_PROP_WRITE, LEGATTDB_PERM_WRITE_CMD|LEGATTDB_PERM_WRITE_REQ|LEGATTDB_PERM_READABLE),
	//CHAR_DESCRIPTOR_UUID16_WRITABLE
   CHAR_DESCRIPTOR_UUID16_WRITABLE( MI_IDX_TOKEN_CFG, UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION,
										   LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_REQ),

   // PRODUCT ID UUID
   CHARACTERISTIC_UUID16(MI_IDX_PRODUCT_ID_CHAR, MI_IDX_PRODUCT_ID_VAL, UUID_CHARACTERISTIC_PRODUCT_ID,
						  LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE),
	//VERSION UUID
   CHARACTERISTIC_UUID16(MI_IDX_VERSION_ID_CHAR, MI_IDX_VERSION_ID_VAL, UUID_CHARACTERISTIC_VERSION,
						 LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE),
  
	//BEACON KEY UUID
   CHARACTERISTIC_UUID16(MI_IDX_BEACON_KEY_CHAR, MI_IDX_BEACON_KEY_VAL, UUID_CHARACTERISTIC_BEACON_KEY,
									   LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE),
   //DEVICE ID UUID
   CHARACTERISTIC_UUID16_WRITABLE( MI_IDX_DEVICE_ID_CHAR,MI_IDX_DEVICE_ID_VAL, UUID_CHARACTERISTIC_DEVICE_ID,
		   LEGATTDB_CHAR_PROP_READ| LEGATTDB_CHAR_PROP_WRITE, LEGATTDB_PERM_READABLE|LEGATTDB_PERM_WRITE_REQ|LEGATTDB_PERM_WRITE_CMD),

  //AUTHENTICATION UUID
   CHARACTERISTIC_UUID16_WRITABLE(MI_IDX_AUTHENTICATION_CHAR,MI_IDX_AUTHENTICATION_VAL,UUID_CHARACTERISTIC_AUTHENTICATION,
						 LEGATTDB_CHAR_PROP_WRITE, LEGATTDB_PERM_WRITE_REQ|LEGATTDB_PERM_WRITE_CMD),


	//WiFi CFG UUID
   CHARACTERISTIC_UUID16_WRITABLE(MI_IDX_WIFICFG_CHAR,MI_IDX_WIFICFG_VAL,UUID_CHARACTERISTIC_WIFICFG,
						 LEGATTDB_CHAR_PROP_WRITE, LEGATTDB_PERM_WRITE_REQ|LEGATTDB_PERM_WRITE_CMD),

};


uint8_t hello_sensor_device_name[]          = "MibleDemo";												//GAP Service characteristic Device Name
uint8_t hello_sensor_appearance_name[2]     = { BIT16_TO_8(APPEARANCE_GENERIC_TAG) };
char    hello_sensor_char_notify_value[]    = "MibleDemo 0"; 											//Notification Name
char    hello_sensor_char_mfr_name_value[]  = { 'C', 'y', 'p', 'r', 'e', 's', 's', 0, };
char    hello_sensor_char_model_num_value[] = { '1', '2', '3', '4',   0,   0,   0,   0 };
uint8_t hello_sensor_char_system_id_value[] = { 0xbb, 0xb8, 0xa1, 0x80, 0x5f, 0x9f, 0x91, 0x71};

/* Holds the global state of the hello sensor application */
hello_sensor_state_t hello_sensor_state;

/* Holds the host info saved in the NVRAM */
host_info_t hello_sensor_hostinfo;

/* Attribute list of the hello sensor */
attribute_t gauAttributes[] =
{
    { HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_NAME_VAL,       sizeof( hello_sensor_device_name ),         hello_sensor_device_name },
    { HANDLE_HSENS_GAP_SERVICE_CHAR_DEV_APPEARANCE_VAL, sizeof(hello_sensor_appearance_name),       hello_sensor_appearance_name },
    { HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL,             sizeof(hello_sensor_char_notify_value),     hello_sensor_char_notify_value },
    //{ HANDLE_HSENS_SERVICE_CHAR_CFG_DESC,               2,                                          (void*)&hello_sensor_hostinfo.characteristic_client_configuration },
	{ MI_IDX_TOKEN_CFG,				2,											(void*)&hello_sensor_hostinfo.characteristic_client_configuration },

	{ HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL,              1,                                          &hello_sensor_hostinfo.number_of_blinks },
    { HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MFR_NAME_VAL,  sizeof(hello_sensor_char_mfr_name_value),   hello_sensor_char_mfr_name_value },
    { HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_MODEL_NUM_VAL, sizeof(hello_sensor_char_model_num_value),  hello_sensor_char_model_num_value },
    { HANDLE_HSENS_DEV_INFO_SERVICE_CHAR_SYSTEM_ID_VAL, sizeof(hello_sensor_char_system_id_value),  hello_sensor_char_system_id_value },
    { HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL_VAL,      1,                                          &hello_sensor_state.battery_level },
};

/* transport configuration */
const wiced_transport_cfg_t  transport_cfg =
{
    WICED_TRANSPORT_UART,
    { WICED_TRANSPORT_UART_HCI_MODE, HCI_UART_DEFAULT_BAUD },
    { 0, 0},
    NULL,
    NULL,
    NULL
};

static wiced_result_t           hello_sensor_management_cback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data );
static wiced_bt_gatt_status_t   hello_sensor_gatts_callback( wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_data);
#ifdef ENABLE_HCI_TRACE
static void                     hello_sensor_hci_trace_cback( wiced_bt_hci_trace_type_t type, uint16_t length, uint8_t* p_data );
#endif
static void                     hello_sensor_set_advertisement_data( void );
static void                     hello_sensor_send_message( void );
static void                     hello_sensor_gatts_increment_notify_value( void );
static void                     hello_sensor_timeout( uint32_t count );
static void                     hello_sensor_fine_timeout( uint32_t finecount );
static void                     hello_sensor_smp_bond_result( uint8_t result );
static void                     hello_sensor_encryption_changed( wiced_result_t result, uint8_t* bd_addr );
static void                     hello_sensor_interrupt_handler( void* user_data, uint8_t pin );
static void                     hello_sensor_application_init( void );
static void                     hello_sensor_conn_idle_timeout ( uint32_t arg );
static void                     hello_sensor_load_keys_for_address_resolution( void );

/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/
static void std_authen_event_cb(mible_std_auth_evt_t evt,
        mible_std_auth_evt_param_t* param);
/*
 *  Entry point to the application. Set device configuration and start BT
 *  stack initialization.  The actual application initialization will happen
 *  when stack reports that BT device is ready.
 */
APPLICATION_START( )
{
    wiced_transport_init( &transport_cfg );

#ifdef WICED_BT_TRACE_ENABLE
    // Set the debug uart as WICED_ROUTE_DEBUG_NONE to get rid of prints
    // wiced_set_debug_uart(WICED_ROUTE_DEBUG_NONE);

    // Set to PUART to see traces on peripheral uart(puart)
    wiced_set_debug_uart( WICED_ROUTE_DEBUG_TO_PUART );
    wiced_hal_puart_select_uart_pads( WICED_PUART_RXD, WICED_PUART_TXD, 0, 0);

    // Set to HCI to see traces on HCI uart - default if no call to wiced_set_debug_uart()
    // wiced_set_debug_uart( WICED_ROUTE_DEBUG_TO_HCI_UART );

    // Use WICED_ROUTE_DEBUG_TO_WICED_UART to send formatted debug strings over the WICED
    // HCI debug interface to be parsed by ClientControl/BtSpy.
    // wiced_set_debug_uart(WICED_ROUTE_DEBUG_TO_WICED_UART);
#endif

    WICED_BT_TRACE( "Hello Sensor Start\n" );
	mijia_gatt_db_init();

    // Register call back and configuration with stack
    wiced_bt_stack_init( hello_sensor_management_cback ,
                    &wiced_bt_cfg_settings, wiced_bt_cfg_buf_pools );
}

/*
 * This function is executed in the BTM_ENABLED_EVT management callback.
 */
void hello_sensor_application_init( void )
{
    wiced_bt_gatt_status_t gatt_status;
    wiced_result_t         result;

    /* Initialize wiced app */
    wiced_bt_app_init();
    //wiced_bt_app_led_init();

    /* Configure buttons available on the platform (pin should be configured before registering interrupt handler ) */
    wiced_hal_gpio_configure_pin( WICED_GPIO_BUTTON, WICED_GPIO_BUTTON_SETTINGS( GPIO_EN_INT_RISING_EDGE ), WICED_GPIO_BUTTON_DEFAULT_STATE );
    wiced_hal_gpio_register_pin_for_interrupt( WICED_GPIO_BUTTON, hello_sensor_interrupt_handler, NULL );

    /* Register with stack to receive GATT callback */
    gatt_status = wiced_bt_gatt_register( hello_sensor_gatts_callback );

    WICED_BT_TRACE( "wiced_bt_gatt_register: %d\n", gatt_status );

    /*  Tell stack to use our GATT databse */
    gatt_status =  wiced_bt_gatt_db_init( hello_sensor_gatt_database, sizeof(hello_sensor_gatt_database) );

    WICED_BT_TRACE("wiced_bt_gatt_db_init %d\n", gatt_status);

#ifdef ENABLE_HCI_TRACE
    /* Register callback for receiving hci traces */
    wiced_bt_dev_register_hci_trace( hello_sensor_hci_trace_cback );
#endif

    /* Starting the app timers , seconds timer and the ms timer  */
    wiced_bt_app_start_timer( HELLO_SENSOR_APP_TIMEOUT_IN_SECONDS, HELLO_SENSOR_APP_FINE_TIMEOUT_IN_MS,
        hello_sensor_timeout, hello_sensor_fine_timeout );

    /* Enable privacy to advertise with RPA */
    //wiced_bt_ble_enable_privacy ( WICED_TRUE );

    /* Load the address resolution DB with the keys stored in the NVRAM */
    hello_sensor_load_keys_for_address_resolution();

    /* Set the advertising params and make the device discoverable */
#if 0
    hello_sensor_set_advertisement_data();

    result =  wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
#else
	mible_server_info_init(&dev_info, MODE_STANDARD);
	mible_server_miservice_init();
	mible_std_auth_evt_register(std_authen_event_cb);
	advertising_init();
    advertising_start();
#endif
	
    WICED_BT_TRACE( "wiced_bt_start_advertisements %d\n", result );

    /*
     * Set flag_stay_connected to remain connected after all messages are sent
     * Reset flag to 0, to disconnect
     */
    hello_sensor_state.flag_stay_connected = 1;
}

/*
 *  Pass protocol traces up through the UART
 */
#ifdef ENABLE_HCI_TRACE
void hello_sensor_hci_trace_cback( wiced_bt_hci_trace_type_t type, uint16_t length, uint8_t* p_data )
{
    //send the trace
    wiced_transport_send_hci_trace( NULL, type, length, p_data  );
}
#endif

/*
 * Setup advertisement data with 16 byte UUID and device name
 */
void hello_sensor_set_advertisement_data(void)
{
    wiced_bt_ble_advert_elem_t adv_elem[4];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    /*
     * hci_control_le_local_name - Advertising Complete Name
     * Note : Max Length is 8 bytes for the Advertisement Name, rest of 21 bytes are used for BTM_BLE_ADVERT_TYPE_FLAG,BTM_BLE_ADVERT_TYPE_128SRV_COMPLETE
     */
    uint8_t  hci_control_le_local_name[]      =   "MibleDemo" ; //Alternate way to declare {'h', 'e', 'l', 'l', 'o', 0x00, 0x00};
    uint8_t hello_service_uuid[LEN_UUID_128] = { UUID_HELLO_SERVICE };

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_128SRV_COMPLETE;
    adv_elem[num_elem].len          = sizeof(hello_service_uuid);
    adv_elem[num_elem].p_data       = ( uint8_t* )hello_service_uuid;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen(hci_control_le_local_name);
    adv_elem[num_elem].p_data       = ( uint8_t* )hci_control_le_local_name;
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);
}

/*
 * This function is invoked when advertisements stop.  If we are configured to stay connected,
 * disconnection was caused by the peer, start low advertisements, so that peer can connect
 * when it wakes up
 */
void hello_sensor_advertisement_stopped( void )
{
    wiced_result_t result;

    if ( hello_sensor_state.flag_stay_connected && !hello_sensor_state.conn_id )
    {
        result =  wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_LOW, 0, NULL );
        WICED_BT_TRACE( "wiced_bt_start_advertisements: %d\n", result );
    }
    else
    {
        WICED_BT_TRACE( "ADV stop\n");
    }
}

/* 
 * The function invoked on timeout of app seconds timer.
 */
void hello_sensor_timeout( uint32_t count )
{
    hello_sensor_state.timer_count++;
    // print for first 10 seconds, then once every 10 seconds thereafter
    if ((hello_sensor_state.timer_count <= 10) || (hello_sensor_state.timer_count % 10 == 0))
        WICED_BT_TRACE("hello_sensor_timeout: %d\n", hello_sensor_state.timer_count);
}

/* 
 * The function invoked on timeout of app milliseconds fine timer
 */
void hello_sensor_fine_timeout( uint32_t finecount )
{
    hello_sensor_state.fine_timer_count++;
    if(0 == (hello_sensor_state.fine_timer_count & 0x1f))
    {
        WICED_BT_TRACE("hello_sensor_fine_timeout: %d\n", hello_sensor_state.fine_timer_count);
    }
}

/*
 * Process SMP bonding result. If we successfully paired with the
 * central device, save its BDADDR in the NVRAM and initialize
 * associated data
 */

void hello_sensor_smp_bond_result( uint8_t result )
{
    wiced_result_t status;
    uint8_t written_byte = 0;
    WICED_BT_TRACE( "hello_sensor, bond result: %d\n", result );

    /* Bonding success */
    if ( result == WICED_BT_SUCCESS )
    {
        /* Pack the data to be stored into the hostinfo structure */
        memcpy( hello_sensor_hostinfo.bdaddr, hello_sensor_state.remote_addr, sizeof( BD_ADDR ) );

        /* Write to NVRAM */
        written_byte = wiced_hal_write_nvram( HELLO_SENSOR_VS_ID, sizeof(hello_sensor_hostinfo), (uint8_t*)&hello_sensor_hostinfo, &status );
        WICED_BT_TRACE("NVRAM write: %d\n", written_byte);
    }
}


/*
 * Process notification from stack that encryption has been set. If connected
 * client is registered for notification or indication, it is a good time to
 * send it out
 */
void hello_sensor_encryption_changed( wiced_result_t result, uint8_t* bd_addr )
{
    WICED_BT_TRACE( "encryp change bd ( %B ) res: %d ", hello_sensor_hostinfo.bdaddr,  result);

    /* Connection has been encrypted meaning that we have correct/paired device
     * restore values in the database
     */
    wiced_hal_read_nvram( HELLO_SENSOR_VS_ID, sizeof(hello_sensor_hostinfo), (uint8_t*)&hello_sensor_hostinfo, &result );

    // If there are outstanding messages that we could not send out because
    // connection was not up and/or encrypted, send them now.  If we are sending
    // indications, we can send only one and need to wait for ack.
    while ( ( hello_sensor_state.num_to_write != 0 ) && !hello_sensor_state.flag_indication_sent )
    {
        hello_sensor_state.num_to_write--;
        hello_sensor_send_message();
    }

    // If configured to disconnect after delivering data, start idle timeout
    // to do disconnection
    if ( ( !hello_sensor_state.flag_stay_connected ) && !hello_sensor_state.flag_indication_sent  )
    {
        wiced_bt_app_start_conn_idle_timer( HELLO_SENSOR_CONN_IDLE_TIMEOUT_IN_SECONDS, hello_sensor_conn_idle_timeout );
    }
}

/*
 * Button Interrupt can be handled here.
 */
void hello_sensor_interrupt_handler(void* data, uint8_t pin )
{

    // Blink as configured
    wiced_bt_app_hal_led_blink( WICED_PLATFORM_LED_1, 250, 250, hello_sensor_hostinfo.number_of_blinks );

    /* Increment the last byte of the hello sensor notify value */
    hello_sensor_gatts_increment_notify_value();

    /* Remember how many messages we need to send */
    hello_sensor_state.num_to_write++;

    /* If connection is down, start high duty advertisements, so client can connect */
    if ( hello_sensor_state.conn_id == 0 )
    {
        wiced_result_t result;

        WICED_BT_TRACE( "ADV start high\n");

        result = wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_LOW, 0, NULL );

        WICED_BT_TRACE( "wiced_bt_start_advertisements:%d\n", result );
        return;
    }

    /*
     * Connection up.
     * Send message if client registered to receive indication
     * or notification. After we send an indication wait for the ack
     * before we can send anything else
     */
    while ( ( hello_sensor_state.num_to_write != 0 ) && !hello_sensor_state.flag_indication_sent )
    {
        hello_sensor_state.num_to_write--;
        hello_sensor_send_message();
    }

    // if we sent all messages, start connection idle timer to disconnect
    if ( !hello_sensor_state.flag_stay_connected && !hello_sensor_state.flag_indication_sent )
    {
        wiced_bt_app_start_conn_idle_timer( HELLO_SENSOR_CONN_IDLE_TIMEOUT_IN_SECONDS, hello_sensor_conn_idle_timeout );
    }
}

/* 
 * Function on connection idle timeout initiates the gatt disconnect
 */
void hello_sensor_conn_idle_timeout ( uint32_t arg )
{
    WICED_BT_TRACE( "hello_sensor_conn_idle_timeout\n" );

    /* Stopping the app timers */
    wiced_bt_app_stop_timer();

    /* Initiating the gatt disconnect */
    wiced_bt_gatt_disconnect( hello_sensor_state.conn_id );
}

/*
 * hello_sensor bt/ble link management callback
 */
wiced_result_t hello_sensor_management_cback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_result_t                    result = WICED_BT_SUCCESS;
    wiced_bt_dev_encryption_status_t *p_status;
    wiced_bt_dev_ble_pairing_info_t  *p_info;
    wiced_bt_ble_advert_mode_t       *p_mode;
    uint8_t                          *p_keys;

    WICED_BT_TRACE("hello_sensor_management_cback: %x\n", event );

    switch( event )
    {
    /* Bluetooth  stack enabled */
    case BTM_ENABLED_EVT:
        hello_sensor_application_init();
        break;

    case BTM_DISABLED_EVT:
        break;

    case BTM_USER_CONFIRMATION_REQUEST_EVT:
        WICED_BT_TRACE("numeric_value: %d \n", p_event_data->user_confirmation_request.numeric_value);
        wiced_bt_dev_confirm_req_reply( WICED_BT_SUCCESS , p_event_data->user_confirmation_request.bd_addr);
        break;

    case BTM_PASSKEY_NOTIFICATION_EVT:
        WICED_BT_TRACE("PassKey Notification. BDA %B, Key %d \n", p_event_data->user_passkey_notification.bd_addr, p_event_data->user_passkey_notification.passkey );
        wiced_bt_dev_confirm_req_reply(WICED_BT_SUCCESS, p_event_data->user_passkey_notification.bd_addr );
        break;

    case BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT:
        p_event_data->pairing_io_capabilities_ble_request.local_io_cap   = BTM_IO_CAPABILITIES_NONE;
        p_event_data->pairing_io_capabilities_ble_request.oob_data   = BTM_OOB_NONE;
        p_event_data->pairing_io_capabilities_ble_request.auth_req   = BTM_LE_AUTH_REQ_SC_BOND;
        p_event_data->pairing_io_capabilities_ble_request.max_key_size  = 0x10;
        p_event_data->pairing_io_capabilities_ble_request.init_keys     = BTM_LE_KEY_PENC|BTM_LE_KEY_PID|BTM_LE_KEY_PCSRK|BTM_LE_KEY_LENC;
        p_event_data->pairing_io_capabilities_ble_request.resp_keys     = BTM_LE_KEY_PENC|BTM_LE_KEY_PID|BTM_LE_KEY_PCSRK|BTM_LE_KEY_LENC;
        break;

    case BTM_PAIRING_COMPLETE_EVT:
        p_info =  &p_event_data->pairing_complete.pairing_complete_info.ble;
        WICED_BT_TRACE( "Pairing Complete: %d",p_info->reason);
        hello_sensor_smp_bond_result( p_info->reason );
        break;

    case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
            /* save keys to NVRAM */
            p_keys = (uint8_t*)&p_event_data->paired_device_link_keys_update;
            wiced_hal_write_nvram ( HELLO_SENSOR_PAIRED_KEYS_VS_ID, sizeof( wiced_bt_device_link_keys_t ), p_keys ,&result );
            WICED_BT_TRACE("keys save to NVRAM %B result: %d \n", p_keys, result);
        break;

     case  BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT:
            /* read keys from NVRAM */
            p_keys = (uint8_t *)&p_event_data->paired_device_link_keys_request;
            wiced_hal_read_nvram( HELLO_SENSOR_PAIRED_KEYS_VS_ID, sizeof(wiced_bt_device_link_keys_t), p_keys, &result );
            WICED_BT_TRACE("keys read from NVRAM %B result: %d \n", p_keys, result);
        break;

    case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT:
            /* save keys to NVRAM */
            p_keys = (uint8_t*)&p_event_data->local_identity_keys_update;
            wiced_hal_write_nvram ( HELLO_SENSOR_LOCAL_KEYS_VS_ID, sizeof( wiced_bt_local_identity_keys_t ), p_keys ,&result );
            WICED_BT_TRACE("local keys save to NVRAM result: %d \n", result);
        break;


    case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT:
            /* read keys from NVRAM */
            p_keys = (uint8_t *)&p_event_data->local_identity_keys_request;
            wiced_hal_read_nvram( HELLO_SENSOR_LOCAL_KEYS_VS_ID, sizeof(wiced_bt_local_identity_keys_t), p_keys, &result );
            WICED_BT_TRACE("local keys read from NVRAM result: %d \n",  result);
        break;

        case BTM_ENCRYPTION_STATUS_EVT:
            p_status = &p_event_data->encryption_status;
            WICED_BT_TRACE( "Encryption Status Event: bd ( %B ) res %d", p_status->bd_addr, p_status->result);
            hello_sensor_encryption_changed( p_status->result, p_status->bd_addr );
        break;

        case BTM_SECURITY_REQUEST_EVT:
            wiced_bt_ble_security_grant( p_event_data->security_request.bd_addr, WICED_BT_SUCCESS );
        break;

    case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
        p_mode = &p_event_data->ble_advert_state_changed;
        WICED_BT_TRACE( "Advertisement State Change: %d\n", *p_mode);
        if ( *p_mode == BTM_BLE_ADVERT_OFF )
        {
            hello_sensor_advertisement_stopped();
        }
        break;

    default:
        break;
    }

    return result;
}


/*
 * Check if client has registered for notification/indication
 * and send message if appropriate
 */
void hello_sensor_send_message( void )
{
    WICED_BT_TRACE( "hello_sensor_send_message: CCC:%d\n", hello_sensor_hostinfo.characteristic_client_configuration );

    /* If client has not registered for indication or notification, no action */
    if ( hello_sensor_hostinfo.characteristic_client_configuration == 0 )
    {
        return;
    }
    else if ( hello_sensor_hostinfo.characteristic_client_configuration & GATT_CLIENT_CONFIG_NOTIFICATION )
    {
        uint8_t *p_attr = (uint8_t*)hello_sensor_char_notify_value;

        wiced_bt_gatt_send_notification( hello_sensor_state.conn_id, HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL, sizeof(hello_sensor_char_notify_value), p_attr );
    }
    else
    {
        if ( !hello_sensor_state.flag_indication_sent )
        {
            uint8_t *p_attr = (uint8_t *)hello_sensor_char_notify_value;

            hello_sensor_state.flag_indication_sent = TRUE;

            wiced_bt_gatt_send_indication( hello_sensor_state.conn_id, HANDLE_HSENS_SERVICE_CHAR_NOTIFY_VAL, sizeof(hello_sensor_char_notify_value), p_attr );
        }
    }
}

/*
 * Find attribute description by handle
 */
attribute_t * hello_sensor_get_attribute( uint16_t handle )
{
    int i;
    for ( i = 0; i <  sizeof( gauAttributes ) / sizeof( gauAttributes[0] ); i++ )
    {
        if ( gauAttributes[i].handle == handle )
        {
            return ( &gauAttributes[i] );
        }
    }
    //WICED_BT_TRACE( "\r\nIt's mijia's attribute, handle=0x%2x", handle );
    return NULL;
}


/*
 * Process Read request or command from peer device
 */
wiced_bt_gatt_status_t hello_sensor_gatts_req_read_handler( uint16_t conn_id, wiced_bt_gatt_read_t * p_read_data )
{
    attribute_t *puAttribute;
    int          attr_len_to_copy;

	if((puAttribute = hello_sensor_get_attribute(p_read_data->handle) ) != NULL)
	{
	  //WICED_BT_TRACE("read_hndlr attr not found hdl:%x\n", p_read_data->handle );
	  //return WICED_BT_GATT_INVALID_HANDLE;
	  /* Dummy battery value read increment */
		if( p_read_data->handle == HANDLE_HSENS_BATTERY_SERVICE_CHAR_LEVEL_VAL)
	    {
	        if ( hello_sensor_state.battery_level++ > 5)
	        {
	            hello_sensor_state.battery_level = 0;
	        }
	    }
	    attr_len_to_copy = puAttribute->attr_len;

	    if ( p_read_data->offset >= puAttribute->attr_len )
	    {
	        attr_len_to_copy = 0;
	    }

	    if ( attr_len_to_copy != 0 )
	    {
	        uint8_t *from;
	        int      to_copy = attr_len_to_copy - p_read_data->offset;


	        if ( to_copy > *p_read_data->p_val_len )
	        {
	            to_copy = *p_read_data->p_val_len;
	        }

	        from = ((uint8_t *)puAttribute->p_attr) + p_read_data->offset;
	        *p_read_data->p_val_len = to_copy;

	        memcpy( p_read_data->p_val, from, to_copy);
			MI_LOG_INFO("Read gatt(Cypress):char uuid hand=0x%2x, offset=%d, length=%d, data=",p_read_data->handle, p_read_data->offset, to_copy);
		    MI_LOG_HEXDUMP(p_read_data->p_val,to_copy);
		}
	}
	else 
	{
		switch ( p_read_data->handle )
		{
		    case MI_IDX_TOKEN_VAL: //no readbal properities
			case MI_IDX_PRODUCT_ID_VAL:
			case MI_IDX_VERSION_ID_VAL:
			case MI_IDX_BEACON_KEY_VAL:
			case MI_IDX_DEVICE_ID_VAL:
				//WICED_BT_TRACE( "****HANDLE_CHAR_VALUE_READ NON CFG*****\r\n");
				mijia_gatt_read(p_read_data->handle,p_read_data->p_val, p_read_data->p_val_len,p_read_data->offset);
				break;

		    default:
				MI_LOG_INFO("****gatt req read, not supported: handle=0x%02x****\r\n",p_read_data->handle);
	        	return WICED_BT_GATT_INVALID_HANDLE;
	        	break;
		}
	}
	return WICED_BT_GATT_SUCCESS;
}

/*
 * Process write request or write command from peer device
 */
wiced_bt_gatt_status_t hello_sensor_gatts_req_write_handler( uint16_t conn_id, wiced_bt_gatt_write_t * p_data )
{
    wiced_bt_gatt_status_t result    = WICED_BT_GATT_SUCCESS;
    uint8_t                *p_attr   = p_data->p_val;
    uint8_t                nv_update = WICED_FALSE;
	mible_gatts_evt_param_t gatts_params = {0};

    MI_LOG_INFO("gatt req write: conn_id=%d, hdl=:0x%2x, prep=%d, offset=%d, len=%d, value=", conn_id, p_data->handle, p_data->is_prep, p_data->offset, p_data->val_len );
	MI_LOG_HEXDUMP(p_data->p_val, p_data->val_len);
    switch ( p_data->handle )
    {
	    /* By writing into Characteristic Client Configuration descriptor
	     * peer can enable or disable notification or indication */
	    //case HANDLE_HSENS_SERVICE_CHAR_CFG_DESC:
		case MI_IDX_TOKEN_CFG:
	        if ( p_data->val_len != 2 )
	        {
	            return WICED_BT_GATT_INVALID_ATTR_LEN;
	        }
	        hello_sensor_hostinfo.characteristic_client_configuration = p_attr[0] | ( p_attr[1] << 8 );
	        nv_update = WICED_TRUE;
	        break;

	    case HANDLE_HSENS_SERVICE_CHAR_BLINK_VAL:
	        if ( p_data->val_len != 1 )
	        {
	            return WICED_BT_GATT_INVALID_ATTR_LEN;
	        }
	        hello_sensor_hostinfo.number_of_blinks = p_attr[0];
	        if ( hello_sensor_hostinfo.number_of_blinks != 0 )
	        {
	            MI_LOG_INFO( "hello_sensor_write_handler:num blinks: %d\n", hello_sensor_hostinfo.number_of_blinks );
	            wiced_bt_app_hal_led_blink( WICED_PLATFORM_LED_1, 250, 250, hello_sensor_hostinfo.number_of_blinks );
	            nv_update = WICED_TRUE;
	        }
	        break;    
			
		case MI_IDX_WIFICFG_VAL:
		case MI_IDX_TOKEN_VAL:
		case MI_IDX_DEVICE_ID_VAL:
		case MI_IDX_AUTHENTICATION_VAL:
			if(mijia_gatt_write(conn_id,p_data)==MI_SUCCESS)
				result = WICED_BT_GATT_SUCCESS;
			else
				result = WICED_BT_GATT_ERROR;
			break;

	    default:
	        result = WICED_BT_GATT_INVALID_HANDLE;
			MI_LOG_INFO("****gatt req write, not supported: handle=0x%02x****\r\n", p_data->handle);
	        break;
    }

	gatts_params.conn_handle = conn_id;
	gatts_params.write.value_handle =p_data->handle;
	gatts_params.write.data = p_data->p_val;
	gatts_params.write.len = p_data->val_len;
	gatts_params.write.offset = p_data->offset;
	mible_gatts_event_callback(MIBLE_GATTS_EVT_WRITE_PERMIT_REQ,&gatts_params);
	//mible_std_gatts_event_handler(MIBLE_GATTS_EVT_WRITE_PERMIT_REQ,&gatts_params);

    if ( nv_update )
    {
        wiced_result_t rc;
        int bytes_written = wiced_hal_write_nvram( HELLO_SENSOR_VS_ID, sizeof(hello_sensor_hostinfo), (uint8_t*)&hello_sensor_hostinfo, &rc );
        WICED_BT_TRACE("NVRAM write:%d rc:%d\r\n", bytes_written, rc);
    }

    return result;
}

/*
 * Write Execute Procedure
 */
wiced_bt_gatt_status_t hello_sensor_gatts_req_write_exec_handler( uint16_t conn_id, wiced_bt_gatt_exec_flag_t exec_falg )
{
    WICED_BT_TRACE("write exec: flag:%d\n", exec_falg);
    return WICED_BT_GATT_SUCCESS;
}

/*
 * Process MTU request from the peer
 */
wiced_bt_gatt_status_t hello_sensor_gatts_req_mtu_handler( uint16_t conn_id, uint16_t mtu)
{
    WICED_BT_TRACE("req_mtu: %d\n", mtu);
    return WICED_BT_GATT_SUCCESS;
}

/*
 * Process indication confirm. If client wanted us to use indication instead of
 * notifications we have to wait for confirmation after every message sent.
 * For example if user pushed button twice very fast
 * we will send first message, then
 * wait for confirmation, then
 * send second message, then
 * wait for confirmation and
 * if configured start idle timer only after that.
 */
wiced_bt_gatt_status_t hello_sensor_gatts_req_conf_handler( uint16_t conn_id, uint16_t handle )
{
    WICED_BT_TRACE( "hello_sensor_indication_cfm, conn %d hdl %d\n", conn_id, handle );

    if ( !hello_sensor_state.flag_indication_sent )
    {
        WICED_BT_TRACE("Hello: Wrong Confirmation!");
        return WICED_BT_GATT_SUCCESS;
    }

    hello_sensor_state.flag_indication_sent = 0;

    /* We might need to send more indications */
    if ( hello_sensor_state.num_to_write )
    {
        hello_sensor_state.num_to_write--;
        hello_sensor_send_message();
    }
    /* if we sent all messages, start connection idle timer to disconnect */
    if ( !hello_sensor_state.flag_stay_connected && !hello_sensor_state.flag_indication_sent )
    {
       wiced_bt_app_start_conn_idle_timer( HELLO_SENSOR_CONN_IDLE_TIMEOUT_IN_SECONDS, hello_sensor_conn_idle_timeout );
    }

    return WICED_BT_GATT_SUCCESS;

}

/* This function is invoked when connection is established */
wiced_bt_gatt_status_t hello_sensor_gatts_connection_up( wiced_bt_gatt_connection_status_t *p_status )
{
    wiced_result_t result;
    uint8_t        bytes_written = 0;

    WICED_BT_TRACE( "hello_sensor_conn_up %B id:%d\n:", p_status->bd_addr, p_status->conn_id);

    /* Update the connection handler.  Save address of the connected device. */
    hello_sensor_state.conn_id = p_status->conn_id;
    memcpy(hello_sensor_state.remote_addr, p_status->bd_addr, sizeof(BD_ADDR));

    /* Stop advertising */
    result =  wiced_bt_start_advertisements( BTM_BLE_ADVERT_OFF, 0, NULL );

    WICED_BT_TRACE( "Stopping Advertisements%d\n", result );

    /* Stop idle timer */
    wiced_bt_app_stop_conn_idle_timer();

    /* Updating the bd address in the  host info in NVRAM  */
    memcpy( hello_sensor_hostinfo.bdaddr, p_status->bd_addr, sizeof( BD_ADDR ) );

    hello_sensor_hostinfo.characteristic_client_configuration = 0;
    hello_sensor_hostinfo.number_of_blinks                    = 0;
	//store the host address
	memcpy(host_addr,hello_sensor_hostinfo.bdaddr,6);
	//mijia gap connection event
	mijia_gap_setup(MIBLE_GAP_EVT_CONNECTED,p_status);

    /* Save the  host info in NVRAM  */
    bytes_written = wiced_hal_write_nvram( HELLO_SENSOR_VS_ID, sizeof(hello_sensor_hostinfo), (uint8_t*)&hello_sensor_hostinfo, &result );
    WICED_BT_TRACE("NVRAM write %d\n", bytes_written);

    return WICED_BT_GATT_SUCCESS;
}

/*
 * This function is invoked when connection is lost
 */
wiced_bt_gatt_status_t hello_sensor_gatts_connection_down( wiced_bt_gatt_connection_status_t *p_status )
{
    wiced_result_t result;

    WICED_BT_TRACE( "connection_down %B conn_id:%d reason:%d\n", hello_sensor_state.remote_addr, p_status->conn_id, p_status->reason );

    /* Resetting the device info */
    memset( hello_sensor_state.remote_addr, 0, 6 );
    hello_sensor_state.conn_id = 0;

    /*
     * If we are configured to stay connected, disconnection was
     * caused by the peer, start low advertisements, so that peer
     * can connect when it wakes up
     */
    if ( hello_sensor_state.flag_stay_connected )
    {
        result =  wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_LOW, 0, NULL );
        WICED_BT_TRACE( "wiced_bt_start_advertisements %d\n", result );
    }
	mijia_gap_setup(MIBLE_GAP_EVT_DISCONNET,p_status);

    return WICED_BT_SUCCESS;
}

/*
 * Connection up/down event
 */
wiced_bt_gatt_status_t hello_sensor_gatts_conn_status_cb( wiced_bt_gatt_connection_status_t *p_status )
{
    if ( p_status->connected )
    {
        return hello_sensor_gatts_connection_up( p_status );
    }

    return hello_sensor_gatts_connection_down( p_status );
}

/*
 * Process GATT request from the peer
 */
wiced_bt_gatt_status_t hello_sensor_gatts_req_cb( wiced_bt_gatt_attribute_request_t *p_data )
{
    wiced_result_t result = WICED_BT_GATT_INVALID_PDU;

    WICED_BT_TRACE( "hello_sensor_gatts_req_cb. conn %d, type %d\n", p_data->conn_id, p_data->request_type );

    switch ( p_data->request_type )
    {
    case GATTS_REQ_TYPE_READ:
        result = hello_sensor_gatts_req_read_handler( p_data->conn_id, &(p_data->data.read_req) );
        break;

    case GATTS_REQ_TYPE_WRITE:
        result = hello_sensor_gatts_req_write_handler( p_data->conn_id, &(p_data->data.write_req) );
        break;

    case GATTS_REQ_TYPE_WRITE_EXEC:
        result = hello_sensor_gatts_req_write_exec_handler( p_data->conn_id, p_data->data.exec_write );
        break;

    case GATTS_REQ_TYPE_MTU:
        result = hello_sensor_gatts_req_mtu_handler( p_data->conn_id, p_data->data.mtu );
        break;

    case GATTS_REQ_TYPE_CONF:
        result = hello_sensor_gatts_req_conf_handler( p_data->conn_id, p_data->data.handle );
        break;

   default:
        break;
    }

    return result;
}

/*
 * Callback for various GATT events.  As this application performs only as a GATT server, some of the events are ommitted.
 */
wiced_bt_gatt_status_t hello_sensor_gatts_callback( wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_data)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_INVALID_PDU;

    switch(event)
    {
    case GATT_CONNECTION_STATUS_EVT:
        result = hello_sensor_gatts_conn_status_cb( &p_data->connection_status );
        break;

    case GATT_ATTRIBUTE_REQUEST_EVT:
        result = hello_sensor_gatts_req_cb( &p_data->attribute_request );
        break;

    default:
        break;
    }

    return result;
}

/*
 * Keep number of the button pushes in the last byte of the Hello message.
 * That will guarantee that if client reads it, it will have correct data.
 */
void hello_sensor_gatts_increment_notify_value( void )
{
    /* Getting the last byte */
    int last_byte = sizeof( hello_sensor_char_notify_value ) - 1 ;
    char c = hello_sensor_char_notify_value[last_byte];

    c++;
    if ( (c < '0') || (c > '9') )
    {
        c = '0';
    }
    hello_sensor_char_notify_value[last_byte] = c;
}

static void hello_sensor_load_keys_for_address_resolution( void )
{
    wiced_bt_device_link_keys_t link_keys;
    wiced_result_t              result;
    uint8_t                     *p;

    memset( &link_keys, 0, sizeof(wiced_bt_device_link_keys_t));
    p = (uint8_t*)&link_keys;
    wiced_hal_read_nvram( HELLO_SENSOR_PAIRED_KEYS_VS_ID, sizeof(wiced_bt_device_link_keys_t), p, &result);

    if(result == WICED_BT_SUCCESS)
    {
        result = wiced_bt_dev_add_device_to_address_resolution_db ( &link_keys, link_keys.key_data.ble_addr_type );
    }
    WICED_BT_TRACE("hello_sensor_load_keys_for_address_resolution %B result:%d \n", p, result );
}
static void mible_service_init_cmp(void)
{
    MI_LOG_INFO("mible_service_init_cmp\r\n");
}

static void mible_connected(void)
{
    MI_LOG_INFO("mible_connected \r\n");
}

static void mible_disconnected(void)
{
    MI_LOG_INFO("mible_disconnected \r\n");
    //advertising_init();
    advertising_start();
}

static void mible_bonding_evt_callback(mible_bonding_state state)
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
