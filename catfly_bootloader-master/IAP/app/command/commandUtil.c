#include "commandInclude.h"

u8 commandStringBuffer[SERIAL_MAX_LENGTH] = {0};//用于暂时缓存串口中的命令数据
u8 commandStringLength = 0;						//记录串口中的命令的长度

static u8 paramBuffer[PARAM_COVERT_MAX_LENGTH] = {0};	//内置的用于辅助参数转换的变量
static u8 paramLength = 0;			//内置用于表示参数长度的变量

//此处存放字符串处理以及字符串转换的代码


/*******************************串口命令预处理函数定义********************************/
//将串口缓冲区数据拿出来以备以后处理
void Command_Copy(void)
{
	u8 i = 0;
	commandStringLength = (serial_Buffer_Length&0xff);
	for(i = 0; i < commandStringLength; i++)
	{
		commandStringBuffer[i] = serial_Buffer[i];
	}
	//清除串口缓冲区标志
	serial_Buffer_Length = 0;
}

//检测串口接收到数据是否全部是空格,不是则返回1,是返回0
u8 Command_Is_Vailed(void)
{
	u8 i = 0;
	for(i = 0; i < (serial_Buffer_Length&0xff);i++)
	{
		if(serial_Buffer[i] == ' ')continue;
		else return 1;
	}
	return 0;
}


//去除命令字符串的前面的空格字符
void Command_Remove_Space_Head(void)
{
	u8 index = 0;
	u8 i = 0;
	for(index = 0; index < commandStringLength; index++)
	{
		if(commandStringBuffer[index] == ' ')continue;
		else break;
	}
	if(index == 0)//前面没有空格
	{
		return;
	}
	else
	{
		//删除空格
		for(i = 0; i < (commandStringLength-index);i++)
		{
			commandStringBuffer[i] = commandStringBuffer[index+i];
		}
		commandStringLength -= index;
	}
}

//去除命令字符串后面的空格
void Command_Remove_Space_End(void)
{
	u8 i = 0;
	//寻找字符串最尾巴上空格的位置
	for(i = commandStringLength; i > 0; i--)
	{
		if(commandStringBuffer[i-1] == ' ')continue;//如果这个是空格,继续下一次寻找
		else break;//不是空格,到此为止
	}
	if(i == commandStringLength)//尾上没有空格
	{
		return;
	}
	else		//尾上有空格
	{
		commandStringBuffer[i] = '\0';
		commandStringLength = i;
		return;
	}
	
}

//去除命令字符串中间的空格,将连续两个的空格合并成一个
void Command_Remove_Space_Inner(void)
{
	u8 spaceCount;
	u8 i = 0;
	u8 j = 0;
	for(i = 0; i < commandStringLength; i++)
	{
		//此时检测到一个空格
		if(commandStringBuffer[i] == ' ')
		{
			//立刻查看下一个是不是空格
			if(commandStringBuffer[i+1] == ' ')
			{
				spaceCount = 2;
				//下一个也是空格,此时说明已经有了两个连续的空格了必须立刻查找到结束的空格在哪
				for(j = i+2; j < commandStringLength; j++)
				{
					//当不是空格的时候跳出来,是空格就一直加
					if(commandStringBuffer[j] == ' ')spaceCount++;
					else break;
				}
				//跳出来根据space的值来移动数组,同时减小长度
				//i是第一个空格,i+1是第二个空格,最后一个空格是spaceCount-2
				for(j = i+1;j < commandStringLength-spaceCount+1;j++)
				{
					//要跳过spacecount-1的数量,来拷贝有效字符
					commandStringBuffer[j] = commandStringBuffer[j+spaceCount-1];
				}
				//最后修改长度,长度缩减量是空格数-1,因为保留了一个空格
				commandStringLength -= (spaceCount-1);
			}
			else
			{
				//下一个不是空格,说明是只有一个空格的环境,不用操心,进行下一次循环
				continue;
			}
		}
		
	}
}

/***********************************************************************************************/

/***********************************串口命令解析函数*******************************************/
//获取字符串长度
u8 StringGetLength(u8* string)
{
	u8 i = 0; 
	while(*(string+i) != '\0')
	{
		i++;
	}
	return i;
}

//寻找命令字符串中的空格位置,也就是空格的下标
//返回0标识找不到空格,序列号从1开始
u8 Command_Find_Space_Postion(u8 index)
{
	//这个函数针对的是给定的字符串,不是任意字符串
	u8 i = 0;
	u8 spaceCount = 0;
	for(i = 0; i < commandStringLength; i++)
	{
		if(commandStringBuffer[i] == ' ')
		{
			spaceCount++;
			if(spaceCount == index)
				return i;
		}
		else 
			continue;
	}
	return 0;
}

