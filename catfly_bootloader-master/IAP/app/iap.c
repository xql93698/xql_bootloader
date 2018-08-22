#include "iap.h"

#define FLASH_APP1_ADDR		0x08006000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
											//�����Ŀռ�ΪIAPʹ��

u16 iapbuf[1024] = {0}; //���ڻ������ݵ�����
u16 receiveDataCur = 0;	//��ǰiapbuffer���Ѿ��������ݳ���,һ���������֮��д��flash������
u32 addrCur = FLASH_APP1_ADDR;			//��ǰϵͳд���ַ,ÿ��д��֮���ַ����2048

//��ʼ����
void iap_down_s(void)
{
	u16 i = 0;
	u16 temp = 0;
	u16 receiveCount;
	printf("begin,wait data download\r\n");
	receiveMode = 1;//���ڽ������ؽ�������ģʽ
	while(1)
	{
		//ѭ����������,ÿ�α���Ҫ��128����������,���û��128,˵���������һ������
		//���յ�һ������֮��,����һ��С����,�������,ϵͳ������֮�󷵻�һ��iap_over
		if(serial_Buffer_Length & 0x8000)
		{
			receiveCount = (u8)(serial_Buffer_Length&0x00ff);
			if(receiveCount == 128)//����һ��,��䲢�鿴�Ƿ�����1024�ֽ�,����д������
			{
				for(i = 0; i < receiveCount; i+=2)
				{
					//���ݰ�λ�ں�Ϊ16λ
					temp = (((u16)serial_Buffer[i+1])<<8) + ((u16)serial_Buffer[i]);
					iapbuf[receiveDataCur] = temp;
					receiveDataCur++;//���֮��receiveDataCur++;
				}
				receiveExpectCount = 0;//�����������ģʽ
				serial_Buffer_Length = 0;//�����������־
				printf(".");//ÿ�ν���һ�����ݴ�һ���� 
				//��ʱ��Ҫ���receiveDataCur��ֵ,Ҫ�Ƿ�����,����Ҫд��
				if(receiveDataCur == 1024)
				{
					//д��flash��
					STMFLASH_Write(addrCur,iapbuf,1024);
					//printf("\r\nwrite addr %x,length 1024\r\n",addrCur);
					addrCur += 2048;//��ַ+2048
					//д��֮��receiveDataCurҪ����ȴ���һ�δ���
					receiveDataCur = 0;
				}
				else //�п������һ����128�����ݵ�������û��2048������,��ʱ��չһ��ָ������������һ����д��
				{
					
				}
				//��û����,�ȴ���һ�����ݹ���
			}
			else	//������һ��,˵�����ݴ����������һ��,д������
			{
				//û��һ��ҲҪ���͵�������
				for(i = 0; i < receiveCount; i+=2)
				{
					//���ݰ�λ�ں�Ϊ16λ
					temp = (((u16)serial_Buffer[i+1])<<8) + ((u16)serial_Buffer[i]);
					iapbuf[receiveDataCur] = temp;
					receiveDataCur++;//���֮��receiveDataCur++;
				}
				receiveExpectCount = 0;//�����������ģʽ
				serial_Buffer_Length = 0;//�����������־
				printf(".");//ÿ�ν���һ�����ݴ�һ���� 
				//֮���Ҫ��������д�뵽������
				STMFLASH_Write(addrCur,iapbuf,receiveDataCur);//������һЩ�����ֽ�д��ȥ.
				//printf("\r\nwrite addr %x,length %d\r\n",addrCur,receiveDataCur);
				//д��֮��Ҫ�ѵ�ַ�ָ���ԭ����λ��
				addrCur = FLASH_APP1_ADDR;
				receiveDataCur = 0;
				//д��֮��Ҫ�˳�����ѭ����������λ��,�Ѿ���������
				printf("download over\r\n");
				//ͬʱ,ҲҪ�˳�����ѭ��ģʽ
				receiveMode = 0;
				return;
			}
		}
	}
}
//���һ����128�����ݵ�������û��2048������
//�յ����ָ����receiveDataCur��addrCur��ֵ,
//�������д��
void iap_over_s(void)
{
	//���ʱ��,��Ȼ�ڴ�������ģʽ
	if(receiveDataCur != 0)
	{
		STMFLASH_Write(addrCur,iapbuf,receiveDataCur);//������һЩ�����ֽ�д��ȥ.
		//printf("write addr %x,length %d",addrCur,receiveDataCur);
		addrCur = FLASH_APP1_ADDR;
		receiveDataCur = 0;
		//ͬʱ,ҲҪ�˳�����ģʽ
		receiveMode = 0;
	}
	printf("ok\r\n");
}


typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.
iapfun jump2app;

//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.0x20000000��sram����ʼ��ַ,Ҳ�ǳ����ջ����ַ
	{ 
		//printf("ok\r\n");
		//Delay_Ms(10);
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
	else
	{
	//	printf("program in flash is error\r\n");
	}
}

//��ת��app��������
void iap_jump_app_s(void)
{
	iap_load_app(FLASH_APP1_ADDR);//��ת��app�ĸ�λ������ַ
}



//����app�̻�����
void iap_set_flag_s(void)
{
	Test_Write(APP_CONFIG_ADDR,APP_CONFIG_SET_VALUE);
	printf("ok\r\n");
}

//���app�̻�����
void iap_clear_flag(void)
{
	Test_Write(APP_CONFIG_ADDR,APP_CONFIG_CLEAR_VALUE);
	printf("ok\r\n");
}
















