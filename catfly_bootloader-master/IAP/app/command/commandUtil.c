#include "commandInclude.h"

u8 commandStringBuffer[SERIAL_MAX_LENGTH] = {0};//������ʱ���洮���е���������
u8 commandStringLength = 0;						//��¼�����е�����ĳ���

static u8 paramBuffer[PARAM_COVERT_MAX_LENGTH] = {0};	//���õ����ڸ�������ת���ı���
static u8 paramLength = 0;			//�������ڱ�ʾ�������ȵı���

//�˴�����ַ��������Լ��ַ���ת���Ĵ���


/*******************************��������Ԥ����������********************************/
//�����ڻ����������ó����Ա��Ժ���
void Command_Copy(void)
{
	u8 i = 0;
	commandStringLength = (serial_Buffer_Length&0xff);
	for(i = 0; i < commandStringLength; i++)
	{
		commandStringBuffer[i] = serial_Buffer[i];
	}
	//������ڻ�������־
	serial_Buffer_Length = 0;
}

//��⴮�ڽ��յ������Ƿ�ȫ���ǿո�,�����򷵻�1,�Ƿ���0
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


//ȥ�������ַ�����ǰ��Ŀո��ַ�
void Command_Remove_Space_Head(void)
{
	u8 index = 0;
	u8 i = 0;
	for(index = 0; index < commandStringLength; index++)
	{
		if(commandStringBuffer[index] == ' ')continue;
		else break;
	}
	if(index == 0)//ǰ��û�пո�
	{
		return;
	}
	else
	{
		//ɾ���ո�
		for(i = 0; i < (commandStringLength-index);i++)
		{
			commandStringBuffer[i] = commandStringBuffer[index+i];
		}
		commandStringLength -= index;
	}
}

//ȥ�������ַ�������Ŀո�
void Command_Remove_Space_End(void)
{
	u8 i = 0;
	//Ѱ���ַ�����β���Ͽո��λ��
	for(i = commandStringLength; i > 0; i--)
	{
		if(commandStringBuffer[i-1] == ' ')continue;//�������ǿո�,������һ��Ѱ��
		else break;//���ǿո�,����Ϊֹ
	}
	if(i == commandStringLength)//β��û�пո�
	{
		return;
	}
	else		//β���пո�
	{
		commandStringBuffer[i] = '\0';
		commandStringLength = i;
		return;
	}
	
}

//ȥ�������ַ����м�Ŀո�,�����������Ŀո�ϲ���һ��
void Command_Remove_Space_Inner(void)
{
	u8 spaceCount;
	u8 i = 0;
	u8 j = 0;
	for(i = 0; i < commandStringLength; i++)
	{
		//��ʱ��⵽һ���ո�
		if(commandStringBuffer[i] == ' ')
		{
			//���̲鿴��һ���ǲ��ǿո�
			if(commandStringBuffer[i+1] == ' ')
			{
				spaceCount = 2;
				//��һ��Ҳ�ǿո�,��ʱ˵���Ѿ��������������Ŀո��˱������̲��ҵ������Ŀո�����
				for(j = i+2; j < commandStringLength; j++)
				{
					//�����ǿո��ʱ��������,�ǿո��һֱ��
					if(commandStringBuffer[j] == ' ')spaceCount++;
					else break;
				}
				//����������space��ֵ���ƶ�����,ͬʱ��С����
				//i�ǵ�һ���ո�,i+1�ǵڶ����ո�,���һ���ո���spaceCount-2
				for(j = i+1;j < commandStringLength-spaceCount+1;j++)
				{
					//Ҫ����spacecount-1������,��������Ч�ַ�
					commandStringBuffer[j] = commandStringBuffer[j+spaceCount-1];
				}
				//����޸ĳ���,�����������ǿո���-1,��Ϊ������һ���ո�
				commandStringLength -= (spaceCount-1);
			}
			else
			{
				//��һ�����ǿո�,˵����ֻ��һ���ո�Ļ���,���ò���,������һ��ѭ��
				continue;
			}
		}
		
	}
}

/***********************************************************************************************/

/***********************************���������������*******************************************/
//��ȡ�ַ�������
u8 StringGetLength(u8* string)
{
	u8 i = 0; 
	while(*(string+i) != '\0')
	{
		i++;
	}
	return i;
}

