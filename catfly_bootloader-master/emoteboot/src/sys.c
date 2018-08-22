#include "sys.h"


void NVIC_Group_Init(void)	//系统中断分组
{
	NVIC_PriorityGroupConfig(BOARD_NVIC_GROUP);//配置系统中断
}

