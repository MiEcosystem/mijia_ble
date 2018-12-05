/**
 * SUPPORT:
 * 1. Standard ble device (MI_BLE_AUTHEN);
 * 2. Remote device (MI_REMOTE_CONTROLLER);
 * 3. Wifi_Config device (MI_WIFI_CONFIG)
 * */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "mible_server.h"
#include "mible_api.h"
#include "mible_FSM.h"
#include "mible_port.h"
#include "mible_log.h"
#include "string.h"
#include "mible_crypto.h"
#include "mible_beacon.h"


#define MIBLE_SRV_UUID                         	0XFE95//{0x95,0xFE}  

#define MIBLE_STD_CHAR_NUM					   	7

#define MIBLE_CHAR_UUID_TOKEN                  	0x0001
#define MIBLE_CHAR_UUID_PRODUCTID              	0x0002
#define MIBLE_CHAR_UUID_VERSION                	0x0004
#define MIBLE_CHAR_UUID_WIFICFG                	0x0005
#define MIBLE_CHAR_UUID_AUTHENTICATION         	0x0010
#define MIBLE_CHAR_UUID_DID                    	0x0013
#define MIBLE_CHAR_UUID_BEACONKEY              	0x0014
#define MIBLE_CHAR_UUID_DEVICE_LIST             0x0015 // no use for now
#define MIBLE_CHAR_UUID_SECURE                 	0x0016 // no use for now 

#define MIBLE_CHAR_LEN_TOKEN                   	12
#define MIBLE_CHAR_LEN_PRODUCTID               	2
#define MIBLE_CHAR_LEN_VERSION                 	10
#define MIBLE_CHAR_LEN_WIFICFG                 	20
#define MIBLE_CHAR_LEN_AUTHENTICATION          	4
#define MIBLE_CHAR_LEN_DID                     	20
#define MIBLE_CHAR_LEN_BEACONKEY               	12
#define MIBLE_CHAR_LEN_DEVICE                  	20
#define MIBLE_CHAR_LEN_SECURE                  	20


#define MIBLE_STD_REGISTER_AUTH 				0xDE85CA90
#define MIBLE_STD_LOGIN_AUTH 					0xCD43BC00
#define MIBLE_STD_WAIT_T2_ACK_TOKEN 			0xFA54AB92
#define MIBLE_STD_BONDING_SUCC_AUTH				0xAC61B130
#define MIBLE_STD_BONDING_FAIL_AUTH				0xE941BF30
#define MIBLE_STD_WAIT_LOGIN_ACK_TOKEN 		 	0x93BFAC09
#define MIBLE_STD_LOGIN_CFM_TOKEN				0x369A58C9

#define MIBLE_DID_RECORD_ID						0x01
#define MIBLE_TOKEN_RECORD_ID					0x02
#define MIBLE_BEACONKEY_RECORD_ID				0x03

#define CHAR_TOKEN_POSITION						0
#define CHAR_PID_POSITION						1
#define CHAR_VERSION_POSITION					2
#define CHAR_WIFICFG_POSITION					3
#define CHAR_AUTHEN_POSITION					4
#define CHAR_DID_POSITION						5
#define CHAR_BEACONKEY_POSITION					6

#define FSM_LIST_NUM							9

#ifndef MIBLE_MAX_USERS
#define MIBLE_MAX_USERS 4
#endif

uint16_t mible_server_connection_handle=0xffff;

//uint16_t server_conn_handle; 
static uint8_t version_fix[7]="1.1.2_";
typedef struct {
	uint16_t srv_handle; 
	uint16_t token_handle;  // add characteristic handle is value handle 
	uint16_t pid_handle;
	uint16_t version_handle;
	uint16_t wifi_cfg_handle;
	uint16_t auth_handle;
	uint16_t did_handle;
	uint16_t beaconkey_handle;
	uint16_t device_list_handle; 
	uint16_t security_handle;
}mible_service_handle_t;
static mible_service_handle_t service_handle;


typedef struct {
	uint8_t did[20];//if exist DID record, init using record ;if not init 0xFF; 
	uint8_t token[12]; 
	uint8_t beacon_key[12];  // random   
 	mible_addr_t mac;
	uint8_t pid[2]; // must init 
	uint8_t version[10]; // must init
	mible_gatts_bonding_property bonding_property;
}mible_service_info;
uint8_t token_tmp[12];
static __ALIGN(4) mible_service_info srv_info;

/* FSM state*/
enum{
	MIBLE_SERVER_FSM_STA_IDLE = 0,
	MIBLE_SERVER_FSM_STA_START = 1,
	MIBLE_SERVER_FSM_STA_WAIT_T1 = 2,
	MIBLE_SERVER_FSM_STA_WAIT_LOGIN_ACK = 3,
	MIBLE_SERVER_FSM_STA_WAIT_T2_ACK = 4,
	MIBLE_SERVER_FSM_STA_WAIT_DID = 5,
	MIBLE_SERVER_FSM_STA_WAIT_CLOUD_ACK = 6,
	MIBLE_SERVER_FSM_STA_WAIT_APP_ACK = 7,
	MIBLE_SERVER_FSM_STA_BONDING = 8,
	MIBLE_SERVER_FSM_STA_LOGIN_SUCC = 9,
	MIBLE_SERVER_FSM_STA_WIFI_CONFIG = 10,
};

/* FSM trigger event*/
enum{
	STD_SERVER_FSM_EVT_INIT_CMP,	
	STD_SERVER_FSM_EVT_AUTH_CHANGED,
  	STD_SERVER_FSM_EVT_TOKEN_CHANGED,	
	STD_SERVER_FSM_EVT_DID_CHANGED,
	STD_SERVER_FSM_EVT_CONFIG_WIFI_CHANGED,
};

static enum{
	REGISTER,
	LOGIN,
}mible_authen_mode;



/*static FSM handler*/
static void mible_server_init_handler(void* arg);
static void mible_server_start_handler(void* arg);
static void mible_server_wait_t1_handler(void* arg);
static void mible_server_wait_t2_ack_handler(void* arg);
static void mible_server_wait_did_handler(void* arg);
static void mible_server_wait_cloud_ack_handler(void* arg);
static void mible_server_wait_app_ack_handler(void* arg);
static void mible_server_wait_login_ack_handler(void* arg);
static void mible_server_wait_wifi_config_handler(void* arg);

static void mible_server_bonding_handler(void);
static void mible_server_login_succ_handler(void);

static bool mible_server_isValid_did(uint8_t* did);
static void mible_server_timeout_handler(void*);
static void mible_server_free(mible_gatts_write_t* std_gatts_write);

