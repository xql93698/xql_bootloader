#ifndef __SYS_H_
#define __SYS_H_
#include "stm32f10x.h"

//λ��ģʽ����IO�ڶ���
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)   

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  


//ϵͳ�ж����ȼ����� 2λ��ռ 2λ�����ȼ�
//ϵͳĬ���жϷ���
#define BOARD_NVIC_GROUP	NVIC_PriorityGroup_2	//2BIT 2BIT

//NVIC�жϷ����Լ�ϵͳ���ж����ȼ��Ķ���
typedef enum PREEMPTPRIORITY
{
	PreemptPriority0 = 0,
	PreemptPriority1 = 1,
	PreemptPriority2 = 2,
	PreemptPriority3 = 3
}PREEMPTPRIORITY;

typedef enum SUBPRIORITY
{
	SubPriority0 = 0,
	SubPriority1 = 1,
	SubPriority2 = 2,
	SubPriority3 = 3
}SUBPRIORITY;

//����һ�ж����ȼ�,���ȼ����,��ֹ���ʧ
#define UART1_PreemptionPriority		PreemptPriority0
#define UART1_SubPriority				SubPriority0

//ʵʱʱ���ж�,���ȼ����,��ֹ���ʧ
#define RTC_PreemptionPriority			PreemptPriority3
#define RTC_SubPriority					SubPriority3

//���ڿ��Ź��ж�,���ȼ����,��ֹ���ʧ
#define WWDG_PreemptionPriority			PreemptPriority2
#define WWDG_SubPriority				SubPriority3

//�����ж����ȼ�,���Ե�һЩ
#define KEYUP_PreemptionPriority		PreemptPriority3
#define KEYUP_SubPriority				SubPriority3
//�����ж����ȼ�
#define KEYDOWN_PreemptionPriority		PreemptPriority3
#define KEYDOWN_SubPriority				PreemptPriority3
//�����ж����ȼ�
#define KEYRIGHT_PreemptionPriority		PreemptPriority3
#define KEYRIGHT_SubPriority			PreemptPriority3
//�����ж����ȼ�
#define KEYLEFT_PreemptionPriority		PreemptPriority3
#define KEYLEFT_SubPriority				PreemptPriority3
//�ⲿ��������ж����ȼ�
#define REMOTE_PreemptionPriority		PreemptPriority1
#define REMOTE_SubPriority				PreemptPriority1
//485ͨѶ�ж����ȼ�
#define RS485_PreemptionPriority		PreemptPriority3
#define RS485_SubPriority				PreemptPriority3



//USB�����ж����ȼ� ,���е����ȼ�
#define USB_WAKE_UP_PreemptionPriority	PreemptPriority2	//usb�����ж�
#define USB_WAKE_UP_SubPriority			SubPriority2
//USB�����ȼ��жϵ��ж����ȼ�,�������ڴ���
#define USB_LP_PreemptionPriority		PreemptPriority1	//usb�����ȼ��ж�(��Ӧusb�����ȼ��ж�)
#define USB_LP_SubPriority				PreemptPriority1
//USB�����ȼ����ж����ȼ�
#define USB_HP_PreemptionPriority		PreemptPriority1	//usb�����ȼ��ж�(��Ӧusb�����ȼ��ж�)
#define USB_HP_SubPriority				PreemptPriority1

void NVIC_Group_Init(void);	//ϵͳ�жϷ���


#endif



