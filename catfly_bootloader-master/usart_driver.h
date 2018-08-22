#ifndef FRAME_DRIVER_H_INCLUDED
#define FRAME_DRIVER_H_INCLUDED

#define FRAME_BUFF_LEN 2048

//data link layer
#define SSOH  0x01     //֡��ʼ����ʮ�����Ʊ���Ϊ 01
#define SEOT  0x05     // ֡������
#define SESC  0x1b      // Ϊ��֤͸�����䣬�������ݲ��ֵĿ����ַ�ǰ�����ת���ַ�
#include <stdint.h>
//frame

//#define ACK  0x06
//#define NACK 0x05



typedef struct usart_driver{
uint8_t *buf;            //Ҫ���͵�֡�ṹ
uint8_t *rec_buf;        //���յ���֡�ṹ
uint8_t escflag;         // esc��־λ����ʾ�Ƿ����ת���ַ�
uint8_t recSucess;        //�Ƿ���ȷ���յ�֡�����յ�EOTʱ����1
int     receiveLen;      // ���յ���֡����

void (*send)(struct usart_driver *driver,uint8_t *buf,int len);     // *buf Ҫ���͵����ݲ���
void (*read)(struct usart_driver *driver,uint8_t byte);              //������ȡ������Ҫ����input����(ackָ��Ľ�������)

}usart_driver;

extern usart_driver __usart_driver;
#endif // FRAME_DRIVER_H_INCLUDED
