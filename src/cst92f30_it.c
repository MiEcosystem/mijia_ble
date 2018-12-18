#include "cst92f30.h"
#include "rcc.h"
#include "common.h"
#include "ll_common.h"
#include "OSAL.h"
#include "rcc.h"
#include "pwrmgr.h"
#include "hal_defs.h"
#include "hal_mcu.h"
#include "cst92f30_btlib.h"
#include "cst92f30_it.h"
#include "uart.h"
extern void clear_timer(TIM_TypeDef *TIMx);
extern int  clearTimerInt(TIM_TypeDef *TIMx);

void HardFault_Handler(void)
{
	while(1);
}

void TIM1To4_IRQHandler(void)
{
    HAL_ENTER_CRITICAL_SECTION();
	
	 if(TIM4->status & 0x1)
	 {		 
		  clearTimerInt(TIM4);   
	 }
	 
	 if(TIM3->status&0x1)
	 {		 
		  clearTimerInt(TIM3);
		  HalIncSecondTick();	    //unit time is 1s
	 }
	 
	 if(TIM2->status&0x1)   
	 {	
 		 clearTimerInt(TIM2);
		  HalIncSystick();   		//unit time is 0.625ms
	 }
	 
	  if(TIM1->status&0x1)
	 {	
 		  clearTimerInt(TIM1);
		  clear_timer(TIM1);
		  LL_evt_schedule();		 
	 }

   HAL_EXIT_CRITICAL_SECTION();
}


