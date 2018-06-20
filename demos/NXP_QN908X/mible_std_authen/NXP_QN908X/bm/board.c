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

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "board.h"
#include "RNG_Interface.h"
#include "GPIO_adapter.h"
#include "controller_interface.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "led.h"
#include "fsl_calibration.h"
#include "fsl_flash.h"
#include "fsl_rf.h"
#include "fsl_adc.h"
#include "fsl_syscon.h"
#include "nvds.h"
#include "rco32k_calibration.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if cPWR_UsePowerDownMode
static const uint32_t sw_pins_mask = (1U << BOARD_SW1_GPIO_PIN ) | (1U << BOARD_SW2_GPIO_PIN);
#endif

uint8_t  xtalDivRestore = 0;
uint32_t backWakeupEn[2U];
uint32_t backWakeupLvl[2U];

#ifndef gDCDC_Mode
#define gDCDC_Mode 1
#endif

#ifndef gBleRfRxMode
#define gBleRfRxMode kRxModeHighPerformance
#endif

#ifndef gBleUseHSClock2MbpsPhy_c
#define gBleUseHSClock2MbpsPhy_c 0
#endif

flash_config_t g_flash_cfg;

/************************************************************************************
*************************************************************************************
* Private type definitions and macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static uint32_t mAdcVinn;
static float mAdcBandgap;
extern const uint8_t gBD_ADDR_OUI_c[3];


/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
uint8_t nvds_get(uint8_t tag, uint16_t *lengthPtr, uint8_t *buf);
uint8_t nvds_put(uint8_t tag, nvds_tag_len_t length, uint8_t *buf);

/************************************************************************************
*************************************************************************************
* Public functions prototypes
*************************************************************************************
************************************************************************************/
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
void BOARD_GetMCUUid(uint8_t* aOutUid16B, uint8_t* pOutLen)
{   
    uint8_t uid[BD_ADDR_SIZE] = {0xFF};
    int err = -1;
    int i = 0;
    nvds_tag_len_t len = 0;
    uint32_t randomNo = 0;
    *pOutLen = BD_ADDR_SIZE;
    
    FLib_MemCpy(uid, (uint8_t *)FSL_FEATURE_FLASH_ADDR_OF_VENDOR_BD_ADDR, BD_ADDR_SIZE);
    
    while ((i < BD_ADDR_SIZE) && (uid[i] == 0xFF)){ i++; }
    if ( i == BD_ADDR_SIZE ) 
    {
        err = nvds_get(NVDS_TAG_BD_ADDRESS, &len, uid);
        if ((err != NVDS_OK) || (len != NVDS_LEN_BD_ADDRESS))
        {
#if (NVDS_READ_WRITE == 1)
            RNG_GetRandomNo(&randomNo); 
            FLib_MemCpy(uid, (void *)gBD_ADDR_OUI_c, 3);
            FLib_MemCpy(&uid[3], (void *)&randomNo, 3);
            err = nvds_put(NVDS_TAG_BD_ADDRESS, NVDS_LEN_BD_ADDRESS, uid);
            if (err != NVDS_OK)
#endif
                *pOutLen = 0;
        }
        else
        {
            *pOutLen = NVDS_LEN_BD_ADDRESS;
        }
    }
    
    FLib_MemCpy(aOutUid16B, uid, *pOutLen);
    
    return;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

uint32_t BOARD_GetUsartClock(__attribute__((unused)) uint32_t instance)
{
    return CLOCK_GetFreq(kCLOCK_CoreSysClk);
}

uint32_t BOARD_GetCtimerClock(CTIMER_Type *base)
{
    uint32_t freq = 0;

    if (kCTIMER_TimerMode == (base->CTCR & CTIMER_CTCR_CTMODE_MASK))
    {
        freq = CLOCK_GetFreq(kCLOCK_ApbClk);
    }
    //no else - freq = 0 if not in timer mode

    return freq;
}

uint32_t BOARD_GetSpiClock(uint32_t instance)
{
    (void)instance; /* Remove compiler warnings */
    return CLOCK_GetFreq(kCLOCK_BusClk);
}

static bool BOARD_IsADCEnabled(void)
{
    bool result = false;
    if(ADC->CTRL & ADC_CTRL_ENABLE_MASK)
    {
        result = true;
    }

    return result;
}

uint8_t BOARD_GetBatteryLevel(void)
{
    uint32_t adcConvResult = 0;
    float fresult;

    if (!BOARD_IsADCEnabled())
    {
        BOARD_InitAdc();
    }

    /* Select ONLY VCC monitor channel (use |= if any other ADC channels are used and needs to be preserved) */
    ADC->CH_SEL = (1U << ADC_VCC_MONITOR_CHANNEL);
    /* Select channel configuration to be used */
    ADC->CH_CFG = (ADC_CFG_IDX << ADC_VCC_MONITOR_CHANNEL);
    /* Single conversion, software triggered */
    ADC->CTRL |= kADC_ConvModeSingle | ADC_CTRL_TRIGGER(kADC_TriggerSelectSoftware);

    /* Start conversion */
    ADC_DoSoftwareTrigger(ADC);

    /* Wait for ADC conversion to complete */
    while (!(ADC_GetStatusFlags(ADC) & kADC_DataReadyFlag))
    {}

    /* Get the result */
    adcConvResult = ADC_GetConversionResult(ADC);

    /* Convert the result to mV */
    fresult = ADC_ConversionResult2Mv(ADC, ADC_VCC_MONITOR_CHANNEL, ADC_CFG_IDX, mAdcBandgap, mAdcVinn,
                                          adcConvResult);

    /* Return battery level percentage. Note that fresult represents half of VCC so it has to be multiplied by 4 */
    return (uint8_t)((fresult*4*100)/ADC_BATTERY_FULL_VOLTAGE);
}

void BOARD_InitAdc(void)
{
    adc_config_t    adcConfigStruct;
    adc_sd_config_t adcSdConfigStruct;

    /* Power on ADC */
    POWER_EnableADC(true);

    /**
     * Initial ADC to default configuration.
     */
    ADC_GetDefaultConfig(&adcConfigStruct);
    ADC_Init(ADC, &adcConfigStruct);

    /* Initial ADC Sigma Delta(SD) configuration */
    ADC_GetSdDefaultConfig(&adcSdConfigStruct);
    adcSdConfigStruct.refGain = kADC_RefGain1;
    ADC_SetSdConfig(ADC, ADC_CFG_IDX, &adcSdConfigStruct);

    /* Bandgap voltage */
    mAdcBandgap = ADC_GetBandgapCalibrationResult(ADC, ADC_CFG_IDX);

    /* Calibration VINN value */
    mAdcVinn = ADC_GetVinnCalibrationResult(ADC, &adcConfigStruct);

    ADC_PgaChopperEnable(ADC, true);
    ADC_EnableBatteryMonitor(true);

    /* Enable ADC */
    ADC_Enable(ADC, true);
}

#if cPWR_UsePowerDownMode
void BOARD_PrepareDeepSleep()
{
    /* Enable OSC_EN wakeup */
    NVIC_ClearPendingIRQ(OSC_IRQn);
    NVIC_EnableIRQ(OSC_IRQn);
    APP_SaveBleReg();

    /* power on OSC32M and switch to it */
    POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(0U));
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(0U);

    /* Set Xtal divider before the crystal is ready */
    if ((SYSCON->XTAL_CTRL & SYSCON_XTAL_CTRL_XTAL_DIV_MASK) == 0)
    {
        xtalDivRestore = 1;
        SYSCON->XTAL_CTRL |= SYSCON_XTAL_CTRL_XTAL_DIV_MASK;
    }

    //only for PD1 aka deep sleep mode 4
    if (4 == PWR_GetDeepSleepMode())
    { //Switch off 32K clock
#if (defined(BOARD_XTAL1_CLK_HZ) && (BOARD_XTAL1_CLK_HZ == CLK_XTAL_32KHZ))
        POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_XTAL32K_DIS_MASK, SYSCON_PMU_CTRL1_XTAL32K_DIS(1U));
#else
        POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_RCO32K_DIS_MASK, SYSCON_PMU_CTRL1_RCO32K_DIS(1U));
