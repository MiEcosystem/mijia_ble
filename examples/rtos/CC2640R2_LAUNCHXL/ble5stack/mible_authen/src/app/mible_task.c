/******************************************************************************

   @file  mible_task.c

   @brief This file contains the mible task.mostly the Thread that intereact 
   with the BLE stack..

   Group: WCS, BTS
   Target Device: cc13x2_26x2

 ******************************************************************************
   
 Copyright (c) 2018-2019, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
   
   
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#if !(defined __TI_COMPILER_VERSION__)
#include <intrinsics.h>
#endif

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

//#include <xdc/runtime/Log.h> // Comment this in to use xdc.runtime.Log
#include <UartLog.h>  // Comment out if using xdc Log

#include <ti/display/AnsiColor.h>

#include <ti/devices/DeviceFamily.h>

#include <icall.h>
#include <bcomdef.h>
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>

/* Bluetooth Profiles */
#include <devinfoservice.h>

/* Includes needed for reverting to factory and erasing external flash */
#include <ti/drivers/dpl/HwiP.h>

/* Application specific includes */
#include <Board.h>

#include <mible_task.h>
#include <util.h>

#include <portable/cc26xx_api.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
// Task configuration
#define ML_TASK_PRIORITY                     1

#ifndef ML_TASK_STACK_SIZE
#define ML_TASK_STACK_SIZE                   1024
#endif

// Internal Events for RTOS application
#define ML_ICALL_EVT                         ICALL_MSG_EVENT_ID  // Event_Id_31
#define ML_APP_MSG_EVT                       Event_Id_30

// Bitwise OR of all RTOS events to pend on
#define ML_ALL_EVENTS                        (ML_ICALL_EVT | \
                                              ML_APP_MSG_EVT)

// Types of messages that can be sent to the user application task from other
// tasks or interrupts. Note: Messages from BLE Stack are sent differently.
#define ML_PAIRSTATE_EVT         4  /* The pairing state is updated                */
#define ML_PASSCODE_EVT          5  /* A pass-code/PIN is requested during pairing */

// Pass parameter updates to the app for it to decide.
#define DEFAULT_PARAM_UPDATE_REQ_DECISION     GAP_UPDATE_REQ_ACCEPT_ALL

// Address mode of the local device
// Note: When using the DEFAULT_ADDRESS_MODE as ADDRMODE_RANDOM or 
// ADDRMODE_RP_WITH_RANDOM_ID, GAP_DeviceInit() should be called with 
// it's last parameter set to a static random address
#define DEFAULT_ADDRESS_MODE                  ADDRMODE_PUBLIC

/*********************************************************************
 * TYPEDEFS
 */
// Struct for messages sent to the application task
typedef struct
{
    uint8_t event;
    void    *pData;
} pzMsg_t;

#ifdef GAP_BOND_MGR
// Struct for message about sending/requesting passcode from peer.
typedef struct
{
    uint16_t connHandle;
    uint8_t uiInputs;
    uint8_t uiOutputs;
    uint32_t numComparison;
} pzPasscodeReq_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
    uint8_t state;
    uint16_t connHandle;
    uint8_t status;
} pzPairStateData_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPasscodeCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
    uint8_t deviceAddr[B_ADDR_LEN];
    uint16_t connHandle;
    uint8_t uiInputs;
    uint8_t uiOutputs;
    uint32_t numComparison;
} pzPasscodeData_t;
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Task configuration
Task_Struct pzTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(appTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t appTaskStack[ML_TASK_STACK_SIZE];

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Queue object used for app messages
static Queue_Struct appMsgQueue;
static Queue_Handle appMsgQueueHandle;

// GAP GATT Attributes
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "MiBLE Authen";

static struct mible_port_cb_s *ml_port_cb;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/* Task functions */
static void ml_init(void);
static void ml_taskFxn(UArg a0,
                                UArg a1);

/* Event message processing functions */
static void ml_processStackEvent(uint32_t stack_event);
static void ml_processApplicationMessage(pzMsg_t *pMsg);
static uint8_t ml_processGATTMsg(gattMsgEvent_t *pMsg);
static void ml_processGapMessage(gapEventHdr_t *pMsg);
static void ml_processHCIMsg(ICall_HciExtEvt *pMsg);
#ifdef GAP_BOND_MGR
static void ml_processPairState(pzPairStateData_t *pPairState);
static void ml_processPasscode(pzPasscodeReq_t *pReq);

/* Stack or profile callback function */
static void ml_passcodeCb(uint8_t *pDeviceAddr,
                                   uint16_t connHandle,
                                   uint8_t uiInputs,
                                   uint8_t uiOutputs,
                                   uint32_t numComparison);
static void ml_pairStateCb(uint16_t connHandle,
                                    uint8_t state,
                                    uint8_t status);

/* Utility functions */
static status_t ml_enqueueMsg(uint8_t event,
                                   void *pData);
#endif

/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8_t assertCause,
                          uint8_t assertSubcause);

