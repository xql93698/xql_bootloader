#ifndef FRAME_DRIVER_H_INCLUDED
#define FRAME_DRIVER_H_INCLUDED

#define FRAME_BUFF_LEN 2048

//data link layer
#define SSOH 'A'
#define SEOT 'B'
#define SESC 'C'
#include <stdint.h>
//frame

//#define ACK  0x06
//#define NACK 0x05



typedef struct zxy_framer{
uint8_t *buf;
uint8_t *rec_buf;
uint8_t escflag;
uint8_t recSucess;
int     receiveLen;

void (*send)(struct zxy_framer *framer,uint8_t *buf,int len);
void (*read)(struct zxy_framer *framer,uint8_t byte);

}zxy_framer;

extern zxy_framer __zxy_framer;
#endif // FRAME_DRIVER_H_INCLUDED
