/*
 * Copyright (c) 2013-2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
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

#include <stdint.h>
#include <stdlib.h>
#include "GPIO_Adapter.h"
#include "gpio_pins.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

gpioInputPinConfig_t switchPins[] = {
    {
        .gpioPort = gpioPort_A_c, 
        .gpioPin = BOARD_SW1_GPIO_PIN, 
        .pullSelect = pinPull_Up_c,
        .interruptSelect = pinInt_FallingEdge_c
    },
    {
        .gpioPort = gpioPort_A_c, 
        .gpioPin = BOARD_SW2_GPIO_PIN, 
        .pullSelect = pinPull_Up_c,
        .interruptSelect = pinInt_FallingEdge_c
    }
};

/* Declare Output GPIO pins */
gpioOutputPinConfig_t ledPins[] = {{.gpioPort = gpioPort_A_c, /* RED */
                                    .gpioPin = 31U,
                                    .outputLogic = 0,
                                    .slewRate = pinSlewRate_Slow_c,
                                    .driveStrength = pinDriveStrength_Low_c},
                                   {.gpioPort = gpioPort_A_c, /* GREEN */
                                    .gpioPin = 25U,
                                    .outputLogic = 0,
                                    .slewRate = pinSlewRate_Slow_c,
                                    .driveStrength = pinDriveStrength_Low_c},
                                   {.gpioPort = gpioPort_A_c, /* BLUE */
                                    .gpioPin = 13U,
                                    .outputLogic = 0,
                                    .slewRate = pinSlewRate_Slow_c,
                                    .driveStrength = pinDriveStrength_Low_c}};

/* RF_DENY Coexistence Pin */
gpioInputPinConfig_t  gCoexistence_RfDeny = {
    .gpioPort = gpioPort_A_c,
    .gpioPin = 17,
    .pullSelect = pinPull_Down_c, 
    .interruptSelect = pinInt_EitherEdge_c
};
