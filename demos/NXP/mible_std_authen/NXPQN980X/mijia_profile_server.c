/*! *********************************************************************************
* \addtogroup Private Profile Server
* @{
********************************************************************************** */
/*!
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
*
* \file
*
* This file is the source file for the QPP Server application
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* #undef CR_INTEGER_PRINTF to force the usage of the sprintf() function provided
 * by the compiler in this file. The sprintf() function is #included from
 * the <stdio.h> file. */
#ifdef CR_INTEGER_PRINTF
    #undef CR_INTEGER_PRINTF
#endif

/* Framework / Drivers */
#include "stdio.h"
#include "RNG_Interface.h"
#include "Keyboard.h"
#include "LED.h"
#include "TimersManager.h"
#include "FunctionLib.h"
#include "SerialManager.h"
#include "MemManager.h"
#include "Panic.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_handles.h"
#include "gatt_db_app_interface.h"

/* Profile / Services */
#include "battery_interface.h"
#include "device_info_interface.h"

/* Connection Manager */
#include "ble_conn_manager.h"

#include "board.h"
#include "ApplMain.h"
#include "mijia_profile_server.h"

#include "mible_type.h"
#include "mible_api.h"
#include "mible_log.h"
#include "mible_log_internal.h"
/* mible application*/
#include "mible_beacon.h"
#include "mible_server.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define mBatteryLevelReportInterval_c                (10)         /* battery level report interval in seconds  */
#define mQppsTxInterval_c                            (1000)         /* Qpps send data interval in miliseconds  */
#define mQppsTestDataLength                          (20)         /* the length of data that Qpps send every time*/


/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
#if gAppUseBonding_d
    fastWhiteListAdvState_c,
#endif
    fastAdvState_c,
    slowAdvState_c
}advType_t;

typedef struct advState_tag{
    bool_t      advOn;
    advType_t   advType;
}advState_t;

typedef struct appPeerInfo_tag
{
    uint8_t deviceId;
    gattCccdFlags_t cccd_cfg;
    uint64_t bytsSentPerInterval;
    uint64_t bytsReceivedPerInterval;
}appPeerInfo_t;

typedef struct appTxInfo_tag
{
	uint32_t TakenSeconds;
	uint32_t RxSpeed[gAppMaxConnections_c];
	uint32_t TxSpeed[gAppMaxConnections_c];
}txInfo_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
uint8_t gAppSerMgrIf;
/* Adv State */
static advState_t  mAdvState;
static bool_t      mRestartAdv;

bleDeviceAddress_t   gBleDeviceAddress;

/* Service Data*/
//static bool_t           basValidClientList[gAppMaxConnections_c] = { FALSE };


//static uint16_t cpHandles[1] = {value_qpps_rx};

/* Application specific data*/



static appPeerInfo_t mPeerInformation[gAppMaxConnections_c];

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/* Gatt and Att callbacks */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent);
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent);
static void BleApp_Config(void);

/* Timer Callbacks */


static void Mibeacon_Advertise(void);


/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
extern void mible_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent);
extern void mible_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent);

/*app variable*/
device_info dev_info = {
		.bonding = STRONG_BONDING,
		.pid = 0x9c,
		.version = "0000",
};
/*! *********************************************************************************
* \brief    Initializes application specific functionality before the BLE stack init.
*
********************************************************************************** */
void BleApp_Init(void)
{
    /* Initialize application support for drivers */
    BOARD_InitAdc();
    
        /* UI */
    SerialManager_Init();

    /* Register Serial Manager interface */
    Serial_InitInterface(&gAppSerMgrIf, APP_SERIAL_INTERFACE_TYPE, APP_SERIAL_INTERFACE_INSTANCE);

    Serial_SetBaudRate(gAppSerMgrIf, gUARTBaudRate115200_c); 
    
    MI_LOG_INFO("BleApp_Init()\r\n");
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
	MI_LOG_INFO("BleApp_Start()\r\n");
	mible_addr_t mac;
	mible_gap_address_get(mac);
	MI_LOG_HEXDUMP((uint8_t*)mac,6);
	
#if gAppUseBonding_d
    if (gcBondedDevices > 0)
    {
        mAdvState.advType = fastWhiteListAdvState_c;
    }
    else
    {
#endif
        mAdvState.advType = fastAdvState_c;
#if gAppUseBonding_d
    }
#endif

    //BleApp_Advertise();
    Mibeacon_Advertise();
}

