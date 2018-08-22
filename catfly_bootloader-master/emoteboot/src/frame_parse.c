#include "frame_phase.h"
#include "flash.h"
#include <string.h>



#define SECTOR_SIZE			2048          //文件字节数


uint8_t file_buf[3000];               //接收 bin文件的数组
uint32_t app_addr = APP_ADDR;         //APP程序的起始地址
uint32_t buf_index=0;
uint32_t flash_offset=0;
int16_t file_seq=-1;
uint16_t file_flag;
uint8_t  flash_ok=0;
uint8_t  is_flash=0;
uint8_t  ack=0;
void dateLinkSend(struct zxy_framer *framer,uart_frame_t *frame)       //向主机发送的数据链函数
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
		frame_file_t  *file_frame;
		frame_moteid_t *moteid_frame;
		uint32_t moteid;
    //frame->payload_len = len;
    switch(frame->type)
    {
        case APP2BOOT:    //若主机请求mcu重启
					app_addr  = APP_ADDR;  //APP程序的起始地址
					file_seq  = -1;
					buf_index = 0;
					ack = SET;       //准备发送ack?
					if(is_flash)
						break;
				  
					FLASH_Unlock(); //向FLASH写入数据之前，应先将FLASH解锁，此为FLASH解锁函数
					
					while(BOOT_ErasePage(APP2BOOT_ADDR,APP_EXE_FLAG_START_ADDR+0x10000)!=FLASH_COMPLETE);//擦除从BOOT的地址到app起始地址的flash，若擦除不成功，空操作。。
					is_flash=1;     //mcu reboot成功
					FLASH_Lock();   //保护FLASH，上锁	
					__set_PRIMASK(0); //开总中断
        //ack frame callback
        break;

        case FILE_FRAME:
        //FILE_FRAME_callback
				file_frame = (frame_file_t *)frame->buf;//将传送过来的数据给一个文件结构体
				if(file_frame->Sequence <= file_seq)    //文件错误
					break;
				
				ack = SET;       //ack置1,准备发送ack?
				if(file_frame->length<=128)            //文件长度是否合适
				{
					//memcpy(file_buf+buf_index,file_frame->buf,file_frame->length);
					for(i=0;i<file_frame->length;i++)
						*(file_buf+buf_index+i) = *(file_frame->buf+i);//将文件转到file_buf指针指向的地址中
					buf_index+=file_frame->length;
					file_seq=file_frame->Sequence;
				}
				if(buf_index == SECTOR_SIZE)            //是否将文件全部转移到file_buf中
				{

					FLASH_Unlock();                       //解锁FLASH
					is_flash=0;
//					while(isFlashCrcVailed((unsigned char*)(app_addr),file_buf,buf_index)!=1  )
	//				{
		//				isProged = 1;
						while(BOOT_ProgramDatatoFlash(app_addr,file_buf,SECTOR_SIZE)!=FLASH_COMPLETE)  //把数据字节为ECTOR_SIZE的文件写入FLASH
							{
								while(BOOT_ErasePage(app_addr,app_addr+SECTOR_SIZE) !=FLASH_COMPLETE )      //若写入不成功，则擦除FLASH试试
									;
							}
			//		}
					FLASH_Lock();	    //锁定FLASH
					app_addr+=SECTOR_SIZE;//将app_addr增加SECTOR_SIZE字节，以便下次写入
					buf_index=0;
				}
					
        break;
				
				case MOTEID_FRAME:                      //若主机发来mote_id
					
					if(buf_index !=0 )                   //若buf_index不为零，说明没有擦除FLASH
					{

					FLASH_Unlock();                     //解锁FLASH
					//while( isProged !=1 || isFlashCrcVailed((unsigned char*)(app_addr),file_buf,buf_index)!=1  )
				//	{
					//	isProged = 1;
						while(BOOT_ProgramDatatoFlash(app_addr,file_buf,SECTOR_SIZE)!=FLASH_COMPLETE)
							{
								while(BOOT_ErasePage(app_addr,app_addr+SECTOR_SIZE)!=FLASH_COMPLETE ) //重新擦除FLASH
									;
							}
					//}
						FLASH_Lock();	                       //锁定FLASH
						
						app_addr+=buf_index;                 //增加app的地址，以备重新写入FLASH
						buf_index=0;
					}
					
					if(sizeof(frame_moteid_t)==frame->payload_len) //判断发送过来的是不是mote_id
					{
						moteid_frame = (frame_moteid_t *)frame->buf;   //使发送过来的帧结构契合moteid的形式
						//TODO write modeid
						moteid = moteid_frame->moteid << 16;      //左移16位作为moteid
						FLASH_Unlock();                       //解锁FLASH
						while(BOOT_ProgramDatatoFlash(MOIEID_ADDR,(uint8_t *)&moteid,4)!=FLASH_COMPLETE){ //向起始地址为MOIEID_ADDR的内存空间写入4字节的moteid
								while(BOOT_ErasePage(MOIEID_ADDR,MOIEID_ADDR+SECTOR_SIZE)!=FLASH_COMPLETE ) //若没有烧写成功，则擦除一次flash(排除故障)
								;
						}
						FLASH_Lock();	                       //锁定FLASH
						flash_ok = 1;                        //FLASH操作完成
						ack = SET;                            //准备发送ack?
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
dateLinkSend,                      //向主机发送函数              
input                              //接收处理函数
};

