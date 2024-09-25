#ifndef  OSAL_EVENT_H
#define  OSAL_EVENT_H

#include "type.h"
#include "osal_timer.h"


/**
 * @brief 任务链表
 */
typedef void (*TaskInitFuncPtr)(uint8 task_id);
typedef uint16(*TaskEventHandlerPtr)(uint8 task_id, uint16 task_event);
typedef struct _osal_task_def_
{
    struct  _osal_task_def_  *next;
    TaskInitFuncPtr      pfnInit;               //任务初始化函数指针
    TaskEventHandlerPtr  pfnEventProc;     //任务事件处理函数指针
    uint8                taskID;                //任务ID
    uint8                taskPriority;          //任务优先级
    uint16               events;                //任务事件
} osal_task_t;

extern void osal_task_reset(void);
extern void osal_task_add(TaskInitFuncPtr pfnInit, TaskEventHandlerPtr pfnEventProc, uint8 taskPriority);
extern void osal_task_initAll(void);
extern void osal_task_run(void);

extern osal_task_t* osal_task_getActive(void);
extern osal_task_t *osal_task_nextActive(void);
extern osal_task_t *osal_task_find(uint8 taskID);

extern uint8 osal_set_event(uint8 task_id, uint16 event_flag);
extern uint8 osal_clear_event(uint8 task_id, uint16 event_flag);

#endif