#ifdef GAP_BOND_MGR
/*********************************************************************
 * PROFILE CALLBACKS
 */
// GAP Bond Manager Callbacks
static gapBondCBs_t ml_BondMgrCBs =
{
    ml_passcodeCb,     // Passcode callback
    ml_pairStateCb     // Pairing/Bonding state Callback
};
#endif

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      ml_createTask
 *
 * @brief   Task creation function for the Project Zero.
 */
void ml_createTask(void)
{
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = appTaskStack;
    taskParams.stackSize = ML_TASK_STACK_SIZE;
    taskParams.priority = ML_TASK_PRIORITY;

    Task_construct(&pzTask, ml_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      ml_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 */
static void ml_init(void)
{
    // Initialize the mible porting
    miapi_port_initialization(&ml_port_cb);
    // ******************************************************************
    // NO STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
    // ******************************************************************
    // Register the current thread as an ICall dispatcher application
    // so that the application can send and receive messages.
    ICall_registerApp(&selfEntity, &syncEvent);

    // Initialize queue for application messages.
    // Note: Used to transfer control to application thread from e.g. interrupts.
    Queue_construct(&appMsgQueue, NULL);
    appMsgQueueHandle = Queue_handle(&appMsgQueue);

    // ******************************************************************
    // Hardware initialization
    // ******************************************************************

    // Set the Device Name characteristic in the GAP GATT Service
    // For more information, see the section in the User's Guide:
    // http://software-dl.ti.com/lprf/ble5stack-latest/
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

    // Configure GAP for param update
    {
        uint16_t paramUpdateDecision = DEFAULT_PARAM_UPDATE_REQ_DECISION;

        // Pass all parameter update requests to the app for it to decide
        GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, paramUpdateDecision);
    }
#ifdef GAP_BOND_MGR
    // Setup the GAP Bond Manager. For more information see the GAP Bond Manager
    // section in the User's Guide:
    // http://software-dl.ti.com/lprf/ble5stack-latest/
    {
        // Don't send a pairing request after connecting (the peer device must
        // initiate pairing)
        uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
        // Use authenticated pairing: require passcode.
        uint8_t mitm = TRUE;
        // This device only has display capabilities. Therefore, it will display the
        // passcode during pairing. However, since the default passcode is being
        // used, there is no need to display anything.
        uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
        // Request bonding (storing long-term keys for re-encryption upon subsequent
        // connections without repairing)
        uint8_t bonding = TRUE;

        GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t),
                                &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
        GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t),
                                &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t),
                                &bonding);
    }
#endif
    // ******************************************************************
    // BLE Service initialization (Mandatory)
    // ******************************************************************
    GGS_AddService(GATT_ALL_SERVICES);         // GAP GATT Service
    GATTServApp_AddService(GATT_ALL_SERVICES); // GATT Service

    // ******************************************************************
    // Optional, but recomended...
    // ******************************************************************
    // DevInfo_AddService();                      // Device Information Service