//从命令列表中获取指定长度的参数字符串
//paramBuffer 存放获取到的参数位置
//maxParamLength 参数的最大长度,获取的长度超过则获取失败
//ParamRealLength 实际获取到的长度指针
//ParamIndex 第几个参数
//获取成功返回1 获取失败返回0
u8 CommandGetParamStr(u8* paramBuffer,u8 maxParamLength,u8* ParamRealLength,u8 ParamIndex)
{
	u8 paramStart;
	u8 paramEnd;
	u8 paramLength;
	u8 i = 0, j = 0;
	paramStart = Command_Find_Space_Postion(ParamIndex);
	if(paramStart == 0)//没有参数
	{
		return 0;
	}
	else
	{
		paramEnd = Command_Find_Space_Postion(ParamIndex+1);
		if(paramEnd == 0)
		{
			//字符串只带有一个参数,那么参数结尾就是字符串结尾
			paramEnd = commandStringLength;
		}
		//计算出的参数长度是否大于最大参数长度
		paramLength = paramEnd - paramStart-1;//多减去一个,去掉一个空格
		if(paramLength > maxParamLength)
		{
			return 0;//参数过长
		}
		else
		{
			for(i = paramStart+1;i < paramEnd; i++)
			{
				paramBuffer[j] = commandStringBuffer[i];
				j++;
			}
			*ParamRealLength = paramLength;
			return 1;
		}
	}
}
/*********************************************************************************************/

/*******************************字符串参数转换接口***********************/

//将十进制格式的字符串参数转换为数值,返回8位无符号整形 
//value 最终转换值指针
//index 指示第几个参数,第一个参数为1 ......
//返回值 转换是否成功,失败返回1 成功返回0
u8 CommandGetParamToDecU8(u8* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//找不到这么多参数
	//找到之后根据长度计算系数
	//系数计算
	for(i = 1; i < paramLength;i++)
	{
		fac *= 10;
	}
	//校验每个参数字符值是否符合标准,十进制就必须在0-9之间
	for(i = 0;i<paramLength;i++)
	{
		if(paramBuffer[i] > '9' || paramBuffer[i] < '0')
		{
			return 1;//参数错误
		}
	}
	//开始计算
	for(i = 0; i < paramLength;i++)
	{
		valueResult += (paramBuffer[i]-'0')*fac;
		fac/=10;
	}
	//检测最终结果是否大于限制值,如八位那么结果不能大于255
	if(valueResult > 0xff)return 1;//参数错误
	else 
	{
		*value = (u8)valueResult;
		return 0;
	}
}

//与上一个参数类似,检测十六进制参数
u8 CommandGetParamToDecU16(u16* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//找不到这么多参数
	//找到之后根据长度计算系数
	//系数计算
	for(i = 1; i < paramLength;i++)
	{
		fac *= 10;
	}
	//校验每个参数字符值是否符合标准,十进制就必须在0-9之间
	for(i = 0;i<paramLength;i++)
	{
		if(paramBuffer[i] > '9' || paramBuffer[i] < '0')
		{
			return 1;//参数错误
		}
	}
	//开始计算
	for(i = 0; i < paramLength;i++)
	{
		valueResult += (paramBuffer[i]-'0')*fac;
		fac/=10;
	}
	//检测最终结果是否大于限制值,如八位那么结果不能大于255
	if(valueResult > 0xffff)return 1;//参数错误
	else 
	{
		*value = (u16)valueResult;
		return 0;
	}
}


u8 CommandGetParamToDecU32(u32* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//找不到这么多参数
	//找到之后根据长度计算系数
	//系数计算
	for(i = 1; i < paramLength;i++)
	{
		fac *= 10;
	}
	//校验每个参数字符值是否符合标准,十进制就必须在0-9之间
	for(i = 0;i<paramLength;i++)
	{
		if(paramBuffer[i] > '9' || paramBuffer[i] < '0')
		{
			return 1;//参数错误
		}
	}
	//开始计算
	for(i = 0; i < paramLength;i++)
	{
		valueResult += (paramBuffer[i]-'0')*fac;
		fac/=10;
	}
	//检测最终结果是否大于限制值,如八位那么结果不能大于255
	if(valueResult > 0xffffffff)return 1;//参数错误
	else 
	{
		*value = (u32)valueResult;
		return 0;
	}
}

