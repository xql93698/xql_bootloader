/**
  ******************************************************************************
  * @file    emote-bootloader/src/main.c 
  * @author  Ling Wang
  * @version V1.0.0
  * @date    3/31/2015
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"
#define ApplicationAddress 0x08006000  //app代码的起始地址，准确来说，为栈顶地址
typedef  void (*pFunction)(void);

/* Execute application */
void Jump_to_app()
{
	pFunction Jump_To_Application;
  uint32_t JumpAddress;
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)  //检查栈顶地址是否合法,因为堆栈地址要落在ram中才可以，而RAM的起始地址为0x20000000，故作此检验
			{
				//SerialPutChar(0x16);
			  JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4); //第二个字为程序起始地址（复位地址）

				/* Jump to user application */
				Jump_To_Application = (pFunction) JumpAddress;
				/* Initialize user application's Stack Pointer */  
				__set_MSP(*(__IO uint32_t*) ApplicationAddress);//初始化应用程序堆栈指针(第一个字用来存栈顶地址）
				Jump_To_Application();//转挑到APP，执行复位中断程序
			}
}



#include "debugSerial.h"
#include "frame_phase.h"
#include "usart_driver.h"
#include "sys.h"
#include <string.h>

int main(void)
{
	zxy_framer *framer = &__framer;
	usart_driver * uart_driver = &__usart_driver;
	frame_ack_t ack_frame;
  uart_frame_t *payload;
	uart_frame_t response;
	response.type = ACK_FRAME;  //response的作用是对主机消息的确认（ack）
	NVIC_Group_Init();//系统默认中断分组
	Debug_Serial_Init(115200);  //调试串口初始化，波特率115200
	while(1)
	{
									
		if(framer->crcNotVailed)         //没有通过crc校验
		{
			framer->crcNotVailed = RESET;  //crcNotVailed=0
			payload = (uart_frame_t *)uart_driver->rec_buf;//将接收到的帧赋给payload
			ack_frame.command = payload->type;             //type为主机向mcu请求的内容
			ack_frame.status = ACK_FAILED;                //创建ack失败
			response.payload_len = sizeof(frame_ack_t);
			memcpy(response.buf,&ack_frame,response.payload_len);
			framer->send(framer,&response);   //把情况反馈给主机
		}
		if(framer->crcVailed)           //通过了crc验证
		{
			payload = (uart_frame_t *)uart_driver->rec_buf; //上同
			ack_frame.command = payload->type;  //读取主机请求类型
			ack_frame.status = ACK_FAILED;
			response.payload_len = sizeof(frame_ack_t);
			
								__set_PRIMASK(1);   //关总中断
			framer->input(framer,uart_driver->rec_buf,uart_driver->receiveLen-2);//处理主机发来的消息，并作出相应的动作
								__set_PRIMASK(0);   //开总中断
			
			if(ack)                      //若ack置1
				ack_frame.status = ACK_SUCCESS;   //ack状态切换为成功
			memcpy(response.buf,&ack_frame,response.payload_len);
			framer->send(framer,&response);  //向主机发送带有特定消息的ack
			framer->crcVailed = RESET;       //crc校验成功置0
			ack=0;                     //等待做出下一次的回应
			
		}
		
		if(flash_ok)                    //如果对FLASH的操作完成，即已经完成了moteid的写入
		{
			
			USART_DeInit(USART1);         //将USART1的值置为默认复位值（同步或异步的发送/接收器）
			USART_Cmd(USART1,DISABLE);    //禁用USART1的外设
			RCC_RTCCLKCmd(DISABLE);       //禁用RTC时钟
			flash_ok =0;
			Jump_to_app();
			//SCB->VTOR=0x08006000;
		}
	//framer->send(framer,&response);
	}
}



