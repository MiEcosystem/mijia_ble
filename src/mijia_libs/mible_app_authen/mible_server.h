#ifndef MIBLE_SERVER_H__
#define MIBLE_SERVER_H__
/*
 * This libarary support three mible authentication function: standard authentication /
 * remote control authentication / smt wifi configure anthentication
 *
 * */
#include "mible_api.h"
#include "mible_crypto.h"
#include "mible_log.h"
#include "mible_port.h"

//#define MI_WIFI_CONFIG      smt config

//#define MI_REMOTE_CONTROLLER

#define MIBLE_STD_SERVER_CONN_MIN_INTERVAL      15
#define MIBLE_STD_SERVER_CONN_MAX_INTERVAL  	30
#define MIBLE_STD_SERVER_CONN_SLAVE_LATENCY 	0
#define MIBLE_STD_SERVER_CONN_SUP_TIMEOUT 		3000

#define MIBLE_STD_SERVER_TIMEOUT 				15000	//15

extern uint16_t mible_server_connection_handle;

#define WIFI_CONFIG_USER_ID						0x00
#define WIFI_CONFIG_USER_ID_LEN					8
#define WIFI_CONFIG_SSID						0x01
#define WIFI_CONFIG_SSID_LEN					32
#define WIFI_CONFIG_PW							0x02
#define WIFI_CONFIG_PW_LEN						64
#define WIFI_CONFIG_UTC							0x05
#define WIFI_CONFIG_UTC_LEN						4
#define WIFI_CONFIG_DOMAIN						0x06
#define WIFI_CONFIG_DOMAIN_LEN					2
#define WIFI_CONFIG_TIME_ZONE					0x07
#define WIFI_CONFIG_TIME_ZONE_LEN				32
#define WIFI_CONFIG_TYPE						0x08
#define WIFI_CONFIG_TYPE_LEN					32
#define WIFI_CONFIG_BIND_KEY					0x09
#define WIFI_CONFIG_BIND_KEY_LEN				64
#define WIFI_CONFIG_COUNTRY_CODE				0x0A
#define WIFI_CONFIG_COUNTRY_CODE_LEN			2

#define WIFI_CONFIG_RETRY						0x03
#define WIFI_CONFIG_RESTORE						0x04

/*
 * @TYPE	mible_mode_t
 * @brief	use for mible_server_info_init function,
 * 0. invalid mode
 * 1. Standard ble device (MI_BLE_AUTHEN);
 * 2. Remote device (MI_REMOTE_CONTROLLER);
 *
 * */
typedef enum {
    MODE_INVALID,
	MODE_STANDARD,
	MODE_REMOTE,
} mible_op_mode_t;


typedef enum {
	STRONG_BONDING,
	WEAK_BONDING,
} mible_gatts_bonding_property;


/**
 * @TYPE 	mible_bonding_state
 * @brief 	BONDING_SUCC:	register success
 * 			LOGIN_SUCC:		login success
 * 			BONDING_FAIL: 	register fail
 * 			LOGIN_FAIL: 	login fail
 * */
typedef enum {
	BONDING_SUCC,
	LOGIN_SUCC,
	BONDING_FAIL,
	LOGIN_FAIL,
} mible_bonding_state;

/*
 * @TYPE	device_info
 * @brief	bonding:		The bondin property
 * 			pid:			Product ID assigned by MIJIA
 * 			version: 		The version of the program
 * */
typedef struct {
    uint8_t version[4];
	mible_gatts_bonding_property bonding;
	uint16_t pid;
	uint8_t  strict_bind_confirm;
} device_info;

typedef enum {
	MIBLE_STD_AUTH_EVT_SERVICE_INIT_CMP,
	MIBLE_STD_AUTH_EVT_CONNECT,
	MIBLE_STD_AUTH_EVT_DISCONNECT,
	MIBLE_STD_AUTH_EVT_RESULT,
} mible_std_auth_evt_t;

typedef struct {
	mible_status_t ret;
} mible_std_auth_evt_service_init_cmp_param_t;

typedef struct {
	uint16_t conn_handle;
} mible_std_auth_evt_connect_param_t;

typedef struct {
	uint16_t conn_handle;
} mible_std_auth_evt_disconnect_param_t;

typedef struct {
	mible_bonding_state state;
} mible_std_auth_evt_result_t;

typedef struct {
	union {
		mible_std_auth_evt_service_init_cmp_param_t service_init_cmp;
		mible_std_auth_evt_connect_param_t connect;
		mible_std_auth_evt_disconnect_param_t disconnect;
		mible_std_auth_evt_result_t result;
	};
} mible_std_auth_evt_param_t;

typedef void (*mible_std_auth_callback_t)(mible_std_auth_evt_t evt,
		mible_std_auth_evt_param_t* param);

#if defined(WIFI_CONFIG)
/*
 * @TYPE	wifi_info_t
 * @brief	wifi config parameters
 * */
