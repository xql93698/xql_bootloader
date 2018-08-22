#include "usart_driver.h"
#include "serial_posix.h"
#include "serial.h"
#include "debug.h"
#include "frame_phase.h"
#include <stdio.h>

int16_t calculateCRC(unsigned char *_regs,unsigned char arraySize)  //在发送数据时加入crc校验
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

static int isCrcVailed(unsigned char *_regs,unsigned char arraySize)  //在接受数据时，取出后两位crc码，然后重新计算crc值，若两者相等，即通过crc校验
{
    int16_t crc_rec = (*(_regs+arraySize-1)<<8) + (*(_regs+arraySize-2));

    int16_t crc_calc = calculateCRC(_regs,arraySize-2);

    //printf("crc_rec is %x ,calc is %x \n",crc_rec,crc_calc);

    if(crc_rec == crc_calc)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

void send(struct usart_driver *framer,uint8_t *data,int len)
{
    int pos=0;

    if(len<0 || len>FRAME_BUFF_LEN)           //检验发送数据的长度是否合格
        return ;
    //
    int16_t crc16 = calculateCRC(data,len);  // 计算crc值
    uint8_t buf[len+sizeof(crc16)];          // 定义发送数据数组，并指定元素数目（考虑了crc）
    //printf("crc is %x \n",crc16);
    fflush(stdout);                         //冲掉缓冲区内的数据
    memcpy(buf,data,len);                   //把长度为len的数据装入要发送的数组中
    memcpy(buf+len,&crc16,sizeof(crc16));   //把两个字节的crc校验码装入buf的最后两个元素  数据部分
    len+=sizeof(crc16);                      //发送的长度（加入crc之后）
    //put SOH in first byte
    framer->buf[pos++] = SSOH;               //在帧的首部加入SOH，表示这是帧的开始

    for(int i=0; i<len; i++,pos++)
    {
        if ( *(buf+i) == SSOH || *(buf+i) == SEOT || *(buf+i) == SESC) // 判断发送的数据部分是否会出现控制字符，目的 透明传输
        {
            //put ESC
            framer->buf[pos++] = SESC;       //如果出现了控制字符，就在此前加入转义字符 ESC
        }
        // put data into buf
        framer->buf[pos] = *(buf+i);         //如果数据部分没有出现控制字符，就将数据部分加入帧结构中

    }
    //put CRC

    //put EOT
    framer->buf[pos++] = SEOT;                //遍历数据部分之后，加入EOT 帧结束字符

    //send to usart
    SerialWrite(framer->buf,pos);             // 将封装成帧的数据发送出去

}

void usartread(struct usart_driver *driver,uint8_t byte)
{
    zxy_framer *framer_driver =  &__framer;

    if(!driver->escflag && byte == SESC)      //遍历字节，若为转义字符
    {
        driver->escflag = 1;                  // 立flag
        //debug_print("ESC SET\n");
        return;
    }

    if(!driver->escflag && byte == SSOH)      //若为soh ,则为帧开始
    {
        driver->receiveLen=0;
        //debug_print("SOH SET\n");
        return;
    }

    if(!driver->escflag && byte == SEOT)       //若为EOT ,则为帧结束
    {
        driver->recSucess = 1;                  // 成功接收 1 帧
        driver->escflag =0;
        //calc CRC

        if(isCrcVailed(driver->rec_buf,driver->receiveLen))  // 校验 crc
        {
            //for(int i=0;i<driver->receiveLen;i++)
            //printf(" %2x ",*(driver->rec_buf+i));
            //printf("CRC  vaild\n");
            framer_driver->input(framer_driver,driver->rec_buf,driver->receiveLen-2); //若通过crc,则去执行input函数

        }
        else
        {
            for(int i=0;i<driver->receiveLen;i++)
                printf(" %x ",*(driver->rec_buf+i));
            //printf("CRC not vaild\n");
        }
        //debug_print("recSucess %d\n",driver->receiveLen);


            //fflush(stdin);
        return;
    }

    if(driver->escflag)                         //若在接收端检测到立flag
    {
        *(driver->rec_buf+driver->receiveLen++) = byte;  // 则把转义字符ESC加入到接收的帧中
        driver->escflag = 0;
        //printf("rec  esc data\n");
        return;
    }

    *(driver->rec_buf+driver->receiveLen++) = byte;   //若检测到没有立flag, 则把相应的正常字节加入到接收帧中
       // printf("rec  data\n");
    return;

}
    uint8_t buf[FRAME_BUFF_LEN];                 //需要发送的数据部分
    uint8_t rec_buf[FRAME_BUFF_LEN];             //接收到的数据部分

usart_driver __usart_driver=
{
    buf,                                          //发送的帧结构
    rec_buf,                                      //接收到的帧结构
    0,                                            //flag
    0,                                            //接收成功标志
    0,                                            //接收的帧长度
    send,                                         //发送的帧结构
    usartread                                     //接收的帧结构
};

