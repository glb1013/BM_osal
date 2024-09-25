/**
 * @file print_task.c
 * @brief ��ӡ����
 * @version 0.1
 * @date 2019-07-25
 * @author WatWu
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "task_def.h"

uint8 print_task_id;                    //��¼��ӡ���������ID

/**
 * @brief �����ʼ��
 * @param task_id [��ʼ��ʱ�������ǰ���������ID���������ÿһ������]
 */
void print_task_init(uint8 task_id)
{
    print_task_id = task_id;

    //����һ��ѭ����ʱ����ÿ�����ӡ������PRINT_TASK_EVENT_STR�¼�
    osal_start_reload_timer(print_task_id, PRINT_TASK_EVENT_STR, 1000 / TICK_PERIOD_MS);
}

/**
 * @brief ��ǰ������¼��ص�������
 * @param task_id       [����ID]
 * @param task_event    [�յ��ı������¼�]
 * @return uint16       [δ������¼�]
 */
uint16 print_task_event_process(uint8 task_id, uint16 task_event)
{
    if(task_event & SYS_EVENT_MSG)       //�ж��Ƿ�Ϊϵͳ��Ϣ�¼�
    {
        osal_sys_msg_t *msg_pkt;
        msg_pkt = (osal_sys_msg_t *)osal_msg_receive(task_id);      //����Ϣ���л�ȡһ����Ϣ

        while(msg_pkt)
        {
            switch(msg_pkt->hdr.event)      //�жϸ���Ϣ�¼�����
            {
                default:
                    break;
            }

            osal_msg_deallocate((uint8 *)msg_pkt);                  //�ͷ���Ϣ�ڴ�
            msg_pkt = (osal_sys_msg_t *)osal_msg_receive(task_id);  //��ȡ��һ����Ϣ
        }

        // return unprocessed events
        return (task_event ^ SYS_EVENT_MSG);
    }

    if(task_event & PRINT_TASK_EVENT_STR)
    {
        static int print_count = 0;
        printf("Print task printing, total memory : %d byte, used memory : %d byte !\n", MAXMEMHEAP, osal_heap_mem_used());

        print_count++;
        if(print_count % 5 == 0 && print_count != 0)
        {
            //��ͳ����������Ϣ
            general_msg_data_t *msg;
            msg = (general_msg_data_t*)osal_msg_allocate(sizeof(general_msg_data_t) + sizeof(int));
            if(msg != NULL)
            {
                //��Ϣ�ṹ���data����ָ��ƫ�������뵽���ڴ�����ݶ�
                //msg->data = (unsigned char*)( msg + sizeof(osal_event_hdr_t) );
                msg->data = (unsigned char*)(msg + 1);

                msg->hdr.event = PRINTF_STATISTICS_MSG;
                msg->hdr.status = 0;
                *((int*)msg->data) = print_count;

                osal_msg_send(statistics_task_id, (uint8*)msg);
            }
        }

        return task_event ^ PRINT_TASK_EVENT_STR; //���������Ҫ����¼�λ
    }

    return 0;
}
