/*
 * Copyright 2017 NXP
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

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

/* GPIOA24 (number 21), U21[24]/J5[20]/U22[7]/QN_PA24_TRACE_D0/SW1 */
#define BOARD_SW1_PERIPHERAL                                               GPIOA   /*!< Device name: GPIOA */
#define BOARD_SW1_SIGNAL                                                   GPIOA   /*!< GPIOA signal: GPIOA */
#define BOARD_SW1_CHANNEL                                                     24   /*!< GPIOA GPIOA channel: 24 */
#define BOARD_SW1_PIN_NAME                                               GPIOA24   /*!< Pin name */
#define BOARD_SW1_LABEL             "U21[24]/J5[20]/U22[7]/QN_PA24_TRACE_D0/SW1"   /*!< Label */
#define BOARD_SW1_NAME                                                     "SW1"   /*!< Identifier name */
#define BOARD_SW1_DIRECTION                              kPIN_MUX_DirectionInput   /*!< Direction */

/* GPIOA19 (number 26), U21[29]/J5[14]/SW2 */
#define BOARD_SW2_PERIPHERAL                                               GPIOA   /*!< Device name: GPIOA */
#define BOARD_SW2_SIGNAL                                                   GPIOA   /*!< GPIOA signal: GPIOA */
#define BOARD_SW2_CHANNEL                                                     19   /*!< GPIOA GPIOA channel: 19 */
#define BOARD_SW2_PIN_NAME                                               GPIOA19   /*!< Pin name */
#define BOARD_SW2_LABEL                                     "U21[29]/J5[14]/SW2"   /*!< Label */
#define BOARD_SW2_NAME                                                     "SW2"   /*!< Identifier name */
#define BOARD_SW2_DIRECTION                              kPIN_MUX_DirectionInput   /*!< Direction */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitButtons(void);

/* GPIOA31 (number 12), U21[17]/U26[2]/J8[8]/J7[20]/U3[12]/QN_PA31_TDO_SWO/LED */
#define BOARD_LED_RED_PERIPHERAL                                           GPIOA   /*!< Device name: GPIOA */
#define BOARD_LED_RED_SIGNAL                                               GPIOA   /*!< GPIOA signal: GPIOA */
#define BOARD_LED_RED_CHANNEL                                                 31   /*!< GPIOA GPIOA channel: 31 */
#define BOARD_LED_RED_PIN_NAME                                           GPIOA31   /*!< Pin name */
#define BOARD_LED_RED_LABEL "U21[17]/U26[2]/J8[8]/J7[20]/U3[12]/QN_PA31_TDO_SWO/LED" /*!< Label */
#define BOARD_LED_RED_NAME                                             "LED_RED"   /*!< Identifier name */
#define BOARD_LED_RED_DIRECTION                         kPIN_MUX_DirectionOutput   /*!< Direction */

/* GPIOA13 (number 32), U21[35]/U25[2]/J5[12]/QN_PA13_LED */
#define BOARD_LED_BLUE_PERIPHERAL                                          GPIOA   /*!< Device name: GPIOA */
#define BOARD_LED_BLUE_SIGNAL                                              GPIOA   /*!< GPIOA signal: GPIOA */
#define BOARD_LED_BLUE_CHANNEL                                                13   /*!< GPIOA GPIOA channel: 13 */
#define BOARD_LED_BLUE_PIN_NAME                                          GPIOA13   /*!< Pin name */
#define BOARD_LED_BLUE_LABEL                 "U21[35]/U25[2]/J5[12]/QN_PA13_LED"   /*!< Label */
#define BOARD_LED_BLUE_NAME                                           "LED_BLUE"   /*!< Identifier name */
#define BOARD_LED_BLUE_DIRECTION                        kPIN_MUX_DirectionOutput   /*!< Direction */

/* GPIOA25 (number 20), U21[23]/J7[12]/U22[6]/U25[5]/QN_PA25_TRACE_D1/LED */
#define BOARD_LED_GREEN_PERIPHERAL                                         GPIOA   /*!< Device name: GPIOA */
#define BOARD_LED_GREEN_SIGNAL                                             GPIOA   /*!< GPIOA signal: GPIOA */
#define BOARD_LED_GREEN_CHANNEL                                               25   /*!< GPIOA GPIOA channel: 25 */
#define BOARD_LED_GREEN_PIN_NAME                                         GPIOA25   /*!< Pin name */
#define BOARD_LED_GREEN_LABEL "U21[23]/J7[12]/U22[6]/U25[5]/QN_PA25_TRACE_D1/LED"  /*!< Label */
#define BOARD_LED_GREEN_NAME                                         "LED_GREEN"   /*!< Identifier name */
#define BOARD_LED_GREEN_DIRECTION                       kPIN_MUX_DirectionOutput   /*!< Direction */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLEDs(void);

