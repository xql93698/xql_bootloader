#include "commandList.h"


//命令列表,命令最长50字节
u8 commandStringList[][50] = \
{
	"help",\
	"list",\
	"iap_down",\
	"iap_jump_app",\
	"iap_over",\
	"iap_set_flag",\
	"iap_clear_flag"
};



//回调函数数组定义
Command_Proc_Func Command_Proc_Func_Table[] = 
{
	Help_Proc_Func,
	List_Proc_Func,
	iap_down_s,
	iap_jump_app_s,
	iap_over_s,
	iap_set_flag_s,
	iap_clear_flag
};




