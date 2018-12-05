#ifndef MIBLE_BEACON_H__
#define MIBLE_BEACON_H__

#include "mible_api.h"
#include "mible_log.h"
#include "mible_port.h"
#include "mible_type.h"

#define MIBLE_MAX_ADV_LENGTH                        31

typedef struct {
    uint8_t         time_protocol       :1;
    uint8_t         reserved1           :1;
    uint8_t         reserved2           :1;
    uint8_t         is_encrypt          :1;

    uint8_t         mac_include         :1;
    uint8_t         cap_include         :1;
    uint8_t         obj_include         :1;
    uint8_t         mesh                :1;

    uint8_t         reserved4           :1;
    uint8_t         bond_confirm        :1;
    uint8_t         secure_auth         :1;
    uint8_t         secure_login        :1;

    uint8_t         version             :4;
} mibeacon_frame_ctrl_t;

typedef struct {
    uint16_t        type;
    uint8_t         len;
    uint8_t         val[17];
 } mibeacon_obj_t;

typedef enum {
    MI_EVT_BASE          = 0x0000,
    MI_EVT_CONNECT       = 0x0001,
    MI_EVT_SIMPLE_PAIR   = 0x0002,
    MI_EVT_LOCK          = 0x0005,
    MI_EVT_DOOR          = 0x0007,

    MI_STA_BASE         = 0x1000,
    MI_STA_BUTTON       = 0x1001,
    MI_STA_SLEEP        = 0x1002,
    MI_STA_RSSI         = 0x1003,
    MI_STA_TEMPARATURE  = 0x1004,
    MI_STA_WATER_BOIL   = 0x1005,
    MI_STA_HUMIDITY     = 0x1006,
    MI_STA_LUMINA       = 0x1007,
    MI_STA_SOIL_PF      = 0x1008,
    MI_STA_SOIL_EC      = 0x1009,
    MI_STA_BATTERY      = 0x100A,
    MI_STA_LOCK         = 0x100E,
    MI_STA_DOOR         = 0x100F,

} mibeacon_obj_name_t;

typedef struct {
    uint8_t         connectable  :1;
    uint8_t         centralable  :1;
    uint8_t         encryptable  :1;
    uint8_t         bondAbility  :2;
    uint8_t         IO_capability:1;
    uint8_t         reserved     :2;
} mibeacon_capability_t;

typedef struct {
    uint8_t in_digits            :1;
    uint8_t in_alphabet          :1;
    uint8_t in_nfc_tag           :1;
    uint8_t in_image             :1;
    uint8_t out_digits           :1;
    uint8_t out_alphabet         :1;
    uint8_t out_nfc_tag          :1;
    uint8_t out_image            :1;

    uint8_t reserved             :8;
} mibeacon_cap_sub_io_t;

typedef struct {
    uint8_t         pb_adv       :1;
    uint8_t         pb_gatt      :1;
    uint8_t         is_auth      :1;
    uint8_t         is_prov      :1;
    uint8_t         version      :4;

    uint8_t         reserved     :8;
} mibeacon_mesh_t;

typedef struct {
    mibeacon_frame_ctrl_t   frame_ctrl;
    uint16_t                pid;
    mible_addr_t           *p_mac;
    mibeacon_capability_t  *p_capability;
    mibeacon_cap_sub_io_t  *p_cap_sub_IO;
    uint8_t 			   *p_wifi_mac;
    mibeacon_obj_t         *p_obj;
    uint8_t                 obj_num;
    mibeacon_mesh_t        *p_mesh;
} mibeacon_config_t;

mible_status_t mibeacon_init(uint8_t *key);

mible_status_t mibeacon_data_set(mibeacon_config_t const * const config,
        uint8_t *p_output, uint8_t *p_output_len);

mible_status_t mible_service_data_set(mibeacon_config_t const * const config,
        uint8_t *p_output, uint8_t *p_output_len);

mible_status_t mible_manu_data_set(mibeacon_config_t const * const config,
        uint8_t *p_output, uint8_t *p_output_len);

int mibeacon_obj_enque(mibeacon_obj_name_t evt, uint8_t len, void *val);

void set_beacon_key(uint8_t *p_key);

#endif
