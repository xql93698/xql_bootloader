#include "flash.h"



/* Base address of the Flash sectors */
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif  





/**
  * @brief  ��������д��ָ����ַ��Flash�� ��
  * @param  Address Flash��ʼ��ַ��
  * @param  Data ���ݴ洢����ʼ��ַ��
  * @param  DataNum �����ֽ�����
  * @retval ������д״̬��
  */
FLASH_Status BOOT_ProgramDatatoFlash(uint32_t StartAddress,uint8_t *pData,uint32_t DataNum) 
{
  FLASH_Status FLASHStatus = FLASH_COMPLETE;

  uint32_t i;

  if(StartAddress<APP2BOOT_ADDR){
    return FLASH_ERROR_PG;
  }
   /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  for(i=0;i<(DataNum>>2);i++)
  {
    FLASHStatus = FLASH_ProgramWord(StartAddress, *((uint32_t*)pData));
    if (FLASHStatus == FLASH_COMPLETE){
      StartAddress += 4;
      pData += 4;
    }else{ 
      return FLASHStatus;
    }
  }
  return	FLASHStatus;
}
/**
  * @brief  ����ָ�����������Flash���� ��
  * @param  StartPage ��ʼ������ַ
  * @param  EndPage ����������ַ
  * @retval ��������״̬  
  */
FLASH_Status BOOT_ErasePage(uint32_t StartPageAddr,uint32_t EndPageAddr)
{
  uint32_t i;
  FLASH_Status FLASHStatus=FLASH_COMPLETE;

  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  for(i=StartPageAddr;i<=EndPageAddr;i+=PAGE_SIZE){
    FLASHStatus = FLASH_ErasePage(i);
    if(FLASHStatus!=FLASH_COMPLETE){
      FLASH_Lock();
      return	FLASHStatus;	
    }
  }
  FLASH_Lock();
  return FLASHStatus;
}


typedef  void (*pFunction)(void);
/**
  * @brief  ���Ƴ�����ת��ָ��λ�ÿ�ʼִ�� ��
  * @param  Addr ����ִ�е�ַ��
  * @retval ������ת״̬��
  */
void JumpToApplication(uint32_t Addr)
{
  static pFunction Jump_To_Application;
  __IO uint32_t JumpAddress; 
  /* Test if user code is programmed starting from address "ApplicationAddress" */
  if (((*(__IO uint32_t*)Addr) & 0x2FFE0000 ) == 0x20000000)
  { 
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*) (Addr + 4);
    Jump_To_Application = (pFunction) JumpAddress;

    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*)Addr);
		__set_PRIMASK(0);//�ر������ж�
    Jump_To_Application();
  }
}










//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr; 
}


#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
    u16 i;
    for(i=0;i<NumToWrite;i++)
    {
        FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
        WriteAddr+=2;//��ַ����2.
    }  
} 

//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif	


u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
    u32 secpos;	   //������ַ
    u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
    u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
    u16 i;    
    u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
    if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
    FLASH_Unlock();						//����
    offaddr=WriteAddr-STM32_FLASH_BASE;		        //ʵ��ƫ�Ƶ�ַ.
    secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
    secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
    secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
    if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
    while(1) 
    {	
        STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
        for(i=0;i<secremain;i++)//У������
        {
            if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
        }
        if(i<secremain)//��Ҫ����
        {
            FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
            for(i=0;i<secremain;i++)//����
            {
                STMFLASH_BUF[i+secoff]=pBuffer[i];	  
            }
            STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
        }else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
        if(NumToWrite==secremain)break;//д�������
        else//д��δ����
        {
            secpos++;				//������ַ��1
            secoff=0;				//ƫ��λ��Ϊ0 	 
            pBuffer+=secremain;  	//ָ��ƫ��
            WriteAddr+=secremain;	//д��ַƫ��	   
            NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
            if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
            else secremain=NumToWrite;//��һ����������д����
        }	 
    };	
    FLASH_Lock();//����
}
#endif



//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
    u16 i;
    for(i=0;i<NumToRead;i++)
    {
        pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
        ReadAddr+=2;//ƫ��2���ֽ�.	
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
    STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}
















