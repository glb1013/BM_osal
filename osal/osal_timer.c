
#include "osal.h"
#include "osal_timer.h"
#include "osal_memory.h"
#include "osal_task.h"

typedef struct
{
    void   *next;
    uint8  task_id;             //响应的任务ID
    uint8  resv;                //
    uint16 event_flag;          //定时事件，定时时间减完产生任务事件
    uint16 timeout;             //定时时间，每过一个系统时钟会自减
    uint16 reloadTimeout;       //重装定时时间
} osal_timer_t;                 //任务定时器，链表结构

typedef struct
{
    uint8  decr_tick;           //任务定时器更新时自减的数值单位
    uint8  isActive;            //标识硬件定时器是否运行
    uint32 system_tick;         //记录系统时钟
}osal_timer_ctrl_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
osal_timer_t  *g_timersQueue;     //任务定时器链表头指针

/*********************************************************************
 * LOCAL VARIABLES
 */
static volatile osal_timer_ctrl_t g_timer_ctrl;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
osal_timer_t *osalAddTimer(uint8 task_id, uint16 event_flag, uint16 timeout);
osal_timer_t *osalFindTimer(uint8 task_id, uint16 event_flag);
void osalDeleteTimer(osal_timer_t *rmTimer);
void osal_timer_activate(uint8 turn_on);
void osal_timer_hw_setup(uint8 turn_on);

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn osal_timer_init
 *
 * @brief   Initialization for the OSAL Timer System.
 *
 * @param   none
 *
 * @return
 */
void osal_timer_init(void)
{
    OSAL_TIMER_TICKINIT();    //初始化硬件定时器

    // Initialize the rollover modulo
    g_timer_ctrl.decr_tick = TIMER_DECR_TIME;

    // Initialize the system timer
    osal_timer_activate(FALSE);
    g_timer_ctrl.isActive = FALSE;

    g_timer_ctrl.system_tick = 0;
}

/*********************************************************************
 * @fn osalAddTimer
 *
 * @brief   Add a timer to the timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 * @param   timeout
 *
 * @return  osal_timer_t * - pointer to newly created timer
 */
osal_timer_t * osalAddTimer(uint8 task_id, uint16 event_flag, uint16 timeout)
{
    osal_timer_t *newTimer;
    osal_timer_t *srchTimer;

    // Look for an existing timer first
    newTimer = osalFindTimer(task_id, event_flag);
    if(newTimer)
    {
        // Timer is found - update it.
        newTimer->timeout = timeout;

        return (newTimer);
    }
    else
    {
        // New Timer
        newTimer = osal_mem_alloc(sizeof(osal_timer_t));

        if(newTimer)
        {
            // Fill in new timer
            newTimer->task_id = task_id;
            newTimer->event_flag = event_flag;
            newTimer->timeout = timeout;
            newTimer->next = (void *)NULL;
            newTimer->reloadTimeout = 0;

            // Does the timer list already exist
            if(g_timersQueue == NULL)
            {
                // Start task list
                g_timersQueue = newTimer;
            }
            else
            {
                // Add it to the end of the timer list
                srchTimer = g_timersQueue;

                // Stop at the last record
                while(srchTimer->next)
                    srchTimer = srchTimer->next;

                // Add to the list
                srchTimer->next = newTimer;
            }
            return (newTimer);
        }
        else
            return ((osal_timer_t *)NULL);
    }
}

/*********************************************************************
 * @fn osalFindTimer
 *
 * @brief   Find a timer in a timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 *
 * @return  osal_timer_t *
 */
osal_timer_t *osalFindTimer(uint8 task_id, uint16 event_flag)
{
    osal_timer_t *srchTimer;

    // Head of the timer list
    srchTimer = g_timersQueue;

    // Stop when found or at the end
    while(srchTimer)
    {
        if(srchTimer->event_flag == event_flag &&
                srchTimer->task_id == task_id)
            break;

        // Not this one, check another
        srchTimer = srchTimer->next;
    }

    return (srchTimer);
}

/*********************************************************************
 * @fn osalDeleteTimer
 *
 * @brief   Delete a timer from a timer list.
 *
 * @param   table
 * @param   rmTimer
 *
 * @return  none
 */