#endif
    }

#if defined(CFG_SWD_WAKEUP)
    POWER_EnableSwdWakeup();
#endif

    /* Enable GPIO wakeup */
    NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);
    NVIC_EnableIRQ(EXT_GPIO_WAKEUP_IRQn);

    /* Backup GPIO wakeup setting for GPIO wakeup source interrupt generating */
    backWakeupEn[0U] = SYSCON->PIO_WAKEUP_EN0;
    backWakeupEn[1U] = SYSCON->PIO_WAKEUP_EN1 &
                       (SYSCON_PIO_WAKEUP_EN1_PB00_WAKEUP_EN_MASK | SYSCON_PIO_WAKEUP_EN1_PB01_WAKEUP_EN_MASK |
                        SYSCON_PIO_WAKEUP_EN1_PB02_WAKEUP_EN_MASK);
    backWakeupLvl[0U] = SYSCON->PIO_WAKEUP_LVL0;
    backWakeupLvl[1U] = SYSCON->PIO_WAKEUP_LVL1;

    POWER_LatchIO();

    /* Power down affects calibration's FSM, turn it off before power down.
     * Turn off Ble core's high frequency clock before power down.
     */
    SYSCON->CLK_DIS = SYSCON_CLK_DIS_CLK_CAL_DIS_MASK | SYSCON_CLK_DIS_CLK_BLE_DIS_MASK;
}



