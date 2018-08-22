#include "frame_phase.h"
#include "flash.h"
#include <string.h>



#define SECTOR_SIZE			2048


uint8_t file_buf[3000];
uint32_t app_addr = APP_ADDR;
uint32_t buf_index=0;
uint32_t flash_offset=0;
int16_t file_seq=-1;
uint16_t file_flag;
uint8_t  flash_ok=0;
uint8_t  is_flash=0;
uint8_t  ack=0;
void dateLinkSend(struct zxy_framer *framer,uart_frame_t *frame)
{
	usart_driver *usart = &__usart_driver;
	uint8_t len = sizeof(uart_frame_t)-MAX_MTU_LEN+frame->payload_len;
	frame->seq = framer->seq;
	//printf("uart_frame_t len is %d  MAX_MTU_LEN is %d, payload_len is %d %d\r\n",sizeof(uart_frame_t),MAX_MTU_LEN,frame->payload_len,len);
	usart->send(usart,(uint8_t *)frame,len);
	framer->seq++;
}

void input(struct zxy_framer *phaser,uint8_t *buf,uint8_t len)
{
		uint16_t i=0;
    uart_frame_t *frame = (uart_frame_t *)buf;
//		frame_moteid_t *moteid_frame;
		frame_file_t  *file_frame;
    //frame->payload_len = len;
    switch(frame->type)
    {
        case APP2BOOT:
					app_addr  = APP_ADDR;
					file_seq  = -1;
					buf_index = 0;
					ack = SET;
					if(is_flash)
						break;
				  
					FLASH_Unlock();
					
					while(BOOT_ErasePage(APP2BOOT_ADDR,APP_EXE_FLAG_START_ADDR+0xF000)!=FLASH_COMPLETE);
					is_flash=1;
					FLASH_Lock();	
					__set_PRIMASK(0);
        //ack frame callback
        break;

        case FILE_FRAME:
        //FILE_FRAME_callback
				file_frame = (frame_file_t *)frame->buf;
				if(file_frame->Sequence <= file_seq)
					break;
				
				ack = SET;
				if(file_frame->length<=128)
				{
					//memcpy(file_buf+buf_index,file_frame->buf,file_frame->length);
					for(i=0;i<file_frame->length;i++)
						*(file_buf+buf_index+i) = *(file_frame->buf+i);
					buf_index+=file_frame->length;
					file_seq=file_frame->Sequence;
				}
				if(buf_index == SECTOR_SIZE)
				{

					FLASH_Unlock();
					is_flash=0;
					
					while(BOOT_ProgramDatatoFlash(app_addr,file_buf,SECTOR_SIZE)!=FLASH_COMPLETE)
						{
							while(BOOT_ErasePage(app_addr,app_addr+SECTOR_SIZE) !=FLASH_COMPLETE )
								;

						}
					FLASH_Lock();	
					app_addr+=SECTOR_SIZE;
					buf_index=0;
				}
					
        break;
				
				case MOTEID_FRAME:
					
					if(buf_index !=0 )
					{

						FLASH_Unlock();
					while(BOOT_ProgramDatatoFlash(app_addr,file_buf,SECTOR_SIZE)!=FLASH_COMPLETE)
						{
							while(BOOT_ErasePage(app_addr,app_addr+SECTOR_SIZE)!=FLASH_COMPLETE ) 
								;
						}
						FLASH_Lock();	
						
						app_addr+=buf_index;
						buf_index=0;
					}
					
					if(sizeof(frame_moteid_t)==frame->payload_len)
					{
						//moteid_frame = (frame_moteid_t *)frame->buf;
						//TODO write modeid
						FLASH_Unlock();
						while(BOOT_ProgramDatatoFlash(MOIEID_ADDR,frame->buf,4)!=FLASH_COMPLETE);
						FLASH_Lock();	
						flash_ok = 1;
						ack = SET;
					}

				break;
				
        default:
        //unrecognise frame
        break;

    }

}

zxy_framer __framer={
0,
0,
0,
dateLinkSend,
input
};

