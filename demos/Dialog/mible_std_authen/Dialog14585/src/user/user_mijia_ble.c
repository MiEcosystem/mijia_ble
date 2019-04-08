#include "user_mijia_ble.h"
#include "mible_api.h"
#include "arch_console.h"
#include "mible_server.h"
#include "mible_beacon.h"
#include "mible_log.h"
#include "arch_console.h"
#include "app_easy_timer.h"

static void std_authen_event_cb(mible_std_auth_evt_t evt,
        mible_std_auth_evt_param_t* param);

static void advertising_init(void);
static void advertising_start(void);
/*app variable*/
device_info dev_info = {
	.bonding = WEAK_BONDING,
	.pid = 0x9c,
	.version = "4321",
};

//device_info dev_info = {
//	.bonding = STRONG_BONDING,
//	.pid = 930,
//	.version = "4321",
//};

void simulation_miserver_test(void)
{
	MI_LOG_INFO("simulation_miserver_test\r\n");
	mible_std_auth_evt_register(std_authen_event_cb);
	mible_server_info_init(&dev_info, MODE_STANDARD);
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
//static void advertising_start(void)
//{

//	MI_LOG_INFO("advertising_start\r\n");
//	mibeacon_frame_ctrl_t frame_ctrl = {
//	.time_protocol = 0,
//	.is_encrypt = 0,
//	.mac_include = 1,
//	.cap_include = 1,
//	.obj_include = 0,
//	.bond_confirm = 0,
//	.version = 0x03,
//	};
//	mibeacon_capability_t cap = {.connectable = 1,
//	                             .encryptable = 1,
//	                             .bondAbility = 1};

//	
//	mible_addr_t dev_mac;
//	mible_gap_address_get(dev_mac);
//	
//	mibeacon_config_t mibeacon_cfg = {
//	.frame_ctrl = frame_ctrl,
//	.pid =dev_info.pid,
//	.p_mac = (mible_addr_t*)dev_mac, 
//	.p_capability = &cap,
//	.p_obj = NULL,
//	};
//	
//	uint8_t service_data[25];
//	uint8_t service_data_len=0;
//	
////	mible_gap_adv_param_t adv_param =(mible_gap_adv_param_t){
////		.adv_data = "",
////		.adv_len = 0,
////		.scan_rsp_data = "",
////		.scan_rsp_len = 0,
////		.adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
////		.adv_interval_min = 0x00a0,
////		.adv_interval_max = 0x00b0,
////		.ch_mask = {0},
////	};
//	
//	if(MI_SUCCESS != mibeacon_service_data_set(&mibeacon_cfg, service_data, &service_data_len)){
//		MI_LOG_ERROR("mibeacon_data_set failed. \r\n");
//		return;
//	}

//	if(MI_SUCCESS != mible_adv_data_set(service_data, service_data_len, 0x06, adv_param.adv_data, &adv_param.adv_len)){
//		MI_LOG_ERROR("mible_adv_data_set failed. \r\n");
//		return;
//	}	

//	adv_param.adv_len -= 3;
//	for(uint8_t i=0;i<adv_param.adv_len;i++){
//		adv_param.adv_data[i] = adv_param.adv_data[i+3];
//	}

//	
//	if(MI_SUCCESS != mible_gap_adv_start(&adv_param)){
//		MI_LOG_ERROR("mible_gap_adv_start failed. \r\n");
//		return;
//	}

//	return;
//	
//}