static const mible_FSM_list_t std_server_FSM_list[] = {
	{MIBLE_SERVER_FSM_STA_IDLE, STD_SERVER_FSM_EVT_INIT_CMP, 
		mible_server_init_handler}, //next: MIBLE_SERVER_FSM_STA_START 
	
	{MIBLE_SERVER_FSM_STA_START, STD_SERVER_FSM_EVT_AUTH_CHANGED,
		mible_server_start_handler},//next: MIBLE_SERVER_FSM_STA_WAIT_T1 / MIBLE_SERVER_FSM_STA_WAIT_LOGGING_ACK

	{MIBLE_SERVER_FSM_STA_WAIT_T1, STD_SERVER_FSM_EVT_TOKEN_CHANGED,
		mible_server_wait_t1_handler}, //next: MIBLE_SERVER_FSM_STA_WAIT_T2_ACK

	{MIBLE_SERVER_FSM_STA_WAIT_LOGIN_ACK, STD_SERVER_FSM_EVT_TOKEN_CHANGED, 
		mible_server_wait_login_ack_handler},//next: MIBLE_SERVER_FSM_STA_LOGIN_SUCC / MIBLE_SERVER_FSM_STA_START 

	{MIBLE_SERVER_FSM_STA_WAIT_T2_ACK, STD_SERVER_FSM_EVT_TOKEN_CHANGED,
		mible_server_wait_t2_ack_handler}, //next: MIBLE_SERVER_FSM_STA_WAIT_APP_ACK/ MIBLE_SERVER_FSM_STA_WAIT_DID/ MIBLE_SERVER_FSM_STA_CLOUD_ACK/MIBLE_SERVER_FSM_STA_BONDING

	{MIBLE_SERVER_FSM_STA_WAIT_APP_ACK, STD_SERVER_FSM_EVT_AUTH_CHANGED,
		mible_server_wait_app_ack_handler}, // next:MIBLE_SERVER_FSM_STA_START/ MIBLE_SERVER_FSM_STA_BONDING
 	
	{MIBLE_SERVER_FSM_STA_WAIT_DID, STD_SERVER_FSM_EVT_DID_CHANGED,
		mible_server_wait_did_handler}, //next: MIBLE_SERVER_FSM_STA_START/ MIBLE_SERVER_FSM_STA_WAIT_APP_ACK/ MIBLE_SERVER_FSM_STA_CLOUD_ACK

	{MIBLE_SERVER_FSM_STA_WAIT_CLOUD_ACK, STD_SERVER_FSM_EVT_AUTH_CHANGED,
		mible_server_wait_cloud_ack_handler}, //next: MIBLE_SERVER_FSM_STA_START/ MIBLE_SERVER_FSM_STA_BONDING

	{MIBLE_SERVER_FSM_STA_WIFI_CONFIG, STD_SERVER_FSM_EVT_CONFIG_WIFI_CHANGED,
   		mible_server_wait_wifi_config_handler},// next: MIBLE_SERVER_FSM_STA_WIFI_CONFIG,MIBLE_SERVER_FSM_STA_START
};

static mible_FSM_t std_server_FSM;
static uint8_t data_buffer[20] = {0}; // for data send
static void* server_timer ;
static uint8_t tick[4] = {0}; 

static uint8_t m_std_auth_users=0;

static mible_std_auth_callback_t m_std_auth_cb_table[MIBLE_MAX_USERS];

static uint8_t m_wifi_config_users=0;
static mible_wifi_config_callback_t m_wifi_config_cb_table[MIBLE_MAX_USERS];
wifi_info_t wifi_info;

static mible_function_t MIBLE_FUNCTION = STD_AUTHEN; 

/*
 * initialize service and characteristic 
 * */
mible_status_t mible_server_miservice_init(void)
{
	MI_LOG_DEBUG("[mible_server_miservice_init]\r\n");
	mible_status_t ret = 0;
	mible_gatts_db_t mi_std_gatts_db;
	mible_gatts_srv_db_t *p_mi_service = (mible_gatts_srv_db_t*)malloc(sizeof(mible_gatts_srv_db_t)); 
	mible_gatts_char_db_t *p_mi_char = (mible_gatts_char_db_t *)malloc(MIBLE_STD_CHAR_NUM*sizeof(mible_gatts_char_db_t));

	mi_std_gatts_db = (mible_gatts_db_t){
		.p_srv_db = p_mi_service,
		.srv_num = 1,
	};

// service init 
	*p_mi_service = (mible_gatts_srv_db_t){
		.srv_type = MIBLE_PRIMARY_SERVICE,
		.srv_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_SRV_UUID, },
		.char_num = MIBLE_STD_CHAR_NUM,
		.p_char_db = p_mi_char,
	};

// char token init 
	*(p_mi_char + CHAR_TOKEN_POSITION) = (mible_gatts_char_db_t){
		.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_CHAR_UUID_TOKEN, },	
		.char_property = MIBLE_WRITE | MIBLE_NOTIFY,
		.p_value = data_buffer,
		.char_value_len = MIBLE_CHAR_LEN_TOKEN,
		.is_variable_len = true,
		.rd_author = false,
		.wr_author = false,
		.char_desc_db = (mible_gatts_char_desc_db_t) {.extend_prop = NULL, .char_format = NULL, .user_desc = NULL,},
	};	
	

// char pid init 
	*(p_mi_char + CHAR_PID_POSITION) = (mible_gatts_char_db_t){
		.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_CHAR_UUID_PRODUCTID, },	
		.char_property = MIBLE_READ,
		.p_value = srv_info.pid,
		.char_value_len = MIBLE_CHAR_LEN_PRODUCTID,
		.is_variable_len = false,
		.rd_author = false,
		.wr_author = false,
		.char_desc_db = (mible_gatts_char_desc_db_t) {.extend_prop = NULL, .char_format = NULL, .user_desc = NULL,},
	};	

// char version init 
	uint8_t version_encrypt[MIBLE_CHAR_LEN_VERSION]={0};
	mible_std_server_encrypt(srv_info.version, MIBLE_CHAR_LEN_VERSION, version_encrypt);
	*(p_mi_char + CHAR_VERSION_POSITION) = (mible_gatts_char_db_t){
		.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_CHAR_UUID_VERSION, },	
		.char_property = MIBLE_READ,
		.p_value = version_encrypt,
		.char_value_len = MIBLE_CHAR_LEN_VERSION,
		.is_variable_len = false,
		.rd_author = false,
		.wr_author = false,
		.char_desc_db = (mible_gatts_char_desc_db_t) {.extend_prop = NULL, .char_format = NULL, .user_desc = NULL,},
	};

// char wifi_cfg init 
	*(p_mi_char + CHAR_WIFICFG_POSITION) = (mible_gatts_char_db_t){
		.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_CHAR_UUID_WIFICFG,},	
		.char_property = MIBLE_WRITE | MIBLE_NOTIFY,
		.p_value = data_buffer,
		.char_value_len = MIBLE_CHAR_LEN_WIFICFG,
		.is_variable_len = false,
		.rd_author = false,
		.wr_author = false,
		.char_desc_db = (mible_gatts_char_desc_db_t) {.extend_prop = NULL, .char_format = NULL, .user_desc = NULL,},
	};	

// char auth init 
	*(p_mi_char + CHAR_AUTHEN_POSITION) = (mible_gatts_char_db_t){
		.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_CHAR_UUID_AUTHENTICATION,},	
		.char_property = MIBLE_WRITE,
		.p_value = data_buffer,
		.char_value_len = MIBLE_CHAR_LEN_AUTHENTICATION,
		.is_variable_len = false,
		.rd_author = false,
		.wr_author = false,
		.char_desc_db = (mible_gatts_char_desc_db_t) {.extend_prop = NULL, .char_format = NULL, .user_desc = NULL,},
	};	

