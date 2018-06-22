#ifndef MIBLE_FSM_H_
#define MIBLE_FSM_H_
#include "mible_port.h"
#include "mible_type.h"
#include "stddef.h"

typedef struct {
    uint16_t state; /*! The current state of mi service */
    uint16_t event; /*! The event ID */
    mible_handler_t handler; /*! The corresponding event handler */
} mible_FSM_list_t;

typedef struct {
    uint16_t cur_state;
    uint16_t list_len;
    const mible_FSM_list_t *list;
} mible_FSM_t;

/*init FSM*/
void mible_FSM_init(mible_FSM_t *p_FSM, uint16_t init_state,
		const mible_FSM_list_t *list, uint16_t list_len);

/*set current state*/
void mible_FSM_set(mible_FSM_t *p_FSM,uint16_t state);

/*get current state*/
uint16_t mible_FSM_get(mible_FSM_t *p_FSM);

/*FSM transition*/
mible_handler_t mible_FSM_transition(mible_FSM_t *p_FSM, uint16_t event);

#endif
