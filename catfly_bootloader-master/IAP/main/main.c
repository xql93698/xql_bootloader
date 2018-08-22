#include "mainInclude.h"
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
	response.type = ACK_FRAME;
	NVIC_Group_Init();//系统默认中断分组
	Debug_Serial_Init(115200);
	while(1)
	{

		if(framer->crcNotVailed)
		{
			framer->crcNotVailed = RESET;
			payload = (uart_frame_t *)uart_driver->rec_buf;
			ack_frame.command = payload->type;
			ack_frame.status = ACK_FAILED;
			response.payload_len = sizeof(frame_ack_t);
			memcpy(response.buf,&ack_frame,response.payload_len);
			framer->send(framer,&response);
		}
		if(framer->crcVailed)
		{
			payload = (uart_frame_t *)uart_driver->rec_buf;
			ack_frame.command = payload->type;
			ack_frame.status = ACK_FAILED;
			response.payload_len = sizeof(frame_ack_t);
			
								__set_PRIMASK(1);
			framer->input(framer,uart_driver->rec_buf,uart_driver->receiveLen-2);
								__set_PRIMASK(0);
			
			if(ack)
				ack_frame.status = ACK_SUCCESS;
			memcpy(response.buf,&ack_frame,response.payload_len);
			framer->send(framer,&response);
			framer->crcVailed = RESET;
		}
		
		if(flash_ok)
		{
			
			USART_DeInit(USART1);
			USART_Cmd(USART1,DISABLE);
			RCC_RTCCLKCmd(DISABLE);
			flash_ok =0;
			SCB->VTOR=0x08006000;
			JumpToApplication(0x08006000);
		}
	//framer->send(framer,&response);
	}
}




