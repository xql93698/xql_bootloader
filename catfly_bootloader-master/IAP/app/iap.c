#include "iap.h"

#define FLASH_APP1_ADDR		0x08006000  	//第一个应用程序起始地址(存放在FLASH)
											//保留的空间为IAP使用

u16 iapbuf[1024] = {0}; //用于缓存数据的数组
u16 receiveDataCur = 0;	//当前iapbuffer中已经填充的数据长度,一次填充满了之后写入flash并清零
u32 addrCur = FLASH_APP1_ADDR;			//当前系统写入地址,每次写入之后地址增加2048

//开始下载
void iap_down_s(void)
{
	u16 i = 0;
	u16 temp = 0;
	u16 receiveCount;
	printf("begin,wait data download\r\n");
	receiveMode = 1;//串口进入下载接收数据模式
	while(1)
	{
		//循环接收数据,每次必须要发128个数据下来,如果没有128,说明这是最后一包数据
		//接收到一包数据之后,返回一个小数点,发送完成,系统编程完成之后返回一个iap_over
		if(serial_Buffer_Length & 0x8000)
		{
			receiveCount = (u8)(serial_Buffer_Length&0x00ff);
			if(receiveCount == 128)//满足一包,填充并查看是否有了1024字节,有了写入闪存
			{
				for(i = 0; i < receiveCount; i+=2)
				{
					//数据八位融合为16位
					temp = (((u16)serial_Buffer[i+1])<<8) + ((u16)serial_Buffer[i]);
					iapbuf[receiveDataCur] = temp;
					receiveDataCur++;//完成之后receiveDataCur++;
				}
				receiveExpectCount = 0;//清除期望接收模式
				serial_Buffer_Length = 0;//清除串口满标志
				printf(".");//每次接受一次数据打一个点 
				//此时需要检测receiveDataCur的值,要是放满了,就需要写入
				if(receiveDataCur == 1024)
				{
					//写入flash中
					STMFLASH_Write(addrCur,iapbuf,1024);
					//printf("\r\nwrite addr %x,length 1024\r\n",addrCur);
					addrCur += 2048;//地址+2048
					//写完之后receiveDataCur要清零等待下一次传输
					receiveDataCur = 0;
				}
				else //有可能最后一包有128个数据但是最终没有2048个数据,此时扩展一个指令用于完成最后一个的写入
				{
					
				}
				//还没放满,等待下一次数据过来
			}
			else	//不满足一包,说明数据传送这是最后一包,写入闪存
			{
				//没有一包也要传送到缓存中
				for(i = 0; i < receiveCount; i+=2)
				{
					//数据八位融合为16位
					temp = (((u16)serial_Buffer[i+1])<<8) + ((u16)serial_Buffer[i]);
					iapbuf[receiveDataCur] = temp;
					receiveDataCur++;//完成之后receiveDataCur++;
				}
				receiveExpectCount = 0;//清除期望接收模式
				serial_Buffer_Length = 0;//清除串口满标志
				printf(".");//每次接受一次数据打一个点 
				//之后就要将这数据写入到闪存中
				STMFLASH_Write(addrCur,iapbuf,receiveDataCur);//将最后的一些内容字节写进去.
				//printf("\r\nwrite addr %x,length %d\r\n",addrCur,receiveDataCur);
				//写完之后要把地址恢复到原来的位置
				addrCur = FLASH_APP1_ADDR;
				receiveDataCur = 0;
				//写完之后要退出下载循环并告诉上位机,已经下载完了
				printf("download over\r\n");
				//同时,也要退出下载循环模式
				receiveMode = 0;
				return;
			}
		}
	}
}
//最后一包有128个数据但是最终没有2048个数据
//收到这个指令检测receiveDataCur和addrCur的值,
//完成最后的写入
void iap_over_s(void)
{
	//这个时候,依然在串口下载模式
	if(receiveDataCur != 0)
	{
		STMFLASH_Write(addrCur,iapbuf,receiveDataCur);//将最后的一些内容字节写进去.
		//printf("write addr %x,length %d",addrCur,receiveDataCur);
		addrCur = FLASH_APP1_ADDR;
		receiveDataCur = 0;
		//同时,也要退出下载模式
		receiveMode = 0;
	}
	printf("ok\r\n");
}


typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
iapfun jump2app;

//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.0x20000000是sram的起始地址,也是程序的栈顶地址
	{ 
		//printf("ok\r\n");
		//Delay_Ms(10);
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
	else
	{
	//	printf("program in flash is error\r\n");
	}
}

//跳转到app区域运行
void iap_jump_app_s(void)
{
	iap_load_app(FLASH_APP1_ADDR);//跳转到app的复位向量地址
}



//设置app固化配置
void iap_set_flag_s(void)
{
	Test_Write(APP_CONFIG_ADDR,APP_CONFIG_SET_VALUE);
	printf("ok\r\n");
}

//清除app固化配置
void iap_clear_flag(void)
{
	Test_Write(APP_CONFIG_ADDR,APP_CONFIG_CLEAR_VALUE);
	printf("ok\r\n");
}
















