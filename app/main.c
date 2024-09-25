/**
 * @file main.c
 * @brief osal运行例程入口
 * @version 0.1
 * @date 2019-07-25
 * @author WatWu
 */

#include "task_def.h"

/**
 * @brief 程序入口
 * @param argc [传入的参数数量]
 * @param argv [传入的参数列表]
 * @return int [无]
 */
int main(int argc, char *argv[])
{
    argc = argc;
    argv = argv;

    //系统硬件、外设等初始化

    //禁止中断
    HAL_DISABLE_INTERRUPTS();

    //osal操作系统初始化
    osal_init_system();

    //添加任务
    osal_task_add(print_task_init, print_task_event_process, TASK_PRIORITY_PRINTF);
    osal_task_add(statistics_task_init, statistics_task_event_process, TASK_PRIORITY_STATISTICS);

    //添加的任务统一进行初始化
    osal_task_initAll();

    osal_mem_kick();

    //允许中断
    HAL_ENABLE_INTERRUPTS();

    //设置初始任务事件，上电就需要自动轮询的任务事件可在此添加

    //启动osal系统，不会再返回
    osal_task_run();
}
