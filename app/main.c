/**
 * @file main.c
 * @brief osal�����������
 * @version 0.1
 * @date 2019-07-25
 * @author WatWu
 */

#include "task_def.h"

/**
 * @brief �������
 * @param argc [����Ĳ�������]
 * @param argv [����Ĳ����б�]
 * @return int [��]
 */
int main(int argc, char *argv[])
{
    argc = argc;
    argv = argv;

    //ϵͳӲ��������ȳ�ʼ��

    //��ֹ�ж�
    HAL_DISABLE_INTERRUPTS();

    //osal����ϵͳ��ʼ��
    osal_init_system();

    //�������
    osal_task_add(print_task_init, print_task_event_process, TASK_PRIORITY_PRINTF);
    osal_task_add(statistics_task_init, statistics_task_event_process, TASK_PRIORITY_STATISTICS);

    //��ӵ�����ͳһ���г�ʼ��
    osal_task_initAll();

    osal_mem_kick();

    //�����ж�
    HAL_ENABLE_INTERRUPTS();

    //���ó�ʼ�����¼����ϵ����Ҫ�Զ���ѯ�������¼����ڴ����

    //����osalϵͳ�������ٷ���
    osal_task_run();
}
