#ifndef FRAME_DRIVER_H_INCLUDED
#define FRAME_DRIVER_H_INCLUDED

#define FRAME_BUFF_LEN 2048

//data link layer
#define SSOH  0x01     //帧开始符，十六进制编码为 01
#define SEOT  0x05     // 帧结束符
#define SESC  0x1b      // 为保证透明传输，而在数据部分的控制字符前加入的转义字符
#include <stdint.h>
//frame

//#define ACK  0x06
//#define NACK 0x05



typedef struct usart_driver{
uint8_t *buf;            //要发送的帧结构
uint8_t *rec_buf;        //接收到的帧结构
uint8_t escflag;         // esc标志位，表示是否出现转义字符
uint8_t recSucess;        //是否正确接收到帧，当收到EOT时，置1
int     receiveLen;      // 接收到的帧长度

void (*send)(struct usart_driver *driver,uint8_t *buf,int len);     // *buf 要发送的数据部分
void (*read)(struct usart_driver *driver,uint8_t byte);              //主机读取函数，要调用input函数(ack指令的解析函数)

}usart_driver;

extern usart_driver __usart_driver;
#endif // FRAME_DRIVER_H_INCLUDED
