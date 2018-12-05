//mible_ll_callback
#include "mible_api.h"
#include "mible_server.h"

/* GAP, GATTS, GATTC event callback function */

/**
 *@brief    This function is MIBLE GAP related event callback function.
 *@param    [in] evt : GAP EVENT
 *			[in] param : callback parameters corresponding to evt
 *@return   Void
 *@note     You should support this function in your own ble stack .
 *          Make sure when the corresponding event occurs, be able to call this
 *function
 *          and pass in the corresponding parameters.
*/
void mible_std_gap_event_handler(mible_gap_evt_t evt,
    mible_gap_evt_param_t* param)
{
    switch (evt) {
    case MIBLE_GAP_EVT_CONNECTED:
		mible_std_server_gap_evt_connected(param); 
        break;
    case MIBLE_GAP_EVT_DISCONNET:
		mible_std_server_gap_evt_disconnected(param);
        break;
    case MIBLE_GAP_EVT_ADV_REPORT:
		break;
    case MIBLE_GAP_EVT_CONN_PARAM_UPDATED:
        break;
	default:
		break;
    }
}
/**
 *@brief    This function is MIBLE GATTS related event callback function.
 *@param    [in] evt : GATTS EVENT
 *			[in] param : callback parameters corresponding to evt
 *@return   Void
 *@note     You should support this function in your own ble stack .
            Make sure when the corresponding event occurs, be able to call this
 function and pass in the corresponding parameters.
*/
void mible_std_gatts_event_handler(mible_gatts_evt_t evt,
    mible_gatts_evt_param_t* param)
{
    switch (evt) {
    case MIBLE_GATTS_EVT_WRITE:
		mible_std_server_gatts_evt_write(param);
        break;

    case MIBLE_GATTS_EVT_READ_PERMIT_REQ:
		mible_std_server_gatts_evt_read_permit_req(param);
        break;

    case MIBLE_GATTS_EVT_WRITE_PERMIT_REQ:
		mible_std_server_gatts_evt_write_permit_req(param);
        break;

	case MIBLE_GATTS_EVT_IND_CONFIRM:
		break;
    }
}

/**
 *@brief    This function is MIBLE GATTC related event callback function.
 *@param    [in] evt : GATTC EVENT
 *			[in] param : callback parameters corresponding to evt
 *@return   Void
 *@note     You should support this function in your own ble stack .
            Make sure when the corresponding event occurs, be able to call this
 function and pass in the corresponding parameters.
*/
void mible_std_gattc_event_handler(mible_gattc_evt_t evt,
    mible_gattc_evt_param_t* param)
{
    switch (evt) {
    case MIBLE_GATTC_EVT_PRIMARY_SERVICE_DISCOVER_RESP:
        break;
    case MIBLE_GATTC_EVT_CHR_DISCOVER_BY_UUID_RESP:
        break;
    case MIBLE_GATTC_EVT_CCCD_DISCOVER_RESP:
        break;
    case MIBLE_GATTC_EVT_READ_CHAR_VALUE_BY_UUID_RESP:
        break;
    case MIBLE_GATTC_EVT_WRITE_RESP:
        break;
    default:
		;
    }
}
/*
 *@brief 	This function is mible_arch api related event callback function.
 *@param 	[in] evt: asynchronous function complete event 
 *			[in] param: the return of asynchronous function 
 *@note  	You should support this function in corresponding asynchronous function. 
 *          For now, mible_gatts_service_int and mible_record_write is asynchronous. 
 * */
void mible_std_arch_event_handler(mible_arch_event_t evt, 
		mible_arch_evt_param_t* param)
{
	switch(evt) {
		case MIBLE_ARCH_EVT_GATTS_SRV_INIT_CMP:
			mible_std_server_arch_service_init_complete(param->srv_init_cmp);
        case MIBLE_ARCH_EVT_RECORD_WRITE:
			mible_std_server_record_write_cmp(param->record);
            break;
        case MIBLE_ARCH_EVT_RECORD_DELETE:
            break;
        default:
            break;
	}
}
