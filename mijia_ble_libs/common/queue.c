#include <string.h>
#include "mible_type.h"
#include "queue.h"

#define IS_POWER_OF_TWO(A) ( ((A) != 0) && ((((A) - 1) & (A)) == 0) )

int queue_init(queue_t *q, void *buf, char size, char elem_size)
{
    if (buf == NULL || q == NULL)
        return MI_ERR_INVALID_PARAM;

    if (!IS_POWER_OF_TWO(size))
        return MI_ERR_DATA_SIZE;

    q->buf = buf;
    q->mask = size - 1;
    q->elem_size = elem_size;
    q->rd_ptr = 0;
    q->wr_ptr = 0;

    return MI_SUCCESS;
}

int enqueue(queue_t *q, void *in)
{
    if (((q->wr_ptr - q->rd_ptr) & q->mask) == q->mask) {
        return MI_ERR_NO_MEM;
    }
    
    /* q->buf[q->wr_ptr++] = in; */
    memcpy((char*)q->buf + q->wr_ptr * q->elem_size, in, q->elem_size);
    q->wr_ptr++;
    q->wr_ptr &= q->mask;
    
    return MI_SUCCESS;
}

int dequeue(queue_t *q, void *out)
{
    if (((q->wr_ptr - q->rd_ptr) & q->mask) > 0) {
        /* *out = q->buf[q->rd_ptr++]; */
        memcpy(out, (char*)q->buf + q->rd_ptr * q->elem_size, q->elem_size);
        q->rd_ptr++;
        q->rd_ptr &= q->mask;
        return MI_SUCCESS;
    } else
        return MI_ERR_NOT_FOUND;
}
