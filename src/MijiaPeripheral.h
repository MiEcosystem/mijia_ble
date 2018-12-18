/**************************************************************************************************
  Filename:       MijiaPeripheral.h
  Revised:
  Revision:

  Description:    This file contains the mijia Peripheral sample application
                  definitions and prototypes.


**************************************************************************************************/

#ifndef MIJIAPERIPHERAL_H
#define MIJIAPERIPHERAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */


// mijia Peripheral Task Events
#define SBP_START_DEVICE_EVT                           0x0001

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void MijiaPeripheral_Init( uint8 task_id );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 MijiaPeripheral_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MIJIAPERIPHERAL_H */
