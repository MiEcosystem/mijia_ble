/**************************************************************************************************
  Filename:       MijiaPeripheral.c
  Revised:
  Revision:

  Description:    This file contains the mijia Peripheral application


**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "OSAL.h"
#include "OSAL_PwrMgr.h"
//#include "OnBoard.h"
#include "gatt.h"
#include "hci.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "mijiaService.h"

#include "peripheral.h"
#include "gapbondmgr.h"

#include "MijiaPeripheral.h"
#include "log.h"
#include "global_config.h"
#include "mible_api.h"
#include "chipsea_func.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD               5000

// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160


#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     16

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     80

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          500

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL 		  6 //手机连接设备到设备发送连接参数请求之间的时间，时间太小会存在手机的兼容性问题

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 MijiaPeripheral_TaskID;   // Task ID for internal task/event processing

static gaprole_States_t gapProfileState = GAPROLE_INIT;

static uint8 defaultAddr[6] = {
	0x81,
	0x82,
	0x83,
	0x84,
	0xAA,
	0xBB
};

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[31] =
{
  // complete name
  0x14,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  0x53,   // 'S'
  0x69,   // 'i'
  0x6d,   // 'm'
  0x70,   // 'p'
  0x6c,   // 'l'
  0x65,   // 'e'
  0x42,   // 'B'
  0x4c,   // 'L'
  0x45,   // 'E'
  0x50,   // 'P'
  0x65,   // 'e'
  0x72,   // 'r'
  0x69,   // 'i'
  0x70,   // 'p'
  0x68,   // 'h'
  0x65,   // 'e'
  0x72,   // 'r'
  0x61,   // 'a'
  0x6c,   // 'l'

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),   // 100ms
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),   // 1s
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};
static uint8 scanRspDataLen = 30;//available data of scanRspData

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[31] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16( MIJIA_SERVICE_UUID ),
  HI_UINT16( MIJIA_SERVICE_UUID ),

};
static uint8 advertDataLen = 7;//available data of advertData

// GAP GATT Attributes
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "mijia Peripheral";

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void MijiaPeripheralProcessOSALMsg( osal_event_hdr_t *pMsg );
static void MijiaPeripheralStateNotificationCB( gaprole_States_t newState );
static void MijiaPeripheralRssiNotificationCB( int8 rssi  );
static void MijiaPeripheralConParaUpdateNotificationCB(void);
static void MijiaProfileParameterChangeCB( uint8 paramID ,uint8 len);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t MijiaPeripheralNotificationCB =
{
    MijiaPeripheralStateNotificationCB,  // Profile State Change Callbacks
    MijiaPeripheralRssiNotificationCB,   // When a valid RSSI is read from controller (not used by application)
    MijiaPeripheralConParaUpdateNotificationCB
};

// mijia Profile Charactersitic value change callback
static MijiaProfileCBs_t MijiaProfileCBs =
{
    MijiaProfileParameterChangeCB    // Charactersitic value change callback
};
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      MijiaPeripheral_Init
 *
 * @brief   Initialization function for the mijia Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void timer_callback(void* arv)
{
    // LOG("%s(%d):%s",__func__,__LINE__,(char*)arv);

    uint8_t tempDat[30] = {0};
    uint8_t tempLen = 0;
    mible_gatts_value_get(0,45,tempDat,&tempLen);
    LOG("%ss(%d):gat data:",__func__,__LINE__);
    LOG_HEX(tempDat,tempLen);
    LOG("\n");
	// mible_gatts_value_set(0,40,0,tempDat,tempLen);
	mible_gatts_notify_or_indicate(0,0,40,0,tempDat,tempLen,0);
}
void MijiaPeripheral_Init( uint8 task_id )
{
    MijiaPeripheral_TaskID = task_id;

	osal_memcpy((void *)0x1fff11f9,defaultAddr,6);	//modify device MAC address

    // Initialize GATT attributes
    GGS_AddService( GATT_ALL_SERVICES );            // GAP
    GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
    DevInfo_AddService();                           // Device Information Service
    mible_services_init(MijiaPeripheral_TaskID);
    // MijiaProfile_AddService(MIJIA_SERVICE_BIT);

    // mible_gap_adv_data_set(advertData,advertDataLen,scanRspData,scanRspDataLen);
    if(mible_get_advertising_data(advertData,&advertDataLen) == MI_SUCCESS){
        mible_gap_adv_data_set(advertData,advertDataLen,NULL,0);
    }
    else{
        LOG("advertising data get error\n");
        return;
    }

    mible_gap_adv_param_t mi_adv_param;
    mi_adv_param.adv_interval_min = 80;
    mi_adv_param.adv_interval_max = 160;
    mi_adv_param.adv_type = MIBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED;
    mible_gap_adv_start(&mi_adv_param);

    mible_gap_conn_param_t mi_conn_param;
    mi_conn_param.min_conn_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    mi_conn_param.max_conn_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    mi_conn_param.slave_latency     = DEFAULT_DESIRED_SLAVE_LATENCY;
    mi_conn_param.conn_sup_timeout  = DEFAULT_DESIRED_CONN_TIMEOUT;
    mible_gap_update_conn_params(0,mi_conn_param);

    // Set the GGS Characteristics
    GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );//修改GGS服务服务中设备名字

    // Register callback with MiJia profile
    VOID MijiaProfile_RegisterAppCBs( &MijiaProfileCBs );

    // Setup a delayed profile startup
    osal_set_event( MijiaPeripheral_TaskID, SBP_START_DEVICE_EVT );

	#if 0   //测试mible record使用的函数
    char *  dataIn = "123456789012345678901234567890";
    uint8_t dataOut[30] = {0};
    for(int ii = 10; ii < 10 + 16;ii++){
		osal_memset(dataOut,0,sizeof(dataOut));
		mible_record_read(ii,(uint8_t*)dataOut,ii);
		if(dataOut[0] != '1'){
			mible_record_create(ii,ii);
			mible_record_write(ii,(uint8_t*)dataIn,ii);
			mible_record_read(ii,(uint8_t*)dataOut,ii);
			LOG("%s(%d):write data to flash\n",__func__,__LINE__);
		}
        LOG("%s(%d):%d,%s\n",__func__,__LINE__,ii,(char*)dataOut);
    }
    for(int ii = 10; ii < 10 + 16;ii++){
		osal_memset(dataOut,0,sizeof(dataOut));
		mible_record_read(ii,(uint8_t*)dataOut,ii);
		if(dataOut[0] != '1'){
			mible_record_create(ii,ii);
			mible_record_write(ii,(uint8_t*)dataIn,ii);
			mible_record_read(ii,(uint8_t*)dataOut,ii);
			LOG("%s(%d):write data to flash\n",__func__,__LINE__);
		}
        LOG("%s(%d):%d,%s\n",__func__,__LINE__,ii,(char*)dataOut);
    }
	for(int ii = 10; ii < 10 + 16;ii++){
		osal_memset(dataOut,0,sizeof(dataOut));
		mible_record_delete(ii);
		LOG("%s(%d):%d\n",__func__,__LINE__,mible_record_read(ii,(uint8_t*)dataOut,ii));
        LOG("%s(%d):%d,%s\n",__func__,__LINE__,ii,(char*)dataOut);
    }
	#elif 0   //测试mible timer使用的函数
    char *str1 = "11111111111111\n";
    void *p1 = NULL;
    mible_timer_create(&p1,timer_callback,MIBLE_TIMER_REPEATED);
    mible_timer_start(p1,1000,(void*)str1);
    char *str2 = "22222222222222\n";
    void *p2 = NULL;
    mible_timer_create(&p2,timer_callback,MIBLE_TIMER_REPEATED);
    mible_timer_start(p2,1000,(void*)str2);
    char *str3 = "33333333333333\n";
    void *p3 = NULL;
    mible_timer_create(&p3,timer_callback,MIBLE_TIMER_REPEATED);
    mible_timer_start(p3,1000,(void*)str3);
    char *str4 = "44444444444444\n";
    void *p4 = NULL;
    mible_timer_create(&p4,timer_callback,MIBLE_TIMER_REPEATED);
    mible_timer_start(p4,1000,(void*)str4);
    char *str5 = "55555555555555\n";
    void *p5 = NULL;
    mible_timer_create(&p5,timer_callback,MIBLE_TIMER_REPEATED);
    mible_timer_start(p5,1000,(void*)str5);
    LOG("%s(%d):%d\n",__func__,__LINE__,mible_timer_stop(p1));
    LOG("%s(%d):%d\n",__func__,__LINE__,mible_timer_stop(p2));
    LOG("%s(%d):%d\n",__func__,__LINE__,mible_timer_delete(p3));
    LOG("%s(%d):%d\n",__func__,__LINE__,mible_timer_delete(p4));
	char *strP1 = "restart p1\n";
	char *strP3 = "restart p3\n";
    LOG("%s(%d):%d\n",__func__,__LINE__,mible_timer_start(p1,500,strP1));
    LOG("%s(%d):%d\n",__func__,__LINE__,mible_timer_start(p3,500,strP3));
	#elif 0 //测试mible task使用的函数
    char *str1 = "1234567890\n";
    char *str2 = "1234567890ABC\n";
    char *str3 = "1234567890ABCDEF\n";
    char *str4 = "1234567890ABCDEFGHI\n";
    char *str5 = "1234567890ABCDEFGHIJKL\n";
    mible_task_post(timer_callback,(void*)str1);
    mible_task_post(timer_callback,(void*)str2);
    mible_task_post(timer_callback,(void*)str3);
    mible_task_post(timer_callback,(void*)str4);
    mible_task_post(timer_callback,(void*)str5);
	#elif 0 //测试gatts特征值操作接口
    void *p1 = NULL;
    mible_timer_create(&p1,timer_callback,MIBLE_TIMER_REPEATED);
    mible_timer_start(p1,1000,(void*)NULL);
    #elif 0 //测试随机数生成接口
    uint8_t tempBuf[16] = {0};
    mible_rand_num_generator(tempBuf,16);
    LOG("%s(%d):tempBuf = ",__func__,__LINE__);
    LOG_HEX(tempBuf,16);
    LOG("\n");
    mible_rand_num_generator(tempBuf,16);
    LOG("%s(%d):tempBuf = ",__func__,__LINE__);
    LOG_HEX(tempBuf,16);
    LOG("\n");
    mible_rand_num_generator(tempBuf,16);
    LOG("%s(%d):tempBuf = ",__func__,__LINE__);
    LOG_HEX(tempBuf,16);
    LOG("\n");
    mible_rand_num_generator(tempBuf,16);
    LOG("%s(%d):tempBuf = ",__func__,__LINE__);
    LOG_HEX(tempBuf,16);
    LOG("\n");
    mible_rand_num_generator(tempBuf,16);
    LOG("%s(%d):tempBuf = ",__func__,__LINE__);
    LOG_HEX(tempBuf,16);
    LOG("\n");
	#endif
}

/*********************************************************************
 * @fn      MijiaPeripheral_ProcessEvent
 *
 * @brief   mijia Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 MijiaPeripheral_ProcessEvent( uint8 task_id, uint16 events )
{
    VOID task_id; // OSAL required parameter that isn't used in this function

    if ( events & SYS_EVENT_MSG )
    {
        uint8 *pMsg;

        if ( (pMsg = osal_msg_receive( MijiaPeripheral_TaskID )) != NULL )
        {
            MijiaPeripheralProcessOSALMsg( (osal_event_hdr_t *)pMsg );

            // Release the OSAL message
            VOID osal_msg_deallocate( pMsg );
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if ( events & SBP_START_DEVICE_EVT )
    {
        // Start the Device
        VOID GAPRole_StartDevice( &MijiaPeripheralNotificationCB );

        // Start Bond Manager
        //  VOID GAPBondMgr_Register( &MijiaPeripheral_BondMgrCBs );

        return ( events ^ SBP_START_DEVICE_EVT );
    }

    if (events & MIBLE_TIMER1_EVT)
    {
        timer_execute(MIBLE_TIMER1_EVT);
        return (events ^ MIBLE_TIMER1_EVT);
    }

    if (events & MIBLE_TIMER2_EVT)
    {
        timer_execute(MIBLE_TIMER2_EVT);
        return (events ^ MIBLE_TIMER2_EVT);
    }

    if (events & MIBLE_TIMER3_EVT)
    {
        timer_execute(MIBLE_TIMER3_EVT);
        return (events ^ MIBLE_TIMER3_EVT);
    }

    if (events & MIBLE_TIMER4_EVT)
    {
        timer_execute(MIBLE_TIMER4_EVT);
        return (events ^ MIBLE_TIMER4_EVT);
    }

    if (events & MIBLE_TIMER5_EVT)
    {
        timer_execute(MIBLE_TIMER5_EVT);
        return (events ^ MIBLE_TIMER5_EVT);
    }

    if (events & MIBLE_TASK_EVT){
        mible_tasks_exec();
        return (events ^ MIBLE_TASK_EVT);
    }
    // Discard unknown events
    return 0;
}

/*********************************************************************
 * @fn      MijiaPeripheralProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void MijiaPeripheralProcessOSALMsg( osal_event_hdr_t *pMsg )
{
    switch ( pMsg->event )
    {
    default:
        // do nothing
        break;
    }
}
/*********************************************************************
 * @fn      MijiaPeripheralConParaUpdateNotificationCB
 *
 * @brief    Notification from the profile of a connection parameter change.
 *
 * @param   None
 *
 * @return  none
 */