/* FC0_TXD (number 29), U21[32]/J7[13]/U7[5]/QN_PA16_U0_TXD */
#define BOARD_DEBUG_UART_TX_PERIPHERAL                                 FLEXCOMM0   /*!< Device name: FLEXCOMM0 */
#define BOARD_DEBUG_UART_TX_SIGNAL                                           TXD   /*!< FLEXCOMM0 signal: TXD */
#define BOARD_DEBUG_UART_TX_PIN_NAME                                     FC0_TXD   /*!< Pin name */
#define BOARD_DEBUG_UART_TX_LABEL          "U21[32]/J7[13]/U7[5]/QN_PA16_U0_TXD"   /*!< Label */
#define BOARD_DEBUG_UART_TX_NAME                                 "DEBUG_UART_TX"   /*!< Identifier name */

/* FC0_RXD (number 28), U21[31]/J7[15]/U7[4]/QN_PA17_U0_RXD */
#define BOARD_DEBUG_UART_RX_PERIPHERAL                                 FLEXCOMM0   /*!< Device name: FLEXCOMM0 */
#define BOARD_DEBUG_UART_RX_SIGNAL                                           RXD   /*!< FLEXCOMM0 signal: RXD */
#define BOARD_DEBUG_UART_RX_PIN_NAME                                     FC0_RXD   /*!< Pin name */
#define BOARD_DEBUG_UART_RX_LABEL          "U21[31]/J7[15]/U7[4]/QN_PA17_U0_RXD"   /*!< Label */
#define BOARD_DEBUG_UART_RX_NAME                                 "DEBUG_UART_RX"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitDEBUG_UART(void);

/* FC1_CTS_SDA (number 4), U21[8]/U16[3]/J5[3]/J8[10]/QN_PA07_SDA */
#define BOARD_PMOD_SDA_PERIPHERAL                                      FLEXCOMM1   /*!< Device name: FLEXCOMM1 */
#define BOARD_PMOD_SDA_SIGNAL                                            CTS_SDA   /*!< FLEXCOMM1 signal: CTS_SDA */
#define BOARD_PMOD_SDA_PIN_NAME                                      FC1_CTS_SDA   /*!< Pin name */
#define BOARD_PMOD_SDA_LABEL            "U21[8]/U16[3]/J5[3]/J8[10]/QN_PA07_SDA"   /*!< Label */
#define BOARD_PMOD_SDA_NAME                                           "PMOD_SDA"   /*!< Identifier name */

/* FC1_RTS_SCL (number 5), U21[9]/U16[2]/J5[1]/J8[9]/QN_PA06_SCL */
#define BOARD_PMOD_SCL_PERIPHERAL                                      FLEXCOMM1   /*!< Device name: FLEXCOMM1 */
#define BOARD_PMOD_SCL_SIGNAL                                            RTS_SCL   /*!< FLEXCOMM1 signal: RTS_SCL */
#define BOARD_PMOD_SCL_PIN_NAME                                      FC1_RTS_SCL   /*!< Pin name */
#define BOARD_PMOD_SCL_LABEL             "U21[9]/U16[2]/J5[1]/J8[9]/QN_PA06_SCL"   /*!< Label */
#define BOARD_PMOD_SCL_NAME                                           "PMOD_SCL"   /*!< Identifier name */

/* FC2_SCL_MISO (number 6), U21[10]/U15[2]/J5[11]/J8[3]/QN_PA05_MISO */
#define BOARD_PMOD_MISO_PERIPHERAL                                     FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_PMOD_MISO_SIGNAL                                          SCL_MISO   /*!< FLEXCOMM2 signal: SCL_MISO */
#define BOARD_PMOD_MISO_PIN_NAME                                    FC2_SCL_MISO   /*!< Pin name */
#define BOARD_PMOD_MISO_LABEL         "U21[10]/U15[2]/J5[11]/J8[3]/QN_PA05_MISO"   /*!< Label */
#define BOARD_PMOD_MISO_NAME                                         "PMOD_MISO"   /*!< Identifier name */

