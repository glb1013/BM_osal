#ifndef OSAL_H
#define OSAL_H

#include "osal_type.h"
#include "hal.h"

/* event定义： 16bits
* 高8bits：保留为osal系统使用，所有task通用。
* 低8bits：各个task专用的event定义
*/
#define TASK_EVENT_DEF(n)       BIT_MASK((n & 0x07U))
typedef enum {
    SYS_EVENT_MSG = BIT_MASK(15),    //用于收发消息
    SYS_EVENT_0   = BIT_MASK(14),
    SYS_EVENT_1   = BIT_MASK(13),
    SYS_EVENT_2   = BIT_MASK(12),
    SYS_EVENT_3   = BIT_MASK(11),
    SYS_EVENT_4   = BIT_MASK(10),
    SYS_EVENT_5   = BIT_MASK(9),
    SYS_EVENT_6   = BIT_MASK(8)
} SYS_EVENT_TYPE_E;

typedef struct
{
    void   *next;
    uint16 len;
    uint8  dest_id;
} osal_msg_hdr_t;

typedef struct
{
    uint8 event;
    uint8 status;
} osal_event_hdr_t;

typedef struct
{
    osal_event_hdr_t hdr;
    uint8* Data_t;
} osal_sys_msg_t;                       //默认系统消息结构体

typedef void *osal_msg_queue_t;

extern osal_msg_queue_t g_sys_msg_queue;
extern volatile uint8 gTasksCnt;                  //任务数量统计

uint8 osal_init_system(void);

int osal_strlen(char *pString);
void *osal_memcpy(void *dst, const void *src, unsigned int len);
void *osal_revmemcpy(void *dst, const void *src, unsigned int len);
void *osal_memdup(const void *src, unsigned int len);
uint8 osal_memcmp(const void *src1, const void *src2, unsigned int len);
void *osal_memset(void *dest, uint8 value, int len);

#endif
