/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
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

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v3.0
processor: QN908XC
package_id: QN9080C
mcu_data: ksdk2_0
processor_version: 2.0.0
board: QN908XCDK
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_iocon.h"
#include "pin_mux.h"

/*FUNCTION**********************************************************************
 * 
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 * 
 *END**************************************************************************/
void BOARD_InitBootPins(void) {
    BOARD_InitPins();
}

#define PIO_FUNC_ALT4                 0x04u   /*!< Function control register: Alternative connection 4. */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '28', peripheral: FLEXCOMM0, signal: RXD, pin_signal: GPIOA17/CS3/SD_DAC/CTIMER2_MAT1/FC0_RXD/FC3_MISO/QDEC0_B, identifier: ''}
  - {pin_num: '29', peripheral: FLEXCOMM0, signal: TXD, pin_signal: GPIOA16/CS2/SCT0_OUT1/CTIMER2_MAT0/FC0_TXD/FC3_MOSI/QDEC0_A, identifier: ''}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPins(void) {
  SYSCON->PIO_FUNC_CFG[2] = ((SYSCON->PIO_FUNC_CFG[2] &
    (~(SYSCON_PIO_FUNC_CFG_PA16_FUNC_MASK | SYSCON_PIO_FUNC_CFG_PA17_FUNC_MASK))) /* Mask bits to zero which are setting */
      | SYSCON_PIO_FUNC_CFG_PA16_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN16 (coords: 29) is configured as FC0_TXD */
      | SYSCON_PIO_FUNC_CFG_PA17_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN17 (coords: 28) is configured as FC0_RXD */
    );
}


#define PIN19_IDX                       19u   /*!< Pin number for pin 19 in a port */
#define PIN24_IDX                       24u   /*!< Pin number for pin 24 in a port */
#define PORTA_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitButtons:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '21', peripheral: GPIOA, signal: 'GPIOA, 24', pin_signal: GPIOA24/ACMP0N/CS6/ETM_TRACEDAT0/CTIMER3_CAP0/RFE_RX_EN/FC3_SSEL1/SPIFI_IO0, identifier: SW1,
    direction: INPUT, pull_control: Pull_up, drive_strength: low}
  - {pin_num: '26', peripheral: GPIOA, signal: 'GPIOA, 19', pin_signal: GPIOA19/CS5/SCT0_OUT2/RFE_EN/FC0_SCK/FC3_SSEL3/BLE_IN_PROC, direction: INPUT, pull_control: Pull_up,
    drive_strength: low, drive_extra: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitButtons
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitButtons(void) {
  const uint32_t portA_pin19_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_PULLUP |                                      /* Selects pull-up function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN19_IDX, portA_pin19_config); /* PORTA PIN19 (coords: 26) is configured as GPIOA19 */
  const uint32_t portA_pin24_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_PULLUP |                                      /* Selects pull-up function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN24_IDX, portA_pin24_config); /* PORTA PIN24 (coords: 21) is configured as GPIOA24 */
}


#define PIN13_IDX                       13u   /*!< Pin number for pin 13 in a port */
#define PIN25_IDX                       25u   /*!< Pin number for pin 25 in a port */
#define PIN31_IDX                       31u   /*!< Pin number for pin 31 in a port */
#define PORTA_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitLEDs:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '12', peripheral: GPIOA, signal: 'GPIOA, 31', pin_signal: GPIOA31/DAC/RTC_CAP/CTIMER3_MAT2/SWO/FC3_SCK/SPIFI_CLK, identifier: LED_RED, direction: OUTPUT,
    pull_control: Pull_up, drive_strength: low}
  - {pin_num: '32', peripheral: GPIOA, signal: 'GPIOA, 13', pin_signal: GPIOA13/SCT0_OUT4/ACMP1_OUT/FC1_RXD_SDA/FC3_SSEL1/RFE_EN, direction: OUTPUT, pull_control: Pull_up,
    drive_strength: low}
  - {pin_num: '20', peripheral: GPIOA, signal: 'GPIOA, 25', pin_signal: GPIOA25/ACMP0P/CS7/ETM_TRACEDAT1/CTIMER3_CAP1/RFE_TX_EN/FC3_SSEL0/SPIFI_IO1, identifier: LED_GREEN,
    direction: OUTPUT, pull_control: Pull_up, drive_strength: low}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitLEDs
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitLEDs(void) {
  const uint32_t portA_pin13_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_PULLUP |                                      /* Selects pull-up function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN13_IDX, portA_pin13_config); /* PORTA PIN13 (coords: 32) is configured as GPIOA13 */
  const uint32_t portA_pin25_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_PULLUP |                                      /* Selects pull-up function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN25_IDX, portA_pin25_config); /* PORTA PIN25 (coords: 20) is configured as GPIOA25 */
  const uint32_t portA_pin31_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_PULLUP |                                      /* Selects pull-up function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN31_IDX, portA_pin31_config); /* PORTA PIN31 (coords: 12) is configured as GPIOA31 */
}


