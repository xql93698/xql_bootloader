#include "commandFunc.h"

//�ص�������ȫ�����嶼��������

/****************************************************************************
ϵͳ����ص�����
**************************************************************************/

void Help_Proc_Func(void)
{
	printf("dink shell v0.1\r\n");
	printf("modify by dengxiaojun\r\n");
	printf("2015/1/18 23:29\r\n");
}

void List_Proc_Func(void)
{
	u8 i = 0;
	printf("command num is %d\r\n",COMMAND_NUM);
	for(i = 0; i < COMMAND_NUM; i++)
	{
		printf("%d : %s\r\n",i,commandStringList[i]);
	}
	printf("*****************************************************\r\n");
}


/***************************�û��Զ���ص������Ӵ˿�ʼ*******************************/







