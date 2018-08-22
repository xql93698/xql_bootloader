#ifndef __FLASH_H__
#define __FLASH_H__
#include "stm32f10x.h"  
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 256 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1                      //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 	0x08000000 	//STM32 FLASH的起始地址
#define APP_ADDR					0x08006000
#define MOIEID_ADDR				0x08004000
#define APP2BOOT_ADDR			0x08004000
#define APP_EXE_FLAG_START_ADDR APP_ADDR  //APP程序的起始地址
//FLASH解锁键值
//#define RDP_Key                  ((uint16_t)0x00A5)
//#define FLASH_KEY1               ((uint32_t)0x45670123)
//#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);				//指定地址开始读取指定长度数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据
FLASH_Status BOOT_ProgramDatatoFlash(uint32_t StartAddress,uint8_t *pData,uint32_t DataNum);
FLASH_Status BOOT_ErasePage(uint32_t StartPageAddr,uint32_t EndPageAddr);
void JumpToApplication(uint32_t Addr);
//测试写入
void Test_Write(u32 WriteAddr,u16 WriteData);								   
#endif

