void Mibeacon_Advertise(void)
{
    mible_gap_adv_param_t adv_param =(mible_gap_adv_param_t){
        .adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .adv_interval_min = 0x00a0,
        .adv_interval_max = 0x00b0,
        .ch_mask = {0},
    };
    if(MI_SUCCESS != mible_gap_adv_start(&adv_param)){
        MI_LOG_ERROR("mible_gap_adv_start failed. \r\n");
        return;
    }
}


void Mibeacon_Set_AdvData(void)
{
	    MI_LOG_INFO("advertising init...\n");
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
//		
//    MI_LOG_INFO("advertising_start\r\n");
//    mibeacon_frame_ctrl_t frame_ctrl = {
//    .time_protocol = 0,
//    .is_encrypt = 0,
//    .mac_include = 1,
//    .cap_include = 1,
//    .obj_include = 0,
//    .bond_confirm = 0,
//    .version = 0x03,
//    };
//    mibeacon_capability_t cap = {.connectable = 1,
//                                .encryptable = 1,
//                                .bondAbility = 1};

//    mible_addr_t dev_mac;
//    mible_gap_address_get(dev_mac);
//    
//    mibeacon_config_t mibeacon_cfg = {
//    .frame_ctrl = frame_ctrl,
//    .pid =dev_info.pid,
//    .p_mac = (mible_addr_t*)dev_mac, 
//    .p_capability = &cap,
//    .p_obj = NULL,
//    };
//    
//    uint8_t service_data[31];
//    uint8_t service_data_len=0;
//    
//    if(MI_SUCCESS != mible_service_data_set(&mibeacon_cfg, service_data, &service_data_len)){
//        MI_LOG_ERROR("mibeacon_data_set failed. \r\n");
//        return;
//    }
//    
//    // add flags
//    *(adv_data.adv_data) = 0x02;
//    *(adv_data.adv_data + 1) = 0x01;
//    *(adv_data.adv_data + 2) = 0x06;
//    
//    memcpy(adv_data.adv_data+3, service_data, service_data_len);
//    adv_data.adv_len = service_data_len + 3;
//    
//    mible_gap_adv_data_set(&adv_data);
//    
//    MI_LOG_INFO("adv mi service data:");
//    MI_LOG_HEXDUMP(adv_data.adv_data, adv_data.adv_len);
//    MI_LOG_PRINTF("\r\n");
}
/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    events    Key event structure.
********************************************************************************** */
void BleApp_HandleKeys(key_event_t events)
{ 
    switch (events)
    {
        uint8_t i;
        case gKBD_EventPressPB1_c:
        {
            for (i = 0; i < gAppMaxConnections_c; i++)
            {
              if (mPeerInformation[i].deviceId == gInvalidDeviceId_c)
                break;
            }
            if(i < gAppMaxConnections_c)
              BleApp_Start();
            break;
        }
        case gKBD_EventLongPB1_c:
        {
            for (i = 0; i < gAppMaxConnections_c; i++)
            {
              if (mPeerInformation[i].deviceId != gInvalidDeviceId_c)
                Gap_Disconnect(mPeerInformation[i].deviceId);
            }
            break;
        }
        case gKBD_EventPressPB2_c:
        {
//            static int8_t adv_test = 0;
//            adv_data.adv_data[5] = adv_test++;
//            mible_gap_adv_data_set(&adv_data);
            break;
        }
        default:
            break;
    }
}