// char did init 
	uint8_t did_encrtpied[MIBLE_CHAR_LEN_DID]={0};
	mible_std_server_encrypt(srv_info.did, MIBLE_CHAR_LEN_DID, did_encrtpied);
	
	*(p_mi_char + CHAR_DID_POSITION) = (mible_gatts_char_db_t){
		.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_CHAR_UUID_DID,},	
		.char_property = MIBLE_WRITE | MIBLE_READ,
		.p_value = did_encrtpied,
		.char_value_len = MIBLE_CHAR_LEN_DID,
		.is_variable_len = false,
		.rd_author = false,
		.wr_author = false,
		.char_desc_db = (mible_gatts_char_desc_db_t) {.extend_prop = NULL, .char_format = NULL, .user_desc = NULL,},
	};

// char beaconKey init
	uint8_t beaconkey_encrypied[MIBLE_CHAR_UUID_BEACONKEY]={0};
	mible_std_server_encrypt(srv_info.beacon_key, MIBLE_CHAR_UUID_BEACONKEY, beaconkey_encrypied);
	*(p_mi_char + CHAR_BEACONKEY_POSITION) = (mible_gatts_char_db_t){
		.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = MIBLE_CHAR_UUID_BEACONKEY, },	
		.char_property = MIBLE_READ,
		.p_value = beaconkey_encrypied,
		.char_value_len = MIBLE_CHAR_LEN_BEACONKEY,
		.is_variable_len = false,
		.rd_author = false,
		.wr_author = false,
		.char_desc_db = (mible_gatts_char_desc_db_t) {.extend_prop = NULL, .char_format = NULL, .user_desc = NULL,},
	};	

	
	ret = mible_gatts_service_init(&mi_std_gatts_db);
	if(ret != MI_SUCCESS){
		MI_LOG_ERROR("mible_gatts_service_init failed!\r\n, code = %x",ret);
	}
	return ret;
}

void mible_server_info_init(device_info *info,mible_function_t func)
{
	MI_LOG_DEBUG("[mible_server_info_init]\r\n");	
	MIBLE_FUNCTION = func;
	uint8_t ret=0;
	uint8_t did[20];
	uint8_t beaconKey[12];
	uint8_t token[12];
	uint8_t version[11]={0};

// register
	mible_gap_register(mible_std_gap_event_handler);
	mible_gatts_register(mible_std_gatts_event_handler);
	mible_arch_register(mible_std_arch_event_handler);

	mible_gap_address_get(srv_info.mac);	
	memcpy(srv_info.pid, &info->pid, 2);
	MI_LOG_DEBUG("set pid = %d\r\n",info->pid);	
	MI_LOG_DEBUG("mac: ");
	MI_LOG_HEXDUMP(srv_info.mac,6);
	
// version init  
	memcpy(srv_info.version, version_fix, 6); 
	memcpy(srv_info.version+6, info->version, 4);

	memcpy(version,srv_info.version,10);
	
// bonding property 
	srv_info.bonding_property = info->bonding;


// read beaconKey	
	if(MI_SUCCESS == mible_record_read(MIBLE_BEACONKEY_RECORD_ID, beaconKey, MIBLE_CHAR_LEN_BEACONKEY)){
		memcpy(srv_info.beacon_key, beaconKey, MIBLE_CHAR_LEN_BEACONKEY);	
	}else{
		MI_LOG_ERROR("not found beacon key! @ %s\n", (uint32_t)__func__);

		mible_rand_num_generator(beaconKey, MIBLE_CHAR_LEN_BEACONKEY);
		memcpy(srv_info.beacon_key, beaconKey, MIBLE_CHAR_LEN_BEACONKEY);
		

		ret = mible_record_create(MIBLE_BEACONKEY_RECORD_ID, MIBLE_CHAR_LEN_BEACONKEY);
		if(ret != MI_SUCCESS){
			MI_LOG_ERROR("MIBLE_BEACONKEY_RECORD_ID mible_record_create fail. code = %x\r\n",ret);
		}
		ret = mible_record_write(MIBLE_BEACONKEY_RECORD_ID, srv_info.beacon_key, 
				MIBLE_CHAR_LEN_BEACONKEY);

		if(ret != MI_SUCCESS){
			MI_LOG_ERROR("MIBLE_BEACONKEY_RECORD_ID mible_record_write fail. code = %x\r\n",ret);
		} else {
			MI_LOG_DEBUG("MIBLE_BEACONKEY_RECORD_ID create succ. \r\n");
		}
	}
	//MI_LOG_DEBUG("raw beaconkey: \n");
	//MI_LOG_HEXDUMP(beaconKey, 12);

// read token 
	if(MI_SUCCESS == mible_record_read(MIBLE_TOKEN_RECORD_ID, token, MIBLE_CHAR_LEN_TOKEN)){
		mible_authen_mode = LOGIN;
		//MI_LOG_DEBUG("token: \n");
		//MI_LOG_HEXDUMP(token, MIBLE_CHAR_LEN_TOKEN);
		memcpy(srv_info.token, token, MIBLE_CHAR_LEN_TOKEN);
	} else {
		mible_authen_mode = REGISTER;
		MI_LOG_ERROR("not found token! @ %s\n", (uint32_t)__func__);
	}
	memcpy(token_tmp , srv_info.token, MIBLE_CHAR_LEN_TOKEN);
	
	// read did 
	if(MI_SUCCESS == mible_record_read(MIBLE_DID_RECORD_ID,did,MIBLE_CHAR_LEN_DID)){
		memcpy(srv_info.did, did, MIBLE_CHAR_LEN_DID);
		//MI_LOG_DEBUG("raw did: \r\n");
		//MI_LOG_HEXDUMP(srv_info.did, MIBLE_CHAR_LEN_DID);
	} else {
		MI_LOG_ERROR("not found did! @ %s\n", (uint32_t)__func__);
	}
	
	mibeacon_init(srv_info.beacon_key);
}

/*
 * @brief	Register mible standard authentication event
 * @note 	Application invokes this api to register callback
 * */
int mible_std_auth_evt_register(mible_std_auth_callback_t cb)
{
	int ret;
	if (m_std_auth_users == MIBLE_MAX_USERS){
		ret = MI_ERR_RESOURCES;
	}else{
		m_std_auth_cb_table[m_std_auth_users] = cb;
		m_std_auth_users++;
		ret = MI_SUCCESS;
	}
	return ret;
}


static void mible_std_auth_event_callback(mible_std_auth_evt_t evt,
		mible_std_auth_evt_param_t* p_param)
{
	for(int user = 0; user < MIBLE_MAX_USERS; user++){
		if(m_std_auth_cb_table[user] != NULL){
			m_std_auth_cb_table[user](evt,p_param);
		}
	}
}

/*
 * @brief	Register mible wifi config event
 * @note 	Application invokes this api to register callback
 * */
int mible_wifi_config_evt_register(mible_wifi_config_callback_t cb)
{
	int ret;
	if (m_wifi_config_users == MIBLE_MAX_USERS){
		ret = MI_ERR_RESOURCES;
	}else{
		m_wifi_config_cb_table[m_wifi_config_users] = cb;
		m_wifi_config_users++;
		ret = MI_SUCCESS;
	}
	return ret;
}

static void mible_wifi_config_event_callback(mible_wifi_config_evt_t evt,
		mible_wifi_config_evt_param_t* p_param)
{
	for(int user = 0; user < MIBLE_MAX_USERS; user++){
		if(m_wifi_config_cb_table[user] != NULL){
			m_wifi_config_cb_table[user](evt,p_param);
		}
	}
}

