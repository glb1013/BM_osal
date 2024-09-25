#ifndef APPLICATION_H
#define APPLICATION_H

#include "osal.h"
#include "osal_timer.h"
#include "osal_event.h"
#include "osal_memory.h"
#include "osal_msg.h"

//ȫ�ֱ�������
/*****************************************************************************/

typedef struct
{
    osal_event_hdr_t  hdr;          //����ϵͳ�¼��ṹ
    unsigned char *data;            //����֡������
} general_msg_data_t;               //�Զ���ͨ����Ϣ��ʽ�ṹ��

/*****************************************************************************/

//�������������ID����ʼ���������¼��������������¼���ͳһ�ڴ��ļ���������
/*****************************************************************************/

//����ID����
extern uint8 print_task_id;
extern uint8 statistics_task_id;

//�����ʼ����������
void print_task_init(uint8 task_id);
void statistics_task_init(uint8 task_id);

//�����¼�����������
uint16 print_task_event_process(uint8 task_id, uint16 task_event);
uint16 statistics_task_event_process(uint8 task_id, uint16 task_event);

//�����¼�����
//ϵͳ��Ϣ�¼���Ĭ�ϱ���Ϊosalϵͳʹ�ã������շ���Ϣ
#define SYS_EVENT_MSG               0x8000

//��ӡ����������¼�����
#define    PRINTF_STR               0X0001          //��ӡ�ַ����¼�

//ͳ�������ϵͳ��Ϣ�¼�����
#define PRINTF_STATISTICS           1               //��ӡͳ����Ϣ�¼�

/*****************************************************************************/

void osal_main(void);

#endif
