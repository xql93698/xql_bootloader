#ifndef FRAME_DRIVER_H_INCLUDED
#define FRAME_DRIVER_H_INCLUDED

#define FRAME_BUFF_LEN 2048

//data link layer
#define SSOH  0x01
#define SEOT  0x05
#define SESC  0x1b
#include <stdint.h>
#include "debugSerial.h"
//frame

//#define ACK  0x06
//#define NACK 0x05



typedef struct usart_driver{
uint8_t *buf;
uint8_t *rec_buf;
uint8_t escflag;
uint8_t recSucess;
int     receiveLen;

void (*send)(struct usart_driver *driver,uint8_t *buf,int len);
void (*input)(struct usart_driver *driver,uint8_t byte);

}usart_driver;

extern usart_driver __usart_driver;
#endif // FRAME_DRIVER_H_INCLUDED
