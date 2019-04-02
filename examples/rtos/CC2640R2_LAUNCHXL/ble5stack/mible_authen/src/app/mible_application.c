/******************************************************************************

 @file  mible_application.c

 @brief porting layer for the MiBLE API, for the CC26XX familly devices of TI.

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

#include <string.h>
#include <portable/mible_api.h>
#include <portable/mible_port.h>
#include <portable/cc26xx_api.h>

#include "mible_server.h"
#include "mible_beacon.h"

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))

/*app variable*/
device_info dev_info = {
		.bonding = WEAK_BONDING, // can be modified according to product
		.pid = 0x9c, // product id, can be modified according to product
		.version = "0000",  // can be modified according to product
};

enum
{
    UNIT_0_625_MS = 625,        /**< Number of microseconds in 0.625 milliseconds. */
    UNIT_1_25_MS  = 1250,       /**< Number of microseconds in 1.25 milliseconds. */
    UNIT_10_MS    = 10000       /**< Number of microseconds in 10 milliseconds. */
};

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
    mibeacon_capability_t cap = {.connectable = 1,
                                .encryptable = 1,
                                .bondAbility = 1};

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
    
    mible_gap_adv_data_set(adv_data,adv_len,NULL,0);
    
    MI_LOG_INFO("adv mi service data:");
    MI_LOG_HEXDUMP(adv_data, adv_len);
		
	return;
}

/**@brief Function for starting advertising.
 */
void advertising_start(void){
    mible_gap_adv_param_t adv_param =(mible_gap_adv_param_t){
        .adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .adv_interval_min = MSEC_TO_UNITS(100, UNIT_0_625_MS),
        .adv_interval_max = MSEC_TO_UNITS(200, UNIT_0_625_MS),
        .ch_mask = {0},
    };
    if(MI_SUCCESS != mible_gap_adv_start(&adv_param)){
        MI_LOG_ERROR("mible_gap_adv_start failed. \r\n");
        return;
    }
    else
    {
      MI_LOG_INFO("mible_gap_adv_start OK. \r\n");
        return;
    }
}

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
	//advertising_init();
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

void std_authen_event_cb(mible_std_auth_evt_t evt,
		mible_std_auth_evt_param_t* p_param)
{
	switch(evt){
	case MIBLE_STD_AUTH_EVT_SERVICE_INIT_CMP:
		mible_service_init_cmp();
		break;
	case MIBLE_STD_AUTH_EVT_CONNECT:
		mible_gap_adv_stop();
		mible_connected();
		break;
	case MIBLE_STD_AUTH_EVT_DISCONNECT:
		mible_disconnected();
		break;
	case MIBLE_STD_AUTH_EVT_RESULT:
		mible_bonding_evt_callback(p_param->result.state);
		break;
	default:
		MI_LOG_ERROR("Unkown std authen event\r\n");
		break;
	}
}

// When this function is called, the porting layer and the BLE stack has been initialized already.
void this_is_my_mible_application(void)
{
    mible_std_auth_evt_register(std_authen_event_cb);
      
    mible_record_create(0xBEEF,0);
    
    mible_server_info_init(&dev_info, MODE_STANDARD);
    mible_server_miservice_init();
	advertising_init();
	advertising_start();
}