#define PIN16_IDX                       16u   /*!< Pin number for pin 16 in a port */
#define PIN17_IDX                       17u   /*!< Pin number for pin 17 in a port */
#define PORTA_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitDEBUG_UART:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '29', peripheral: FLEXCOMM0, signal: TXD, pin_signal: GPIOA16/CS2/SCT0_OUT1/CTIMER2_MAT0/FC0_TXD/FC3_MOSI/QDEC0_A, pull_control: High_Z, drive_strength: low}
  - {pin_num: '28', peripheral: FLEXCOMM0, signal: RXD, pin_signal: GPIOA17/CS3/SD_DAC/CTIMER2_MAT1/FC0_RXD/FC3_MISO/QDEC0_B, pull_control: High_Z, drive_strength: low}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitDEBUG_UART
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitDEBUG_UART(void) {
  const uint32_t portA_pin16_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN16_IDX, portA_pin16_config); /* PORTA PIN16 (coords: 29) is configured as FC0_TXD */
  const uint32_t portA_pin17_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN17_IDX, portA_pin17_config); /* PORTA PIN17 (coords: 28) is configured as FC0_RXD */
}


#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port */
#define PIN31_IDX                       31u   /*!< Pin number for pin 31 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */
#define PIN4_IDX                         4u   /*!< Pin number for pin 4 in a port */
#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port */
#define PIN6_IDX                         6u   /*!< Pin number for pin 6 in a port */
#define PIN7_IDX                         7u   /*!< Pin number for pin 7 in a port */
#define PORTA_IDX                        0u   /*!< Port index */
#define PORTB_IDX                        1u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPMod_SPI_I2C:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '4', peripheral: FLEXCOMM1, signal: CTS_SDA, pin_signal: GPIOA7/ADC_VREFI/SCT0_OUT2/CTIMER1_CAP0/FC1_CTS_SDA/BLE_PTI1/SPIFI_CSN, identifier: PMOD_SDA,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '5', peripheral: FLEXCOMM1, signal: RTS_SCL, pin_signal: GPIOA6/SCT0_OUT3/CTIMER0_MAT2/FC1_RTS_SCL/BLE_PTI0/SPIFI_CLK, identifier: PMOD_SCL, pull_control: High_Z,
    drive_strength: low, drive_extra: disabled}
  - {pin_num: '6', peripheral: FLEXCOMM2, signal: SCL_MISO, pin_signal: GPIOA5/ADC3/SCT0_OUT5/CTIMER0_MAT1/FC0_RXD/FC2_SCL_MISO/SPIFI_IO1, identifier: PMOD_MISO,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '7', peripheral: FLEXCOMM2, signal: SDA_MOSI, pin_signal: GPIOA4/ADC2/SCT0_OUT4/CTIMER0_MAT0/FC0_TXD/FC2_SDA_MOSI/SPIFI_IO0, identifier: PMOD_MOSI,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '8', peripheral: FLEXCOMM2, signal: SDA_SSEL0, pin_signal: GPIOA3/QDEC0_B/SCT0_OUT3/CTIMER0_MAT1/FC2_SDA_SSEL0/RFE_TX_EN, identifier: PMOD_SSEL, pull_control: High_Z,
    drive_strength: low}
  - {pin_num: '15', peripheral: FLEXCOMM2, signal: SCK, pin_signal: GPIOA30/ACMP1P/ETM_TRACEDAT3/CTIMER3_MAT1/FC2_SCK/FC3_MOSI/SPIFI_IO3, identifier: PMOD_FC2_SCK,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '33', peripheral: GPIOB, signal: 'GPIOB, 2', pin_signal: GPIOB2/ANT_SW, pull_control: High_Z, drive_strength: low}
  - {pin_num: '12', peripheral: FLEXCOMM3, signal: SCK, pin_signal: GPIOA31/DAC/RTC_CAP/CTIMER3_MAT2/SWO/FC3_SCK/SPIFI_CLK, identifier: PMOD_FC3_SCK, pull_control: High_Z,
    drive_strength: low}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPMod_SPI_I2C
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPMod_SPI_I2C(void) {
  const uint32_t portA_pin3_config = (
    IOCON_FUNC5 |                                            /* Selects pin function 5 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN3_IDX, portA_pin3_config); /* PORTA PIN3 (coords: 8) is configured as FC2_SDA_SSEL0 */
  const uint32_t portA_pin4_config = (
    IOCON_FUNC5 |                                            /* Selects pin function 5 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN4_IDX, portA_pin4_config); /* PORTA PIN4 (coords: 7) is configured as FC2_SDA_MOSI */
  const uint32_t portA_pin5_config = (
    IOCON_FUNC5 |                                            /* Selects pin function 5 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN5_IDX, portA_pin5_config); /* PORTA PIN5 (coords: 6) is configured as FC2_SCL_MISO */
  const uint32_t portA_pin6_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN6_IDX, portA_pin6_config); /* PORTA PIN6 (coords: 5) is configured as FC1_RTS_SCL */
  const uint32_t portA_pin7_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN7_IDX, portA_pin7_config); /* PORTA PIN7 (coords: 4) is configured as FC1_CTS_SDA */
  const uint32_t portA_pin30_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN30_IDX, portA_pin30_config); /* PORTA PIN30 (coords: 15) is configured as FC2_SCK */
  const uint32_t portA_pin31_config = (
    IOCON_FUNC5 |                                            /* Selects pin function 5 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN31_IDX, portA_pin31_config); /* PORTA PIN31 (coords: 12) is configured as FC3_SCK */
  const uint32_t portB_pin2_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTB_IDX, PIN2_IDX, portB_pin2_config); /* PORTB PIN2 (coords: 33) is configured as GPIOB2 */
}


