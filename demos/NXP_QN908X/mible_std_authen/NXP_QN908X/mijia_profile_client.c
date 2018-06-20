/*! *********************************************************************************
* \addtogroup Private Profile Client
* @{
********************************************************************************** */
/*!
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
* \file private_profile_client.c
* This file is the source file for the QPP Client application
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
/* Framework / Drivers */
#include "RNG_Interface.h"
#include "Keyboard.h"
#include "LED.h"
#include "TimersManager.h"
#include "FunctionLib.h"
#include "shell.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_app_interface.h"
#include "gatt_db_handles.h"

/* Profile / Services */
#include "battery_interface.h"
#include "device_info_interface.h"
#include "mijia_profile_interface.h"

#include "ble_conn_manager.h"
#include "ble_service_discovery.h"

#include "ApplMain.h"
#include "mijia_profile_client.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define mQppcTestDataLength                       (20)      /* the length of data that Qppc send every time*/
#define mQppcTXInterval_c                         (100)       /* send data interval in miliseconds  */
#define mQppcThroughputStatisticsInterval_c       (10000)    /* Rx speed interval in miliseconds  */
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

typedef enum appEvent_tag{
    mAppEvt_PeerConnected_c,
    mAppEvt_PairingComplete_c,
    mAppEvt_ServiceDiscoveryComplete_c,
    mAppEvt_ServiceDiscoveryFailed_c,
    mAppEvt_GattProcComplete_c,
    mAppEvt_GattProcError_c
}appEvent_t;

typedef enum appState_tag{
    mAppIdle_c,
    mAppExchangeMtu_c,
    mAppServiceDisc_c,
    mAppEnableNotifications_c,
    mAppRunning_c,
}appState_t;

typedef struct appCustomInfo_tag
{
    qppConfig_t     qppClientConfig;
}appCustomInfo_t;

typedef struct appPeerInfo_tag
{
    deviceId_t      deviceId;
    appCustomInfo_t customInfo;
    bool_t          isBonded;
    appState_t      appState;
    uint64_t        bytsReceivedPerInterval;
    uint64_t        bytsSentPerInterval;
}appPeerInfo_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static appPeerInfo_t mPeerInformation[gAppMaxConnections_c];

#if gAppUseBonding_d
static bool_t mRestoringBondedLink = FALSE;
#endif

static bool_t   mScanningOn = FALSE;

/* Buffer used for Characteristic related procedures */
static gattAttribute_t      *mpCharProcBuffer = NULL;
static gattCharacteristic_t *mpCharBuffer = NULL;
/* Timers */
static tmrTimerID_t mQppcTxTimerId;
static tmrTimerID_t mQppcThroughputStatisticsTimerId;
/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/

/* Host Stack callbacks */
static void BleApp_ScanningCallback 
(
    gapScanningEvent_t* pScanningEvent
);

static void BleApp_ConnectionCallback 
(
    deviceId_t peerDeviceId, 
    gapConnectionEvent_t* pConnectionEvent
);

static void BleApp_GattClientCallback 
(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
);

static void BleApp_GattNotificationCallback
(
    deviceId_t          serverDeviceId, 
    uint16_t characteristicValueHandle,
    uint8_t* aValue,
    uint16_t valueLength
);

static void BleApp_ServiceDiscoveryCallback
(
    deviceId_t deviceId,
    servDiscEvent_t* pEvent
);

static void BleApp_Config(void);

void BleApp_StateMachineHandler
(
    deviceId_t peerDeviceId,
    uint8_t event
);

static bool_t CheckScanEvent(gapScannedDevice_t* pData);

static void BleApp_StoreServiceHandles
(
    deviceId_t peerDeviceId,
    gattService_t   *pService
);
static void QppcTXTimerCallback(void* pParam);