mible_status_t mible_std_server_gap_evt_connected(mible_gap_evt_param_t *gap_param)
{
	MI_LOG_DEBUG("[mible_std_server_gap_evt_connected]\r\n");

	mible_status_t ret;
	mible_server_connection_handle = gap_param->conn_handle;

	mible_gap_conn_param_t conn_param = {
		.min_conn_interval = MIBLE_STD_SERVER_CONN_MAX_INTERVAL,  // 30ms 
		.max_conn_interval = MIBLE_STD_SERVER_CONN_MIN_INTERVAL,  // 40ms
		.slave_latency = MIBLE_STD_SERVER_CONN_SLAVE_LATENCY,
		.conn_sup_timeout = MIBLE_STD_SERVER_CONN_SUP_TIMEOUT, // 3s 	
	};
	
	MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
	mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);

	ret = mible_gap_update_conn_params(mible_server_connection_handle, conn_param); 
	if(MI_SUCCESS != ret){
		MI_LOG_ERROR("mible gap update connecter parameters failed. code = %x\r\n");
	}

// start timer
	ret = mible_timer_start(server_timer, MIBLE_STD_SERVER_TIMEOUT, NULL);
	if(ret != MI_SUCCESS){
		MI_LOG_ERROR("mible_timer_start fail. code = %x \r\n",ret);
	}else{
		MI_LOG_DEBUG("mible_timer_start succ. \r\n");
	}
	mible_std_auth_evt_param_t param;
	param.connect.conn_handle = mible_server_connection_handle;
	mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_CONNECT,&param);

	return ret;
}
mible_status_t mible_std_server_gap_evt_disconnected(mible_gap_evt_param_t *gap_param)
{
	
	mible_status_t ret;
	ret = mible_timer_stop(server_timer);
	if(ret != MI_SUCCESS){
		MI_LOG_ERROR("mible_timer_stop fail. code = %x \r\n",ret);
	}
	MI_LOG_DEBUG("[mible_std_server_gap_evt_disconnected]\r\n");
	MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
	mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);

	mible_std_auth_evt_param_t param;
	param.connect.conn_handle = mible_server_connection_handle;
	mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_DISCONNECT,&param);
	mible_server_connection_handle = 0xFFFF;

	return MI_SUCCESS;
}



mible_status_t mible_std_server_gap_evt_conn_params_update(
		mible_gap_evt_param_t *gap_param)
{

	MI_LOG_DEBUG("[mible_std_server_gap_evt_conn_params_update]\r\n");
	
	return MI_SUCCESS;
}

mible_status_t mible_std_server_gatts_evt_write(mible_gatts_evt_param_t *gatts_param)
{	
		MI_LOG_DEBUG("[mible_std_server_gatts_evt_write]\r\n");
		mible_handler_t handler=NULL;	
		if(gatts_param->conn_handle != mible_server_connection_handle){
			MI_LOG_ERROR("conn_handle is NULL\r\n");
			return MIBLE_ERR_UNKNOWN; 	
		}

		MI_LOG_DEBUG("char handle 0x%02x be written !!!\r\n",gatts_param->write.value_handle);
		MI_LOG_HEXDUMP(gatts_param->write.data,gatts_param->write.len);
		
		uint8_t *gatts_data = (uint8_t*)malloc(gatts_param->write.len);
		mible_gatts_write_t *std_gatts_write = (mible_gatts_write_t*)malloc(sizeof(mible_gatts_write_t));
		memcpy(std_gatts_write, &gatts_param->write,sizeof(mible_gatts_write_t));
		std_gatts_write->data = gatts_data;
		memcpy(std_gatts_write->data, gatts_param->write.data, gatts_param->write.len);
		
		if(gatts_param->write.value_handle == service_handle.token_handle){
	// TOKEN 
			MI_LOG_DEBUG("TOKEN writed !!!\r\n");
			handler = mible_FSM_transition(&std_server_FSM, STD_SERVER_FSM_EVT_TOKEN_CHANGED);
		}else if(gatts_param->write.value_handle == service_handle.auth_handle){
	// AUTH
			MI_LOG_DEBUG("AUTH writed !!!\r\n");

			mible_FSM_get(&std_server_FSM);
			MI_LOG_DEBUG("current FSM %d\r\n",std_server_FSM.cur_state);
			handler = mible_FSM_transition(&std_server_FSM, STD_SERVER_FSM_EVT_AUTH_CHANGED);

		}else if(gatts_param->write.value_handle == service_handle.did_handle){
	// DID
			MI_LOG_DEBUG("DID writed !!!\r\n");
			handler = mible_FSM_transition(&std_server_FSM, STD_SERVER_FSM_EVT_DID_CHANGED);
		
		}else if(gatts_param->write.value_handle == service_handle.wifi_cfg_handle){
	// WIFI
			MI_LOG_DEBUG("wifi_config writed !!! \r\n");
			handler = mible_FSM_transition(&std_server_FSM, STD_SERVER_FSM_EVT_CONFIG_WIFI_CHANGED);
		
		}else{
			MI_LOG_DEBUG("unknown writen: handle %x, len %d value %d\r\n",gatts_param->write.value_handle,gatts_param->write.len,*gatts_param->write.data);
			mible_server_free(std_gatts_write);
			return MIBLE_ERR_UNKNOWN;
		}	
		if(handler != NULL){
			return mible_task_post(handler,std_gatts_write);
		}
		mible_server_free(std_gatts_write);
	
	return MI_SUCCESS;
}

mible_status_t mible_std_server_gatts_evt_read_permit_req(
		mible_gatts_evt_param_t *gatts_param)
{
	return MI_SUCCESS;
}

mible_status_t mible_std_server_gatts_evt_write_permit_req(
		mible_gatts_evt_param_t *gatts_param)
{
	return MI_SUCCESS;
}

mible_status_t mible_std_server_arch_service_init_complete(
		mible_arch_gatts_srv_init_cmp_t srv_init_cmp)
{
	MI_LOG_DEBUG("[mible_std_server_arch_service_init_complete]\r\n");
	if(srv_init_cmp.status != MI_SUCCESS){
		MI_LOG_DEBUG("service_init FAIL.  code = %x",srv_init_cmp.status);
		free(srv_init_cmp.p_gatts_db->p_srv_db->p_char_db);	
		free(srv_init_cmp.p_gatts_db->p_srv_db);
		return srv_init_cmp.status;
	}
	service_handle.srv_handle =srv_init_cmp.p_gatts_db->p_srv_db->srv_handle;
	mible_gatts_char_db_t *p_mi_char = srv_init_cmp.p_gatts_db->p_srv_db->p_char_db;
	service_handle.auth_handle = p_mi_char[CHAR_AUTHEN_POSITION].char_value_handle;
	service_handle.token_handle = p_mi_char[CHAR_TOKEN_POSITION].char_value_handle;
	service_handle.did_handle = p_mi_char[CHAR_DID_POSITION].char_value_handle;
	service_handle.pid_handle = p_mi_char[CHAR_PID_POSITION].char_value_handle;
	service_handle.beaconkey_handle = p_mi_char[CHAR_BEACONKEY_POSITION].char_value_handle;
	service_handle.wifi_cfg_handle = p_mi_char[CHAR_WIFICFG_POSITION].char_value_handle;
	service_handle.version_handle = p_mi_char[CHAR_VERSION_POSITION].char_value_handle;
	
	free(srv_init_cmp.p_gatts_db->p_srv_db->p_char_db);	
	free(srv_init_cmp.p_gatts_db->p_srv_db);

	if(srv_init_cmp.status == MI_SUCCESS){
	// init state machine 
		mible_FSM_init(&std_server_FSM, MIBLE_SERVER_FSM_STA_IDLE, std_server_FSM_list, FSM_LIST_NUM);
		mible_server_init_handler(NULL);	

	}else{
		MI_LOG_ERROR("std server service init fail. code = %x\r\n",srv_init_cmp.status);	
		return srv_init_cmp.status;
	}
	
	mible_gatts_value_set(service_handle.srv_handle,service_handle.pid_handle,0,srv_info.pid,MIBLE_CHAR_LEN_PRODUCTID);
	
	mible_std_auth_evt_param_t param;
	param.service_init_cmp.ret = MI_SUCCESS;
	mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_SERVICE_INIT_CMP, &param);

	return MI_SUCCESS;
}

