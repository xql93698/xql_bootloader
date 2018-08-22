#include "delay.h"

static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数

void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//bit2清空,选择外部时钟  HCLK/8
	fac_us=(SystemCoreClock/1000000)/8;	 //1us需计算的脉冲个数,其中(SystemFrequency/1000000)代表1US的脉冲个数,8是需要8分频
	fac_ms=(u16)fac_us*1000;				 //1ms需计算的脉冲个数
	if(fac_us==0)fac_us=1;
}

static void Delay_1Ms(void)
{
	u32 temp;		   
	SysTick->LOAD=(u32)fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
}

static void Delay_1Us(void)
{
	u32 temp;	    	 
	SysTick->LOAD=fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
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