/* FC2_SDA_MOSI (number 7), U21[11]/U15[5]/J5[13]/J8[2]/QN_PA04_MOSI */
#define BOARD_PMOD_MOSI_PERIPHERAL                                     FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_PMOD_MOSI_SIGNAL                                          SDA_MOSI   /*!< FLEXCOMM2 signal: SDA_MOSI */
#define BOARD_PMOD_MOSI_PIN_NAME                                    FC2_SDA_MOSI   /*!< Pin name */
#define BOARD_PMOD_MOSI_LABEL         "U21[11]/U15[5]/J5[13]/J8[2]/QN_PA04_MOSI"   /*!< Label */
#define BOARD_PMOD_MOSI_NAME                                         "PMOD_MOSI"   /*!< Identifier name */

/* FC2_SDA_SSEL0 (number 8), U21[12]/U15[1]/J5[15]/J8[1]/QN_PA03_SSEL */
#define BOARD_PMOD_SSEL_PERIPHERAL                                     FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_PMOD_SSEL_SIGNAL                                         SDA_SSEL0   /*!< FLEXCOMM2 signal: SDA_SSEL0 */
#define BOARD_PMOD_SSEL_PIN_NAME                                   FC2_SDA_SSEL0   /*!< Pin name */
#define BOARD_PMOD_SSEL_LABEL         "U21[12]/U15[1]/J5[15]/J8[1]/QN_PA03_SSEL"   /*!< Label */
#define BOARD_PMOD_SSEL_NAME                                         "PMOD_SSEL"   /*!< Identifier name */

/* FC2_SCK (number 15), U21[18]/U15[6]/J5[9]/J8[4]/QN_PA30_TRACE_D3/SCK */
#define BOARD_PMOD_FC2_SCK_PERIPHERAL                                  FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_PMOD_FC2_SCK_SIGNAL                                            SCK   /*!< FLEXCOMM2 signal: SCK */
#define BOARD_PMOD_FC2_SCK_PIN_NAME                                      FC2_SCK   /*!< Pin name */
#define BOARD_PMOD_FC2_SCK_LABEL "U21[18]/U15[6]/J5[9]/J8[4]/QN_PA30_TRACE_D3/SCK" /*!< Label */
#define BOARD_PMOD_FC2_SCK_NAME                                   "PMOD_FC2_SCK"   /*!< Identifier name */

/* GPIOB2 (number 33), U21[36]/J7[10]/J8[7]/QN_PB02 */
#define BOARD_PMOD_PB02_PERIPHERAL                                         GPIOB   /*!< Device name: GPIOB */
#define BOARD_PMOD_PB02_SIGNAL                                             GPIOB   /*!< GPIOB signal: GPIOB */
#define BOARD_PMOD_PB02_CHANNEL                                                2   /*!< GPIOB GPIOB channel: 2 */
#define BOARD_PMOD_PB02_PIN_NAME                                          GPIOB2   /*!< Pin name */
#define BOARD_PMOD_PB02_LABEL                     "U21[36]/J7[10]/J8[7]/QN_PB02"   /*!< Label */
#define BOARD_PMOD_PB02_NAME                                         "PMOD_PB02"   /*!< Identifier name */

/* FC3_SCK (number 12), U21[17]/U26[2]/J8[8]/J7[20]/U3[12]/QN_PA31_TDO_SWO/LED */
#define BOARD_PMOD_FC3_SCK_PERIPHERAL                                  FLEXCOMM3   /*!< Device name: FLEXCOMM3 */
#define BOARD_PMOD_FC3_SCK_SIGNAL                                            SCK   /*!< FLEXCOMM3 signal: SCK */
#define BOARD_PMOD_FC3_SCK_PIN_NAME                                      FC3_SCK   /*!< Pin name */
#define BOARD_PMOD_FC3_SCK_LABEL "U21[17]/U26[2]/J8[8]/J7[20]/U3[12]/QN_PA31_TDO_SWO/LED" /*!< Label */
#define BOARD_PMOD_FC3_SCK_NAME                                   "PMOD_FC3_SCK"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPMod_SPI_I2C(void);