#ifdef GAP_BOND_MGR
    // Start Bond Manager and register callback
    VOID GAPBondMgr_Register(&ml_BondMgrCBs);
#endif
    
    // Register with GAP for HCI/Host messages. This is needed to receive HCI
    // events. For more information, see the HCI section in the User's Guide:
    // http://software-dl.ti.com/lprf/ble5stack-latest/
    GAP_RegisterForMsgs(selfEntity);

    // Register for GATT local events and ATT Responses pending for transmission
    // TODO check if needed... GATT_RegisterForMsgs(selfEntity);

#ifdef GAP_BOND_MGR
    // Initialize GATT Client, used by GAPBondMgr to look for RPAO characteristic for network privacy
    GATT_InitClient();
#endif
    
    //Initialize GAP layer for Peripheral role and register to receive GAP events
    GAP_DeviceInit(GAP_PROFILE_PERIPHERAL, selfEntity, DEFAULT_ADDRESS_MODE, NULL);
}

/*********************************************************************
 * @fn      ml_taskFxn
 *
 * @brief   Application task entry point for the Project Zero.
 *
 * @param   a0, a1 - not used.
 */
static void ml_taskFxn(UArg a0, UArg a1)
{
    // Initialize application
    ml_init();

    // Application main loop
    for(;; )
    {
        uint32_t events;

        // Waits for an event to be posted associated with the calling thread.
        // Note that an event associated with a thread is posted when a
        // message is queued to the message receive queue of the thread
        events = Event_pend(syncEvent, Event_Id_NONE, ML_ALL_EVENTS,
                            ICALL_TIMEOUT_FOREVER);

        if(events)
        {
            ICall_EntityID dest;
            ICall_ServiceEnum src;
            ICall_HciExtEvt *pMsg = NULL;

            // Fetch any available messages that might have been sent from the stack
            if(ICall_fetchServiceMsg(&src, &dest,
                                     (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
            {
                uint8_t safeToDealloc = TRUE;

                if((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
                {
                    ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

                    // Check for BLE stack events first
                    if(pEvt->signature == 0xffff)
                    {
                        // Process stack events
                        ml_processStackEvent(pEvt->event_flag);
                    }
                    else
                    {
                        if (ml_port_cb)
                        {
                            ml_port_cb->stackEvent(pMsg->hdr.event, pMsg);
                        }
                        switch(pMsg->hdr.event)
                        {
                        case GAP_MSG_EVENT:
                            // Process GAP message
                            ml_processGapMessage((gapEventHdr_t*) pMsg);
                            break;

                        case GATT_MSG_EVENT:
                            // Process GATT message
                            safeToDealloc =
                                ml_processGATTMsg(
                                    (gattMsgEvent_t *)pMsg);
                            break;

                        case HCI_GAP_EVENT_EVENT:
                            ml_processHCIMsg(pMsg);
                            break;

                        default:
                            // do nothing
                            break;
                        }
                    }
                }

                if(pMsg && safeToDealloc)
                {
                    ICall_freeMsg(pMsg);
                }
            }

            // Process messages sent from another task or another context.
            while(!Queue_empty(appMsgQueueHandle))
            {
                pzMsg_t *pMsg = (pzMsg_t *)Util_dequeueMsg(appMsgQueueHandle);
                if(pMsg)
                {
                    // Process application-layer message probably sent from ourselves.
                    ml_processApplicationMessage(pMsg);
                    // Free the received message.
                    ICall_free(pMsg);
                }
            }

            if (ml_port_cb->taskExec)
            {
                ml_port_cb->taskExec();
            }

        }
    }
}

/*********************************************************************
 * @fn      ml_processStackEvent
 *
 * @brief   Process stack event. The event flags received are user-selected
 *          via previous calls to stack APIs.
 *
 * @param   stack_event - mask of events received
 *
 * @return  none
 */
static void ml_processStackEvent(uint32_t stack_event)
{
    // Intentionally blank
}

/*********************************************************************
 * @fn      ml_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t ml_processGATTMsg(gattMsgEvent_t *pMsg)
{
    // Free message payload. Needed only for ATT Protocol messages
    GATT_bm_free(&pMsg->msg, pMsg->method);

    // It's safe to free the incoming message
    return(TRUE);
}

/*********************************************************************
 * @fn      ml_processApplicationMessage
 *
 * @brief   Handle application messages
 *
 *          These are messages not from the BLE stack, but from the
 *          application itself.
 *
 *          For example, in a Software Interrupt (Swi) it is not possible to
 *          call any BLE APIs, so instead the Swi function must send a message
 *          to the application Task for processing in Task context.
 *
 * @param   pMsg  Pointer to the message of type pzMsg_t.
 */
static void ml_processApplicationMessage(pzMsg_t *pMsg)
{
    switch(pMsg->event)
    {
      case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
          AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
          break;

#ifdef GAP_BOND_MGR
      case ML_PAIRSTATE_EVT: /* Message about the pairing state */
          ml_processPairState((pzPairStateData_t*)(pMsg->pData));
          break;

      case ML_PASSCODE_EVT: /* Message about pairing PIN request */
      {
          pzPasscodeReq_t *pReq = (pzPasscodeReq_t *)pMsg->pData;
          ml_processPasscode(pReq);
      }
      break;
#endif
      
      default:
        break;
    }

    if(pMsg->pData != NULL)
    {
        ICall_free(pMsg->pData);
    }
}

/*********************************************************************
 * @fn      ml_processGapMessage
 *
 * @brief   Process an incoming GAP event.
 *
 * @param   pMsg - message to process
 */
static void ml_processGapMessage(gapEventHdr_t *pMsg)
{
    switch(pMsg->opcode)
    {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
        gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

        if(pPkt->hdr.status == SUCCESS)
        {
            // Signal the mible port that stack has initialize.
            if (ml_port_cb)
            {
                ml_port_cb->init(syncEvent, selfEntity);
            }

            // Call the project_zero_main function
            {
                extern void this_is_my_mible_application(void);
                this_is_my_mible_application();
            }
        }
        break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:
        break;

    case GAP_LINK_TERMINATED_EVENT:
        break;

    case GAP_LINK_PARAM_UPDATE_EVENT:
        break;

    case GAP_PAIRING_REQ_EVENT:
        // Disable advertising so that the peer device can be added to
        // the resolving list
        break;

    default:
        break;
    }
}

void ml_processHCIMsg(ICall_HciExtEvt *pEvt)
{
    ICall_Hdr *pMsg = (ICall_Hdr *)pEvt;

    // Process HCI message
    switch(pMsg->status)
    {
        case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
            AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
        break;

        // HCI Commands Events
        case HCI_COMMAND_STATUS_EVENT_CODE:
        break;

        default:
            break;
    }
}

#ifdef GAP_BOND_MGR
/*********************************************************************
 * @fn      ml_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @param   pPairData - pointer to pair state data container
 */
static void ml_processPairState(pzPairStateData_t *pPairData)
{
    uint8_t state = pPairData->state;
    uint8_t status = pPairData->status;

    switch(state)
    {
    case GAPBOND_PAIRING_STATE_STARTED:
        Log_info0("Pairing started");
        break;

    case GAPBOND_PAIRING_STATE_COMPLETE:
        if(status == SUCCESS)
        {
            Log_info0("Pairing success");
        }
        else
        {
            Log_info1("Pairing fail: %d", status);
        }
        break;

    case GAPBOND_PAIRING_STATE_ENCRYPTED:
        if(status == SUCCESS)
        {
            Log_info0("Encryption success");
        }
        else
        {
            Log_info1("Encryption failed: %d", status);
        }
        break;

    case GAPBOND_PAIRING_STATE_BOND_SAVED:
        if(status == SUCCESS)
        {
            Log_info0("Bond save success");
        }
        else
        {
            Log_info1("Bond save failed: %d", status);
        }
        break;

    default:
        break;
    }
}

/*********************************************************************
 * @fn      ml_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @param   pReq - pointer to passcode req
 */
static void ml_processPasscode(pzPasscodeReq_t *pReq)
{
    Log_info2("BondMgr Requested passcode. We are %s passcode %06d",
              (uintptr_t)(pReq->uiInputs ? "Sending" : "Displaying"),
              B_APP_DEFAULT_PASSCODE);

    // Send passcode response.
    GAPBondMgr_PasscodeRsp(pReq->connHandle, SUCCESS, B_APP_DEFAULT_PASSCODE);
}

/******************************************************************************
 *****************************************************************************
 *
 *  Handlers of direct system callbacks.
 *
 *  Typically enqueue the information or request as a message for the
 *  application Task for handling.
 *
 ****************************************************************************
 *****************************************************************************/

/*
 *  Callbacks from the Stack Task context (GAP or Service changes)
 *****************************************************************************/

/*********************************************************************
 * @fn      ml_pairStateCb
 *
 * @brief   Pairing state callback.
 *
 * @param   connHandle - connection handle
 *          state - pair state
 *          status - pair status
 */
static void ml_pairStateCb(uint16_t connHandle, uint8_t state,
                                    uint8_t status)
{
    pzPairStateData_t *pairState =
        (pzPairStateData_t *)ICall_malloc(sizeof(pzPairStateData_t));

    if(pairState != NULL)
    {
        pairState->state = state;
        pairState->connHandle = connHandle;
        pairState->status = status;

        if(ml_enqueueMsg(ML_PAIRSTATE_EVT, pairState) != SUCCESS)
        {
          ICall_free(pairState);
        }
    }
}

/*********************************************************************
 * @fn      ml_passcodeCb
 *
 * @brief   Passcode callback.
 *
 * @param   pDeviceAddr - not used
 *          connHandle - connection handle
 *          uiInpuits - if TRUE, the local device should accept a passcode input
 *          uiOutputs - if TRUE, the local device should display the passcode
 *          numComparison - the code that should be displayed for numeric
 *          comparison pairing. If this is zero, then passcode pairing is occurring.
 */
static void ml_passcodeCb(uint8_t *pDeviceAddr,
                                   uint16_t connHandle,
                                   uint8_t uiInputs,
                                   uint8_t uiOutputs,
                                   uint32_t numComparison)
{
    pzPasscodeReq_t *req =
        (pzPasscodeReq_t *)ICall_malloc(sizeof(pzPasscodeReq_t));
    if(req != NULL)
    {
        req->connHandle = connHandle;
        req->uiInputs = uiInputs;
        req->uiOutputs = uiOutputs;
        req->numComparison = numComparison;

        if(ml_enqueueMsg(ML_PASSCODE_EVT, req) != SUCCESS)
        {
          ICall_free(req);
        }
    }
    ;
}

/*
 *  Callbacks from Swi-context or stack Context
 *****************************************************************************/

/******************************************************************************
 *****************************************************************************
 *
 *  Utility functions
 *
 ****************************************************************************
 *****************************************************************************/

/*********************************************************************
 * @fn     ml_enqueueMsg
 *
 * @brief  Utility function that sends the event and data to the application.
 *         Handled in the task loop.
 *
 * @param  event    Event type
 * @param  pData    Pointer to message data
 */
static status_t ml_enqueueMsg(uint8_t event, void *pData)
{
    uint8_t success;
    pzMsg_t *pMsg = ICall_malloc(sizeof(pzMsg_t));

    if(pMsg)
    {
        pMsg->event = event;
        pMsg->pData = pData;

        success = Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t *)pMsg);
        return (success) ? SUCCESS : FAILURE;
    }

    return(bleMemAllocError);
}
#endif

/*********************************************************************
*********************************************************************/
