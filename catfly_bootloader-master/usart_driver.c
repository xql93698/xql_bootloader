#include "usart_driver.h"
#include "serial_posix.h"
#include "serial.h"
#include "debug.h"
#include "frame_phase.h"
#include <stdio.h>

int16_t calculateCRC(unsigned char *_regs,unsigned char arraySize)  //�ڷ�������ʱ����crcУ��
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

static int isCrcVailed(unsigned char *_regs,unsigned char arraySize)  //�ڽ�������ʱ��ȡ������λcrc�룬Ȼ�����¼���crcֵ����������ȣ���ͨ��crcУ��
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

    if(len<0 || len>FRAME_BUFF_LEN)           //���鷢�����ݵĳ����Ƿ�ϸ�
        return ;
    //
    int16_t crc16 = calculateCRC(data,len);  // ����crcֵ
    uint8_t buf[len+sizeof(crc16)];          // ���巢���������飬��ָ��Ԫ����Ŀ��������crc��
    //printf("crc is %x \n",crc16);
    fflush(stdout);                         //����������ڵ�����
    memcpy(buf,data,len);                   //�ѳ���Ϊlen������װ��Ҫ���͵�������
    memcpy(buf+len,&crc16,sizeof(crc16));   //�������ֽڵ�crcУ����װ��buf���������Ԫ��  ���ݲ���
    len+=sizeof(crc16);                      //���͵ĳ��ȣ�����crc֮��
    //put SOH in first byte
    framer->buf[pos++] = SSOH;               //��֡���ײ�����SOH����ʾ����֡�Ŀ�ʼ

    for(int i=0; i<len; i++,pos++)
    {
        if ( *(buf+i) == SSOH || *(buf+i) == SEOT || *(buf+i) == SESC) // �жϷ��͵����ݲ����Ƿ����ֿ����ַ���Ŀ�� ͸������
        {
            //put ESC
            framer->buf[pos++] = SESC;       //��������˿����ַ������ڴ�ǰ����ת���ַ� ESC
        }
        // put data into buf
        framer->buf[pos] = *(buf+i);         //������ݲ���û�г��ֿ����ַ����ͽ����ݲ��ּ���֡�ṹ��

    }
    //put CRC

    //put EOT
    framer->buf[pos++] = SEOT;                //�������ݲ���֮�󣬼���EOT ֡�����ַ�

    //send to usart
    SerialWrite(framer->buf,pos);             // ����װ��֡�����ݷ��ͳ�ȥ

}

void usartread(struct usart_driver *driver,uint8_t byte)
{
    zxy_framer *framer_driver =  &__framer;

    if(!driver->escflag && byte == SESC)      //�����ֽڣ���Ϊת���ַ�
    {
        driver->escflag = 1;                  // ��flag
        //debug_print("ESC SET\n");
        return;
    }

    if(!driver->escflag && byte == SSOH)      //��Ϊsoh ,��Ϊ֡��ʼ
    {
        driver->receiveLen=0;
        //debug_print("SOH SET\n");
        return;
    }

    if(!driver->escflag && byte == SEOT)       //��ΪEOT ,��Ϊ֡����
    {
        driver->recSucess = 1;                  // �ɹ����� 1 ֡
        driver->escflag =0;
        //calc CRC

        if(isCrcVailed(driver->rec_buf,driver->receiveLen))  // У�� crc
        {
            //for(int i=0;i<driver->receiveLen;i++)
            //printf(" %2x ",*(driver->rec_buf+i));
            //printf("CRC  vaild\n");
            framer_driver->input(framer_driver,driver->rec_buf,driver->receiveLen-2); //��ͨ��crc,��ȥִ��input����

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

    if(driver->escflag)                         //���ڽ��ն˼�⵽��flag
    {
        *(driver->rec_buf+driver->receiveLen++) = byte;  // ���ת���ַ�ESC���뵽���յ�֡��
        driver->escflag = 0;
        //printf("rec  esc data\n");
        return;
    }

    *(driver->rec_buf+driver->receiveLen++) = byte;   //����⵽û����flag, �����Ӧ�������ֽڼ��뵽����֡��
       // printf("rec  data\n");
    return;

}
    uint8_t buf[FRAME_BUFF_LEN];                 //��Ҫ���͵����ݲ���
    uint8_t rec_buf[FRAME_BUFF_LEN];             //���յ������ݲ���

usart_driver __usart_driver=
{
    buf,                                          //���͵�֡�ṹ
    rec_buf,                                      //���յ���֡�ṹ
    0,                                            //flag
    0,                                            //���ճɹ���־
    0,                                            //���յ�֡����
    send,                                         //���͵�֡�ṹ
    usartread                                     //���յ�֡�ṹ
};