static void get_address(gapGenericEvent_t* pGenericEvent)
{
    if (pGenericEvent->eventType == gPublicAddressRead_c)
    {
        /* Use address read from the controller */
        FLib_MemCpy(gBleDeviceAddress, pGenericEvent->eventData.aAddress, sizeof(bleDeviceAddress_t));
    }
}
/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
void BleApp_GenericCallback (gapGenericEvent_t* pGenericEvent)
{
    /* Call BLE Conn Manager */
    get_address(pGenericEvent);
    BleConnManager_GenericEvent(pGenericEvent);
    
    switch (pGenericEvent->eventType)
    {
        case gInitializationComplete_c:    
        {
            BleApp_Config();
        }
        break;
        
        case gAdvertisingParametersSetupComplete_c:
        {
            App_StartAdvertising(BleApp_AdvertisingCallback, BleApp_ConnectionCallback);
        }
        break;         

        default: 
            break;
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief        Configures BLE Stack after initialization. Usually used for
*               configuring advertising, scanning, white list, services, et al.
*
********************************************************************************** */
extern uint16_t HandlesForWriteNotifications[];
extern uint8_t NumOfHandlesForWriteNotifications;
static void BleApp_Config()
{
    /* Configure as GAP peripheral */
//    BleConnManager_GapPeripheralConfig();
   
    mible_server_info_init(&dev_info);
	mible_server_miservice_init();
    Mibeacon_Set_AdvData();
	
//    for(uint8_t i; i < NumberOfElements(cpHandles); i++)
//    {
//        HandlesForWriteNotifications[NumOfHandlesForWriteNotifications++] = cpHandles[i];
//    }
    
    /* Register for callbacks*/
    GattServer_RegisterHandlesForWriteNotifications(NumOfHandlesForWriteNotifications, HandlesForWriteNotifications);//(NumberOfElements(cpHandles), cpHandles);
    App_RegisterGattServerCallback(BleApp_GattServerCallback);

    mAdvState.advOn = FALSE;
    for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
    {
        mPeerInformation[i].deviceId= gInvalidDeviceId_c;
    }


    /* Allocate application timers */
}


/*! *********************************************************************************
* \brief        Handles BLE Advertising callback from host stack.
*
* \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
********************************************************************************** */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent)
{
    switch (pAdvertisingEvent->eventType)
    {
        case gAdvertisingStateChanged_c:
        {
            mAdvState.advOn = !mAdvState.advOn;
            
            if (!mAdvState.advOn && mRestartAdv)
            {
//                BleApp_Advertise();
                Mibeacon_Advertise();
                break;
            }                

            LED_StopFlashingAllLeds();
            Led1Flashing();

            if(!mAdvState.advOn)
            {
                Led2Flashing();
                Led3Flashing();
                Led4Flashing();
            }
        }
        break;

        case gAdvertisingCommandFailed_c:
        {
            panic(0,0,0,0);
        }
        break;

        default:
            break;
    }
}

/*! *********************************************************************************
* \brief        Handles BLE Connection callback from host stack.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
********************************************************************************** */
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
{
    /* Connection Manager to handle Host Stack interactions */
    BleConnManager_GapPeripheralEvent(peerDeviceId, pConnectionEvent);
    
    mible_ConnectionCallback(peerDeviceId, pConnectionEvent);

    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            mPeerInformation[peerDeviceId].deviceId = peerDeviceId;
//            Serial_Print(gAppSerMgrIf,"Connected with peerDeviceId = 0x",gAllowToBlock_d);
//            Serial_PrintHex(gAppSerMgrIf, &peerDeviceId, 1, gAllowToBlock_d);
//            Serial_Print(gAppSerMgrIf, "\r\n", gAllowToBlock_d);
            /* Advertising stops when connected */
            mAdvState.advOn = FALSE;            
        
            /* Subscribe client*/

            /* UI */            
            LED_StopFlashingAllLeds();
            Led1On();                  
            /* Stop Advertising Timer*/
            mAdvState.advOn = FALSE;
          
        }
        break;
        
        case gConnEvtDisconnected_c:
        {

        }
        break;
    default:
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles GATT server callback from host stack.
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    pServerEvent    Pointer to gattServerEvent_t.
********************************************************************************** */


static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    uint16_t handle;
    uint8_t status;
    
    mible_GattServerCallback(deviceId, pServerEvent);
    
    switch (pServerEvent->eventType)
    {
        case gEvtAttributeWritten_c:
        {
            handle = pServerEvent->eventData.attributeWrittenEvent.handle;
            status = gAttErrCodeNoError_c;
            GattServer_SendAttributeWrittenStatus(deviceId, handle, status);
        }
        break;
        
        case gEvtAttributeWrittenWithoutResponse_c:
        {
            handle = pServerEvent->eventData.attributeWrittenEvent.handle;
            
        }
        break;
        
        case gEvtCharacteristicCccdWritten_c:
        {
        }
        break;
        
        case gEvtHandleValueConfirmation_c:
        {
        }
        break;
        
        case gEvtAttributeRead_c:
        {
        }
        break;
        
    default:	
        break;
    }
}



/*! *********************************************************************************
* @}
********************************************************************************** */
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
	Mibeacon_Advertise();
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
