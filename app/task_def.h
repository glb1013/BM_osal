/*****************************************************************************/
#ifndef APPLICATION_H
#define APPLICATION_H

#include "osal.h"
#include "osal_timer.h"
#include "osal_task.h"
#include "osal_memory.h"
#include "osal_msg.h"

/*
****************************************************************************
所有任务的任务ID、初始化函数、事件处理函数、任务事件都统一在此文件声明或定义
****************************************************************************
*/
typedef struct
{
    osal_event_hdr_t  hdr;          //操作系统事件结构
    unsigned char *data;            //命令帧操作数
} general_msg_data_t;               //自定义通用消息格式结构体

//任务定义
enum {
    TASK_PRIORITY_PRINTF = 1,
    TASK_PRIORITY_STATISTICS
};

//任务事件定义：每个task可以定义独有的task_event_type
typedef enum {
    PRINT_TASK_EVENT_STR = TASK_EVENT_DEF(0),          //打印字符串事件
} PRINTF_TASK_EVENT_TYPE_E;

//统计任务的系统消息事件定义
typedef enum {
    PRINTF_STATISTICS_MSG = 1U,               //打印统计消息事件
} PRINTF_TASK_MSG_TYPE_E;

//全局变量声明
//任务ID声明
extern uint8 print_task_id;
extern uint8 statistics_task_id;

//任务初始化函数声明&事件处理函数声明
void print_task_init(uint8 task_id);
uint16 print_task_event_process(uint8 task_id, uint16 task_event);
void statistics_task_init(uint8 task_id);
uint16 statistics_task_event_process(uint8 task_id, uint16 task_event);

/*****************************************************************************/

void osal_main(void);

#endif
