#include <string.h>
#include "mible_beacon.h"
#include "mible_type.h"
#include "mible_api.h"

#include "queue.h"
#include "ccm.h"

#undef  MI_LOG_MODULE_NAME
#define MI_LOG_MODULE_NAME "MIBEACON"
#include "mible_log.h"

#ifdef USE_MI_CONFIG
#include "mi_config.h"
#else
#define EVT_MAX_SIZE           16
#define EVT_QUEUE_SIZE         8
#endif

#define BLE_UUID_MI_SERVICE                         0XFE95
#define BLE_UUID_COMPANY_ID_XIAOMI                  0X038F

#define LO_BYTE(val) (uint8_t)val
#define HI_BYTE(val) (uint8_t)(val>>8)
#define IS_POWER_OF_TWO(A) ( ((A) != 0) && ((((A) - 1) & (A)) == 0) )
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define CHECK_ADV_LEN(len)                                                     \
    do {                                                                       \
        if ((len) > MIBLE_MAX_ADV_LENGTH)                                      \
        return MI_ERR_DATA_SIZE;                                               \
    } while(0)

typedef uint8_t mi_obj_element_t[EVT_MAX_SIZE];

static void * mibeacon_timer;
static uint8_t frame_cnt;
static volatile bool m_beacon_timer_is_running;
static volatile uint8_t m_beacon_key_is_vaild;
static uint8_t beacon_key[16];
static queue_t mi_obj_queue;

static struct {
    uint8_t  mac[6];
    uint16_t pid;
    uint8_t  cnt;
    uint8_t  rand[3];
} beacon_nonce;

static int event_encode(mibeacon_obj_t *p_obj, uint8_t *output)
{
    output[0] = p_obj->type;
    output[1] = p_obj->type >> 8;
    output[2] = p_obj->len;
    memcpy(output+3, p_obj->val, p_obj->len);
    return 0;
}

static int calc_objs_bytes(mibeacon_obj_t *p_obj, uint8_t num)
{
    // 3 = 2 bytes object ID + 1 byte length
    uint8_t sum = num * 3;

    if (p_obj == NULL)
        return 0;

    for (uint8_t i = 0; i < num; i++)
        sum += p_obj[i].len;

    return sum;
}


static void mibeacon_timer_handler(void * p_context)
{
    uint32_t errno;
    mi_obj_element_t elem = {0};
    uint8_t scan_rsp_data[31];
    uint8_t scan_rsp_dlen = 0;

    errno = dequeue(&mi_obj_queue, (void*)elem);

    if (errno != MI_SUCCESS) {
        m_beacon_timer_is_running = false;
        mible_timer_stop(mibeacon_timer);
        errno = mible_gap_adv_data_set(NULL, 0, scan_rsp_data, 0);
        MI_ERR_CHECK(errno);
        MI_LOG_INFO("mibeacon event adv end.\n");
    } else {
        m_beacon_timer_is_running = true;
        mible_timer_start(mibeacon_timer, 3000, NULL);

        mibeacon_config_t beacon_cfg = {
        	.frame_ctrl.version = 4,
        	.frame_ctrl.is_encrypt = 1,
        	.pid = beacon_nonce.pid,
        	.p_obj = (void*)elem,
        	.obj_num = 1,
        };

        mible_manu_data_set(&beacon_cfg, scan_rsp_data, &scan_rsp_dlen);
        MI_LOG_HEXDUMP(scan_rsp_data, scan_rsp_dlen);
        MI_LOG_INFO("mibeacon event adv ...\n");
        errno = mible_gap_adv_data_set(NULL, 0, scan_rsp_data, scan_rsp_dlen);
        MI_ERR_CHECK(errno);
    }
}


void set_beacon_key(uint8_t *p_key)
{
    if (p_key == NULL) {
        m_beacon_key_is_vaild = 0;
    } else {
        mible_gap_address_get(beacon_nonce.mac);
        memcpy(beacon_key, p_key, sizeof(beacon_key));
        m_beacon_key_is_vaild = 1;
    }
}

mible_status_t mibeacon_init(uint8_t *key)
{
    static mi_obj_element_t obj_buf[EVT_QUEUE_SIZE];

    mible_status_t errno;

    if (key != NULL) set_beacon_key(key);

    errno = queue_init(&mi_obj_queue, (void*) obj_buf, ARRAY_SIZE(obj_buf), sizeof(obj_buf[0]));
    MI_ERR_CHECK(errno);

    errno = mible_timer_create(&mibeacon_timer, mibeacon_timer_handler, MIBLE_TIMER_SINGLE_SHOT);
    MI_ERR_CHECK(errno);

    return errno;
}

