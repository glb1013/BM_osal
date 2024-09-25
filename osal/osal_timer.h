#ifndef OSAL_TIMER_H
#define OSAL_TIMER_H

#include "osal_type.h"
#include "hal_timer.h"

#define TIMER_DECR_TIME             1     //����ʱ������ʱ�Լ�����ֵ��λ

extern void osal_timer_init(void);
extern uint8 osal_start_timerEx(uint8 task_id, uint16 event_id, uint16 timeout_value);
extern uint8 osal_start_reload_timer(uint8 taskID, uint16 event_id, uint16 timeout_value);
extern uint8 osal_stop_timerEx(uint8 task_id, uint16 event_id);
extern uint16 osal_get_timeoutEx(uint8 task_id, uint16 event_id);
extern uint8 osal_timer_activeNum(void);
extern uint32 osal_getSystemClock(void);
extern void osal_update_timers(void);

#endif