static void MijiaPeripheralConParaUpdateNotificationCB( void )
{
    gap_evt_dispatch(MIBLE_GAP_EVT_CONN_PARAM_UPDATED);
}

/*********************************************************************
 * @fn      MijiaPeripheralRssiNotificationCB
 *
 * @brief   Notification from the profile of a rssi change.
 *
 * @param   rssi - new rssi
 *
 * @return  none
 */
static void MijiaPeripheralRssiNotificationCB( int8  rssi )
{
//    notifyBuf[15]++;
//    notifyBuf[16]=rssi;
//    notifyBuf[17]=HI_UINT16(g_conn_param_foff);
//    notifyBuf[18]=LO_UINT16(g_conn_param_foff);;
//    notifyBuf[19]=g_conn_param_carrSens;
}

/*********************************************************************
 * @fn      MijiaPeripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void MijiaPeripheralStateNotificationCB( gaprole_States_t newState )
{
    switch ( newState )
    {
        case GAPROLE_STARTED:{
            uint8 ownAddress[B_ADDR_LEN];
            GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);
			LOG("MAC address:%x:%x:%x:%x:%x:%x\n",ownAddress[5],ownAddress[4],ownAddress[3],ownAddress[2],ownAddress[1],ownAddress[0]);

            uint8 systemId[DEVINFO_SYSTEM_ID_LEN];
            // use 6 bytes of device address for 8 bytes of system ID value
            systemId[0] = ownAddress[0];
            systemId[1] = ownAddress[1];
            systemId[2] = ownAddress[2];
            // set middle bytes to zero
            systemId[4] = 0x00;
            systemId[3] = 0x00;
            // shift three bytes up
            systemId[7] = ownAddress[5];
            systemId[6] = ownAddress[4];
            systemId[5] = ownAddress[3];
            DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

            uint8 initial_advertising_enable = TRUE;//true
            GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
            }break;

        case GAPROLE_ADVERTISING:
            LOG("Advertising\n");
            break;

        case GAPROLE_CONNECTED:
			LOG("Connected\n");
            gap_evt_dispatch(MIBLE_GAP_EVT_CONNECTED);
            break;

        case GAPROLE_CONNECTED_ADV:
			LOG("Connected Advertising\n");
            break;

        case GAPROLE_WAITING:
			LOG("Disconnected\n");
            gap_evt_dispatch(MIBLE_GAP_EVT_DISCONNET);
            break;

        case GAPROLE_WAITING_AFTER_TIMEOUT:
			LOG("Timed Out\n");
            break;

        case GAPROLE_ERROR:
			LOG("Error\n");
            break;

        default:
            break;
    }
    gapProfileState = newState;

    LOG("[GAP ROLE %d]\n",newState);

    VOID gapProfileState;
}

/*********************************************************************
 * @fn      MijiaProfileParameterChangeCB
 *
 * @brief   Callback from MiJia Profile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void MijiaProfileParameterChangeCB( uint8 paramID ,uint8 len)
{
    gatts_evt_dispatch(MIBLE_GATTS_EVT_WRITE,paramID,len);
}

/*********************************************************************
*********************************************************************/