/*
 * @brief   set mibeacon service data  
 * @param   [in] config: mibeacon configure data 
 *          [out] p_output: pointer to mibeacon data  (watch out array out of bounds) 
 *          [out] p_output_len: pointer to mibeacon data length 
 * @return  MI_ERR_INVALID_PARAM:   Invalid pointer supplied or mismatched frmctl.  
 *          MI_ERR_INVALID_LENGTH:  Adv data length exceeds MIBLE_MAX_ADV_LENGTH-7.
 *          MI_ERR_INTERNAL:        Not found rand num used to encrypt data.
 *          MI_SUCCESS:             Set successfully.
 * */
mible_status_t mibeacon_data_set(mibeacon_config_t const * const config, 
        uint8_t *output, uint8_t *output_len)
{
    mibeacon_frame_ctrl_t *p_frame_ctrl = (void*)output;
    uint8_t len, *p_obj_head = NULL, *head = output;
    uint32_t errno;
    
    if (config == NULL || output == NULL || output_len == NULL) {
        *output_len = 0;
        return MI_ERR_INVALID_PARAM;
    }

    beacon_nonce.pid = config->pid;

    /*  encode frame_ctrl and product_id */
    memcpy(output, (uint8_t*)config, 4);
    output     += 4;

    /*  encode frame cnt */
    output[0] = (uint8_t) ++frame_cnt;
    output   += 1;

    /*  encode gap mac */
    if (config->p_mac != NULL) {
        p_frame_ctrl->mac_include = 1;
        memcpy(output, config->p_mac, 6);
        output += 6;
    }

    /*  encode capability */
    if (config->p_capability != NULL) {
        p_frame_ctrl->cap_include = 1;
        mibeacon_capability_t *p_cap = (void*)output;
        output += 1;
        *p_cap = *config->p_capability;

		/*	encode WIFI mac address */
		if(config->p_capability->bondAbility == 3){
			if(config->p_wifi_mac){
				memcpy(output,config->p_wifi_mac,2);	
				output += 2;
			}	
		}

        if (config->p_cap_sub_IO != NULL) {
            p_cap->IO_capability = 1;
            memcpy(output, config->p_cap_sub_IO, sizeof(mibeacon_cap_sub_io_t));
            output += sizeof(mibeacon_cap_sub_io_t);
        }

    }
    
    len = output - head;
    CHECK_ADV_LEN(len);

    /*  encode objects */
    if (config->p_obj != NULL) {
        CHECK_ADV_LEN(len + calc_objs_bytes(config->p_obj, config->obj_num));
        p_frame_ctrl->obj_include = 1;
        p_obj_head = output;
        for (uint8_t i = 0, max = config->obj_num; i < max; i++) {
            event_encode(config->p_obj + i, output);
            output += 3 + config->p_obj[i].len;
        }
        len = output - head;

    }

    /* encrypt objects and encode Nonce, MIC */
    if (p_frame_ctrl->is_encrypt == 1 ) {
        // 7 = 3 bytes random + 4 bytes MIC
        CHECK_ADV_LEN(len + 7);

        if(m_beacon_key_is_vaild) {
            beacon_nonce.cnt = frame_cnt;
            errno = mible_rand_num_generator(beacon_nonce.rand, 3);
            
            if (errno != MI_SUCCESS) {
                MI_ERR_CHECK(errno);
                return MI_ERR_INTERNAL;
            }

            uint8_t mic[4];
            uint8_t aad = 0x11;
            uint8_t objs_len = output - p_obj_head;
            aes_ccm_encrypt_and_tag(beacon_key,
                        (uint8_t*)&beacon_nonce, sizeof(beacon_nonce),
                                           &aad, sizeof(aad),
                           (uint8_t*)p_obj_head, objs_len,
                           (uint8_t*)p_obj_head,
                                            mic, 4);
            
            memcpy(output, beacon_nonce.rand, 3);
            output += sizeof(beacon_nonce.rand);

            memcpy(output, mic, sizeof(mic));
            output += sizeof(mic);
            len = output - head;
        } else {
            p_frame_ctrl->is_encrypt = 0;
            return MI_ERR_INTERNAL;
        }
    }

    if (config->p_mesh != NULL) {
        CHECK_ADV_LEN(len + sizeof(mibeacon_mesh_t));
        p_frame_ctrl->mesh = 1;
        memcpy(output, config->p_mesh, sizeof(mibeacon_mesh_t));
        output += sizeof(mibeacon_mesh_t);
        len = output - head;
    }

    *output_len = len;

    return MI_SUCCESS;
}

