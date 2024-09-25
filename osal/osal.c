
#include "osal.h"
#include "osal_task.h"
#include "osal_memory.h"

#include <string.h>

/*********************************************************************
 * @fn osal_init_system
 *
 * @brief
 *
 *   This function initializes the "task" system by creating the
 *   tasks defined in the task table (OSAL_Tasks.h).
 *
 * @param   void
 *
 * @return  RET_SUCCESS
 */
uint8 osal_init_system(void)
{
    // Initialize the Memory Allocation System
    osal_mem_init();

    // Initialize the message queue
    osal_msg_init();

    osal_timer_init();

    osal_task_reset();

    return (RET_SUCCESS);
}

/*********************************************************************
 * @fn osal_task_run
 *
 * @brief
 *
 *   This function is the main loop function of the task system.  It
 *   will look through all task events and call the task_event_processor()
 *   function for the task with the event.  If there are no events (for
 *   all tasks), this function puts the processor into Sleep.
 *   This Function doesn't return.
 *
 * @param   void
 *
 * @return  none
 */
void osal_task_run(void)
{
    uint16 events;
    uint16 retEvents;
    osal_task_t* curTask;
    while(1)
    {
        curTask = osal_task_nextActive();
        if(curTask)
        {
            HAL_ENTER_CRITICAL_SECTION();
            events = curTask->events;
            // Clear the Events for this task
            curTask->events = 0;
            HAL_EXIT_CRITICAL_SECTION();

            if(events != 0)
            {
                // Call the task to process the event(s)
                if(curTask->pfnEventProc)
                {
                    retEvents = (curTask->pfnEventProc)(curTask->taskID, events);

                    // Add back unprocessed events to the current task
                    HAL_ENTER_CRITICAL_SECTION();
                    curTask->events |= retEvents;
                    HAL_EXIT_CRITICAL_SECTION();
                }
            }
        }
    }
}
