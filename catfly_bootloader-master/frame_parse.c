#include "frame_phase.h"
#include <stdio.h>
#include "control.h"

void dateLinkSend(struct zxy_framer *framer,uart_frame_t *frame)       //数据链发送函数
{
usart_driver *usart = &__usart_driver;
uint8_t len = sizeof(uart_frame_t)-MAX_MTU_LEN+frame->payload_len;     //发送的数据长度
frame->seq = framer->seq;                                              // 帧结构的序列号seq 为 0
//printf("uart_frame_t len is %d  MAX_MTU_LEN is %d, payload_len is %d %d\r\n",sizeof(uart_frame_t),MAX_MTU_LEN,frame->payload_len,len);
usart->send(usart,(uint8_t *)frame,len);                               //加上crc校验，封装成帧，然后透明传输 发送出去帧结构
framer->seq++;                                                          //序列号+1，发送下一个帧结构
}

void input(struct zxy_framer *phaser,uint8_t *buf,uint8_t len)
{
    zxy_control *control = &__control;                                //主机的初始状态为 请求mcu 重启
    uart_frame_t *frame = (uart_frame_t *)buf;                       //强制类型转换，将uint8-t类型的指针buf 转换为uart_frame 类型的frame指针
    frame_ack_t *ack;
    frame->payload_len = len;
    switch(frame->type)
    {
        case ACK_FRAME:                                               //如果是ack 的回复
        ack=(frame_ack_t *)frame->buf;
        if(ack->status == SUCCESS)                                     //若主机成功接收ACK
        {
            switch(ack->command)                                       //则对ACK的命令进行解析
            {
                case APP2BOOT:                                         //若MCU进行了重启
                    //length veify
                    printf("\nMCU enter Bootloader aleady\n");         //打印 MCU已进入Bootloader程序
                    //control->change_status(control,)
                    control->change_status(control,HOST_DOWNLOAD_BIN,HOST_PREPARE_BIN,100); //改变主机状态，将主机状态从 下载bin文件 转换为准备bin 文件，并设定状态转换延时 100
                break;

                case FILE_FRAME:                                       //若mcu成功接收了 bin 文件
                     control->change_status(control,control->last_status,HOST_PREPARE_BIN,100); //主机准备发下一个 bin 文件
                     printf("\nsend next file frame\n");
                break;

                case MOTEID_FRAME:                                     //若mcu成功接收mote_id
                    printf("\nmote id write success\n");
                    control->change_status(control,control->last_status,HOST_FINISHED,100);     //主机完成任务
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
dateLinkSend,                             //send 函数
input                                     //ack命令的解析函数
};