/*
 * @brief   Set <service data>. 
 * @param   [in] config: mibeacon configure data 
 *          [out] p_output: pointer to mibeacon data  (watch out array out of bounds) 
 *          [out] p_output_len: pointer to mibeacon data length 
 * @return  MI_ERR_INVALID_PARAM:   Invalid pointer supplied.  
 *          MI_SUCCESS:             Set successfully.
 *          MI_ERR_DATA_SIZE:       Adv bytes excceed the maximun.
 * */
mible_status_t mible_service_data_set(mibeacon_config_t const * const config,
        uint8_t *p_output, uint8_t *p_output_len)
{
    uint32_t errno;
    uint8_t data_len = 0;

// check input
    if(config == NULL || p_output == NULL || p_output_len == NULL){
        MI_LOG_ERROR("error parameters.\n");
        return MI_ERR_INVALID_PARAM;
    }

    p_output[1] = 0x16;
    p_output[2] = LO_BYTE(BLE_UUID_MI_SERVICE);
    p_output[3] = HI_BYTE(BLE_UUID_MI_SERVICE);
    errno = mibeacon_data_set(config, &p_output[4], &data_len);
    p_output[0] = 3 + data_len;

    if (errno != MI_SUCCESS) {
        MI_ERR_CHECK(errno);
        return MI_ERR_DATA_SIZE;
    }
    
    *p_output_len = 4 + data_len;

    return MI_SUCCESS;
}

/*
 * @brief   Set <manufacturer> data. 
 * @param   [in] config: mibeacon configure data 
 *          [out] p_output: pointer to mibeacon data  (watch out array out of bounds) 
 *          [out] p_output_len: pointer to mibeacon data length  
 * @return  MI_ERR_INVALID_PARAM:   Invalid pointer supplied.  
 *          MI_ERR_INVALID_LENGTH:  Data length exceeds MIBLE_MAX_ADV_LENGTH. 
 *          MI_SUCCESS:             Set successfully.
 * @Note:   p_obj[obj_num-1]
 * */
mible_status_t mible_manu_data_set(mibeacon_config_t const * const config,
        uint8_t *p_output, uint8_t *p_output_len)
{
    uint32_t errno;
    uint8_t data_len = 0;
// check input
    if(config == NULL || p_output == NULL || p_output_len == NULL){
        MI_LOG_ERROR("error parameters.\n");
        return MI_ERR_INVALID_PARAM;
    }

    p_output[1] = 0xFF;
    p_output[2] = LO_BYTE(BLE_UUID_COMPANY_ID_XIAOMI);
    p_output[3] = HI_BYTE(BLE_UUID_COMPANY_ID_XIAOMI);
    errno = mibeacon_data_set(config, &p_output[4], &data_len);
    p_output[0] = 3 + data_len;
    
    if (errno != MI_SUCCESS) {
        MI_ERR_CHECK(errno);
        return MI_ERR_DATA_SIZE;
    }

    *p_output_len = 4 + data_len;

    return MI_SUCCESS;
}

int mibeacon_obj_enque(mibeacon_obj_name_t evt, uint8_t len, void *val)
{
    uint32_t errno;
    mi_obj_element_t elem;

    if (len > EVT_MAX_SIZE-3)
        return MI_ERR_DATA_SIZE;

    elem[0] = evt;
    elem[1] = evt >> 8;
    elem[2] = len;
    memcpy(elem+3, (uint8_t*)val, len);

    errno = enqueue(&mi_obj_queue, (char*)elem);
    if(errno != MI_SUCCESS) {
        MI_LOG_ERROR("push beacon event errno %d\n", errno);
        return MI_ERR_RESOURCES;
    }

    if (m_beacon_timer_is_running != true ) {
        /* All event will be processed in mibeacon_timer_handler() */
        errno = mible_timer_start(mibeacon_timer, 10, NULL);
        MI_ERR_CHECK(errno);
    }

    return MI_SUCCESS;
}