//从命令字符串中获取参数并转换参数,将0x格式的字符串转换为数值 成功返回0 失败返回1
//参数类型必须是0x开头的
u8 CommandGetParamToHexU8(u8* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//找不到这么多参数
	//检测参数长度,因为开头必须为0x,所以长度必须为3以上
	if(paramLength <= 2)return 1;//失败
	//计算系数
	for(i = 3; i < paramLength; i++)
	{
		fac *= 16;				//因为0x占用了两个字节 第一未为1 所以乘法运算从第三个开始
	}
	//检测开头是否正确 0x
	if(paramBuffer[0] == '0' &&(paramBuffer[1] == 'x'||paramBuffer[1] == 'X'))
	{
		//检测每一位数据是否正确并计算最终值
		for(i = 2; i < paramLength; i++)
		{
			if(paramBuffer[i] >= '0' && paramBuffer[i] <= '9')
			{
				result = paramBuffer[i] -'0';
			}
			else if(paramBuffer[i] >= 'a' && paramBuffer[i] <= 'f')
			{
				result = paramBuffer[i] -'a'+10;
			}
			else if(paramBuffer[i] >= 'A' && paramBuffer[i] <= 'F')
			{
				result = paramBuffer[i] -'A'+10;
			}
			else
			{
				//出现范围之外的数据,返回1
				return 1;
			}
			valueResult += (u32)(result*fac);
			fac /= 16;
		}
		//计算完成,检测参数是否超过范围
		if(valueResult > 0xff)return 1;//参数错误
		else 
		{
			*value = (u8)valueResult;
			return 0;
		}
	}
	else
	{
		//参数开头不对
		return 1;
	}
}


//从命令字符串中获取参数并转换参数,将0x格式的字符串转换为数值 成功返回0 失败返回1
u8 CommandGetParamToHexU16(u16* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//找不到这么多参数
	//检测参数长度,因为开头必须为0x,所以长度必须为3以上
	if(paramLength <= 2)return 1;//失败
	//计算系数
	for(i = 3; i < paramLength; i++)
	{
		fac *= 16;//因为0x占用了两个字节 第一未为1 所以乘法运算从第三个开始
	}
	//检测开头是否正确 0x
	if(paramBuffer[0] == '0' &&(paramBuffer[1] == 'x'||paramBuffer[1] == 'X'))
	{
		//检测每一位数据是否正确并计算最终值
		for(i = 2; i < paramLength; i++)
		{
			if(paramBuffer[i] >= '0' && paramBuffer[i] <= '9')
			{
				result = paramBuffer[i] -'0';
			}
			else if(paramBuffer[i] >= 'a' && paramBuffer[i] <= 'f')
			{
				result = paramBuffer[i] -'a'+10;
			}
			else if(paramBuffer[i] >= 'A' && paramBuffer[i] <= 'F')
			{
				result = paramBuffer[i] -'A'+10;
			}
			else
			{
				//出现范围之外的数据,返回1
				return 1;
			}
			valueResult += (u32)(result*fac);
			fac /= 16;
		}
		//计算完成,检测参数是否超过范围
		if(valueResult > 0xffff)return 1;//参数错误
		else 
		{
			*value = (u16)valueResult;
			return 0;
		}
	}
	else
	{
		//参数开头不对
		return 1;
	}
}

//从命令字符串中获取参数并转换参数,将0x格式的字符串转换为数值 成功返回0 失败返回1
u8 CommandGetParamToHexU32(u32* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//找不到这么多参数
	//检测参数长度,因为开头必须为0x,所以长度必须为3以上
	if(paramLength <= 2)return 1;//失败
	//计算系数
	for(i = 3; i < paramLength; i++)
	{
		fac *= 16;//因为0x占用了两个字节 第一未为1 所以乘法运算从第三个开始
	}
	//检测开头是否正确 0x
	if(paramBuffer[0] == '0' &&(paramBuffer[1] == 'x'||paramBuffer[1] == 'X'))
	{
		//检测每一位数据是否正确并计算最终值
		for(i = 2; i < paramLength; i++)
		{
			if(paramBuffer[i] >= '0' && paramBuffer[i] <= '9')
			{
				result = paramBuffer[i] -'0';
			}
			else if(paramBuffer[i] >= 'a' && paramBuffer[i] <= 'f')
			{
				result = paramBuffer[i] -'a'+10;
			}
			else if(paramBuffer[i] >= 'A' && paramBuffer[i] <= 'F')
			{
				result = paramBuffer[i] -'A'+10;
			}
			else
			{
				//出现范围之外的数据,返回1
				return 1;
			}
			valueResult += (u32)(result*fac);
			fac /= 16;
		}
		//计算完成,检测参数是否超过范围
		if(valueResult > 0xffffffff)return 1;//参数错误
		else 
		{
			*value = (u32)valueResult;
			return 0;
		}
	}
	else
	{
		//参数开头不对
		return 1;
	}
}




/************************************************************************/





























