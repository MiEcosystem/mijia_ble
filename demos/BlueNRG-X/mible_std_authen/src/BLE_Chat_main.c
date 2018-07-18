/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : BLE_Chat_main.c
* Author             : AMS - VMA RF Application Team
* Version            : V1.1.0
* Date               : 15-January-2016
* Description        : BlueNRG-1 main file for Chat demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/** @addtogroup BlueNRG1_demonstrations_applications
 * BlueNRG-1 Chat demo \see BLE_Chat_main.c for documentation.
 *
 *@{
 */

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h" 
#include "sleep.h"
#include "bluenrg1_stack.h"
#include "gp_timer.h"
#include "app_state.h"
#include "chat.h"
#include "SDK_EVAL_Config.h"
#include "Chat_config.h"
#include "OTA_btl.h"
#include "mible_api.h"
#include "mible_port.h"
#include "mible_beacon.h"
#include "mible_server.h"
#include "mible_crypto.h"
#include "mible_FSM.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_CHAT_VERSION_STRING "1.0.0" 




/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void receiveUartData(uint8_t *data, uint16_t len);
/*
  UUIDs:
  D973F2E0-B19E-11E2-9E96-0800200C9A66
  D973F2E1-B19E-11E2-9E96-0800200C9A66
  D973F2E2-B19E-11E2-9E96-0800200C9A66
  */

/*******************************************************************************
* Function Name  : CHAT_DeviceInit.
* Description    : Init the Chat device.
* Input          : none.
* Return         : Status.
*******************************************************************************/
uint8_t deviceInit(uint8_t isSlaveDev)
{
  uint8_t ret;
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  uint8_t name[] = {'b', 'l', 'u', 'e', 'n', 'r', 'g', 'm'};
 
  uint8_t role = GAP_PERIPHERAL_ROLE;
  uint8_t bdaddr[] = {0xa1, 0x00, 0x00, 0xE1, 0x80, 0x02};
  if(!isSlaveDev){
    role = GAP_CENTRAL_ROLE;
    bdaddr[0] = 0xb1;
  }
  
  /* Configure Public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if(ret != BLE_STATUS_SUCCESS){
    printf("Setting BD_ADDR failed: 0x%02x\r\n", ret);
    return ret;
  }

  /* Set the TX power to -2 dBm */
  aci_hal_set_tx_power_level(1, 4);

  /* GATT Init */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gatt_init(): 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gatt_init() --> SUCCESS\r\n");
  }
  /* GAP Init */
  ret = aci_gap_init(role, 0x00, 0x08, &service_handle, 
                     &dev_name_char_handle, &appearance_char_handle);
  printf ("service_handle=%d dev_name_char_handle=%d appearance_char_handle=%d\r\n",service_handle,dev_name_char_handle,appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gap_init() 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gap_init() --> SUCCESS\r\n");
  }
  /* Set the device name */
  
  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle,
                                   0, sizeof(name), name);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in Gatt Update characteristic value 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gatt_update_char_value() --> SUCCESS\r\n");
  }

  return BLE_STATUS_SUCCESS;
}


void receiveUartData(uint8_t *data, uint16_t len){
  uint8_t i = 0;
  for(;i<len;i++){
    printf("0x%x\n",data[i]);
  }
}
/*app variable*/

device_info dev_info = {
  .bonding = WEAK_BONDING, // can be modified according to product
  .pid = 0x9c, // product id, can be modified according to product
  .version = "0000",	// can be modified according to product
};

/**@brief Function for starting advertising.

 */

void advertising_start(void)
{
  mible_gap_adv_param_t adv_param =(mible_gap_adv_param_t){
    .adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
    .adv_interval_min = 0X0020,
    .adv_interval_max = 0X0020,
    .ch_mask = {0},
  };

  if(MI_SUCCESS != mible_gap_adv_start(&adv_param)){
      MI_LOG_ERROR("mible_gap_adv_start failed. \r\n");
      return;
  }
}


/**@brief Function for initializing the Advertising functionality.

 */

static void advertising_init(void)
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
  mibeacon_capability_t cap = {
    .connectable = 1,
    .encryptable = 1,
    .bondAbility = 1
  };
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
  int ret,i;
  for(i=0;i<31;i++)
    printf("adv_data[%d] = 0x%x\r\n",i,adv_data[i]);
  printf("adv_len = %d\r\n",adv_len);
  advertising_start();
  ret = mible_gap_adv_data_set(adv_data,adv_len,NULL,0);
  printf("mible_gap_adv_data_set=%d\r\n",ret);

  MI_LOG_INFO("adv mi service data:");
  MI_LOG_HEXDUMP(adv_data, adv_len);
  MI_LOG_PRINTF("test adv\r\n");
  return;
}

void check_flash_record(void);
void mible_loop(void);
int main(void) 
{
  uint8_t ret;
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* Init Clock */
  Clock_Init();
  /* Configure I/O communication channel:
       It requires the void IO_Receive_Data(uint8_t * rx_data, uint16_t data_size) function
       where user received data should be processed */
  SdkEvalComIOConfig(receiveUartData);
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
    printf("BLE Stack Initialized \n");
    deviceInit(1);
    check_flash_record();
    mible_server_info_init(&dev_info);
    mible_server_miservice_init();
    advertising_init();

  while(1){
    /* Disable UART IRQ to avoid calling BLE stack functions while BTLE_StackTick() is running. */
    NVIC_DisableIRQ(UART_IRQn);
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
    NVIC_EnableIRQ(UART_IRQn);
    mible_loop();
  
    /* Power Save management */
   // BlueNRG_Sleep(SLEEPMODE_WAKETIMER, 0, 0, 0);
  }
} /* end main() */

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}

#endif

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
  advertising_start();
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
/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
 */