void osalDeleteTimer(osal_timer_t *rmTimer)
{
    // Does the timer list really exist
    if(rmTimer)
    {
        // Clear the event flag and osalTimerUpdate() will delete
        // the timer from the list.
        rmTimer->event_flag = 0;
    }
}

/*********************************************************************
 * @fn osal_timer_activate
 *
 * @brief
 *
 *   Turns the hardware timer on or off
 *
 * @param  uint8 turn_on - false - turn off, true - turn on
 *
 * @return  none
 */
void osal_timer_activate(uint8 turn_on)
{
    osal_timer_hw_setup(turn_on);
    g_timer_ctrl.isActive = turn_on;
}

/*********************************************************************
 * @fn osal_timer_hw_setup
 *
 * @brief
 *
 *   Setup the timer hardware.
 *
 * @param  uint8 turn_on
 *
 * @return  void
 */
void osal_timer_hw_setup(uint8 turn_on)
{
    if(turn_on)
    {
        OSAL_TIMER_TICKSTART() ;
    }
    else
    {
        OSAL_TIMER_TICKSTOP();
    }
}

/*********************************************************************
 * @fn osal_start_timerEx
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event.
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or RET_NO_TIMER_AVAIL.
 */
uint8 osal_start_timerEx(uint8 taskID, uint16 event_id, uint16 timeout_value)
{
    osal_timer_t *newTimer;

    HAL_ENTER_CRITICAL_SECTION(); // Hold off interrupts.

    // Add timer
    newTimer = osalAddTimer(taskID, event_id, timeout_value);

    if(newTimer)
    {
        // Does the hal timer need to be started?
        if(g_timer_ctrl.isActive == FALSE)
        {
            osal_timer_activate(TRUE);
        }
    }

    HAL_EXIT_CRITICAL_SECTION(); // Re-enable interrupts.

    return ((newTimer != NULL) ? RET_SUCCESS : RET_TIMER_INVALID);
}

/*********************************************************************
 * @fn osal_start_reload_timer
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event
 *   and the timer will be reloaded with the timeout value.
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or RET_NO_TIMER_AVAIL.
 */
uint8 osal_start_reload_timer(uint8 taskID, uint16 event_id, uint16 timeout_value)
{
    osal_timer_t *newTimer;

    HAL_ENTER_CRITICAL_SECTION(); // Hold off interrupts.

    // Add timer
    newTimer = osalAddTimer(taskID, event_id, timeout_value);
    if(newTimer)
    {
        // Load the reload timeout value
        newTimer->reloadTimeout = timeout_value;
    }

    if(newTimer)
    {
        // Does the hal timer need to be started?
        if(g_timer_ctrl.isActive == FALSE)
        {
            osal_timer_activate(TRUE);
        }
    }

    HAL_EXIT_CRITICAL_SECTION(); // Re-enable interrupts.

    return ((newTimer != NULL) ? RET_SUCCESS : RET_TIMER_INVALID);
}

/*********************************************************************
 * @fn osal_stop_timerEx
 *
 * @brief
 *
 *   This function is called to stop a timer that has already been started.
 *   If RET_SUCCESS, the function will cancel the timer and prevent the event
 *   associated with the timer from being set for the calling task.
 *
 * @param   uint8 task_id - task id of timer to stop
 * @param   uint16 event_id - identifier of the timer that is to be stopped
 *
 * @return  SUCCESS or RET_INVALID_EVENT_ID
 */
uint8 osal_stop_timerEx(uint8 task_id, uint16 event_id)
{
    osal_timer_t *foundTimer;

    HAL_ENTER_CRITICAL_SECTION(); // Hold off interrupts.

    // Find the timer to stop
    foundTimer = osalFindTimer(task_id, event_id);
    if(foundTimer)
    {
        osalDeleteTimer(foundTimer);
    }

    HAL_EXIT_CRITICAL_SECTION(); // Re-enable interrupts.

    return ((foundTimer != NULL) ? RET_SUCCESS : RET_EVENT_INVALID_ID);
}

/*********************************************************************
 * @fn osal_get_timeoutEx
 *
 * @brief
 *
 * @param   uint8 task_id - task id of timer to check
 * @param   uint16 event_id - identifier of timer to be checked
 *
 * @return  Return the timer's tick count if found, zero otherwise.
 */
