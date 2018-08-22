#ifndef FRAME_PHASE_H_INCLUDED
#define FRAME_PHASE_H_INCLUDED

#include "usart_driver.h"

#define MAX_MTU_LEN     255
#define MAX_FILE_LEN    128


#define ACK_FRAME       (0x02)          //若主机成功接收到了ACK，则对ACK的命令进行解析
#define FILE_FRAME      (0x03)           //ACK的具体命令如左所示
#define MOTEID_FRAME    (0x04)
#define APP2BOOT        (0x4c)


#define SUCCESS     0x80                  //是否成功接收到了ACK（status）
#define FAILED      0x00
//01 02 02 07 4c 80 05
//app frame
typedef struct {
  uint8_t command;                      /**< Sequence number */
  uint8_t status;
}frame_ack_t;

typedef struct {
  uint16_t  Sequence;
  uint16_t  length;                       /**< Sequence number */    //bin 文件结构体
  uint8_t   buf[MAX_FILE_LEN];
}frame_file_t;

typedef struct {
  uint16_t moteid;                      /**< Sequence number */
}frame_moteid_t;
//play load
typedef struct {
  uint8_t type;
  uint8_t seq;                   /**< Frame type field  */            //向 mcu 发送相关数据的结构体
  uint8_t payload_len;
  uint8_t buf[MAX_MTU_LEN];                //payload
}uart_frame_t;



typedef struct zxy_framer{
uint8_t seq;                         //应该是类似于帧结构的序列号，发送多帧或者多条数据
void (*send)(struct zxy_framer *phaser,uart_frame_t *frame);      //向 mcu 发送相关数据的函数
void (*input)(struct zxy_framer *phaser,uint8_t *buf,uint8_t len);            //datalink   ack命令的解析函数

}zxy_framer;

extern zxy_framer __framer;
#endif // FRAME_PHASE_H_INCLUDED