static void QppcThroughputStatisticsTimerCallback(void* pParam);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief    Initializes application specific functionality before the BLE stack init.
*
********************************************************************************** */
void BleApp_Init1(void)
{
    /* UI */
    shell_init("BLE QPP Client>");
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start1(void)
{
    if (!mScanningOn)
    {
        App_StartScanning(&gScanParams, BleApp_ScanningCallback,TRUE);
    }
}

/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    events    Key event structure.
********************************************************************************** */
void BleApp_HandleKeys1(key_event_t events)
{
   
    switch (events)
    {
        case gKBD_EventPressPB1_c:
        {
            BleApp_Start();
            break;
        }
        case gKBD_EventLongPB1_c:
        {
            for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
            {
                if (mPeerInformation[i].deviceId != gInvalidDeviceId_c)
                    Gap_Disconnect(mPeerInformation[i].deviceId);
            }
            break;
        }
        case gKBD_EventPressPB2_c:
        {
            for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
            {
                if ((mPeerInformation[i].deviceId != gInvalidDeviceId_c)&&(!TMR_IsTimerActive(mQppcTxTimerId)))
                {
                    mpCharBuffer = MEM_BufferAlloc(sizeof(gattCharacteristic_t));
                    if (!mpCharBuffer)
                    {
                        shell_write("\n\r--> mpCharBuffer memory alloc error!");
                        return;
                    }
                    TMR_StartLowPowerTimer(mQppcTxTimerId, gTmrLowPowerIntervalMillisTimer_c,
                                            mQppcTXInterval_c, QppcTXTimerCallback, NULL);
                    break;
                }

            }
            break;
        }
        case gKBD_EventLongPB2_c:
	{
            if(TMR_IsTimerActive(mQppcTxTimerId))
            {
                MEM_BufferFree(mpCharBuffer);
                mpCharBuffer = NULL;
                TMR_StopTimer(mQppcTxTimerId);
            }
            break;       
        }
        default:
            break;
    }
 
}

/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
void BleApp_GenericCallback1 (gapGenericEvent_t* pGenericEvent)
{
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);
    
    switch (pGenericEvent->eventType)
    {
        case gInitializationComplete_c:    
        {
            BleApp_Config();
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
static void BleApp_Config()
{
    /* Configure as GAP Central */
    BleConnManager_GapCentralConfig();

    /* Register for callbacks*/
    App_RegisterGattClientProcedureCallback(BleApp_GattClientCallback);
    App_RegisterGattClientNotificationCallback(BleApp_GattNotificationCallback);
    BleServDisc_RegisterCallback(BleApp_ServiceDiscoveryCallback);

    for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
    {
        mPeerInformation[i].appState = mAppIdle_c;
	mPeerInformation[i].deviceId = gInvalidDeviceId_c;
        mPeerInformation[i].bytsReceivedPerInterval = 0;
        mPeerInformation[i].bytsSentPerInterval = 0;
    }

    mScanningOn = FALSE;
    
    /* Allocate timer */
    mQppcTxTimerId = TMR_AllocateTimer();
    mQppcThroughputStatisticsTimerId = TMR_AllocateTimer();
    /* UI */
    shell_write("\r\nPress SW4 to connect to a QPP server!\r\n");

}


/*! *********************************************************************************
* \brief        Handles BLE Scanning callback from host stack.
*
* \param[in]    pScanningEvent    Pointer to gapScanningEvent_t.
********************************************************************************** */
static void BleApp_ScanningCallback (gapScanningEvent_t* pScanningEvent)
{
    switch (pScanningEvent->eventType)
    {
        case gDeviceScanned_c:
        {
            if (CheckScanEvent(&pScanningEvent->eventData.scannedDevice))
            {        
                gConnReqParams.peerAddressType = pScanningEvent->eventData.scannedDevice.addressType;
                FLib_MemCpy(gConnReqParams.peerAddress, 
                            pScanningEvent->eventData.scannedDevice.aAddress,
                            sizeof(bleDeviceAddress_t));
                Gap_StopScanning();
                App_Connect(&gConnReqParams, BleApp_ConnectionCallback);
            }
        }        
        break;
        case gScanStateChanged_c:
        {
            mScanningOn = !mScanningOn;
            if (mScanningOn)
            {
                LED_StopFlashingAllLeds();
                Led1Flashing();
                shell_write("\r\nScanning...\r\n");              
            }
        }
        break;
        
        case gScanCommandFailed_c:
        {
        }
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
	BleConnManager_GapCentralEvent(peerDeviceId, pConnectionEvent);

    switch (pConnectionEvent->eventType)
    {
        case gConnEvtConnected_c:
        {
            /* UI */
            LED_StopFlashingAllLeds();
            Led1On();
            shell_write("\r\nConnected!\r\n");
            
            mPeerInformation[peerDeviceId].deviceId = peerDeviceId;
            mPeerInformation[peerDeviceId].isBonded = FALSE;
            	
#if gAppUseBonding_d            
            Gap_CheckIfBonded(peerDeviceId, &mPeerInformation[peerDeviceId].isBonded);
            
            if ((mPeerInformation[peerDeviceId].isBonded) &&
                (gBleSuccess_c == Gap_LoadCustomPeerInformation(peerDeviceId,
                    (void*) &mPeerInformation[peerDeviceId].customInfo, 0, sizeof (appCustomInfo_t))))
            {              
                mRestoringBondedLink = TRUE;
                /* Restored custom connection information. Encrypt link */
                Gap_EncryptLink(peerDeviceId);                
            }
#endif
            BleApp_StateMachineHandler(mPeerInformation[peerDeviceId].deviceId, mAppEvt_PeerConnected_c);
            if(!TMR_IsTimerActive(mQppcThroughputStatisticsTimerId))
            {
                TMR_StartLowPowerTimer(mQppcThroughputStatisticsTimerId, gTmrLowPowerIntervalMillisTimer_c,
                       mQppcThroughputStatisticsInterval_c, QppcThroughputStatisticsTimerCallback, NULL);
            }
        }
        break;
        
        case gConnEvtDisconnected_c:
        {
            mPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;
            mPeerInformation[peerDeviceId].appState = mAppIdle_c;
            mPeerInformation[peerDeviceId].bytsReceivedPerInterval = 0;
            mPeerInformation[peerDeviceId].bytsSentPerInterval = 0;
            
            /* Reset Service Discovery to be sure*/
            BleServDisc_Stop(peerDeviceId);
            for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
            {
                if(mPeerInformation[i].deviceId != gInvalidDeviceId_c)
                    break;
                if(i==(gAppMaxConnections_c-1))
                {
                    TMR_StopTimer(mQppcThroughputStatisticsTimerId);
                    TMR_StopTimer(mQppcTxTimerId);
                    MEM_BufferFree(mpCharBuffer);
                    mpCharBuffer = NULL;
                }
            }
            
            /* UI */
            LED_TurnOffAllLeds();
            LED_StartFlash(LED_ALL);   
            shell_write("\r\nDisconnected!\r\n");
        }
        break;		

#if gAppUsePairing_d		
        case gConnEvtPairingComplete_c:
        {
            if (pConnectionEvent->eventData.pairingCompleteEvent.pairingSuccessful)
            {
                BleApp_StateMachineHandler(mPeerInformation[peerDeviceId].deviceId, mAppEvt_PairingComplete_c);
            }
        }
        break;

        case gConnEvtEncryptionChanged_c:
        {
            if (mRestoringBondedLink)
            {
                mRestoringBondedLink = FALSE;
                uint16_t value = gCccdNotification_c;
                GattClient_WriteCharacteristicDescriptor(mPeerInformation[peerDeviceId].deviceId,
                                                         mpCharProcBuffer, 
                                                         sizeof(value), (void*)&value);
            }
        }
        break;		
#endif /* gAppUsePairing_d */

    default:
        break;
    }
}

static void BleApp_ServiceDiscoveryCallback(deviceId_t peerDeviceId, servDiscEvent_t* pEvent)
{
    switch(pEvent->eventType)
    {
        case gServiceDiscovered_c:
        {
            BleApp_StoreServiceHandles(peerDeviceId,pEvent->eventData.pService);
        }
        break;

        case gDiscoveryFinished_c:
        {
            if (pEvent->eventData.success)
            {
                BleApp_StateMachineHandler(peerDeviceId, mAppEvt_ServiceDiscoveryComplete_c);
            }
            else
            {
                BleApp_StateMachineHandler(peerDeviceId, mAppEvt_ServiceDiscoveryFailed_c);
            }
        }
        break;

        default:
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles GATT client callback from host stack.
*
* \param[in]    serverDeviceId      GATT Server device ID.
* \param[in]    procedureType    	Procedure type.
* \param[in]    procedureResult    	Procedure result.
* \param[in]    error    			Callback result.
********************************************************************************** */
static void BleApp_GattClientCallback(
    deviceId_t              serverDeviceId,
    gattProcedureType_t     procedureType,
    gattProcedureResult_t   procedureResult,
    bleResult_t             error
)
{
    if (procedureResult == gGattProcError_c)
    {    
        attErrorCode_t attError = (attErrorCode_t) (error & 0xFF);
        if (attError == gAttErrCodeInsufficientEncryption_c     ||
            attError == gAttErrCodeInsufficientAuthorization_c  ||
            attError == gAttErrCodeInsufficientAuthentication_c)
        {
            /* Start Pairing Procedure */
            Gap_Pair(serverDeviceId, &gPairingParameters);
        }
        
        BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcError_c);
    }
    else if (procedureResult == gGattProcSuccess_c)
    {

    	BleApp_StateMachineHandler(serverDeviceId, mAppEvt_GattProcComplete_c);
    }

    /* Signal Service Discovery Module */
    BleServDisc_SignalGattClientEvent(serverDeviceId, procedureType,procedureResult, error);
    
}

/*! *********************************************************************************
* \brief        Handles GATT client notification callback from host stack.
*
* \param[in]    serverDeviceId      			GATT Server device ID.
* \param[in]    characteristicValueHandle   		Handle.
* \param[in]    aValue    					Pointer to value.
* \param[in]    valueLength    				Value length.
********************************************************************************** */

static void BleApp_GattNotificationCallback
(
    deviceId_t  serverDeviceId, 
    uint16_t    characteristicValueHandle,
    uint8_t*    aValue,
    uint16_t    valueLength
)
{
    if (characteristicValueHandle == mPeerInformation[serverDeviceId].customInfo.qppClientConfig.hTxData)
    {
        mPeerInformation[serverDeviceId].bytsReceivedPerInterval = mPeerInformation[serverDeviceId].bytsReceivedPerInterval+ valueLength;
    }
}

/*! *********************************************************************************
* \brief        Stores handles for the specified service.
*
* \param[in]    pService    Pointer to gattService_t.
********************************************************************************** */
static void BleApp_StoreServiceHandles
(
    deviceId_t 	        peerDeviceId,
    gattService_t       *pService
)
{
    uint8_t i,j;
    static uint8_t  base_uuid[16] = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    if (pService->uuidType == gBleUuidType16_c) 
    {		
        base_uuid[12] = pService->uuid.uuid16;
        base_uuid[13] = pService->uuid.uuid16>>8;
    }
		
    if((FLib_MemCmp(base_uuid, uuid_service_qpps, 16)) || (FLib_MemCmp(pService->uuid.uuid128, uuid_service_qpps, 16)))
    {
        /* Found QPPS Service */
        mPeerInformation[peerDeviceId].customInfo.qppClientConfig.hService = pService->startHandle;
        
        for (i = 0; i < pService->cNumCharacteristics; i++)
        {
            if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType128_c) &&
                FLib_MemCmp(pService->aCharacteristics[i].value.uuid.uuid128 , uuid_qpps_characteristics_tx, 16))
            {
                /* Found QPPS TX Char */
                mPeerInformation[peerDeviceId].customInfo.qppClientConfig.hTxData = pService->aCharacteristics[i].value.handle;
                
                for (j = 0; j < pService->aCharacteristics[i].cNumDescriptors; j++)
                {
                    if ((pService->aCharacteristics[i].aDescriptors[j].uuidType == gBleUuidType16_c)&&
					( pService->aCharacteristics[i].aDescriptors[j].uuid.uuid16 == gBleSig_CCCD_d))
                    {

                        mPeerInformation[peerDeviceId].customInfo.qppClientConfig.hTxCccd= pService->aCharacteristics[i].aDescriptors[j].handle;

                    }
                }
            }
            
            else if ((pService->aCharacteristics[i].value.uuidType == gBleUuidType128_c) &&
            		 FLib_MemCmp(pService->aCharacteristics[i].value.uuid.uuid128 , uuid_qpps_characteristics_rx, 16))
            {
            	/* Found QPPS RX Char */
                mPeerInformation[peerDeviceId].customInfo.qppClientConfig.hRxData = pService->aCharacteristics[i].value.handle;
            }
        }
    }
}


static void QppcTXTimerCallback(void* pParam)
{
    static uint8_t index = 0;
    uint8_t i;
    uint8_t TestData[mQppcTestDataLength];
    uint16_t length = mQppcTestDataLength;
    for(i = 1; i < length; i++)
    {
        TestData[i] = index;
    }
    TestData[0] = index;
    index++;

    for(i=0; i < gAppMaxConnections_c; i++)
    {
        if (mPeerInformation[i].deviceId != gInvalidDeviceId_c)
        {
            mpCharBuffer->value.handle = mPeerInformation[i].customInfo.qppClientConfig.hRxData;
            GattClient_CharacteristicWriteWithoutResponse(mPeerInformation[i].deviceId, mpCharBuffer, length, (uint8_t *)TestData);
            mPeerInformation[i].bytsSentPerInterval= mPeerInformation[i].bytsSentPerInterval + length;
        }
    }
}

static void QppcThroughputStatisticsTimerCallback(void* pParam)
{
    uint32_t TakenSeconds = mQppcThroughputStatisticsInterval_c/1000;
    uint32_t RxSpeed[gAppMaxConnections_c] ;
    uint32_t TxSpeed[gAppMaxConnections_c] ;
    for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
    {
        if(mPeerInformation[i].deviceId != gInvalidDeviceId_c)
        {
            RxSpeed[i] = (mPeerInformation[i].bytsReceivedPerInterval/TakenSeconds) * 8;
            TxSpeed[i] = (mPeerInformation[i].bytsSentPerInterval/TakenSeconds) * 8;
            mPeerInformation[i].bytsReceivedPerInterval = 0;
            mPeerInformation[i].bytsSentPerInterval = 0;
            shell_write("\n\r-->QPP client, deviceId = ");
            shell_writeDec(i);
            shell_write(",RX speed = ");
            shell_writeDec(RxSpeed[mPeerInformation[i].deviceId]);
            shell_write("bps, TX speed = ");
            shell_writeDec(TxSpeed[mPeerInformation[i].deviceId]);
            shell_write("bps!\n\r"); 
        }
    }
}

static bool_t MatchDataInAdvElementList(gapAdStructure_t *pElement, void *pData, uint8_t iDataLen)
{ 
    uint8_t i;
    
    for (i=0; i < pElement->length; i+=iDataLen)
    {
        if (FLib_MemCmp(pData, &pElement->aData[i], iDataLen))
        {
            return TRUE;
        } 
    }    
    return FALSE;
}

static bool_t CheckScanEvent(gapScannedDevice_t* pData)
{
    uint8_t index = 0;
    uint8_t name[16];
    uint8_t nameLength;
    bool_t foundMatch = FALSE;
    
    while (index < pData->dataLength)
    {
        gapAdStructure_t adElement;
        
        adElement.length = pData->data[index];
        adElement.adType = (gapAdType_t)pData->data[index + 1];
        adElement.aData = &pData->data[index + 2];
      
         /* Search for Temperature Custom Service */
        if ((adElement.adType == gAdIncomplete128bitServiceList_c) ||
          (adElement.adType == gAdComplete128bitServiceList_c))
        {
            foundMatch = MatchDataInAdvElementList(&adElement, &uuid_service_qpps, 16);
        }
        
        if ((adElement.adType == gAdShortenedLocalName_c) ||
          (adElement.adType == gAdCompleteLocalName_c))
        {
            nameLength = MIN(adElement.length, 16);
            FLib_MemCpy(name, adElement.aData, nameLength);
        }
        
        /* Move on to the next AD elemnt type */
        index += adElement.length + sizeof(uint8_t);
    }
    
    if (foundMatch)
    {
        /* UI */
        shell_write("\r\nFound device: \r\n");
        shell_writeN((char*)name, nameLength-1);
        SHELL_NEWLINE();
        shell_writeHex(pData->aAddress, 6);
    }
    return foundMatch;
}
void BleApp_StateMachineHandler(deviceId_t peerDeviceId, uint8_t event)
{
    switch (mPeerInformation[peerDeviceId].appState)
    {
        case mAppIdle_c:
        {
            if (event == mAppEvt_PeerConnected_c)
            {
                /* Moving to Exchange MTU State */
                mPeerInformation[peerDeviceId].appState = mAppExchangeMtu_c;
				GattClient_ExchangeMtu(peerDeviceId);

            }
        }
        break;
        
        case mAppExchangeMtu_c:
        {
            if (event == mAppEvt_GattProcComplete_c)
            {
                /* Moving to Service Discovery State*/
                mPeerInformation[peerDeviceId].appState = mAppServiceDisc_c;
                
                /* Start Service Discovery*/
                BleServDisc_Start(peerDeviceId);
            }
            else if (event == mAppEvt_GattProcError_c) 
            {
                Gap_Disconnect(peerDeviceId);
            }
        }
        break;
		
        case mAppServiceDisc_c:
        {
            if (event == mAppEvt_ServiceDiscoveryComplete_c)
            {
            	mPeerInformation[peerDeviceId].appState = mAppEnableNotifications_c;
                if (mPeerInformation[peerDeviceId].customInfo.qppClientConfig.hTxCccd)
                {              					
                    mpCharProcBuffer = MEM_BufferAlloc(sizeof(gattAttribute_t) + 23);                                                    
                    if (!mpCharProcBuffer)
                      return;
                    mpCharProcBuffer->handle = mPeerInformation[peerDeviceId].customInfo.qppClientConfig.hTxCccd;
					mpCharProcBuffer->paValue = (uint8_t*)(mpCharProcBuffer + 1);
					GattClient_ReadCharacteristicDescriptor(mPeerInformation[peerDeviceId].deviceId, mpCharProcBuffer ,23);
                }
            }
            else if (event == mAppEvt_ServiceDiscoveryFailed_c)
            {
                Gap_Disconnect(peerDeviceId);
            }
        }
        break;

        case mAppEnableNotifications_c:
        {
            if (event == mAppEvt_GattProcComplete_c)
            {
                if (mpCharProcBuffer && (!(mpCharProcBuffer->paValue[0] & gCccdNotification_c))&&
                    (mpCharProcBuffer->handle == mPeerInformation[peerDeviceId].customInfo.qppClientConfig.hTxCccd))
                {
                    uint16_t value = gCccdNotification_c;
                	/* Moving to Running State*/
                    mPeerInformation[peerDeviceId].appState = mAppRunning_c;
                    GattClient_WriteCharacteristicDescriptor(peerDeviceId,
                                                             mpCharProcBuffer,
                                                             sizeof(value), (void*)&value);
                }
            }
            else if (event == mAppEvt_PairingComplete_c)
            {
                /* Continue after pairing is complete */
                GattClient_ReadCharacteristicDescriptor(peerDeviceId, mpCharProcBuffer ,23);
            }
        }
        break;

        case mAppRunning_c:
        {
            if (event == mAppEvt_GattProcComplete_c)
            {
                if (mpCharProcBuffer)
                {
                    MEM_BufferFree(mpCharProcBuffer);
                    mpCharProcBuffer = NULL;
		}
							
#if gAppUseBonding_d                   
                /* Write data in NVM */
                Gap_SaveCustomPeerInformation(mPeerInformation[peerDeviceId].deviceId, 
                                              (void*) &mPeerInformation[peerDeviceId].customInfo, 0, 
                                              sizeof (appCustomInfo_t));
#endif                            
            }
        }
        break;
    }
}


/*! *********************************************************************************
* @}
********************************************************************************** */
