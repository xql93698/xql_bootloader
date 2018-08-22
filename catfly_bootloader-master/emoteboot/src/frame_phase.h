#ifndef FRAME_PHASE_H_INCLUDED
#define FRAME_PHASE_H_INCLUDED

#include "usart_driver.h"

#define MAX_MTU_LEN     255
#define MAX_FILE_LEN    128


#define ACK_FRAME       (0x02)
#define FILE_FRAME      (0x03)
#define MOTEID_FRAME    (0x04)
#define APP2BOOT        (0x4c)


#define ACK_SUCCESS     0x80
#define ACK_FAILED      0x00
//01 02 02 07 4c 80 05
//app frame
typedef struct {
  uint8_t command;                      /**< Sequence number */
  uint8_t status;
}frame_ack_t;

typedef struct {
  uint16_t  Sequence;
  uint16_t  length;                      /**< Sequence number */
  uint8_t   buf[MAX_FILE_LEN];
}frame_file_t;

typedef struct {
  uint16_t moteid;                      /**< Sequence number */
}frame_moteid_t;
//play load
typedef struct {
  uint8_t type;
  uint8_t seq;                   /**< Frame type field  */
  uint8_t payload_len;
  uint8_t buf[MAX_MTU_LEN];
}uart_frame_t;



typedef struct zxy_framer{
uint8_t seq;
uint8_t crcNotVailed;
uint8_t crcVailed;
void (*send)(struct zxy_framer *phaser,uart_frame_t *frame);
void (*input)(struct zxy_framer *phaser,uint8_t *buf,uint8_t len);

}zxy_framer;
extern uint8_t  ack;
extern zxy_framer __framer;
extern uint8_t flash_ok;
#endif // FRAME_PHASE_H_INCLUDED
