/*
   catflyFlash - Open Source ST STM32 flash program for *nix
   Copyright (C) 2018 Zhang XiaoYuan  <569hmq@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include "serial_posix.h"
#include "serial.h"
#include "debug.h"
#include "control.h"
#include "usart_driver.h"
#include "frame_phase.h"
#include "file.h"
#define APP2BOOT_TIMEOUT   10

usart_driver *usart = &__usart_driver;
zxy_framer *datalink = &__framer;
file_ops *fops =&__file_ops;
  char c;
/* uart interrpt function*/
void signal_handler_IO (int status)
{

}



int main(int argc, char *argv[])
{
    char *device, *imageFile;
    uint16_t moteid = 0;
    /*Parameters*/
    zxy_control *control = &__control;
    uart_frame_t        payload;     //用来请求mcu重启和发送moteid的结构体
    uart_frame_t        file_payload; //用于准备bin 文件并将其发送的结构体
    //frame_app2boot_t    frame_app2boot;
    frame_file_t        frame_file;      //file->fops->frame_file->file_payload  大致这样一个顺序（Bin 文件的历程）
    frame_moteid_t      frame_moteid;  //用于发送moteid的结构体

    if( argc < 4)
    {
        printf("Please input image file name and serial device name: \n");
        printf("FORMAT: emoteflash imageFile moteid deviceName \n");
        return 0;
    }
    else
    {
        imageFile = argv[1];           //bin文件名
        moteid = atoi(argv[2]);       //将字符串中的数字字符转为整型数字  即moteid
        device = argv[3];             // 串口
    }

    /*Print version information*/
    printf("CatFly stm32 flash progrmming version 1.0.0, COPYRIGHT ZXY 2018.\n");
    /*Init serial port*/
    if(SerialInt(device)<0)           //串口初始化
    {
        printf("serial port init failed...\n");
        return 0;
    }
     printf("Serial open success\n");

    fops->open(fops,imageFile);       //打开文件名为 imageFile的 bin 文件，最终将数据传送到数组指针 buf 中

    while(1)
    {
        switch(control->status)       // 主机状态判断
        {
        case HOST_REQUEST_REBOOT:     //主机状态为 请求 MCU 重启

            //send reboot command
            payload.payload_len=0;
            payload.type=APP2BOOT;    //要发送的数据带有的请求信息为 请求 mcu 重启
            datalink->send(datalink,&payload);  //发送数据
            //set timer
            //printf("requset downding\n");
            control->change_status(control,HOST_REQUEST_REBOOT,HOST_WAIT_ACK,HOST_ERASE_TIMEOUT);//主机状态改变函数，当前状态 请求mcu重启，下一状态 等待ack,设置时间定时(擦除时间)，停止等待ack

        break;

        case HOST_PREPARE_BIN:        //主机准备bin文件

            file_payload.payload_len=fops->create(fops,&frame_file);      //读取文件长度
            if(frame_file.length<=MAX_FILE_LEN && frame_file.length >0){   //判断文件长度（是否存在 bin 文件）
                // printf("prepare downding\n");
                file_payload.type = FILE_FRAME;
                memcpy(file_payload.buf,&frame_file,file_payload.payload_len);//将文件复制到file_payload的buf数组中
                control->change_status(control,HOST_PREPARE_BIN,HOST_DOWNLOAD_BIN,0); //切换主机状态，从准备 bin 文件到向mcu下在bin 文件
            }

            if(file_payload.payload_len==0)                               //若不存在 bin 文件，即不需要向mcu下载bin 文件，就直接发送moteid
            {
                //printf("bin file transmit finished \n");
                control->change_status(control,HOST_PREPARE_BIN,HOST_SEND_MOTE_ID,0); //将主机状态转换为 发送 mote_id
                break;
            }

            break;


        case HOST_DOWNLOAD_BIN:                    //主机向 MCU 下载bin 文件
           // printf("start downding\n");
            datalink->send(datalink,&file_payload);  //向mcu发送准备好的 bin 文件
            control->change_status(control,HOST_DOWNLOAD_BIN,HOST_WAIT_ACK,HOST_COMMUNI_TIMEOUT); //改变主机状态，由下载 bin文件到等待ack,设置定时（HOST_COMMUNI_TIMEOUT）,停止等待ack
        break;


        case HOST_SEND_MOTE_ID:                    //主机向mcu发送 mote_id
           // printf("send mote id \n");
            payload.type = MOTEID_FRAME;
            payload.payload_len = sizeof(frame_moteid);
            frame_moteid.moteid = moteid;         //将 moteid装入结构体中
            memcpy(payload.buf,&frame_moteid,payload.payload_len); //将mote_id复制到payload的数组buf中
            datalink->send(datalink,&payload);       //将mote_id发送出去（送至mcu）

            control->change_status(control,HOST_SEND_MOTE_ID,HOST_WAIT_ACK,HOST_COMMUNI_TIMEOUT);  //改变主机状态，由发送mote_id 到等待ack ,并设置定时（HOST_COMMUNI_TIMEOUT）

        break;


        case HOST_FINISHED:                       //主机状态：完成
            //printf("mote flash succeess \n");
            SerialClose();                        //关闭串口
            fops->close(fops);                    //关闭 bin文件
            return 0;
        break;

        case HOST_WAIT_ACK:                       //主机状态：等待ack
            //good performance
            if(SerialGetChar(&c))                   //正常接收到了 ack
                usart->read(usart,c);               //对ack的内容进行解读
            //max time out
            if(control->is_max_time_out(control,APP2BOOT_TIMEOUT))      //是否超过设定的最大重传次数
            {
                SerialClose();                                          //若超过了最大重传次数，关闭串口，退出 bin 文件的发送，不一直重传
                fops->close(fops);
                return 0;
            }
            //if time out change staus to last
            if(control->check_timer(control))            //是否等待ack超时
            {
                control->resume_last_mission(control);   //若超时，重复之前的任务
                break;
            }

        break;

        }
       // serial reciver process


    }





    return 1;
}