/* SWCLK (number 23), U21[26]/J5[16]/U3[4]/QN_PA22_TCK_SWDCLK */
#define BOARD_SWCLK_PERIPHERAL                                               SWD   /*!< Device name: SWD */
#define BOARD_SWCLK_SIGNAL                                                 SWCLK   /*!< SWD signal: SWCLK */
#define BOARD_SWCLK_PIN_NAME                                               SWCLK   /*!< Pin name */
#define BOARD_SWCLK_LABEL              "U21[26]/J5[16]/U3[4]/QN_PA22_TCK_SWDCLK"   /*!< Label */
#define BOARD_SWCLK_NAME                                                 "SWCLK"   /*!< Identifier name */

/* SWDIO (number 22), U21[25]/J5[18]/U5[4]/QN_PA23_TMS_SWDIO */
#define BOARD_SWDIO_PERIPHERAL                                               SWD   /*!< Device name: SWD */
#define BOARD_SWDIO_SIGNAL                                                 SWDIO   /*!< SWD signal: SWDIO */
#define BOARD_SWDIO_PIN_NAME                                               SWDIO   /*!< Pin name */
#define BOARD_SWDIO_LABEL               "U21[25]/J5[18]/U5[4]/QN_PA23_TMS_SWDIO"   /*!< Label */
#define BOARD_SWDIO_NAME                                                 "SWDIO"   /*!< Identifier name */

/* SWO (number 12), U21[17]/U26[2]/J8[8]/J7[20]/U3[12]/QN_PA31_TDO_SWO/LED */
#define BOARD_SWO_PERIPHERAL                                                 SWD   /*!< Device name: SWD */
#define BOARD_SWO_SIGNAL                                                     SWO   /*!< SWD signal: SWO */
#define BOARD_SWO_PIN_NAME                                                   SWO   /*!< Pin name */
#define BOARD_SWO_LABEL "U21[17]/U26[2]/J8[8]/J7[20]/U3[12]/QN_PA31_TDO_SWO/LED"   /*!< Label */
#define BOARD_SWO_NAME                                                     "SWO"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSWD_DEBUG(void);

/* ETM_TRACEDAT2 (number 16), U21[19]/J7[9]/U23[6]/QN_PA29_TRACE_D2/PWM7 */
#define BOARD_ETM_TRACEDAT2_PERIPHERAL                                       ETM   /*!< Device name: ETM */
#define BOARD_ETM_TRACEDAT2_SIGNAL                                 ETM_TRACEDATA   /*!< ETM signal: ETM_TRACEDATA */
#define BOARD_ETM_TRACEDAT2_CHANNEL                                            2   /*!< ETM ETM_TRACEDATA channel: 2 */
#define BOARD_ETM_TRACEDAT2_PIN_NAME                               ETM_TRACEDAT2   /*!< Pin name */
#define BOARD_ETM_TRACEDAT2_LABEL   "U21[19]/J7[9]/U23[6]/QN_PA29_TRACE_D2/PWM7"   /*!< Label */
#define BOARD_ETM_TRACEDAT2_NAME                                 "ETM_TRACEDAT2"   /*!< Identifier name */

/* ETM_TRACECLK (number 17), U21[20]/J7[18]/U24[4]/QN_PA28_TRACE_CLK */
#define BOARD_ETM_TRACECLK_PERIPHERAL                                        ETM   /*!< Device name: ETM */
#define BOARD_ETM_TRACECLK_SIGNAL                                   ETM_TRACECLK   /*!< ETM signal: ETM_TRACECLK */
#define BOARD_ETM_TRACECLK_PIN_NAME                                 ETM_TRACECLK   /*!< Pin name */
#define BOARD_ETM_TRACECLK_LABEL       "U21[20]/J7[18]/U24[4]/QN_PA28_TRACE_CLK"   /*!< Label */
#define BOARD_ETM_TRACECLK_NAME                                   "ETM_TRACECLK"   /*!< Identifier name */

