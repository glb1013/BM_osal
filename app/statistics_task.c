/**
 * @file statistics_task.c
 * @brief ͳ������
 * @version 0.1
 * @date 2019-07-25
 * @author WatWu
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "task_event.h"

uint8 statistics_task_id;                    //��¼ͳ�����������ID

/**
 * @brief �����ʼ��
 * @param task_id [��ʼ��ʱ�������ǰ���������ID���������ÿһ������]
 */
void statistics_task_init(uint8 task_id)
{
    statistics_task_id = task_id;
}

/**
 * @brief ��ǰ������¼��ص�������
 * @param task_id       [����ID]
 * @param task_event    [�յ��ı������¼�]
 * @return uint16       [δ������¼�]
 */
uint16 statistics_task_event_process(uint8 task_id, uint16 task_event)
{
    if(task_event & SYS_EVENT_MSG)       //�ж��Ƿ�Ϊϵͳ��Ϣ�¼�
    {
        osal_sys_msg_t *msg_pkt;
        msg_pkt = (osal_sys_msg_t *)osal_msg_receive(task_id);      //����Ϣ���л�ȡһ����Ϣ

        while(msg_pkt)
        {
            switch(msg_pkt->hdr.event)      //�жϸ���Ϣ�¼�����
            {
                case PRINTF_STATISTICS:
                {
                    int count = *(int*)(((general_msg_data_t*)msg_pkt)->data);
                    printf("Statistics task receive print task printf count : %d\n", count);
                    break;
                }

                default:
                    break;
            }

            osal_msg_deallocate((uint8 *)msg_pkt);                  //�ͷ���Ϣ�ڴ�
            msg_pkt = (osal_sys_msg_t *)osal_msg_receive(task_id);  //��ȡ��һ����Ϣ
        }

        // return unprocessed events
        return (task_event ^ SYS_EVENT_MSG);
    }

    return 0;
}
