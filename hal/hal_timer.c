/**
 * @file timer.c
 * @brief 硬件定时器实现，为osal操作系统提供系统滴答心跳时钟，移植时需要修改该文件
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

//此处添加硬件定时器中断溢出函数，中断周期1～10ms，并在中断函数中调用系统时钟更新函数osal_update_timers()
//即每次系统滴答心跳时调用一次osal_update_timers()

//当前例程基于linux运行，使用线程休眠的方式模拟硬件定时器来实现心跳

static pthread_t hal_timer_pthread_fd;

/**
 * @brief 定时器线程，为osal提供滴答心跳，在单片机平台应该使用硬件定时器实现
 * @param pro       [线程函数参数列表]
 * @return void*    [无]
 */
static void* hal_timer_pthread(void *pro)
{
    pro = pro;
    while(1)
    {
        usleep(10 * 1000);      //10ms的心跳
        osal_update_timers();
    }

    return 0;
}

/**
 * @brief 硬件定时器初始化，设定系统时钟
 */
void OSAL_TIMER_TICKINIT(void)
{
    //创建定时器线程，使用线程来模拟定时器
    int ret = pthread_create(&hal_timer_pthread_fd, NULL, hal_timer_pthread, NULL);
    if(ret != 0)
    {
        perror("Create hal timer error");
        exit(1);
    }
    printf("Init hal timer ok !\n");
}

/**
 * @brief 开启硬件定时器，OSAL会根据程序中软件定时器的实际使用动态开启和关闭，为空则一直开启
 */
void OSAL_TIMER_TICKSTART(void)
{

}

/**
 * @brief 关闭硬件定时器，为空则一直不关闭
 */
void OSAL_TIMER_TICKSTOP(void)
{

}
#endif
