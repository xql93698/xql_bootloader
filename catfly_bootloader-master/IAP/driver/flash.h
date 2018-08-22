#ifndef __FLASH_H__
#define __FLASH_H__
#include "stm32f10x.h"  
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 256 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1                      //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 	0x08000000 	//STM32 FLASH����ʼ���
#define APP_ADDR					0x08006000
#define MOIEID_ADDR				0x08005000
#define APP2BOOT_ADDR			0x08004000
#define APP_EXE_FLAG_START_ADDR APP_ADDR
//FLASH������ֵ
//#define RDP_Key                  ((uint16_t)0x00A5)
//#define FLASH_KEY1               ((uint32_t)0x45670123)
//#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);				//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
FLASH_Status BOOT_ProgramDatatoFlash(uint32_t StartAddress,uint8_t *pData,uint32_t DataNum);
FLASH_Status BOOT_ErasePage(uint32_t StartPageAddr,uint32_t EndPageAddr);
void JumpToApplication(uint32_t Addr);
//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);								   
#endif

















