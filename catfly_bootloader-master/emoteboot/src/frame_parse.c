#include "frame_phase.h"
#include "flash.h"
#include <string.h>



#define SECTOR_SIZE			2048          //�ļ��ֽ���


uint8_t file_buf[3000];               //���� bin�ļ�������
uint32_t app_addr = APP_ADDR;         //APP�������ʼ��ַ
uint32_t buf_index=0;
uint32_t flash_offset=0;
int16_t file_seq=-1;
uint16_t file_flag;
uint8_t  flash_ok=0;
uint8_t  is_flash=0;
uint8_t  ack=0;
void dateLinkSend(struct zxy_framer *framer,uart_frame_t *frame)       //���������͵�����������
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
        case APP2BOOT:    //����������mcu����
					app_addr  = APP_ADDR;  //APP�������ʼ��ַ
					file_seq  = -1;
					buf_index = 0;
					ack = SET;       //׼������ack?
					if(is_flash)
						break;
				  
					FLASH_Unlock(); //��FLASHд������֮ǰ��Ӧ�Ƚ�FLASH��������ΪFLASH��������
					
					while(BOOT_ErasePage(APP2BOOT_ADDR,APP_EXE_FLAG_START_ADDR+0x10000)!=FLASH_COMPLETE);//������BOOT�ĵ�ַ��app��ʼ��ַ��flash�����������ɹ����ղ�������
					is_flash=1;     //mcu reboot�ɹ�
					FLASH_Lock();   //����FLASH������	
					__set_PRIMASK(0); //�����ж�
        //ack frame callback
        break;

        case FILE_FRAME:
        //FILE_FRAME_callback
				file_frame = (frame_file_t *)frame->buf;//�����͹��������ݸ�һ���ļ��ṹ��
				if(file_frame->Sequence <= file_seq)    //�ļ�����
					break;
				
				ack = SET;       //ack��1,׼������ack?
				if(file_frame->length<=128)            //�ļ������Ƿ����
				{
					//memcpy(file_buf+buf_index,file_frame->buf,file_frame->length);
					for(i=0;i<file_frame->length;i++)
						*(file_buf+buf_index+i) = *(file_frame->buf+i);//���ļ�ת��file_bufָ��ָ��ĵ�ַ��
					buf_index+=file_frame->length;
					file_seq=file_frame->Sequence;
				}
				if(buf_index == SECTOR_SIZE)            //�Ƿ��ļ�ȫ��ת�Ƶ�file_buf��
				{

					FLASH_Unlock();                       //����FLASH
					is_flash=0;
//					while(isFlashCrcVailed((unsigned char*)(app_addr),file_buf,buf_index)!=1  )
	//				{
		//				isProged = 1;
						while(BOOT_ProgramDatatoFlash(app_addr,file_buf,SECTOR_SIZE)!=FLASH_COMPLETE)  //�������ֽ�ΪECTOR_SIZE���ļ�д��FLASH
							{
								while(BOOT_ErasePage(app_addr,app_addr+SECTOR_SIZE) !=FLASH_COMPLETE )      //��д�벻�ɹ��������FLASH����
									;
							}
			//		}
					FLASH_Lock();	    //����FLASH
					app_addr+=SECTOR_SIZE;//��app_addr����SECTOR_SIZE�ֽڣ��Ա��´�д��
					buf_index=0;
				}
					
        break;
				
				case MOTEID_FRAME:                      //����������mote_id
					
					if(buf_index !=0 )                   //��buf_index��Ϊ�㣬˵��û�в���FLASH
					{

					FLASH_Unlock();                     //����FLASH
					//while( isProged !=1 || isFlashCrcVailed((unsigned char*)(app_addr),file_buf,buf_index)!=1  )
				//	{
					//	isProged = 1;
						while(BOOT_ProgramDatatoFlash(app_addr,file_buf,SECTOR_SIZE)!=FLASH_COMPLETE)
							{
								while(BOOT_ErasePage(app_addr,app_addr+SECTOR_SIZE)!=FLASH_COMPLETE ) //���²���FLASH
									;
							}
					//}
						FLASH_Lock();	                       //����FLASH
						
						app_addr+=buf_index;                 //����app�ĵ�ַ���Ա�����д��FLASH
						buf_index=0;
					}
					
					if(sizeof(frame_moteid_t)==frame->payload_len) //�жϷ��͹������ǲ���mote_id
					{
						moteid_frame = (frame_moteid_t *)frame->buf;   //ʹ���͹�����֡�ṹ����moteid����ʽ
						//TODO write modeid
						moteid = moteid_frame->moteid << 16;      //����16λ��Ϊmoteid
						FLASH_Unlock();                       //����FLASH
						while(BOOT_ProgramDatatoFlash(MOIEID_ADDR,(uint8_t *)&moteid,4)!=FLASH_COMPLETE){ //����ʼ��ַΪMOIEID_ADDR���ڴ�ռ�д��4�ֽڵ�moteid
								while(BOOT_ErasePage(MOIEID_ADDR,MOIEID_ADDR+SECTOR_SIZE)!=FLASH_COMPLETE ) //��û����д�ɹ��������һ��flash(�ų�����)
								;
						}
						FLASH_Lock();	                       //����FLASH
						flash_ok = 1;                        //FLASH�������
						ack = SET;                            //׼������ack?
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
dateLinkSend,                      //���������ͺ���              
input                              //���մ�����
};

