/**
 ****************************************************************************************
 *
 * @file usr_design.h
 *
 * @brief Product related design header file.
 *
 * Copyright(C) 2015 NXP Semiconductors N.V.
 * All rights reserved.
 *
 * $Rev: 1.0 $
 *
 ****************************************************************************************
 */

#ifndef USR_DESIGN_H_
#define USR_DESIGN_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_env.h"
#include "gpio.h"

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

struct usr_env_tag
{
    uint16_t    led1_on_dur;
    uint16_t    led1_off_dur;
};

extern struct usr_env_tag usr_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

extern void app_task_msg_hdl(ke_msg_id_t const msgid, void const *param);
extern int app_led_timer_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int app_gap_adv_intv_update_timer_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void usr_sleep_restore(void);
extern void usr_button1_cb(void);
extern int app_button_timer_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void usr_init(void);
extern void gpio_interrupt_callback(enum gpio_pin pin);

extern void Mibeacon_Advertise(void);
extern void Mibeacon_Set_AdvData(void);
extern void mible_init(void);

extern int app_mible_timer_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);

#endif