uint16 osal_get_timeoutEx(uint8 task_id, uint16 event_id)
{
    uint16 rtrn = 0;
    osal_timer_t *tmr;

    HAL_ENTER_CRITICAL_SECTION(); // Hold off interrupts.

    tmr = osalFindTimer(task_id, event_id);

    if(tmr)
    {
        rtrn = tmr->timeout;
    }

    HAL_EXIT_CRITICAL_SECTION(); // Re-enable interrupts.

    return rtrn;
}

/*********************************************************************
 * @fn osal_timer_activeNum
 *
 * @brief
 *
 *   This function counts the number of active timers.
 *
 * @return  uint8 - number of timers
 */
uint8 osal_timer_activeNum(void)
{
    uint8 num_timers = 0;
    osal_timer_t *srchTimer;

    HAL_ENTER_CRITICAL_SECTION(); // Hold off interrupts.

    // Head of the timer list
    srchTimer = g_timersQueue;

    // Count timers in the list
    while(srchTimer != NULL)
    {
        num_timers++;
        srchTimer = srchTimer->next;
    }

    HAL_EXIT_CRITICAL_SECTION(); // Re-enable interrupts.

    return num_timers;
}

/*********************************************************************
 * @fn osalTimerUpdate
 *
 * @brief   Update the timer structures for a timer tick.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osalTimerUpdate(uint16 updateTime)
{
    osal_timer_t *srchTimer;
    osal_timer_t *prevTimer;

    HAL_ENTER_CRITICAL_SECTION(); // Hold off interrupts.
    // Update the system time
    g_timer_ctrl.system_tick += updateTime;
    HAL_EXIT_CRITICAL_SECTION(); // Re-enable interrupts.

    // Look for open timer slot
    if(g_timersQueue != NULL)
    {
        // Add it to the end of the timer list
        srchTimer = g_timersQueue;
        prevTimer = (void *)NULL;

        // Look for open timer slot
        while(srchTimer)
        {
            osal_timer_t *freeTimer = NULL;

            HAL_ENTER_CRITICAL_SECTION(); // Hold off interrupts.

            if(srchTimer->timeout <= updateTime)
            {
                srchTimer->timeout = 0;
            }
            else
            {
                srchTimer->timeout = srchTimer->timeout - updateTime;
            }

            // Check for reloading
            if((srchTimer->timeout == 0) && (srchTimer->reloadTimeout) && (srchTimer->event_flag))
            {
                // Notify the task of a timeout
                osal_set_event(srchTimer->task_id, srchTimer->event_flag);

                // Reload the timer timeout value
                srchTimer->timeout = srchTimer->reloadTimeout;
            }

            // When timeout or delete (event_flag == 0)
            if(srchTimer->timeout == 0 || srchTimer->event_flag == 0)
            {
                // Take out of list
                if(prevTimer == NULL)
                    g_timersQueue = srchTimer->next;
                else
                    prevTimer->next = srchTimer->next;

                // Setup to free memory
                freeTimer = srchTimer;

                // Next
                srchTimer = srchTimer->next;
            }
            else
            {
                // Get next
                prevTimer = srchTimer;
                srchTimer = srchTimer->next;
            }

            HAL_EXIT_CRITICAL_SECTION(); // Re-enable interrupts.

            if(freeTimer)
            {
                if(freeTimer->timeout == 0)
                {
                    osal_set_event(freeTimer->task_id, freeTimer->event_flag);
                }
                osal_mem_free(freeTimer);
            }
        }
    }
}

/*********************************************************************
 * @fn osal_getSystemClock()
 *
 * @brief   Read the local system clock.
 *
 * @param   none
 *
 * @return  local clock in milliseconds
 */
uint32 osal_getSystemClock(void)
{
    return (g_timer_ctrl.system_tick);
}

/*********************************************************************
 * @fn osal_update_timers
 *
 * @brief   Update the timer structures for timer ticks.
 *
 * @param   none
 *
 * @return  none
 */
//在每次系统定时溢出时调用一次，更新计时器
void osal_update_timers(void)
{
    osalTimerUpdate(g_timer_ctrl.decr_tick);
}
