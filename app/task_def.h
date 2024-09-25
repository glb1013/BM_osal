/*****************************************************************************/
#ifndef APPLICATION_H
#define APPLICATION_H

#include "osal.h"
#include "osal_timer.h"
#include "osal_task.h"
#include "osal_memory.h"
#include "osal_msg.h"

/*
****************************************************************************
�������������ID����ʼ���������¼��������������¼���ͳһ�ڴ��ļ���������
****************************************************************************
*/
typedef struct
{
    osal_event_hdr_t  hdr;          //����ϵͳ�¼��ṹ
    unsigned char *data;            //����֡������
} general_msg_data_t;               //�Զ���ͨ����Ϣ��ʽ�ṹ��

//������
enum {
    TASK_PRIORITY_PRINTF = 1,
    TASK_PRIORITY_STATISTICS
};

//�����¼����壺ÿ��task���Զ�����е�task_event_type
typedef enum {
    PRINT_TASK_EVENT_STR = TASK_EVENT_DEF(0),          //��ӡ�ַ����¼�
} PRINTF_TASK_EVENT_TYPE_E;

//ͳ�������ϵͳ��Ϣ�¼�����
typedef enum {
    PRINTF_STATISTICS_MSG = 1U,               //��ӡͳ����Ϣ�¼�
} PRINTF_TASK_MSG_TYPE_E;

//ȫ�ֱ�������
//����ID����
extern uint8 print_task_id;
extern uint8 statistics_task_id;

//�����ʼ����������&�¼�����������
void print_task_init(uint8 task_id);
uint16 print_task_event_process(uint8 task_id, uint16 task_event);
void statistics_task_init(uint8 task_id);
uint16 statistics_task_event_process(uint8 task_id, uint16 task_event);

/*****************************************************************************/

void osal_main(void);

#endif
