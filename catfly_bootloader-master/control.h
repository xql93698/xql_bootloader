#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#define TIMER_TIMEOUT 1

#include <stdio.h>
#include <stdint.h>
                                                              //主机的几种状态
#define HOST_REQUEST_REBOOT        0x01                       //主机请求 mcu 重启
#define HOST_PREPARE_BIN           0x04                       //主机准备 bin文件
#define HOST_DOWNLOAD_BIN          0x02                       //下载 bin 文件
#define HOST_SEND_MOTE_ID          0x03                       //发送mote_id

#define HOST_WAIT_ACK              0x05


#define HOST_FINISHED              0x80
#define HOST_ERROR                 0xFF


#define HOST_MAX_RETRY              0x03
#define HOST_ERASE_TIMEOUT          1200
#define HOST_COMMUNI_TIMEOUT         500
typedef struct zxy_control
{
    uint8_t  status;                                       //主机状态
    long     timer;                                        //设定定时器的定时时间
    uint8_t  last_status;                                  //上一次主机状态
    long     last_timeout;                                 //上一次设置的定时
    uint8_t   timeout_cout;                                //重传次数
    void (*set_timer)(struct zxy_control *control,long time);//设置定时时间（当前时间+time）
    int  (*check_timer)(struct zxy_control *control);         //等待ack是否超时
    void (*change_status)(struct zxy_control *control,int current,int next,long time);//主机状态改变函数
    int  (*is_max_time_out)(struct zxy_control *control,int maxTimeOut); //最大重传次数
    void (*resume_last_mission)(struct zxy_control *control); //恢复上次任务（重传）
}zxy_control;

extern zxy_control __control;

#endif // CONTROL_H_INCLUDED