//Ѱ�������ַ����еĿո�λ��,Ҳ���ǿո���±�
//����0��ʶ�Ҳ����ո�,���кŴ�1��ʼ
u8 Command_Find_Space_Postion(u8 index)
{
	//���������Ե��Ǹ������ַ���,���������ַ���
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

//�������б��л�ȡָ�����ȵĲ����ַ���
//paramBuffer ��Ż�ȡ���Ĳ���λ��
//maxParamLength ��������󳤶�,��ȡ�ĳ��ȳ������ȡʧ��
//ParamRealLength ʵ�ʻ�ȡ���ĳ���ָ��
//ParamIndex �ڼ�������
//��ȡ�ɹ�����1 ��ȡʧ�ܷ���0
u8 CommandGetParamStr(u8* paramBuffer,u8 maxParamLength,u8* ParamRealLength,u8 ParamIndex)
{
	u8 paramStart;
	u8 paramEnd;
	u8 paramLength;
	u8 i = 0, j = 0;
	paramStart = Command_Find_Space_Postion(ParamIndex);
	if(paramStart == 0)//û�в���
	{
		return 0;
	}
	else
	{
		paramEnd = Command_Find_Space_Postion(ParamIndex+1);
		if(paramEnd == 0)
		{
			//�ַ���ֻ����һ������,��ô������β�����ַ�����β
			paramEnd = commandStringLength;
		}
		//������Ĳ��������Ƿ��������������
		paramLength = paramEnd - paramStart-1;//���ȥһ��,ȥ��һ���ո�
		if(paramLength > maxParamLength)
		{
			return 0;//��������
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

/*******************************�ַ�������ת���ӿ�***********************/

//��ʮ���Ƹ�ʽ���ַ�������ת��Ϊ��ֵ,����8λ�޷������� 
//value ����ת��ֵָ��
//index ָʾ�ڼ�������,��һ������Ϊ1 ......
//����ֵ ת���Ƿ�ɹ�,ʧ�ܷ���1 �ɹ�����0
u8 CommandGetParamToDecU8(u8* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//�Ҳ�����ô�����
	//�ҵ�֮����ݳ��ȼ���ϵ��
	//ϵ������
	for(i = 1; i < paramLength;i++)
	{
		fac *= 10;
	}
	//У��ÿ�������ַ�ֵ�Ƿ���ϱ�׼,ʮ���ƾͱ�����0-9֮��
	for(i = 0;i<paramLength;i++)
	{
		if(paramBuffer[i] > '9' || paramBuffer[i] < '0')
		{
			return 1;//��������
		}
	}
	//��ʼ����
	for(i = 0; i < paramLength;i++)
	{
		valueResult += (paramBuffer[i]-'0')*fac;
		fac/=10;
	}
	//������ս���Ƿ��������ֵ,���λ��ô������ܴ���255
	if(valueResult > 0xff)return 1;//��������
	else 
	{
		*value = (u8)valueResult;
		return 0;
	}
}

//����һ����������,���ʮ�����Ʋ���
u8 CommandGetParamToDecU16(u16* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//�Ҳ�����ô�����
	//�ҵ�֮����ݳ��ȼ���ϵ��
	//ϵ������
	for(i = 1; i < paramLength;i++)
	{
		fac *= 10;
	}
	//У��ÿ�������ַ�ֵ�Ƿ���ϱ�׼,ʮ���ƾͱ�����0-9֮��
	for(i = 0;i<paramLength;i++)
	{
		if(paramBuffer[i] > '9' || paramBuffer[i] < '0')
		{
			return 1;//��������
		}
	}
	//��ʼ����
	for(i = 0; i < paramLength;i++)
	{
		valueResult += (paramBuffer[i]-'0')*fac;
		fac/=10;
	}
	//������ս���Ƿ��������ֵ,���λ��ô������ܴ���255
	if(valueResult > 0xffff)return 1;//��������
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
	if(result == 0)return 1;//�Ҳ�����ô�����
	//�ҵ�֮����ݳ��ȼ���ϵ��
	//ϵ������
	for(i = 1; i < paramLength;i++)
	{
		fac *= 10;
	}
	//У��ÿ�������ַ�ֵ�Ƿ���ϱ�׼,ʮ���ƾͱ�����0-9֮��
	for(i = 0;i<paramLength;i++)
	{
		if(paramBuffer[i] > '9' || paramBuffer[i] < '0')
		{
			return 1;//��������
		}
	}
	//��ʼ����
	for(i = 0; i < paramLength;i++)
	{
		valueResult += (paramBuffer[i]-'0')*fac;
		fac/=10;
	}
	//������ս���Ƿ��������ֵ,���λ��ô������ܴ���255
	if(valueResult > 0xffffffff)return 1;//��������
	else 
	{
		*value = (u32)valueResult;
		return 0;
	}
}

//�������ַ����л�ȡ������ת������,��0x��ʽ���ַ���ת��Ϊ��ֵ �ɹ�����0 ʧ�ܷ���1
//�������ͱ�����0x��ͷ��
u8 CommandGetParamToHexU8(u8* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//�Ҳ�����ô�����
	//����������,��Ϊ��ͷ����Ϊ0x,���Գ��ȱ���Ϊ3����
	if(paramLength <= 2)return 1;//ʧ��
	//����ϵ��
	for(i = 3; i < paramLength; i++)
	{
		fac *= 16;				//��Ϊ0xռ���������ֽ� ��һδΪ1 ���Գ˷�����ӵ�������ʼ
	}
	//��⿪ͷ�Ƿ���ȷ 0x
	if(paramBuffer[0] == '0' &&(paramBuffer[1] == 'x'||paramBuffer[1] == 'X'))
	{
		//���ÿһλ�����Ƿ���ȷ����������ֵ
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
				//���ַ�Χ֮�������,����1
				return 1;
			}
			valueResult += (u32)(result*fac);
			fac /= 16;
		}
		//�������,�������Ƿ񳬹���Χ
		if(valueResult > 0xff)return 1;//��������
		else 
		{
			*value = (u8)valueResult;
			return 0;
		}
	}
	else
	{
		//������ͷ����
		return 1;
	}
}


//�������ַ����л�ȡ������ת������,��0x��ʽ���ַ���ת��Ϊ��ֵ �ɹ�����0 ʧ�ܷ���1
u8 CommandGetParamToHexU16(u16* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//�Ҳ�����ô�����
	//����������,��Ϊ��ͷ����Ϊ0x,���Գ��ȱ���Ϊ3����
	if(paramLength <= 2)return 1;//ʧ��
	//����ϵ��
	for(i = 3; i < paramLength; i++)
	{
		fac *= 16;//��Ϊ0xռ���������ֽ� ��һδΪ1 ���Գ˷�����ӵ�������ʼ
	}
	//��⿪ͷ�Ƿ���ȷ 0x
	if(paramBuffer[0] == '0' &&(paramBuffer[1] == 'x'||paramBuffer[1] == 'X'))
	{
		//���ÿһλ�����Ƿ���ȷ����������ֵ
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
				//���ַ�Χ֮�������,����1
				return 1;
			}
			valueResult += (u32)(result*fac);
			fac /= 16;
		}
		//�������,�������Ƿ񳬹���Χ
		if(valueResult > 0xffff)return 1;//��������
		else 
		{
			*value = (u16)valueResult;
			return 0;
		}
	}
	else
	{
		//������ͷ����
		return 1;
	}
}

