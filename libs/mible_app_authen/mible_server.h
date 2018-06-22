#ifndef MIBLE_SERVER_H__
#define MIBLE_SERVER_H__

#include "mible_api.h"
#include "mible_crypto.h"
#include "mible_log.h"
#include "mible_port.h"


#define MIBLE_STD_SERVER_CONN_MAX_INTERVAL  	0X0018 
#define MIBLE_STD_SERVER_CONN_MIN_INTERVAL  	0X0020
#define MIBLE_STD_SERVER_CONN_SLAVE_LATENCY 	0
#define MIBLE_STD_SERVER_CONN_SUP_TIMEOUT 		0X012c


#define MIBLE_STD_SERVER_TIMEOUT 				10000	//10s

extern uint16_t mible_server_connection_handle;


/*
 * @TYPE	mible_gatts_bonding_propert
 * @brief	STRONG_BONDING:	The account is strongly related to the device, 
 * 			and the device can be bound by others only when the account deletes this device
 * 			WEAK_BONDIND: 	The device can be bound by anyone else. 		
 * 			
 * */
typedef enum{
	STRONG_BONDING,
	WEAK_BONDING,
}mible_gatts_bonding_property;


/**
 * @TYPE 	mible_bonding_state
 * @brief 	BONDING_SUCC:	register success	
 * 			LOGIN_SUCC:		login success 
 * 			BONDING_FAIL: 	register fail 
 * 			LOGIN_FAIL: 	login fail	
 * */
typedef enum{
	BONDING_SUCC,  
	LOGIN_SUCC,  
	BONDING_FAIL,
	LOGIN_FAIL,
}mible_bonding_state;

/*
 * @TYPE	device_info  
 * @brief	bonding:		The bondin property 
 * 			pid:			Product ID assigned by MIJIA
 * 			version: 		The version of the program 
 * */
typedef struct{
	mible_gatts_bonding_property bonding;
	uint16_t pid;
	uint8_t version[4];
}device_info;

/*
 * @brief 	Gatts infomation initialization 	
 * @param 	[IN] info: TYPE device_info 
 * @note 	This function should be called in a very forward position.
 * 			Must be called before "mible_server_miservice_init" 
 * */
void mible_server_info_init(device_info *info);

/*
 * @brief	Init MI service 		
 * @note 	This function init MI service database, 
 *			Must be call after "mible_server_info_init" and 
 *			before GATT server init in your own stack. 
 * */
mible_status_t mible_server_miservice_init(void);

/*
 * @brief	mible gatt service init complete event handler   
 * */
void mible_service_init_cmp(void);

/*
 * @brief 	mible gap connected event handler 
 * */
void mible_connected(void);

/*
 * @brief	mible gap disconnected event handler 
 * */
void mible_disconnected(void);

/*
 * @brief 	mible standard authentication result  
 * */
void mible_bonding_evt_callback(mible_bonding_state state);

/*
 * @brief 	Encrypt data by TOKEN
 * @param 	[IN] in: data to be encrypted
 *			[IN] inLen: data length
 * 			[OUT] out: encrypted data 
 * @note 	This function should be called in a very forward position.
 * 			Must be called before "mible_server_miservice_init" 
 * */
void mible_std_server_encrypt(uint8_t *in, uint16_t inLen, uint8_t *out); 
/*
 * @brief 	Decrypt data by TOKEN
 * @param 	[IN] in: encrypted data 
 *			[IN] inLen: data length
 * 			[OUT] out: raw data
 * @note 	This function should be called in a very forward position.
 * 			Must be called before "mible_server_miservice_init" 
 * */
void mible_std_server_decrypt(uint8_t *in, uint16_t inLen, uint8_t *out);


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
