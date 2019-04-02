/******************************************************************************

   @file  UartLog.c

   @brief   This file provides a replacement / override for the
          XDC Runtime Log.h API that stores Log_info et al in
          a circular buffer and renders the text strings over
          UART in the Idle loop.

   Group: WCS, BTS
   Target Device: cc13x2_26x2

 ******************************************************************************
   
 Copyright (c) 2015-2019, Texas Instruments Incorporated
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
#include "UartLog.h"
#include <ti/drivers/UART.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>

#include <driverlib/aon_rtc.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
 * CONSTANTS
 */
#ifndef UARTLOG_OUTBUF_LEN
#  define UARTLOG_OUTBUF_LEN     128  /* Size of buffer used for log printout */
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
/* Local reduced copy of Log_EventRec */
/*********************************************************************
 * LOCAL VARIABLES
 */
#if defined(UARTLOG_ENABLE)
static UART_Handle hUart = NULL;

char uartLog_outBuf[UARTLOG_OUTBUF_LEN + 4];
uint16_t uartLog_evtNum = 1;
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      UartLog_init
 *
 * @brief   Initializes module with the handle to the UART.
 *
 * @param   handle - UART driver handle to an initialized and opened UART.
 *
 * @return  None.
 */
void UartLog_doInit(UART_Handle handle)
{
#if defined(UARTLOG_ENABLE)
    hUart = handle;
#endif
}

#if defined(UARTLOG_ENABLE)
void UartLog_log(char *file, int line, uint16_t level, char *fmt, ...)
{
    char       *bufPtr = uartLog_outBuf;
    char       *bufEndPtr = uartLog_outBuf + UARTLOG_OUTBUF_LEN - 2; // Less 2 for \r\n
    va_list va;
    uint32_t timestamp;
    //key = Task_disable();
    uint32_t hwikey;
    hwikey = Hwi_disable();

    /* Print serial number */
    System_snprintf(bufPtr, (bufEndPtr - bufPtr), "#%06u ", uartLog_evtNum++);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    /* Print timestamp */
    timestamp = AONRTCCurrentCompareValueGet();
    uint16_t seconds = timestamp >> 16;
    uint16_t ifraction = timestamp & 0xFFFF;
    int fraction = (int)((double)ifraction / 65536 * 1000);  // Get 3 decimals
    System_snprintf(bufPtr, (bufEndPtr - bufPtr), "[ %d.%03u ] ", seconds,
                    fraction);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    /* Print level, file and line */
    char *infoStr = NULL;
    switch(level)
    {
    case LEVEL_ERROR:
        infoStr = "\x1b[31;1mERROR: \x1b[30;1m(%s:%d) \x1b[0m";
        break;
    case LEVEL_WARNING:
        infoStr = "\x1b[33;1mWARNING: \x1b[30;1m(%s:%d) \x1b[0m";
        break;
    case LEVEL_INFO:
    default:
        infoStr = "\x1b[32;1mINFO: \x1b[30;1m(%s:%d) \x1b[0m";
    }
    System_snprintf(bufPtr, (bufEndPtr - bufPtr), infoStr, file, line);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    /* Ouput everything till now and start over in the buffer. */
    UART_write(hUart, uartLog_outBuf, (bufPtr - uartLog_outBuf));
    bufPtr = uartLog_outBuf;

    /* Log_write() event */
    va_start(va, fmt);
    System_vsnprintf(uartLog_outBuf, (bufEndPtr - bufPtr), fmt, va);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    *bufPtr++ = '\r';
    *bufPtr++ = '\n';

    UART_write(hUart, uartLog_outBuf, (bufPtr - uartLog_outBuf));

    Hwi_restore(hwikey);
}
#endif