/* ETM_TRACEDAT3 (number 15), U21[18]/U15[6]/J5[9]/J8[4]/QN_PA30_TRACE_D3/SCK */
#define BOARD_ETM_TRACEDAT3_PERIPHERAL                                       ETM   /*!< Device name: ETM */
#define BOARD_ETM_TRACEDAT3_SIGNAL                                 ETM_TRACEDATA   /*!< ETM signal: ETM_TRACEDATA */
#define BOARD_ETM_TRACEDAT3_CHANNEL                                            3   /*!< ETM ETM_TRACEDATA channel: 3 */
#define BOARD_ETM_TRACEDAT3_PIN_NAME                               ETM_TRACEDAT3   /*!< Pin name */
#define BOARD_ETM_TRACEDAT3_LABEL "U21[18]/U15[6]/J5[9]/J8[4]/QN_PA30_TRACE_D3/SCK" /*!< Label */
#define BOARD_ETM_TRACEDAT3_NAME                                 "ETM_TRACEDAT3"   /*!< Identifier name */

/* ETM_TRACEDAT0 (number 21), U21[24]/J5[20]/U22[7]/QN_PA24_TRACE_D0/SW1 */
#define BOARD_ETM_TRACEDAT0_PERIPHERAL                                       ETM   /*!< Device name: ETM */
#define BOARD_ETM_TRACEDAT0_SIGNAL                                 ETM_TRACEDATA   /*!< ETM signal: ETM_TRACEDATA */
#define BOARD_ETM_TRACEDAT0_CHANNEL                                            0   /*!< ETM ETM_TRACEDATA channel: 0 */
#define BOARD_ETM_TRACEDAT0_PIN_NAME                               ETM_TRACEDAT0   /*!< Pin name */
#define BOARD_ETM_TRACEDAT0_LABEL   "U21[24]/J5[20]/U22[7]/QN_PA24_TRACE_D0/SW1"   /*!< Label */
#define BOARD_ETM_TRACEDAT0_NAME                                 "ETM_TRACEDAT0"   /*!< Identifier name */

/* ETM_TRACEDAT1 (number 20), U21[23]/J7[12]/U22[6]/U25[5]/QN_PA25_TRACE_D1/LED */
#define BOARD_ETM_TRACEDAT1_PERIPHERAL                                       ETM   /*!< Device name: ETM */
#define BOARD_ETM_TRACEDAT1_SIGNAL                                 ETM_TRACEDATA   /*!< ETM signal: ETM_TRACEDATA */
#define BOARD_ETM_TRACEDAT1_CHANNEL                                            1   /*!< ETM ETM_TRACEDATA channel: 1 */
#define BOARD_ETM_TRACEDAT1_PIN_NAME                               ETM_TRACEDAT1   /*!< Pin name */
#define BOARD_ETM_TRACEDAT1_LABEL "U21[23]/J7[12]/U22[6]/U25[5]/QN_PA25_TRACE_D1/LED" /*!< Label */
#define BOARD_ETM_TRACEDAT1_NAME                                 "ETM_TRACEDAT1"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitETM_TRACE(void);

/* USB_DM (number 18), U21[21]/J7[16]/J1[2]/QN_PA27_USB_DM */
#define BOARD_USB_DM_PERIPHERAL                                             USB0   /*!< Device name: USB0 */
#define BOARD_USB_DM_SIGNAL                                               USB_DM   /*!< USB0 signal: USB_DM */
#define BOARD_USB_DM_PIN_NAME                                             USB_DM   /*!< Pin name */
#define BOARD_USB_DM_LABEL                 "U21[21]/J7[16]/J1[2]/QN_PA27_USB_DM"   /*!< Label */
#define BOARD_USB_DM_NAME                                               "USB_DM"   /*!< Identifier name */

/* USB_DP (number 19), U21[22]/J[7]14/J1[3]/QN_PA26_USB_DP */
#define BOARD_USB_DP_PERIPHERAL                                             USB0   /*!< Device name: USB0 */
#define BOARD_USB_DP_SIGNAL                                               USB_DP   /*!< USB0 signal: USB_DP */
#define BOARD_USB_DP_PIN_NAME                                             USB_DP   /*!< Pin name */
#define BOARD_USB_DP_LABEL                 "U21[22]/J[7]14/J1[3]/QN_PA26_USB_DP"   /*!< Label */
#define BOARD_USB_DP_NAME                                               "USB_DP"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitUSB(void);