/* Reinitialize peripherals after waked up from PD, this function will be called in critical area */
void BOARD_WakeupRestore()
{
    /* Power down affects calibration's FSM, turn it on again after waking from power down.
     * After waking up from power down, XTAL will be turned on automatically. Ble core's high clock source
       can be turn on as soon as possible, so do it in POWER_EnterPowerDown(). */
    SYSCON->CLK_EN = SYSCON_CLK_EN_CLK_CAL_EN_MASK /* | SYSCON_CLK_DIS_CLK_BLE_DIS_MASK */;
    /* imr = 1 */
    CALIB->VCO_MOD_CFG |= CALIB_VCO_MOD_CFG_IMR_MASK;
    /* AA error */
    BLEDP->DP_AA_ERROR_CTRL = 0x0000000EU;

    POWER_RestoreIO();

    /* To generate a pending interrupt for GPIO wakeup source */
    if (backWakeupEn[0U])
    {
        GPIOA->INTTYPECLR = backWakeupEn[0U];
        GPIOA->INTPOLCLR  = backWakeupEn[0U] & backWakeupLvl[0U];  /* low level interrupt (wakeup) */
        GPIOA->INTPOLSET  = backWakeupEn[0U] & ~backWakeupLvl[0U]; /* high level interrupt (wakeup) */
        GPIOA->INTENSET   = backWakeupEn[0U];
    }
    if (backWakeupEn[1U])
    {
        GPIOB->INTTYPECLR = backWakeupEn[1U];
        GPIOB->INTPOLCLR  = backWakeupEn[1U] & backWakeupLvl[1U];  /* low level interrupt (wakeup) */
        GPIOB->INTPOLSET  = backWakeupEn[1U] & ~backWakeupLvl[1U]; /* high level interrupt (wakeup) */
        GPIOB->INTENSET   = backWakeupEn[1U];
    }
    NVIC_DisableIRQ(EXT_GPIO_WAKEUP_IRQn);
    NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);

#if defined(CFG_SWD_WAKEUP)
    POWER_RestoreSwd();
    if (!(CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk))
    {
        /* if waked up by SWDIO */
        if (NVIC_GetPendingIRQ(GPIOA_IRQn) && (GPIOA->INTSTATUS & SWDIO_GPIO_PIN_MASK))
        {
            GPIOA->INTENCLR = SWDIO_GPIO_PIN_MASK;
            GPIOA->INTSTATUS = SWDIO_GPIO_PIN_MASK;
            NVIC_ClearPendingIRQ(GPIOA_IRQn);
            PWR_DisallowDeviceToSleep();
        }
    }
#endif

// only for PD1 aka eep sleep mode 4
    if (4 == PWR_GetDeepSleepMode())
    { //Switch 32K clock on
#if (defined(BOARD_XTAL1_CLK_HZ) && (BOARD_XTAL1_CLK_HZ == CLK_XTAL_32KHZ))
        SYSCON->ANA_CTRL1 = SYSCON->ANA_CTRL1 | SYSCON_ANA_CTRL1_X32_SMT_EN_MASK;
        POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_RCO32K_DIS_MASK | SYSCON_PMU_CTRL1_XTAL32K_DIS_MASK,
                          SYSCON_PMU_CTRL1_RCO32K_DIS(0U) | SYSCON_PMU_CTRL1_XTAL32K_DIS(0U));
        CLOCK_AttachClk(kRCO32K_to_32K_CLK);
        PWRLib_Start_32kXTAL_ready_timer();
        /* Prevent BLE sleep */
        BLE_disable_sleep();
#else
        POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_RCO32K_DIS_MASK, SYSCON_PMU_CTRL1_RCO32K_DIS(0U));
