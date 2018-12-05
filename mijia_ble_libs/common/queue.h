#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <stdint.h>

typedef struct {
    void * buf;
    uint8_t mask;
    uint8_t elem_size;
    uint8_t rd_ptr;
    uint8_t wr_ptr;
} queue_t;

int queue_init(queue_t *q, void *buf, char size, char elem_size);
int enqueue(queue_t *q, void *in);
int dequeue(queue_t *q, void *out);

#endif //__QUEUE_H_