/* FC1_RTS_SCL (number 5), U21[9]/U16[2]/J5[1]/J8[9]/QN_PA06_SCL */
#define BOARD_G_SENSOR_SCL_PERIPHERAL                                  FLEXCOMM1   /*!< Device name: FLEXCOMM1 */
#define BOARD_G_SENSOR_SCL_SIGNAL                                        RTS_SCL   /*!< FLEXCOMM1 signal: RTS_SCL */
#define BOARD_G_SENSOR_SCL_PIN_NAME                                  FC1_RTS_SCL   /*!< Pin name */
#define BOARD_G_SENSOR_SCL_LABEL         "U21[9]/U16[2]/J5[1]/J8[9]/QN_PA06_SCL"   /*!< Label */
#define BOARD_G_SENSOR_SCL_NAME                                   "G_SENSOR_SCL"   /*!< Identifier name */

/* FC1_CTS_SDA (number 4), U21[8]/U16[3]/J5[3]/J8[10]/QN_PA07_SDA */
#define BOARD_G_SENSOR_SDA_PERIPHERAL                                  FLEXCOMM1   /*!< Device name: FLEXCOMM1 */
#define BOARD_G_SENSOR_SDA_SIGNAL                                        CTS_SDA   /*!< FLEXCOMM1 signal: CTS_SDA */
#define BOARD_G_SENSOR_SDA_PIN_NAME                                  FC1_CTS_SDA   /*!< Pin name */
#define BOARD_G_SENSOR_SDA_LABEL        "U21[8]/U16[3]/J5[3]/J8[10]/QN_PA07_SDA"   /*!< Label */
#define BOARD_G_SENSOR_SDA_NAME                                   "G_SENSOR_SDA"   /*!< Identifier name */

/* GPIOA14 (number 31), U21[34]/U16[5]/J5[19]/QN_PA14_INT/G_INT */
#define BOARD_G_INT_PERIPHERAL                                             GPIOA   /*!< Device name: GPIOA */
#define BOARD_G_INT_SIGNAL                                                 GPIOA   /*!< GPIOA signal: GPIOA */
#define BOARD_G_INT_CHANNEL                                                   14   /*!< GPIOA GPIOA channel: 14 */
#define BOARD_G_INT_PIN_NAME                                             GPIOA14   /*!< Pin name */
#define BOARD_G_INT_LABEL              "U21[34]/U16[5]/J5[19]/QN_PA14_INT/G_INT"   /*!< Label */
#define BOARD_G_INT_NAME                                                 "G_INT"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitG_Sensor(void);

/* FC2_SDA_SSEL0 (number 8), U21[12]/U15[1]/J5[15]/J8[1]/QN_PA03_SSEL */
#define BOARD_SPI_FLASH_SSEL_PERIPHERAL                                FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_SPI_FLASH_SSEL_SIGNAL                                    SDA_SSEL0   /*!< FLEXCOMM2 signal: SDA_SSEL0 */
#define BOARD_SPI_FLASH_SSEL_PIN_NAME                              FC2_SDA_SSEL0   /*!< Pin name */
#define BOARD_SPI_FLASH_SSEL_LABEL    "U21[12]/U15[1]/J5[15]/J8[1]/QN_PA03_SSEL"   /*!< Label */
#define BOARD_SPI_FLASH_SSEL_NAME                               "SPI_FLASH_SSEL"   /*!< Identifier name */

/* FC2_SCL_MISO (number 6), U21[10]/U15[2]/J5[11]/J8[3]/QN_PA05_MISO */
#define BOARD_SPI_FLASH_MISO_PERIPHERAL                                FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_SPI_FLASH_MISO_SIGNAL                                     SCL_MISO   /*!< FLEXCOMM2 signal: SCL_MISO */
#define BOARD_SPI_FLASH_MISO_PIN_NAME                               FC2_SCL_MISO   /*!< Pin name */
#define BOARD_SPI_FLASH_MISO_LABEL    "U21[10]/U15[2]/J5[11]/J8[3]/QN_PA05_MISO"   /*!< Label */
#define BOARD_SPI_FLASH_MISO_NAME                               "SPI_FLASH_MISO"   /*!< Identifier name */

/* FC2_SDA_MOSI (number 7), U21[11]/U15[5]/J5[13]/J8[2]/QN_PA04_MOSI */
#define BOARD_SPI_FLASH_MOSI_PERIPHERAL                                FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_SPI_FLASH_MOSI_SIGNAL                                     SDA_MOSI   /*!< FLEXCOMM2 signal: SDA_MOSI */
#define BOARD_SPI_FLASH_MOSI_PIN_NAME                               FC2_SDA_MOSI   /*!< Pin name */
#define BOARD_SPI_FLASH_MOSI_LABEL    "U21[11]/U15[5]/J5[13]/J8[2]/QN_PA04_MOSI"   /*!< Label */
#define BOARD_SPI_FLASH_MOSI_NAME                               "SPI_FLASH_MOSI"   /*!< Identifier name */