#endif
        if(POWER_GpioActiveRequest())
        {
            PWRLib_SendKeyPressOnPD1Wakeup();
        }
    }

    APP_RestoreBleReg();
    NVIC_DisableIRQ(OSC_IRQn);
    if (NVIC_GetPendingIRQ(OSC_IRQn))
    {
        NVIC_ClearPendingIRQ(OSC_IRQn);
        /* BLE wakeup is onging */
        BLE_prevent_sleep_on_wakeup();
    }

    /* wait until XTAL is ready, and switch to it */
    NVIC_ClearPendingIRQ(XTAL_READY_IRQn);
    NVIC_EnableIRQ(XTAL_READY_IRQn);

    while (!(SYSCON_SYS_MODE_CTRL_XTAL_RDY_MASK & SYSCON->SYS_MODE_CTRL))
    {
        POWER_EnterSleep();
    }
    
    NVIC_DisableIRQ(XTAL_READY_IRQn);
    NVIC_ClearPendingIRQ(XTAL_READY_IRQn);

    if (xtalDivRestore)
    {
        SYSCON->XTAL_CTRL &= ~SYSCON_XTAL_CTRL_XTAL_DIV_MASK;
    }

    /* switch to XTAL and power off OSC32M */
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(1U);
    POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(1U));
    
    /* Enable FPU after sleep */
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) || (defined(__VFP_FP__) && !defined(__SOFTFP__))
    SCB->CPACR |= 0x00f00000; /* set CP10, CP11 Full Access */
#endif  

    /* initialize all the GPIO pins for keyboard  and enable IRQ
     * all gpio settings are lost during deep sleep */
    GPIO_SetFallingEdgeInterrupt(GPIOA  , sw_pins_mask);
    GPIO_EnableInterrupt(GPIOA, sw_pins_mask);
    NVIC_EnableIRQ(GPIOA_IRQn);
}

void BOARD_InstallLowPowerCallbacks()
{
    PWR_RegisterLowPowerEnterCallback((pfPWRCallBack_t)BOARD_PrepareDeepSleep);
    PWR_RegisterLowPowerExitCallback((pfPWRCallBack_t)BOARD_WakeupRestore);
}
#endif /* cPWR_UsePowerDownMode */

uint16_t BOARD_GetPotentiometerLevel(void)
{
    uint32_t random = 0;
    RNG_GetRandomNo(&random);
    
    return (random & 0x0FFF);
}


void GPIOA_IRQHandler(void)
{
   Gpio_CommonIsr();
}

void RTC_FR_IRQHandler(void)
{
    StackTimer_ISR();
}

void hardware_init(void)
{
    uint8_t  nvds_error = 0;
    uint8_t  loadCap    = 0;
    uint16_t nvds_len   = 0;
        
    POWER_EnableDCDC(gDCDC_Mode);
    RF_ConfigRxMode(SYSCON, gBleRfRxMode);

    FLASH_GetDefaultConfig(&g_flash_cfg);
//g_flash_cfg.blockBase = 0x0U;
    FLASH_Init(&g_flash_cfg);
    
    /* Configure Xtal Load Capacitors */
    nvds_error = nvds_init((uint8_t *)CFG_NVDS_ADDRESS, CFG_NVDS_SIZE,
                           (uint8_t *)CFG_NVDS_BACKUP_ADDRESS, NULL, &g_flash_cfg);
    if (0 == nvds_error)
    {
        nvds_len    = NVDS_LEN_XCSEL;
        nvds_error  = nvds_get(NVDS_TAG_XCSEL, (uint16_t *)&nvds_len, &loadCap);
        if (0 == nvds_error)
        {
            SYSCON_SetLoadCap(SYSCON, 1, loadCap);
        }

        nvds_len    = NVDS_LEN_32K_XCSEL;
        nvds_error  = nvds_get(NVDS_TAG_32K_XCSEL, (uint16_t *)&nvds_len, &loadCap);
        if (0 == nvds_error)
        {
            SYSCON_SetLoadCap(SYSCON, 0, loadCap);
        }
    }

    CALIB_SystemCalib();
    
#if gBleUseHSClock2MbpsPhy_c
    BOARD_BootClockHSRUN();
    CLOCK_AttachClk(k16M_to_BLE_CLK);
#else
    BOARD_InitBootClocks();
#endif
    
    BOARD_InitPins();
    
#if cPWR_UsePowerDownMode
    /* Install callbacks to handle enter and exit low power */
    BOARD_InstallLowPowerCallbacks();
    /* sw1 can wake the board up */
    SetGpioWakeupLevel(BOARD_SW_GPIO, BOARD_SW1_GPIO_PIN, 0U);
#endif

#if (defined(BOARD_XTAL1_CLK_HZ) && (BOARD_XTAL1_CLK_HZ != CLK_XTAL_32KHZ))
    RCO32K_InitSwCalib(5000);         /* Enable periodic 32k RCO calibration */
#endif
}
/*******************************************************************************
 * EOF
 ******************************************************************************/
