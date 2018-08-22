#ifndef __DEBUGSERIAL_H_
#define __DEBUGSERIAL_H_

#include "sys.h"
#include "stdio.h"

//�����ж����ݻ�����
//���ڽ��ܻ�������󳤶�,��Ҫ����255
#define SERIAL_MAX_LENGTH	250	


//���ڽ������ݻ�����,��С���ݺ�����
extern u8 serial_Buffer[SERIAL_MAX_LENGTH];


//���ڽ������ݵ�״̬����
//0-7λ��ʶ�������ݳ���,���ΪSERIAL_MAX_LENGTH
//14λ��ʶ���յ�'\r'
//15λ��ʶ���յ�'\n',Ҳ���ǽ������
extern u16 serial_Buffer_Length;

extern u8 receiveMode;
extern u8 receiveExpectCount;//�����������ճ���



void Debug_Serial_Init(u32 baud);

void Debug_Serial_Send_Char(u8 val);

void Debug_Serial_Send_Buffer(u8* buffer,u8 length);
uint32_t SerialGetChar(uint8_t *key);


#endif










