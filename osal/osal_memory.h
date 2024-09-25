#ifndef OSALMEM_METRICS_H
#define OSALMEM_METRICS_H

#include "osal_type.h"

#define MAXMEMHEAP              1024*6       //�ڴ�ش�С����λ�ֽ�

#define OSALMEM_METRICS         1            //������Ч�����ڴ�ͳ��

void osal_mem_init(void);
void osal_mem_kick(void);
void *osal_mem_alloc(uint16 size);
void osal_mem_free(void *ptr);

#if OSALMEM_METRICS
uint16 osal_heap_block_max(void);
uint16 osal_heap_block_cnt(void);
uint16 osal_heap_block_free(void);
uint16 osal_heap_mem_used(void);
uint16 osal_heap_high_water(void);
uint16 osal_heap_mem_usage_rate(void);
#endif

#endif
