#include "osal.h"
#include "osal_event.h"
#include "osal_memory.h"

osal_task_t *gTaskHead;
osal_task_t *gActiveTask;

volatile uint8 gTaskId;              //任务ID统计
volatile uint8 gTasksCnt;             //任务数量统计

/*********************************************************************
 * @fn osal_set_event
 *
 * @brief
 *
 *    This function is called to set the event flags for a task.The
 *    event passed in is OR'd into the task's event variable.
 *
 * @param   byte task_id - receiving tasks ID
 * @param   byte event_flag - what event to set
 *
 * @return  RET_SUCCESS, RET_TASK_INVALID
 */
uint8 osal_set_event(uint8 task_id, uint16 event_flag)
{
    osal_task_t *srchTask;

    srchTask = osal_task_find(task_id);
    if(srchTask)
    {
        // Hold off interrupts
        HAL_ENTER_CRITICAL_SECTION();
        // Stuff the event bit(s)
        srchTask->events |= event_flag;
        // Release interrupts
        HAL_EXIT_CRITICAL_SECTION();
    }
    else
        return (RET_TASK_INVALID);

    return (RET_SUCCESS);
}

/*********************************************************************
 * @fn osal_clear_event
 *
 * @brief
 *
 *    This function is called to clear the event flags for a task. The
 *    event passed in is masked out of the task's event variable.
 *
 * @param   uint8 task_id - receiving tasks ID
 * @param   uint8 event_flag - what event to clear
 *
 * @return  SUCCESS, RET_TASK_INVALID
 */
uint8 osal_clear_event(uint8 task_id, uint16 event_flag)
{
    osal_task_t *srchTask;

    srchTask = osal_task_find(task_id);
    if(srchTask)
    {
        // Hold off interrupts
        HAL_ENTER_CRITICAL_SECTION();
        // Stuff the event bit(s)
        srchTask->events &= ~event_flag;
        // Release interrupts
        HAL_EXIT_CRITICAL_SECTION();
    }
    else
        return (RET_TASK_INVALID);

    return (RET_SUCCESS);
}

/***************************************************************************
 * @fn osal_task_reset
 *
 * @brief   init task link's head
 *
 * @param   none
 *
 * @return
 */
void  osal_task_reset(void)
{
    gTaskHead   = (osal_task_t *)NULL;
    gActiveTask = (osal_task_t *)NULL;
    gTaskId = TASK_ID_USER_START;
}

/***************************************************************************
 * @fn osal_task_initAll
 *
 * @brief   init task
 *
 * @param   none
 *
 * @return
 */
void osal_task_initAll(void)
{
    osal_task_t* p_task;
    p_task = gTaskHead;
    while(p_task)
    {
        if(p_task->pfnInit)
        {
            p_task->pfnInit(p_task->taskID);
        }
        p_task = p_task->next;
    }

    //clear current task
    gActiveTask = (osal_task_t *)NULL;
}

/***************************************************************************
 * @fn osal_task_add
 *
 * @brief   osal_task_add
 *
 * @param   none
 *
 * @return
 */
void  osal_task_add(TaskInitFuncPtr pfnInit,
                    TaskEventHandlerPtr pfnEventProc,
                    uint8 taskPriority)
{
    osal_task_t  *TaskNew;
    osal_task_t  *TaskSech;
    osal_task_t  **TaskPTR;
    TaskNew = osal_mem_alloc(sizeof(osal_task_t));
    if(TaskNew)
    {
        TaskNew->pfnInit = pfnInit;
        TaskNew->pfnEventProc = pfnEventProc;
        TaskNew->taskID = gTaskId++;
        TaskNew->events = 0;
        TaskNew->taskPriority = taskPriority;
        TaskNew->next = (osal_task_t *)NULL;

        TaskPTR = &gTaskHead;
        TaskSech = gTaskHead;

        gTasksCnt++;            //任务数量统计

        while(TaskSech)
        {
            if(TaskNew->taskPriority > TaskSech->taskPriority)
            {
                TaskNew->next = TaskSech;
                *TaskPTR = TaskNew;
                return;
            }
            TaskPTR = &TaskSech->next;
            TaskSech = TaskSech->next;
        }
        *TaskPTR = TaskNew;
    }
    return;
}

osal_task_t* osal_task_getActive(void)
{
    return gActiveTask;
}

/*********************************************************************
 * @fn osal_task_nextActive
 *
 * @brief   This function will return the next active task.
 *
 * NOTE:    Task queue is in priority order. We can stop at the
 *          first task that is "ready" (events element non-zero)
 *
 * @param   none
 *
 * @return  pointer to the found task, NULL if not found
 */
osal_task_t *osal_task_nextActive(void)
{
    osal_task_t  *TaskSech;

    // Start at the beginning
    TaskSech = gTaskHead;

    // When found or not
    while(TaskSech)
    {
        if(TaskSech->events)
        {
            // task is highest priority that is ready
            return  TaskSech;
        }
        TaskSech =  TaskSech->next;
    }
    return NULL;
}

/*********************************************************************
 * @fn osalFindActiveTask
 *
 * @brief   This function will return the taskid task.
 *
 * NOTE:    Task queue is in priority order. We can stop at the
 *          first task that is "ready" (events element non-zero)
 *
 * @param   task_id
 *
 * @return  pointer to the found task, NULL if not found
 */
osal_task_t *osal_task_find(uint8 taskID)
{
    osal_task_t *TaskSech;
    TaskSech = gTaskHead;
    while(TaskSech)
    {
        if(TaskSech->taskID == taskID)
        {
            break;
        }
        TaskSech = TaskSech->next;
    }
    return TaskSech;
}
