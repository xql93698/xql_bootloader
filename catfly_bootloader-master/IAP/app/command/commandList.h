#ifndef __COMMANDLIST_H_
#define __COMMANDLIST_H_
#include "commandInclude.h"
//command里面的参数列表和函数列表存放在该文件中

typedef void (*Command_Proc_Func)(void);

extern u8 commandStringList[][50] ;

extern Command_Proc_Func Command_Proc_Func_Table[];




#endif












