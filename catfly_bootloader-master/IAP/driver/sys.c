#include "sys.h"


void NVIC_Group_Init(void)	//ϵͳ�жϷ���
{
	NVIC_PriorityGroupConfig(BOARD_NVIC_GROUP);//����ϵͳ�ж�
}

