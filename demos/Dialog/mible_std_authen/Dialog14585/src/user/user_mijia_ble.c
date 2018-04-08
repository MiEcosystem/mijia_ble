#include "user_mijia_ble.h"
#include "mible_api.h"
#include "arch_console.h"
#include "mible_server.h"
#include "mible_beacon.h"
#include "mible_log.h"
#include "arch_console.h"
#include "app_easy_timer.h"

static void advertising_init(void);
static void advertising_start(void);
/*app variable*/
device_info dev_info = {
	.bonding = STRONG_BONDING,
	.pid = 156,
	.version = "4321",
};



void simulation_miserver_test(void)
{
	MI_LOG_INFO("simulation_miserver_test\r\n");
	mible_server_info_init(&dev_info);
	mible_server_miservice_init();

}

// MI SERVICE 初始化完成
void mible_service_init_cmp(void)
{
	MI_LOG_INFO("mible_service_init_cmp\r\n");
	advertising_init();
	advertising_start();
}
// 连接完成
void mible_connected(void)
{
	MI_LOG_INFO("mible_connected \r\n");
}
// 断开连接 
void mible_disconnected(void)
{
	MI_LOG_INFO("mible_disconnected \r\n");
	advertising_init();
	advertising_start();
}
// mible 应用层认证结果返回 
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
    
    memcpy(adv_data, service_data, service_data_len);
    adv_len = service_data_len;
    
    mible_gap_adv_data_set(adv_data,adv_len,NULL,0);
    
    MI_LOG_INFO("adv mi service data:");
    MI_LOG_HEXDUMP(adv_data, adv_len);
    MI_LOG_PRINTF("\r\n");
	return;
}
void advertising_start(void){
    mible_gap_adv_param_t adv_param =(mible_gap_adv_param_t){
        .adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .adv_interval_min = 0x00a0,
        .adv_interval_max = 0x00b0,
        .ch_mask = {0},
    };

		mible_status_t ret = mible_gap_adv_start(&adv_param);
    if(MI_SUCCESS != ret){
        MI_LOG_ERROR("mible_gap_adv_start failed. code:%d\r\n",ret);
        return;
    }
}
