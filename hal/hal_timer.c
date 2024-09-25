/**
 * @file timer.c
 * @brief Ӳ����ʱ��ʵ�֣�Ϊosal����ϵͳ�ṩϵͳ�δ�����ʱ�ӣ���ֲʱ��Ҫ�޸ĸ��ļ�
 * @version 0.1
 * @date 2019-07-25
 * @author WatWu
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#include <conio.h>
#include <windows.h>
#include "hal_timer.h"
#include "osal_timer.h"

#pragma  comment(lib, "Winmm.lib")

typedef void (*TimerCallback)(void*);
typedef struct {
    uint16_t state;
    MMRESULT handle;
    uint32_t peroid;
    TimerCallback callback;
    void* para;
} sys_timer_t;

volatile sys_timer_t g_hal_timer;

static void hal_timer_func(void* pro)
{
    osal_update_timers();
}
void CALLBACK TimerProc(UINT handle, UINT uMsg, DWORD_PTR user, DWORD_PTR arg1, DWORD_PTR arg2)
{
    if (g_hal_timer.handle != handle || g_hal_timer.state <= 0)
    {
        printf("[ERROR] timer proc: %u %08x %08x %08x\r\n",
            uMsg, (uint32_t)user, (uint32_t)arg1, (uint32_t)arg2);
        return;
    }

    if (g_hal_timer.callback != NULL)
    {
        g_hal_timer.callback(g_hal_timer.para);
    }
    else
    {
        /*printf("[DEBUG] timer proc: %u %u %08x %08x %08x\r\n",
            id, uMsg, (uint32_t)user, (uint32_t)arg1, (uint32_t)arg2);*/
    }
}

void OSAL_TIMER_TICKINIT(void)
{
    g_hal_timer.peroid = TICK_PERIOD_MS; //10ms
    g_hal_timer.callback = hal_timer_func;
    g_hal_timer.para = NULL;
    g_hal_timer.handle = timeSetEvent(g_hal_timer.peroid, 1, TimerProc, g_hal_timer.para, TIME_PERIODIC);
    g_hal_timer.state = TRUE;
}

void OSAL_TIMER_TICKSTART(void)
{

}

void OSAL_TIMER_TICKSTOP(void)
{

}

#else /* linux */
#include <unistd.h>
#include <pthread.h>

#include "timer.h"
#include "osal_timer.h"

//�˴����Ӳ����ʱ���ж�����������ж�����1��10ms�������жϺ����е���ϵͳʱ�Ӹ��º���osal_update_timers()
//��ÿ��ϵͳ�δ�����ʱ����һ��osal_update_timers()

//��ǰ���̻���linux���У�ʹ���߳����ߵķ�ʽģ��Ӳ����ʱ����ʵ������

static pthread_t hal_timer_pthread_fd;

/**
 * @brief ��ʱ���̣߳�Ϊosal�ṩ�δ��������ڵ�Ƭ��ƽ̨Ӧ��ʹ��Ӳ����ʱ��ʵ��
 * @param pro       [�̺߳��������б�]
 * @return void*    [��]
 */
static void* hal_timer_pthread(void *pro)
{
    pro = pro;
    while(1)
    {
        usleep(10 * 1000);      //10ms������
        osal_update_timers();
    }

    return 0;
}

/**
 * @brief Ӳ����ʱ����ʼ�����趨ϵͳʱ��
 */
void OSAL_TIMER_TICKINIT(void)
{
    //������ʱ���̣߳�ʹ���߳���ģ�ⶨʱ��
    int ret = pthread_create(&hal_timer_pthread_fd, NULL, hal_timer_pthread, NULL);
    if(ret != 0)
    {
        perror("Create hal timer error");
        exit(1);
    }
    printf("Init hal timer ok !\n");
}

/**
 * @brief ����Ӳ����ʱ����OSAL����ݳ����������ʱ����ʵ��ʹ�ö�̬�����͹رգ�Ϊ����һֱ����
 */
void OSAL_TIMER_TICKSTART(void)
{

}

/**
 * @brief �ر�Ӳ����ʱ����Ϊ����һֱ���ر�
 */
void OSAL_TIMER_TICKSTOP(void)
{

}
#endif
