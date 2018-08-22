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
#define ApplicationAddress 0x08006000  //app�������ʼ��ַ��׼ȷ��˵��Ϊջ����ַ
typedef  void (*pFunction)(void);

/* Execute application */
void Jump_to_app()
{
	pFunction Jump_To_Application;
  uint32_t JumpAddress;
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)  //���ջ����ַ�Ƿ�Ϸ�,��Ϊ��ջ��ַҪ����ram�вſ��ԣ���RAM����ʼ��ַΪ0x20000000�������˼���
			{
				//SerialPutChar(0x16);
			  JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4); //�ڶ�����Ϊ������ʼ��ַ����λ��ַ��

				/* Jump to user application */
				Jump_To_Application = (pFunction) JumpAddress;
				/* Initialize user application's Stack Pointer */  
				__set_MSP(*(__IO uint32_t*) ApplicationAddress);//��ʼ��Ӧ�ó����ջָ��(��һ����������ջ����ַ��
				Jump_To_Application();//ת����APP��ִ�и�λ�жϳ���
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
	response.type = ACK_FRAME;  //response�������Ƕ�������Ϣ��ȷ�ϣ�ack��
	NVIC_Group_Init();//ϵͳĬ���жϷ���
	Debug_Serial_Init(115200);  //���Դ��ڳ�ʼ����������115200
	while(1)
	{
									
		if(framer->crcNotVailed)         //û��ͨ��crcУ��
		{
			framer->crcNotVailed = RESET;  //crcNotVailed=0
			payload = (uart_frame_t *)uart_driver->rec_buf;//�����յ���֡����payload
			ack_frame.command = payload->type;             //typeΪ������mcu���������
			ack_frame.status = ACK_FAILED;                //����ackʧ��
			response.payload_len = sizeof(frame_ack_t);
			memcpy(response.buf,&ack_frame,response.payload_len);
			framer->send(framer,&response);   //���������������
		}
		if(framer->crcVailed)           //ͨ����crc��֤
		{
			payload = (uart_frame_t *)uart_driver->rec_buf; //��ͬ
			ack_frame.command = payload->type;  //��ȡ������������
			ack_frame.status = ACK_FAILED;
			response.payload_len = sizeof(frame_ack_t);
			
								__set_PRIMASK(1);   //�����ж�
			framer->input(framer,uart_driver->rec_buf,uart_driver->receiveLen-2);//����������������Ϣ����������Ӧ�Ķ���
								__set_PRIMASK(0);   //�����ж�
			
			if(ack)                      //��ack��1
				ack_frame.status = ACK_SUCCESS;   //ack״̬�л�Ϊ�ɹ�
			memcpy(response.buf,&ack_frame,response.payload_len);
			framer->send(framer,&response);  //���������ʹ����ض���Ϣ��ack
			framer->crcVailed = RESET;       //crcУ��ɹ���0
			ack=0;                     //�ȴ�������һ�εĻ�Ӧ
			
		}
		
		if(flash_ok)                    //�����FLASH�Ĳ�����ɣ����Ѿ������moteid��д��
		{
			
			USART_DeInit(USART1);         //��USART1��ֵ��ΪĬ�ϸ�λֵ��ͬ�����첽�ķ���/��������
			USART_Cmd(USART1,DISABLE);    //����USART1������
			RCC_RTCCLKCmd(DISABLE);       //����RTCʱ��
			flash_ok =0;
			Jump_to_app();
			//SCB->VTOR=0x08006000;
		}
	//framer->send(framer,&response);
	}
}



