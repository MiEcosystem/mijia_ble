#include "mible_FSM.h"
#include "mible_port.h"

void mible_FSM_init(mible_FSM_t *p_FSM, uint16_t init_state,
		const mible_FSM_list_t *list, uint16_t list_len)
{
	p_FSM->cur_state = init_state;
	p_FSM->list = list;
	p_FSM->list_len = list_len;
}

void mible_FSM_set(mible_FSM_t *p_FSM,uint16_t state)
{
	p_FSM->cur_state = state;
}

uint16_t mible_FSM_get(mible_FSM_t *p_FSM)
{
	return p_FSM->cur_state;
}

/*Note: State machine no real transfer, should be set in handler*/
mible_handler_t mible_FSM_transition(mible_FSM_t *p_FSM, uint16_t event){
	const mible_FSM_list_t *list = p_FSM->list;
	uint8_t i=0;
	if(NULL == list){
		return NULL;	
	}	

	for(i=0; i<p_FSM->list_len; i++){
		if(list->state == p_FSM->cur_state){
			if(list->event == event){
				return list->handler;
			}	
		}
		list++;
	}
	return NULL;
}
