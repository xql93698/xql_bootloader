#ifndef __COMMANDUTIL_H_
#define __COMMANDUTIL_H_
#include "commandInclude.h"


extern u8 commandStringBuffer[SERIAL_MAX_LENGTH];
extern u8 commandStringLength;

//将串口缓冲区数据拿出来以备以后处理
void Command_Copy(void);
//检测串口接收到数据是否全部是空格,不是则返回1,是返回0
u8 Command_Is_Vailed(void);
//去除命令字符串的前面的空格字符
void Command_Remove_Space_Head(void);
//去除命令字符串后面的空格
void Command_Remove_Space_End(void);
//去除命令字符串中间的空格,将连续两个的空格合并成一个
void Command_Remove_Space_Inner(void);
//获取字符串长度
u8 StringGetLength(u8* string);
//返回0标识找不到空格,序列号从1开始
u8 Command_Find_Space_Postion(u8 index);
//获取成功返回1 获取失败返回0
u8 CommandGetParamStr(u8* paramBuffer,u8 maxParamLength,u8* ParamRealLength,u8 ParamIndex);
//失败返回1 成功返回0
u8 CommandGetParamToDecU8(u8* value,u8 index);
//失败返回1 成功返回0
u8 CommandGetParamToDecU16(u16* value,u8 index);
//失败返回1 成功返回0
u8 CommandGetParamToDecU32(u32* value,u8 index);
//成功返回0 失败返回1
u8 CommandGetParamToHexU8(u8* value,u8 index);
//成功返回0 失败返回1
u8 CommandGetParamToHexU16(u16* value,u8 index);
//成功返回0 失败返回1
u8 CommandGetParamToHexU32(u32* value,u8 index);

#endif






