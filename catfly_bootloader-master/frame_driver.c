#include "frame_driver.h"
#include "serial_posix.h"
#include "serial.h"
#include "debug.h"

int16_t calculateCRC(unsigned char *_regs,unsigned char arraySize)
{
  unsigned int temp, temp2, flag;
  unsigned char i,j;

  temp = 0xFFFF;
  for (i = 0; i < arraySize; i++)
  {
    temp = temp ^ *(_regs+i);
    for (j = 1; j <= 8; j++)
    {
      flag = temp & 0x0001;
      temp >>= 1;
      if (flag)
        temp ^= 0xA001;
    }
  }
  temp2 = temp >> 8;
  temp = (temp << 8) | temp2;
  temp &= 0xFFFF;
  return temp;
}

void send(struct zxy_framer *framer,uint8_t *data,int len)
{
    int pos=0;

    if(len<0 || len>FRAME_BUFF_LEN)
        return ;
    //
    int16_t crc16 = calculateCRC(data,len);
    uint8_t buf[len+sizeof(crc16)];
    printf("crc is %x len is %ld",crc16,sizeof(crc16));
    fflush(stdout);
    memcpy(buf,data,len);
    memcpy(buf+len,&crc16,sizeof(crc16));
    len+=sizeof(crc16);
    //put SOH in first byte
    framer->buf[pos++] = SSOH;

    for(int i=0; i<len; i++,pos++)
    {
        if ( *(buf+i) == SSOH || *(buf+i) == SEOT || *(buf+i) == SESC)
        {
            //put ESC
            framer->buf[pos++] = SESC;
        }
        // put data into buf
        framer->buf[pos] = *(buf+i);

    }
    //put CRC

    //put EOT
    framer->buf[pos++] = SEOT;

    //send to usart
    SerialWrite(framer->buf,pos);

}

void usartread(struct zxy_framer *framer,uint8_t byte)
{
    if(!framer->escflag && byte == SESC)
    {
        framer->escflag = 1;
        debug_print("ESC SET\n");
        return;
    }

    if(!framer->escflag && byte == SSOH)
    {
        framer->receiveLen=0;
        debug_print("SOH SET\n");
        return;
    }

    if(!framer->escflag && byte == SEOT)
    {
        framer->recSucess = 1;
        framer->escflag =0;
        debug_print("recSucess %d\n",framer->receiveLen);

        for(int i=0;i<framer->receiveLen;i++)
            debug_print("%c",*(framer->rec_buf+i));

        fflush(stdout);
        return;
    }

    if(framer->escflag)
    {
        *(framer->rec_buf+framer->receiveLen++) = byte;
        framer->escflag = 0;
        debug_print("rec  esc data\n");
        return;
    }

    *(framer->rec_buf+framer->receiveLen++) = byte;
    debug_print("rec  data\n");
    return;

}
    uint8_t buf[FRAME_BUFF_LEN];
    uint8_t rec_buf[FRAME_BUFF_LEN];

zxy_framer __zxy_framer=
{
    buf,
    rec_buf,
    0,
    0,
    0,
    send,
    usartread
};