typedef struct {
	uint8_t user_id[WIFI_CONFIG_USER_ID_LEN+1];   				// 8
	uint8_t wifi_ssid[WIFI_CONFIG_SSID_LEN+1]; 					// <=32
	uint8_t wifi_ssid_len;
	uint8_t wifi_pw[WIFI_CONFIG_PW_LEN+1]; 						// <=64
	uint8_t wifi_pw_len;
	uint8_t utc[WIFI_CONFIG_UTC_LEN+1]; 						// 4
	uint8_t domain[WIFI_CONFIG_DOMAIN_LEN+1]; 					// 2
	uint8_t time_zone[WIFI_CONFIG_TIME_ZONE_LEN+1]; 			// <=32
	uint8_t time_zone_len;
	uint8_t config_type[WIFI_CONFIG_TYPE_LEN+1]; 				// <=32
	uint8_t config_type_len;
	uint8_t bind_key[WIFI_CONFIG_BIND_KEY_LEN+1]; 				// <=64
	uint8_t bind_key_len;
	uint8_t wifi_contry_code[WIFI_CONFIG_COUNTRY_CODE_LEN+1]; 	// 2
} wifi_info_t;

typedef enum {
	MIBLE_WIFI_CONFIG_EVT_COMMAND,
	MIBLE_WIFI_CONFIG_EVT_DATA,
} mible_wifi_config_evt_t;

typedef enum {
	WIFI_RETRY,
	WIFI_RESTORE,
} mible_wifi_command_t;

typedef struct {
	mible_wifi_command_t command;
} mible_wifi_config_evt_command_param_t;

typedef struct {
	wifi_info_t *p_wifi_info;
} mible_wifi_config_evt_data_param_t;

typedef struct {
	union {
		mible_wifi_config_evt_command_param_t command;
		mible_wifi_config_evt_data_param_t data;
	};
} mible_wifi_config_evt_param_t;


typedef void (*mible_wifi_config_callback_t)(mible_wifi_config_evt_t evt,
		mible_wifi_config_evt_param_t* param);

mible_status_t notify_wifi_status(uint8_t status);

#endif

/*
 * @brief 	Gatts infomation initialization
 * @param 	[IN] info: TYPE device_info.
 * @param   [IN] mode: auth operation mode.
 * @return  0: UNREGISTERED
 *          1: REGISTERED
 * @note 	This function should be called before "mible_server_miservice_init"
 * */
int mible_server_info_init(device_info *info, mible_op_mode_t mode);

/*
 * @brief	Init MI service
 * @note 	This function init MI service database;
 * 			should be call after "mible_server_info_init" and
 *			before your own GATT server init.
 * */
mible_status_t mible_server_miservice_init(void);

/*
 * @brief	Register mible standard authentication event
 * @note 	Application invokes this api to register callback
 * */
int mible_std_auth_evt_register(mible_std_auth_callback_t cb);

#if defined(WIFI_CONFIG)
/*
 * @brief	Register mible wifi config event
 * @note 	Application invokes this api to register callback
 * */
int mible_wifi_config_evt_register(mible_wifi_config_callback_t cb);
#endif

/*
 * @brief 	Encrypt data by TOKEN
 * @param 	[IN] in: data to be encrypted
 *			[IN] inLen: data length
 * 			[OUT] out: encrypted data
 * */
void mible_std_server_encrypt(uint8_t *in, uint16_t inLen, uint8_t *out);

/*
 * @brief 	Decrypt data by TOKEN
 * @param 	[IN] in: encrypted data
 *			[IN] inLen: data length
 * 			[OUT] out: raw data
 * */
void mible_std_server_decrypt(uint8_t *in, uint16_t inLen, uint8_t *out);

/**
 * @brief   Permit bind
 * @note    Only work when strict_bind_confirm == 1; see "mible_server_info_init"
 *
 */
void mible_std_auth_permit_bind(void);

/**
 * @brief   Forbid bind
 * @note    Only work when strict_bind_confirm == 1; see "mible_server_info_init"
 *
 */
void mible_std_auth_forbid_bind(void);

/**
 * @brief    Delete all token in flash
 *
 * @return mible_status_t
 */
mible_status_t mible_std_auth_factory_reset(void);

/*
 * callback handler used by mible_standard_authentication procedure.
 * */
void mible_std_gap_event_handler(mible_gap_evt_t evt, mible_gap_evt_param_t* param);
void mible_std_gatts_event_handler(mible_gatts_evt_t evt, mible_gatts_evt_param_t* param);
void mible_std_gattc_event_handler(mible_gattc_evt_t evt, mible_gattc_evt_param_t* param);
void mible_std_arch_event_handler(mible_arch_event_t evt, mible_arch_evt_param_t* param);

mible_status_t mible_std_server_gap_evt_connected(mible_gap_evt_param_t *gap_param);
mible_status_t mible_std_server_gap_evt_disconnected(mible_gap_evt_param_t *gap_param);
mible_status_t mible_std_server_gap_evt_scan_report(mible_gap_evt_param_t *gap_param);
mible_status_t mible_std_server_gap_evt_conn_params_update(mible_gap_evt_param_t *gap_param);
mible_status_t mible_std_server_gatts_evt_read_permit_req(mible_gatts_evt_param_t *gatts_param);
mible_status_t mible_std_server_gatts_evt_write(mible_gatts_evt_param_t *gatts_param);
mible_status_t mible_std_server_gatts_evt_write_permit_req(mible_gatts_evt_param_t *gatts_param);
mible_status_t mible_std_server_arch_service_init_complete(mible_arch_gatts_srv_init_cmp_t srv_init_cmp);
mible_status_t	mible_std_server_record_write_cmp(mible_arch_record_t record_write_cmp);

#endif