#define PIN22_IDX                       22u   /*!< Pin number for pin 22 in a port */
#define PIN23_IDX                       23u   /*!< Pin number for pin 23 in a port */
#define PIN31_IDX                       31u   /*!< Pin number for pin 31 in a port */
#define PORTA_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitSWD_DEBUG:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '23', peripheral: SWD, signal: SWCLK, pin_signal: SWCLK/GPIOA22/SCT0_IN2/CTIMER3_MAT0/FC2_SDA_SSEL0/FC3_SSEL3/QDEC1_A, pull_control: Pull_down, drive_strength: low}
  - {pin_num: '22', peripheral: SWD, signal: SWDIO, pin_signal: SWDIO/GPIOA23/SCT0_IN3/CTIMER3_MAT1/FC2_SCL_SSEL1/FC3_SSEL2/QDEC1_B, pull_control: High_Z, drive_strength: low}
  - {pin_num: '12', peripheral: SWD, signal: SWO, pin_signal: GPIOA31/DAC/RTC_CAP/CTIMER3_MAT2/SWO/FC3_SCK/SPIFI_CLK, identifier: SWO, pull_control: High_Z, drive_strength: low}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitSWD_DEBUG
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitSWD_DEBUG(void) {
  const uint32_t portA_pin22_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_PULLDOWN |                                    /* Selects pull-down function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN22_IDX, portA_pin22_config); /* PORTA PIN22 (coords: 23) is configured as SWCLK */
  const uint32_t portA_pin23_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN23_IDX, portA_pin23_config); /* PORTA PIN23 (coords: 22) is configured as SWDIO */
  const uint32_t portA_pin31_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN31_IDX, portA_pin31_config); /* PORTA PIN31 (coords: 12) is configured as SWO */
}


