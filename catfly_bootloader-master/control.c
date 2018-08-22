#include "control.h"
#include <sys/time.h>

long get_sys_time_ms(void)                                  //计时函数
{
  struct timeval tv;                                       //Linux系统中定义的结构体，计时器的作用
  gettimeofday(&tv, 0);                                    //获取当前时间距Epoch的时间
   // printf("msecond:%ld\n",tv.tv_sec*1000+tv.tv_usec/1000);  //秒
    return tv.tv_sec*1000+tv.tv_usec/1000;               //返回时间 tv_sec  间隔的整秒数   tv_usec 附加分数为微秒
}

void set_timer(struct zxy_control *control,long time)       //设置定时时间（当前时间+time）
{
    control->timer = get_sys_time_ms()+time;

}

int check_timer(struct zxy_control *control)              //是否超时
 {
    if(get_sys_time_ms() > control->timer)
    {
        control->timeout_cout++;                            //超时次数+1
        return TIMER_TIMEOUT;
    }
    else
        return 0;
 }

 void change_status(struct zxy_control *control,int current,int next,long time)     //主机状态改变函数
 {
        control->status = next;                                                     //即将转变的状态
        control->set_timer(control,time);                                           //设置时间定时
        control->last_status = current;                                             //当前状态
        control->last_timeout = time;                                               //定时的时间

 }

int is_max_time_out(struct zxy_control *control,int maxTimeOut)                    //最大重传次数
{
    if(control->timeout_cout>maxTimeOut-1)                                          //若超过了最大重传次数
    {

        switch(control->last_status)                                                //判断此时主机状态
        {

        case HOST_REQUEST_REBOOT:                                                  //若主机处于请求 mcu重启 状态
            printf("enter bootloader failed ,please reboot node\n");               //打印相关内容：进入 bootloader 失败
            break;

        case HOST_DOWNLOAD_BIN:                                                    //若主机处于 下载bin 状态
            printf("serial commuincation failed ,plase try again\n");               //打印相关内容
            break;

        case HOST_SEND_MOTE_ID:
            printf("may be boot to app failed , check app and reboot node\n");
            break;

        }

     return 1;
    }
    else
        return 0;

}

void resume_last_mission(struct zxy_control *control)                                  //重传 重复之前的任务
{
    change_status(control,control->last_status,control->last_status,control->last_timeout);     //一直重复之前的任务
}

zxy_control __control=
{
0x01,                                     //默认主机初始状态为 请求 mcu 重启
0,
0,
0,
0,
set_timer,
check_timer,
change_status,
is_max_time_out,
resume_last_mission
};