/* FC2_SCK (number 15), U21[18]/U15[6]/J5[9]/J8[4]/QN_PA30_TRACE_D3/SCK */
#define BOARD_SPI_FLASH_FC2_SCK_PERIPHERAL                             FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_SPI_FLASH_FC2_SCK_SIGNAL                                       SCK   /*!< FLEXCOMM2 signal: SCK */
#define BOARD_SPI_FLASH_FC2_SCK_PIN_NAME                                 FC2_SCK   /*!< Pin name */
#define BOARD_SPI_FLASH_FC2_SCK_LABEL "U21[18]/U15[6]/J5[9]/J8[4]/QN_PA30_TRACE_D3/SCK" /*!< Label */
#define BOARD_SPI_FLASH_FC2_SCK_NAME                         "SPI_FLASH_FC2_SCK"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSPI_FLASH(void);

/* FC2_SCK (number 16), U21[19]/J7[9]/U23[6]/QN_PA29_TRACE_D2/PWM7 */
#define BOARD_INITSPI_ETM_TRACEDAT2_PERIPHERAL                         FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_INITSPI_ETM_TRACEDAT2_SIGNAL                                   SCK   /*!< FLEXCOMM2 signal: SCK */
#define BOARD_INITSPI_ETM_TRACEDAT2_PIN_NAME                             FC2_SCK   /*!< Pin name */
#define BOARD_INITSPI_ETM_TRACEDAT2_LABEL "U21[19]/J7[9]/U23[6]/QN_PA29_TRACE_D2/PWM7" /*!< Label */
#define BOARD_INITSPI_ETM_TRACEDAT2_NAME                         "ETM_TRACEDAT2"   /*!< Identifier name */

/* FC2_SCL_MISO (number 18), U21[21]/J7[16]/J1[2]/QN_PA27_USB_DM */
#define BOARD_INITSPI_USB_DM_PERIPHERAL                                FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_INITSPI_USB_DM_SIGNAL                                     SCL_MISO   /*!< FLEXCOMM2 signal: SCL_MISO */
#define BOARD_INITSPI_USB_DM_PIN_NAME                               FC2_SCL_MISO   /*!< Pin name */
#define BOARD_INITSPI_USB_DM_LABEL         "U21[21]/J7[16]/J1[2]/QN_PA27_USB_DM"   /*!< Label */
#define BOARD_INITSPI_USB_DM_NAME                                       "USB_DM"   /*!< Identifier name */

/* FC2_SCL_SSEL1 (number 22), U21[25]/J5[18]/U5[4]/QN_PA23_TMS_SWDIO */
#define BOARD_INITSPI_SWDIO_PERIPHERAL                                 FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_INITSPI_SWDIO_SIGNAL                                     SCL_SSEL1   /*!< FLEXCOMM2 signal: SCL_SSEL1 */
#define BOARD_INITSPI_SWDIO_PIN_NAME                               FC2_SCL_SSEL1   /*!< Pin name */
#define BOARD_INITSPI_SWDIO_LABEL       "U21[25]/J5[18]/U5[4]/QN_PA23_TMS_SWDIO"   /*!< Label */
#define BOARD_INITSPI_SWDIO_NAME                                         "SWDIO"   /*!< Identifier name */

/* FC2_SDA_MOSI (number 19), U21[22]/J[7]14/J1[3]/QN_PA26_USB_DP */
#define BOARD_INITSPI_USB_DP_PERIPHERAL                                FLEXCOMM2   /*!< Device name: FLEXCOMM2 */
#define BOARD_INITSPI_USB_DP_SIGNAL                                     SDA_MOSI   /*!< FLEXCOMM2 signal: SDA_MOSI */
#define BOARD_INITSPI_USB_DP_PIN_NAME                               FC2_SDA_MOSI   /*!< Pin name */
#define BOARD_INITSPI_USB_DP_LABEL         "U21[22]/J[7]14/J1[3]/QN_PA26_USB_DP"   /*!< Label */
#define BOARD_INITSPI_USB_DP_NAME                                       "USB_DP"   /*!< Identifier name */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSPI(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
