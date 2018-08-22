#include "commandFunc.h"

//回调函数的全部定义都放在这里

/****************************************************************************
系统命令回调函数
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


/***************************用户自定义回调函数从此开始*******************************/







