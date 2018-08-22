#include "frame_phase.h"
#include <stdio.h>
#include "control.h"

void dateLinkSend(struct zxy_framer *framer,uart_frame_t *frame)       //���������ͺ���
{
usart_driver *usart = &__usart_driver;
uint8_t len = sizeof(uart_frame_t)-MAX_MTU_LEN+frame->payload_len;     //���͵����ݳ���
frame->seq = framer->seq;                                              // ֡�ṹ�����к�seq Ϊ 0
//printf("uart_frame_t len is %d  MAX_MTU_LEN is %d, payload_len is %d %d\r\n",sizeof(uart_frame_t),MAX_MTU_LEN,frame->payload_len,len);
usart->send(usart,(uint8_t *)frame,len);                               //����crcУ�飬��װ��֡��Ȼ��͸������ ���ͳ�ȥ֡�ṹ
framer->seq++;                                                          //���к�+1��������һ��֡�ṹ
}

void input(struct zxy_framer *phaser,uint8_t *buf,uint8_t len)
{
    zxy_control *control = &__control;                                //�����ĳ�ʼ״̬Ϊ ����mcu ����
    uart_frame_t *frame = (uart_frame_t *)buf;                       //ǿ������ת������uint8-t���͵�ָ��buf ת��Ϊuart_frame ���͵�frameָ��
    frame_ack_t *ack;
    frame->payload_len = len;
    switch(frame->type)
    {
        case ACK_FRAME:                                               //�����ack �Ļظ�
        ack=(frame_ack_t *)frame->buf;
        if(ack->status == SUCCESS)                                     //�������ɹ�����ACK
        {
            switch(ack->command)                                       //���ACK��������н���
            {
                case APP2BOOT:                                         //��MCU����������
                    //length veify
                    printf("\nMCU enter Bootloader aleady\n");         //��ӡ MCU�ѽ���Bootloader����
                    //control->change_status(control,)
                    control->change_status(control,HOST_DOWNLOAD_BIN,HOST_PREPARE_BIN,100); //�ı�����״̬��������״̬�� ����bin�ļ� ת��Ϊ׼��bin �ļ������趨״̬ת����ʱ 100
                break;

                case FILE_FRAME:                                       //��mcu�ɹ������� bin �ļ�
                     control->change_status(control,control->last_status,HOST_PREPARE_BIN,100); //����׼������һ�� bin �ļ�
                     printf("\nsend next file frame\n");
                break;

                case MOTEID_FRAME:                                     //��mcu�ɹ�����mote_id
                    printf("\nmote id write success\n");
                    control->change_status(control,control->last_status,HOST_FINISHED,100);     //�����������
                break;
            }

        }
        else
        {
        // command failed restrans

        }


        //ack frame callback
        break;

        case FILE_FRAME:
        //FILE_FRAME_callback
        printf("FILE");
        break;

        default:
        //unrecognise frame
        break;

    }

}

zxy_framer __framer={
0,
dateLinkSend,                             //send ����
input                                     //ack����Ľ�������
};

