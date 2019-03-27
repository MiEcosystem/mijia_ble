#ifndef __MI_CONFIG_H__
#define __MI_CONFIG_H__
#include <stdint.h>

#define PRODUCT_ID             0x01CF

#define EVT_MAX_SIZE           16
#define EVT_QUEUE_SIZE         8

#define MSC_ADDR               0x2A
#define MSC_SDA_PORT           0
#define MSC_SDA_PIN            29
#define MSC_SDA_LOCATION       0
#define MSC_SCL_PORT           0
#define MSC_SCL_PIN            28
#define MSC_SCL_LOCATION       0

/* DEBUG */
#define PRINT_MSC_INFO         0
#define PRINT_MAC              0
#define PRINT_DEV_PUBKEY       0
#define PRINT_SHA256           0
#define PRINT_SIGN             0
#define PRINT_LTMK             0


#endif  /* __MI_CONFIG_H__ */ 