#define PIN24_IDX                       24u   /*!< Pin number for pin 24 in a port */
#define PIN25_IDX                       25u   /*!< Pin number for pin 25 in a port */
#define PIN28_IDX                       28u   /*!< Pin number for pin 28 in a port */
#define PIN29_IDX                       29u   /*!< Pin number for pin 29 in a port */
#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port */
#define PORTA_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitETM_TRACE:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '16', peripheral: ETM, signal: 'ETM_TRACEDATA, 2', pin_signal: GPIOA29/ACMP1N/ETM_TRACEDAT2/CTIMER3_MAT0/FC2_SCK/FC3_MISO/SPIFI_IO2, pull_control: High_Z,
    drive_strength: low}
  - {pin_num: '17', peripheral: ETM, signal: ETM_TRACECLK, pin_signal: GPIOA28/CLK_AHB/ETM_TRACECLK/RTC_CAP/FC1_SCK/SD_DAC/SPIFI_CSN, pull_control: High_Z, drive_strength: low}
  - {pin_num: '15', peripheral: ETM, signal: 'ETM_TRACEDATA, 3', pin_signal: GPIOA30/ACMP1P/ETM_TRACEDAT3/CTIMER3_MAT1/FC2_SCK/FC3_MOSI/SPIFI_IO3, identifier: ETM_TRACEDAT3,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '21', peripheral: ETM, signal: 'ETM_TRACEDATA, 0', pin_signal: GPIOA24/ACMP0N/CS6/ETM_TRACEDAT0/CTIMER3_CAP0/RFE_RX_EN/FC3_SSEL1/SPIFI_IO0, identifier: ETM_TRACEDAT0,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '20', peripheral: ETM, signal: 'ETM_TRACEDATA, 1', pin_signal: GPIOA25/ACMP0P/CS7/ETM_TRACEDAT1/CTIMER3_CAP1/RFE_TX_EN/FC3_SSEL0/SPIFI_IO1, identifier: ETM_TRACEDAT1,
    pull_control: High_Z, drive_strength: low}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitETM_TRACE
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitETM_TRACE(void) {
  const uint32_t portA_pin24_config = (
    IOCON_FUNC2 |                                            /* Selects pin function 2 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN24_IDX, portA_pin24_config); /* PORTA PIN24 (coords: 21) is configured as ETM_TRACEDAT0 */
  const uint32_t portA_pin25_config = (
    IOCON_FUNC2 |                                            /* Selects pin function 2 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN25_IDX, portA_pin25_config); /* PORTA PIN25 (coords: 20) is configured as ETM_TRACEDAT1 */
  const uint32_t portA_pin28_config = (
    IOCON_FUNC2 |                                            /* Selects pin function 2 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN28_IDX, portA_pin28_config); /* PORTA PIN28 (coords: 17) is configured as ETM_TRACECLK */
  const uint32_t portA_pin29_config = (
    IOCON_FUNC2 |                                            /* Selects pin function 2 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN29_IDX, portA_pin29_config); /* PORTA PIN29 (coords: 16) is configured as ETM_TRACEDAT2 */
  const uint32_t portA_pin30_config = (
    IOCON_FUNC2 |                                            /* Selects pin function 2 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN30_IDX, portA_pin30_config); /* PORTA PIN30 (coords: 15) is configured as ETM_TRACEDAT3 */
}


#define PIN26_IDX                       26u   /*!< Pin number for pin 26 in a port */
#define PIN27_IDX                       27u   /*!< Pin number for pin 27 in a port */
#define PORTA_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitUSB:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '18', peripheral: USB0, signal: USB_DM, pin_signal: GPIOA27/USB_DM/SCT0_IN1/CTIMER1_MAT2/FC2_SCL_MISO/QDEC0_B/BLE_IN_PROC, pull_control: High_Z, drive_strength: low,
    drive_extra: disabled}
  - {pin_num: '19', peripheral: USB0, signal: USB_DP, pin_signal: GPIOA26/USB_DP/SCT0_IN0/CTIMER1_MAT0/FC2_SDA_MOSI/QDEC0_A/BLE_SYNC, pull_control: High_Z, drive_strength: low,
    drive_extra: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitUSB
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitUSB(void) {
  const uint32_t portA_pin26_config = (
    IOCON_FUNC1 |                                            /* Selects pin function 1 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN26_IDX, portA_pin26_config); /* PORTA PIN26 (coords: 19) is configured as USB_DP */
  const uint32_t portA_pin27_config = (
    IOCON_FUNC1 |                                            /* Selects pin function 1 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN27_IDX, portA_pin27_config); /* PORTA PIN27 (coords: 18) is configured as USB_DM */
}


#define PIN14_IDX                       14u   /*!< Pin number for pin 14 in a port */
#define PIN6_IDX                         6u   /*!< Pin number for pin 6 in a port */
#define PIN7_IDX                         7u   /*!< Pin number for pin 7 in a port */
#define PORTA_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitG_Sensor:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '5', peripheral: FLEXCOMM1, signal: RTS_SCL, pin_signal: GPIOA6/SCT0_OUT3/CTIMER0_MAT2/FC1_RTS_SCL/BLE_PTI0/SPIFI_CLK, identifier: G_SENSOR_SCL, pull_control: High_Z,
    drive_strength: low, drive_extra: disabled}
  - {pin_num: '4', peripheral: FLEXCOMM1, signal: CTS_SDA, pin_signal: GPIOA7/ADC_VREFI/SCT0_OUT2/CTIMER1_CAP0/FC1_CTS_SDA/BLE_PTI1/SPIFI_CSN, identifier: G_SENSOR_SDA,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '31', peripheral: GPIOA, signal: 'GPIOA, 14', pin_signal: GPIOA14/CS0/ANT_SW/CTIMER2_CAP0/FC0_RTS/FC3_SSEL0/QDEC1_A, pull_control: High_Z, drive_strength: low}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitG_Sensor
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitG_Sensor(void) {
  const uint32_t portA_pin6_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN6_IDX, portA_pin6_config); /* PORTA PIN6 (coords: 5) is configured as FC1_RTS_SCL */
  const uint32_t portA_pin7_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN7_IDX, portA_pin7_config); /* PORTA PIN7 (coords: 4) is configured as FC1_CTS_SDA */
  const uint32_t portA_pin14_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN14_IDX, portA_pin14_config); /* PORTA PIN14 (coords: 31) is configured as GPIOA14 */
}


#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */
#define PIN4_IDX                         4u   /*!< Pin number for pin 4 in a port */
#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port */
#define PORTA_IDX                        0u   /*!< Port index */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitSPI_FLASH:
- options: {prefix: BOARD_, coreID: cm4}
- pin_list:
  - {pin_num: '8', peripheral: FLEXCOMM2, signal: SDA_SSEL0, pin_signal: GPIOA3/QDEC0_B/SCT0_OUT3/CTIMER0_MAT1/FC2_SDA_SSEL0/RFE_TX_EN, identifier: SPI_FLASH_SSEL,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '6', peripheral: FLEXCOMM2, signal: SCL_MISO, pin_signal: GPIOA5/ADC3/SCT0_OUT5/CTIMER0_MAT1/FC0_RXD/FC2_SCL_MISO/SPIFI_IO1, identifier: SPI_FLASH_MISO,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '7', peripheral: FLEXCOMM2, signal: SDA_MOSI, pin_signal: GPIOA4/ADC2/SCT0_OUT4/CTIMER0_MAT0/FC0_TXD/FC2_SDA_MOSI/SPIFI_IO0, identifier: SPI_FLASH_MOSI,
    pull_control: High_Z, drive_strength: low}
  - {pin_num: '15', peripheral: FLEXCOMM2, signal: SCK, pin_signal: GPIOA30/ACMP1P/ETM_TRACEDAT3/CTIMER3_MAT1/FC2_SCK/FC3_MOSI/SPIFI_IO3, identifier: SPI_FLASH_FC2_SCK,
    pull_control: High_Z, drive_strength: low}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitSPI_FLASH
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitSPI_FLASH(void) {
  const uint32_t portA_pin3_config = (
    IOCON_FUNC5 |                                            /* Selects pin function 5 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN3_IDX, portA_pin3_config); /* PORTA PIN3 (coords: 8) is configured as FC2_SDA_SSEL0 */
  const uint32_t portA_pin4_config = (
    IOCON_FUNC5 |                                            /* Selects pin function 5 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN4_IDX, portA_pin4_config); /* PORTA PIN4 (coords: 7) is configured as FC2_SDA_MOSI */
  const uint32_t portA_pin5_config = (
    IOCON_FUNC5 |                                            /* Selects pin function 5 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN5_IDX, portA_pin5_config); /* PORTA PIN5 (coords: 6) is configured as FC2_SCL_MISO */
  const uint32_t portA_pin30_config = (
    IOCON_FUNC4 |                                            /* Selects pin function 4 */
    IOCON_MODE_HIGHZ |                                       /* Selects High-Z function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTA_IDX, PIN30_IDX, portA_pin30_config); /* PORTA PIN30 (coords: 15) is configured as FC2_SCK */
}

#define PIO_FUNC_ALT4                 0x04u   /*!< Function control register: Alternative connection 4. */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitSPI:
- options: {coreID: cm4}
- pin_list:
  - {pin_num: '16', peripheral: FLEXCOMM2, signal: SCK, pin_signal: GPIOA29/ACMP1N/ETM_TRACEDAT2/CTIMER3_MAT0/FC2_SCK/FC3_MISO/SPIFI_IO2}
  - {pin_num: '18', peripheral: FLEXCOMM2, signal: SCL_MISO, pin_signal: GPIOA27/USB_DM/SCT0_IN1/CTIMER1_MAT2/FC2_SCL_MISO/QDEC0_B/BLE_IN_PROC}
  - {pin_num: '22', peripheral: FLEXCOMM2, signal: SCL_SSEL1, pin_signal: SWDIO/GPIOA23/SCT0_IN3/CTIMER3_MAT1/FC2_SCL_SSEL1/FC3_SSEL2/QDEC1_B}
  - {pin_num: '19', peripheral: FLEXCOMM2, signal: SDA_MOSI, pin_signal: GPIOA26/USB_DP/SCT0_IN0/CTIMER1_MAT0/FC2_SDA_MOSI/QDEC0_A/BLE_SYNC}
  - {pin_num: '23', peripheral: FLEXCOMM2, signal: SDA_SSEL0, pin_signal: SWCLK/GPIOA22/SCT0_IN2/CTIMER3_MAT0/FC2_SDA_SSEL0/FC3_SSEL3/QDEC1_A, identifier: ''}
  - {pin_num: '43', peripheral: FLEXCOMM2, signal: SSEL2, pin_signal: GPIOA11/ADC7/SCT0_IN3/CTIMER1_MAT2/FC2_SSEL2/ACMP1_OUT/BLE_RX}
  - {pin_num: '24', peripheral: FLEXCOMM2, signal: SSEL3, pin_signal: GPIOA21/QDEC1_B/SCT0_OUT0/CTIMER2_MAT1/FC2_SSEL3/FC1_CTS_SDA/SPIFI_CSN}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitSPI
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitSPI(void) {
  SYSCON->PIO_FUNC_CFG[1] = ((SYSCON->PIO_FUNC_CFG[1] &
    (~(SYSCON_PIO_FUNC_CFG_PA11_FUNC_MASK)))                 /* Mask bits to zero which are setting */
      | SYSCON_PIO_FUNC_CFG_PA11_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN11 (coords: 43) is configured as FC2_SSEL2 */
    );
  SYSCON->PIO_FUNC_CFG[2] = ((SYSCON->PIO_FUNC_CFG[2] &
    (~(SYSCON_PIO_FUNC_CFG_PA21_FUNC_MASK | SYSCON_PIO_FUNC_CFG_PA22_FUNC_MASK | SYSCON_PIO_FUNC_CFG_PA23_FUNC_MASK))) /* Mask bits to zero which are setting */
      | SYSCON_PIO_FUNC_CFG_PA21_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN21 (coords: 24) is configured as FC2_SSEL3 */
      | SYSCON_PIO_FUNC_CFG_PA22_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN22 (coords: 23) is configured as FC2_SDA_SSEL0 */
      | SYSCON_PIO_FUNC_CFG_PA23_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN23 (coords: 22) is configured as FC2_SCL_SSEL1 */
    );
  SYSCON->PIO_FUNC_CFG[3] = ((SYSCON->PIO_FUNC_CFG[3] &
    (~(SYSCON_PIO_FUNC_CFG_PA26_FUNC_MASK | SYSCON_PIO_FUNC_CFG_PA27_FUNC_MASK | SYSCON_PIO_FUNC_CFG_PA29_FUNC_MASK))) /* Mask bits to zero which are setting */
      | SYSCON_PIO_FUNC_CFG_PA26_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN26 (coords: 19) is configured as FC2_SDA_MOSI */
      | SYSCON_PIO_FUNC_CFG_PA27_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN27 (coords: 18) is configured as FC2_SCL_MISO */
      | SYSCON_PIO_FUNC_CFG_PA29_FUNC(PIO_FUNC_ALT4)         /* PORTA PIN29 (coords: 16) is configured as FC2_SCK */
    );
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

