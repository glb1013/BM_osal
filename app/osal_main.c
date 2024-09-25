/**
 * @file osal_main.c
 * @brief osal����ϵͳ��������������������ڴ��ļ������
 * @version 0.1
 * @date 2019-07-25
 * @author WatWu
 */

#include "task_event.h"

void osal_main(void)
{
    //ϵͳӲ��������ȳ�ʼ��

    //��ֹ�ж�
    HAL_DISABLE_INTERRUPTS();

    //osal����ϵͳ��ʼ��
    osal_init_system();

    //�������
    osal_add_Task(print_task_init, print_task_event_process, 1);
    osal_add_Task(statistics_task_init, statistics_task_event_process, 2);

    //��ӵ�����ͳһ���г�ʼ��
    osal_Task_init();

    osal_mem_kick();

    //�����ж�
    HAL_ENABLE_INTERRUPTS();

    //���ó�ʼ�����¼����ϵ����Ҫ�Զ���ѯ�������¼����ڴ����

    //����osalϵͳ�������ٷ���
    osal_start_system();
}