static void mible_server_init_handler(void* arg)
{
	MI_LOG_DEBUG("[mible_server_init_handler]\r\n");

	mible_status_t ret;
	mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
	ret = mible_timer_create(&server_timer, mible_server_timeout_handler, MIBLE_TIMER_SINGLE_SHOT);
	if(ret != MI_SUCCESS){
		MI_LOG_ERROR("mible_timer_create failed. code = %x .\r\n",ret);
	}else{
		MI_LOG_DEBUG("mible_timer_create success. \r\n");
	}
}

static void mible_server_start_handler(void* arg)
{
	uint8_t ret=0;
	memcpy(token_tmp , srv_info.token, MIBLE_CHAR_LEN_TOKEN);
	MI_LOG_DEBUG("[mible_server_start_handler]\r\n");
//	MI_LOG_DEBUG("token_tmp:\r\n");
//	MI_LOG_HEXDUMP(token_tmp, MIBLE_CHAR_LEN_TOKEN);
// input check 
	if(arg == NULL){
		MI_LOG_ERROR("[mible_server_start_handler] arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		mible_server_free(arg);
		return;
	}	
// data length check 
	mible_gatts_write_t *write_param = arg;
	if(write_param->len != MIBLE_CHAR_LEN_AUTHENTICATION){
		MI_LOG_ERROR("[mible_server_start_handler] datalen != 4 \r\n");	
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		mible_server_free(arg);
		return;
	}

//
	uint32_t tmp = 0;
	memcpy((uint8_t*)&tmp, write_param->data, 4);
	MI_LOG_DEBUG("DATA = %x\r\n",tmp);
	
	if(tmp == MIBLE_STD_REGISTER_AUTH){
		mible_authen_mode = REGISTER;
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_T1. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_T1);
	}else if(tmp == MIBLE_STD_LOGIN_AUTH){
		mible_authen_mode = LOGIN;
		if(MI_SUCCESS == mible_rand_num_generator(tick, 4)){
//			MI_LOG_DEBUG("tick: \r\n");
//			MI_LOG_HEXDUMP(tick,4);
			uint8_t tick_encrtpied[4];
			mible_std_server_encrypt(tick, 4, tick_encrtpied);
//			MI_LOG_HEXDUMP(tick_encrtpied,4);
			ret = mible_gatts_notify_or_indicate(mible_server_connection_handle,
				service_handle.srv_handle,service_handle.token_handle, 0, 
				tick_encrtpied, 4,1);
			if(MI_SUCCESS == ret){
				MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_LOGIN_ACK. \r\n"); 
				mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_LOGIN_ACK);
			}else{
				MI_LOG_ERROR("mible_gatts_notify_or_indicate failed. code = %x\r\n",ret);	
				MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
				mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
			}
		}else{

			MI_LOG_INFO("mible_rand_num_generator failed. \r\n");
			MI_LOG_INFO("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
			mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		}
	}else{

		MI_LOG_ERROR("[mible_server_start_handler] invalid AUTH data\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
	}
	mible_server_free(arg);
	return;
}

static void mible_server_wait_t1_handler(void* arg)
{
	mible_status_t ret;
	MI_LOG_DEBUG("[mible_server_wait_t1_handler]\r\n");
	if(arg == NULL){
		MI_LOG_ERROR("[mible_server_wait_t1_handler] arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		mible_server_free(arg);
		return;
	}

	mible_gatts_write_t *write_param = arg;
	
	if(write_param->len != MIBLE_CHAR_LEN_TOKEN){
		MI_LOG_ERROR("[mible_server_wait_t1_handler] datalen != MIBLE_CHAR_LEN_TOKEN\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		mible_server_free(arg);
		return;
	}

	//parse token
	mible_mix_1(write_param->data, NULL, (uint8_t *)srv_info.mac, srv_info.pid, token_tmp);
	
	// using token encrypt did
	uint8_t did_encrypt[MIBLE_CHAR_LEN_DID]={0};
	uint8_t tmp[MIBLE_CHAR_LEN_DID]={0};
	mible_internal_encrypt(srv_info.did, MIBLE_CHAR_LEN_DID, token_tmp, MIBLE_CHAR_LEN_TOKEN, did_encrypt);
	
	mible_gatts_value_set(service_handle.srv_handle,service_handle.did_handle,0, did_encrypt, MIBLE_CHAR_LEN_DID);
	mible_internal_decrypt(did_encrypt,MIBLE_CHAR_LEN_DID,token_tmp, MIBLE_CHAR_LEN_TOKEN,tmp);
	
	
	//generate T2
	mible_mix_2(write_param->data, NULL, (uint8_t *)srv_info.mac, srv_info.pid, data_buffer);

//	MI_LOG_HEXDUMP(data_buffer,MIBLE_CHAR_LEN_TOKEN);
	ret = mible_gatts_notify_or_indicate(mible_server_connection_handle,
			service_handle.srv_handle,service_handle.token_handle, 0, 
			data_buffer, MIBLE_CHAR_LEN_TOKEN,1);
	
	
	if(ret != MI_SUCCESS){

		MI_LOG_ERROR("[mible_server_wait_t1_handler] TOKEN notify failed. code=%d\r\n",ret);
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		mible_server_free(arg);
		
		return;
	}

	MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_T2_ACK. \r\n"); 
	mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_T2_ACK);
	mible_server_free(arg);
	return;
}

static void mible_server_wait_t2_ack_handler(void* arg)
{
	MI_LOG_DEBUG("[mible_server_wait_t2_ack_handler]\r\n");	
	if(arg == NULL){
		MI_LOG_ERROR("arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		mible_server_free(arg);
		return;
	}

	mible_gatts_write_t *write_param = arg; 
	if(write_param->len != 4){
		MI_LOG_ERROR("datalen != 4\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		mible_server_free(arg);
		return;	
	}

// token test
	uint32_t tmp=0;
	mible_internal_decrypt(write_param->data,4,token_tmp,
			MIBLE_CHAR_LEN_TOKEN,(uint8_t*)&tmp);
	if(MIBLE_STD_WAIT_T2_ACK_TOKEN != tmp){
		MI_LOG_ERROR("token != MIBLE_STD_WAIT_T2_ACK_TOKEN \r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
	
		mible_server_free(arg);
		return;
	}

	if(MIBLE_FUNCTION == REMOTE_CONTROL){

		MI_LOG_DEBUG("MI AUTHEN NO DID,remote controller \r\n"); 	
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_BONDING);
		mible_server_bonding_handler();
	}else if(MIBLE_FUNCTION == WIFI_CONFIG){ 

		MI_LOG_DEBUG("MI AUTHEN NO DID, wifi configurer \r\n"); 	
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WIFI_CONFIG. \r\n");
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WIFI_CONFIG);
		mible_server_bonding_handler();
	}else{

		if(mible_server_isValid_did(srv_info.did)){  // if did is valid 

			if(srv_info.bonding_property == STRONG_BONDING){  // if strong bonding
			
				MI_LOG_DEBUG("did valid and strong bonding\r\n");
				MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_CLOUD_ACK. \r\n"); 
				mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_CLOUD_ACK);
			}else{  // if weak bonding
			
				MI_LOG_DEBUG("did valid and weak bonding.\r\n");
				MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_APP_ACK. \r\n"); 
				mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_APP_ACK);
			}	
		}else{  // if did is invalid

			MI_LOG_DEBUG("did is invalid\r\n");
			MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_DID. \r\n"); 
			mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_DID);
		}	
	}
	mible_server_free(arg);
}

