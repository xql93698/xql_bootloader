#include "delay.h"

static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������

void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=(SystemCoreClock/1000000)/8;	 //1us�������������,����(SystemFrequency/1000000)����1US���������,8����Ҫ8��Ƶ
	fac_ms=(u16)fac_us*1000;				 //1ms�������������
	if(fac_us==0)fac_us=1;
}

static void Delay_1Ms(void)
{
	u32 temp;		   
	SysTick->LOAD=(u32)fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����
}

static void Delay_1Us(void)
{
	u32 temp;	    	 
	SysTick->LOAD=fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����
}

void Delay_Ms(u32 ms)
{
	u32 i = 0;
	for(i = 0; i < ms; i++)
	{
		Delay_1Ms();
	}
}

void Delay_Us(u32 us)
{
	u32 i = 0;
	for(i = 0; i < us; i++)
	{
		Delay_1Us();
	}
}