//�������ַ����л�ȡ������ת������,��0x��ʽ���ַ���ת��Ϊ��ֵ �ɹ�����0 ʧ�ܷ���1
u8 CommandGetParamToHexU32(u32* value,u8 index)
{
	u8 result;
	u32 valueResult = 0;
	u8 i = 0;
	u32 fac = 1;
	result = CommandGetParamStr(paramBuffer,PARAM_COVERT_MAX_LENGTH,&paramLength,index);
	if(result == 0)return 1;//�Ҳ�����ô�����
	//����������,��Ϊ��ͷ����Ϊ0x,���Գ��ȱ���Ϊ3����
	if(paramLength <= 2)return 1;//ʧ��
	//����ϵ��
	for(i = 3; i < paramLength; i++)
	{
		fac *= 16;//��Ϊ0xռ���������ֽ� ��һδΪ1 ���Գ˷�����ӵ�������ʼ
	}
	//��⿪ͷ�Ƿ���ȷ 0x
	if(paramBuffer[0] == '0' &&(paramBuffer[1] == 'x'||paramBuffer[1] == 'X'))
	{
		//���ÿһλ�����Ƿ���ȷ����������ֵ
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
				//���ַ�Χ֮�������,����1
				return 1;
			}
			valueResult += (u32)(result*fac);
			fac /= 16;
		}
		//�������,�������Ƿ񳬹���Χ
		if(valueResult > 0xffffffff)return 1;//��������
		else 
		{
			*value = (u32)valueResult;
			return 0;
		}
	}
	else
	{
		//������ͷ����
		return 1;
	}
}




/************************************************************************/





