static void mible_server_wait_did_handler(void* arg)
{

	MI_LOG_DEBUG("[mible_server_wait_did_handler]\r\n");	
	if(arg == NULL){
		MI_LOG_ERROR("arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		mible_server_free(arg);
		return;
	}

	mible_gatts_write_t *write_param = arg; 
	if(write_param->len != MIBLE_CHAR_LEN_DID){
		MI_LOG_ERROR("datalen != MIBLE_CHAR_LEN_DID\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		mible_server_free(arg);
		return;	
	}

//decrypted
	
	mible_internal_decrypt(write_param->data,MIBLE_CHAR_LEN_DID,token_tmp,
			MIBLE_CHAR_LEN_TOKEN,srv_info.did);
	
	if(mible_server_isValid_did(srv_info.did)){  // if did is valid 
		
		MI_LOG_DEBUG("new did: \r\n");
		MI_LOG_HEXDUMP(srv_info.did, MIBLE_CHAR_LEN_DID);
		if(srv_info.bonding_property == STRONG_BONDING){  // if strong bonding

			MI_LOG_DEBUG("did valid and strong bonding\r\n");
			MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_CLOUD_ACK. \r\n"); 
			mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_CLOUD_ACK);
		}else{  // if weak bonding

			MI_LOG_DEBUG("did valid.\r\n");
			MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WAIT_APP_ACK. \r\n"); 
			mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WAIT_APP_ACK);
			
		}	
	}else{ // if did is invalid 

		MI_LOG_DEBUG("did invalid, bonding failed. \r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);	
		
	}
	mible_server_free(arg);
	return;
}

static void mible_server_wait_cloud_ack_handler(void *arg)
{
	
	MI_LOG_DEBUG("[mible_server_wait_cloud_ack_handler]\r\n");	
	if(arg == NULL){
		MI_LOG_ERROR("arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		return;
	}

	mible_gatts_write_t *write_param = arg; 
	if(write_param->len != MIBLE_CHAR_LEN_AUTHENTICATION){
		MI_LOG_ERROR("datalen != MIBLE_CHAR_LEN_DID\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		return;	
	}

// test auth 
	uint32_t tmp=0;
	mible_internal_decrypt(write_param->data,4,token_tmp,
			MIBLE_CHAR_LEN_TOKEN,(uint8_t*)&tmp);
	if(MIBLE_STD_BONDING_SUCC_AUTH == tmp){

		MI_LOG_DEBUG("cloud bonding succ.\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_BONDING. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_BONDING);
		mible_server_bonding_handler();
	}else if(MIBLE_STD_BONDING_FAIL_AUTH == tmp ){
	
		MI_LOG_ERROR("cloud bonding failed. \r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
	}	
	
	mible_server_free(arg);
	return;
}

static void mible_server_wait_app_ack_handler(void* arg)
{
	MI_LOG_DEBUG("[mible_server_wait_app_ack_handler]\r\n");	
	if(arg == NULL){
		MI_LOG_ERROR("arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		return;
	}

	mible_gatts_write_t *write_param = arg; 
	if(write_param->len != MIBLE_CHAR_LEN_AUTHENTICATION){
		MI_LOG_ERROR("datalen != MIBLE_CHAR_LEN_DID\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		return;	
	}

// test auth 
	uint32_t tmp=0;
	mible_internal_decrypt(write_param->data,4,token_tmp,
			MIBLE_CHAR_LEN_TOKEN,(uint8_t*)&tmp);
	if(MIBLE_STD_BONDING_SUCC_AUTH == tmp){
		
		MI_LOG_DEBUG("app bonding succ.\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_BONDING. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_BONDING);
		mible_server_bonding_handler();
	}else if(MIBLE_STD_BONDING_FAIL_AUTH == tmp ){
		
		MI_LOG_ERROR("app bonding failed. \r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
	}
	
	mible_server_free(arg);
	return;
}

static void mible_server_wait_login_ack_handler(void *arg)
{

	mible_status_t ret;
	MI_LOG_DEBUG("[mible_server_wait_login_ack_handler]\r\n");	
	if(arg == NULL){
		MI_LOG_ERROR("arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		mible_server_free(arg);
		return;
	}

	mible_gatts_write_t *write_param = arg; 
	if(write_param->len != 4){
		MI_LOG_ERROR("datalen != 4\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		mible_server_free(arg);
		return;	
	}
	
// token test
	uint8_t key[MIBLE_CHAR_LEN_TOKEN]={0};
	uint32_t cfm_data;
//	MI_LOG_DEBUG("token_tmp: \r\n");
//	MI_LOG_HEXDUMP(token_tmp,MIBLE_CHAR_LEN_TOKEN);
	
	key[0] = token_tmp[0] ^ tick[0];
	key[1] = token_tmp[1] ^ tick[1];
	key[2] = token_tmp[2] ^ tick[2];
	key[3] = token_tmp[3] ^ tick[3];
	memcpy(key+4, token_tmp+4, MIBLE_CHAR_LEN_TOKEN-4);
	
	mible_internal_decrypt(write_param->data, write_param->len, key,
                           MIBLE_CHAR_LEN_TOKEN, (uint8_t *)&cfm_data);
		
	
	if(MIBLE_STD_WAIT_LOGIN_ACK_TOKEN != cfm_data){

		MI_LOG_ERROR("token != MIBLE_STD_WAIT_T2_ACK_TOKEN \r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
	}else{

		MI_LOG_DEBUG("login ack token test pass. \r\n");
		cfm_data = MIBLE_STD_LOGIN_CFM_TOKEN;
		mible_internal_encrypt((uint8_t *)&cfm_data, 4, key,
                           MIBLE_CHAR_LEN_TOKEN, (uint8_t *)&cfm_data);
		
		ret = mible_gatts_notify_or_indicate(mible_server_connection_handle,
				service_handle.srv_handle,service_handle.token_handle, 0, 
				(uint8_t *)&cfm_data, 4,1);
		if(ret != MI_SUCCESS){
			MI_LOG_ERROR("mible_gatts_notify_or_indicate fail.\r\n");
		}
		
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_LOGIN_SUCC. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_LOGIN_SUCC);
		mible_server_login_succ_handler();
	}
	mible_server_free(arg);
	return;
}

static void mible_server_bonding_handler(void)
{
	MI_LOG_DEBUG("[mible_server_bonding_handler]\r\n");	
	uint8_t tmp[20]={0};
	mible_status_t ret;
// timer stop
	ret = mible_timer_stop(server_timer);
	if(ret != MI_SUCCESS){
		MI_LOG_ERROR("server timer stop failed. \r\n");
	}
	mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);

	if(MIBLE_FUNCTION == WIFI_CONFIG){

		mible_std_auth_evt_param_t param;
		param.result.state = BONDING_SUCC;
		mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_RESULT,&param);
	
		memset(&wifi_info, 0, sizeof(wifi_info));
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_WIFI_CONFIG. \r\n");
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_WIFI_CONFIG);
	}else{

//record [token] to flash 
		//mible_record_delete(MIBLE_TOKEN_RECORD_ID);
		ret = mible_record_create(MIBLE_TOKEN_RECORD_ID, MIBLE_CHAR_LEN_TOKEN);
		memcpy(srv_info.token,token_tmp,MIBLE_CHAR_LEN_TOKEN); 
		if(ret !=MI_SUCCESS){

			MI_LOG_ERROR("token record create fail. code = %x\r\n",ret);
		}else{
			MI_LOG_DEBUG("token : \r\n");
			MI_LOG_HEXDUMP(token_tmp, MIBLE_CHAR_LEN_TOKEN);
		
			ret = mible_record_write(MIBLE_TOKEN_RECORD_ID, srv_info.token,
			   	MIBLE_CHAR_LEN_TOKEN);
		
			if(ret != MI_SUCCESS){
				MI_LOG_ERROR("token record write fail. code = %x \r\n",ret);
			}
		}
	
// update gatts char [version] (encrtpied by token)
		mible_internal_encrypt(srv_info.version, MIBLE_CHAR_LEN_VERSION, 
				token_tmp, MIBLE_CHAR_LEN_TOKEN, tmp);
		ret = mible_gatts_value_set(service_handle.srv_handle, 
				service_handle.version_handle, 0,
			tmp, MIBLE_CHAR_LEN_VERSION);
	
// update gatts char [did] (encrtpied by token) and record to flash
		mible_internal_encrypt(srv_info.did, MIBLE_CHAR_LEN_DID, 
				token_tmp, MIBLE_CHAR_LEN_TOKEN, tmp);
  //mible_std_server_encrypt(srv_info.did, MIBLE_CHAR_LEN_DID, tmp);
		ret = mible_gatts_value_set(service_handle.srv_handle, 
				service_handle.did_handle, 0,tmp, MIBLE_CHAR_LEN_DID);
		if(ret !=MI_SUCCESS){

			MI_LOG_ERROR("did gatts value set fail. code = %x\r\n", ret);
		}
		// record [DID] 
		//mible_record_delete(MIBLE_DID_RECORD_ID);
		ret = mible_record_create(MIBLE_DID_RECORD_ID, MIBLE_CHAR_LEN_DID);
		if(ret != MI_SUCCESS){

			MI_LOG_ERROR("did record create fail. code = %x \r\n",ret);
		}else{

			ret = mible_record_write(MIBLE_DID_RECORD_ID, srv_info.did, 
					MIBLE_CHAR_LEN_DID);
			if(ret != MI_SUCCESS){
				MI_LOG_ERROR("did record write fail. code = %x \r\n",ret);
			}
		}

		MI_LOG_DEBUG("record did : \r\n");
		MI_LOG_HEXDUMP(srv_info.did, MIBLE_CHAR_LEN_DID);
	
// update gatts char [beaconkey] (encrtpied by token)
		mible_std_server_encrypt(srv_info.beacon_key, MIBLE_CHAR_LEN_BEACONKEY, tmp);	
		mible_gatts_value_set(service_handle.srv_handle, 
				service_handle.beaconkey_handle, 0,tmp, MIBLE_CHAR_LEN_BEACONKEY);
		if(ret != MI_SUCCESS){
			MI_LOG_ERROR("beaconkey gatts value set fail. code = %x \r\n", ret);
		}

		mible_std_auth_evt_param_t param;
		param.result.state = BONDING_SUCC;
		mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_RESULT,&param);
	}
}

static void mible_server_login_succ_handler(void)
{
	MI_LOG_DEBUG("[mible_server_login_succ_handler] \r\n");
	mible_status_t ret;
	
	uint8_t tmp[20]={0};
	mible_internal_encrypt(srv_info.version, MIBLE_CHAR_LEN_VERSION, token_tmp, MIBLE_CHAR_LEN_TOKEN, tmp);
	ret = mible_gatts_value_set(service_handle.srv_handle, service_handle.version_handle, 0,
			tmp, MIBLE_CHAR_LEN_VERSION);
	
	ret = mible_timer_stop(server_timer);
	if(ret != MI_SUCCESS){
		MI_LOG_ERROR("server timer stop failed. \r\n");
	}
	mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);

	mible_std_auth_evt_param_t param;
	param.result.state = LOGIN_SUCC;
	mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_RESULT,&param);

}

void mible_std_server_encrypt(uint8_t *in, uint16_t inLen, uint8_t *out) // encrypt by TOKEN
{
    mible_internal_encrypt(in, inLen, srv_info.token, MIBLE_CHAR_LEN_TOKEN, out);
}

void mible_std_server_decrypt(uint8_t *in, uint16_t inLen, uint8_t *out)
{
    mible_internal_decrypt(in, inLen, srv_info.token, MIBLE_CHAR_LEN_TOKEN, out);
}

static bool mible_server_isValid_did(uint8_t* did)
{
    for (uint8_t i = 0; i < MIBLE_CHAR_LEN_DID; i++) {
        if (did[i] == 'b') {
            if (did[i+1] == 'l' && did[i+2] == 't') {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

static void mible_server_timeout_handler(void *arg)
{
	MI_LOG_ERROR("MIBLE AUTHENTICATION TIMEOUT !!!\r\n");
	MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
	mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
	if(mible_authen_mode == LOGIN)
	{
		mible_std_auth_evt_param_t param;
		param.result.state = LOGIN_FAIL;
		mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_RESULT,&param);

	}else{

		mible_std_auth_evt_param_t param;
		param.result.state = BONDING_FAIL;
		mible_std_auth_event_callback(MIBLE_STD_AUTH_EVT_RESULT,&param);

	}
	
}

mible_status_t	mible_std_server_record_write_cmp(
		mible_arch_record_t record_write_cmp)
{
	MI_LOG_INFO("[Record write complete] record_id: 0x%x, status: 0x%x \r\n", 
			record_write_cmp.id, record_write_cmp.status);
	return MI_SUCCESS; 
}

static void mible_server_free(mible_gatts_write_t* std_gatts_write){
	free(std_gatts_write->data);
	free(std_gatts_write);
}
static uint8_t current_siid_pos = 0;
static uint8_t current_pw_pos = 0;  	// password
static uint8_t current_tz_pos = 0;  	// time_zone
static uint8_t current_cft_pos = 0; 	// config type
static uint8_t current_bk_pos = 0; 		// bind key

static void mible_server_wait_wifi_config_handler(void* arg)
{
	if(MIBLE_FUNCTION != WIFI_CONFIG){
		return;
	}

	MI_LOG_DEBUG("[mible_server_wait_wifi_config_handler]\r\n");	
	if(arg == NULL){
		MI_LOG_ERROR("arg = NULL\r\n");
		MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n"); 
		mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);
		
		mible_server_free(arg);
		return;
	}

	mible_wifi_config_evt_param_t param;
	param.data.p_wifi_info = &wifi_info; 
	mible_gatts_write_t *write_param = arg; 
	mible_internal_decrypt(write_param->data,write_param->len,token_tmp,
				MIBLE_CHAR_LEN_TOKEN,write_param->data);
	MI_LOG_HEXDUMP(write_param->data,write_param->len);

		
	uint8_t message_type = write_param->data[1];
	uint8_t *data = write_param->data+2;
	uint8_t len = write_param->len - 2;
	uint8_t remain_len = write_param->data[0];

	switch(message_type){
	
		case WIFI_CONFIG_USER_ID:

			MI_LOG_DEBUG("WIFI_CONFIG_USER_ID: \r\n");
			if(len>WIFI_CONFIG_USER_ID_LEN+1){
				MI_LOG_ERROR("wifi config user id , len error! \r\n");
			}
			memcpy(wifi_info.user_id,data,len);
			MI_LOG_HEXDUMP(wifi_info.user_id,len);
			break;

		case WIFI_CONFIG_SSID:

			MI_LOG_DEBUG("WIFI_CONFIG_SSID: \r\n");
			if(len+current_siid_pos > WIFI_CONFIG_SSID_LEN+1){
				MI_LOG_ERROR("wifi config ssid , len error! \r\n");
			}
			memcpy(wifi_info.wifi_ssid + current_siid_pos,data,len);
			wifi_info.wifi_ssid_len = len + current_siid_pos;
			current_siid_pos += len; 
			MI_LOG_HEXDUMP(wifi_info.wifi_ssid,wifi_info.wifi_ssid_len);
			break;

		case WIFI_CONFIG_PW:

			MI_LOG_DEBUG("WIFI_CONFIG_PW: \r\n");
			if(len+current_pw_pos>WIFI_CONFIG_PW_LEN+1){
				MI_LOG_ERROR("wifi config password , len error! \r\n");
			}
			memcpy(wifi_info.wifi_pw + current_pw_pos,data,len);
			wifi_info.wifi_pw_len = len + current_pw_pos;
			current_pw_pos += len;
			MI_LOG_HEXDUMP(wifi_info.wifi_pw,wifi_info.wifi_pw_len);

			if(remain_len == len){
				// FSM TRANS
				MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n");
				mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);

				MI_LOG_DEBUG("1 wifi_info: 0x%x \n",&wifi_info);	
				param.data.p_wifi_info = &wifi_info;
				mible_wifi_config_event_callback(MIBLE_WIFI_CONFIG_EVT_DATA,&param);
			}
			break;

		case WIFI_CONFIG_UTC:

			MI_LOG_DEBUG("WIFI_CONFIG_UTC: \r\n");
			if(len>WIFI_CONFIG_UTC_LEN+1){
				MI_LOG_ERROR("wifi config utc , len error! \r\n");
			}
			memcpy(wifi_info.utc,data,len);
			MI_LOG_HEXDUMP(wifi_info.utc,len);
			break;

		case WIFI_CONFIG_DOMAIN:

			MI_LOG_DEBUG("WIFI_CONFIG_DOMAIN: \r\n");
			if(len>WIFI_CONFIG_DOMAIN_LEN+1){
				MI_LOG_ERROR("wifi config domain , len error! \r\n");
			}
			memcpy(wifi_info.domain,data,len);
			MI_LOG_HEXDUMP(wifi_info.domain,len);
			break;

		case WIFI_CONFIG_TIME_ZONE:

			MI_LOG_DEBUG("WIFI_CONFIG_TIME_ZONE: \r\n");
			if(len + current_tz_pos >WIFI_CONFIG_PW_LEN+1){
				MI_LOG_ERROR("wifi config time zone, len error! \r\n");
			}
			memcpy(wifi_info.time_zone + current_tz_pos, data, len);
			wifi_info.time_zone_len = len + current_tz_pos;
			current_tz_pos += len;
			MI_LOG_HEXDUMP(wifi_info.time_zone, wifi_info.time_zone_len);
			break;

		case WIFI_CONFIG_TYPE:

			MI_LOG_DEBUG("WIFI_CONFIG_TYPE: \r\n");
			if(len + current_cft_pos >WIFI_CONFIG_TYPE_LEN+1){
				MI_LOG_ERROR("wifi config type, len error! \r\n");
			}
			memcpy(wifi_info.config_type + current_cft_pos, data, len);
			wifi_info.config_type_len = len + current_cft_pos;
			current_cft_pos += len; 
			MI_LOG_HEXDUMP(wifi_info.config_type, wifi_info.config_type_len);
			break;

		case WIFI_CONFIG_BIND_KEY:

			MI_LOG_DEBUG("WIFI_CONFIG_BIND_KEY: \r\n");
			if(len + current_bk_pos > WIFI_CONFIG_TYPE_LEN+1){
				MI_LOG_ERROR("wifi config bind key, len error! \r\n");
			}
			memcpy(wifi_info.bind_key + current_bk_pos, data, len);
			wifi_info.bind_key_len = len + current_bk_pos;
			current_bk_pos += len; 
			MI_LOG_HEXDUMP(wifi_info.bind_key, wifi_info.bind_key_len);
			break;

		case WIFI_CONFIG_COUNTRY_CODE:

			MI_LOG_DEBUG("WIFI_CONFIG_COUNTRY_CODE: \r\n");
			if(len>WIFI_CONFIG_DOMAIN_LEN+1){
				MI_LOG_ERROR("wifi config country code, len error! \r\n");
			}
			memcpy(wifi_info.wifi_contry_code,data,len);
			MI_LOG_HEXDUMP(wifi_info.wifi_contry_code,len);
			break;

		case WIFI_CONFIG_RETRY:

			current_bk_pos = 0;
			current_pw_pos = 0;
		   	current_tz_pos = 0;
			current_cft_pos = 0; 
			current_siid_pos = 0; 	
			MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n");
			mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);

			param.command.command = WIFI_RETRY;
			mible_wifi_config_event_callback(MIBLE_WIFI_CONFIG_EVT_COMMAND,&param);

			break;

		case WIFI_CONFIG_RESTORE:

			current_bk_pos = 0;
			current_pw_pos = 0;
		   	current_tz_pos = 0;
			current_cft_pos = 0; 
			current_siid_pos = 0; 	
			MI_LOG_DEBUG("next FSM state: MIBLE_SERVER_FSM_STA_START. \r\n");
			mible_FSM_set(&std_server_FSM, MIBLE_SERVER_FSM_STA_START);

			param.command.command = WIFI_RESTORE;
			mible_wifi_config_event_callback(MIBLE_WIFI_CONFIG_EVT_COMMAND,&param);

			break;

		default:

			MI_LOG_ERROR("unknown type! \n");
			break;
	}
	mible_server_free(arg);
	return;
}

mible_status_t notify_wifi_status(uint8_t status)
{
	if(mible_server_connection_handle != 0xFFFF){
			
		return  mible_gatts_notify_or_indicate(mible_server_connection_handle,
				service_handle.srv_handle, service_handle.wifi_cfg_handle, 0, 
				&status, 1, 1);
	}	
	return MI_ERR_INVALID_STATE;
} 



