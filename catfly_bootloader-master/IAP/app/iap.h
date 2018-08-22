#ifndef __IAP_H_
#define __IAP_H_
#include "flash.h"
#include "debugSerial.h"
#include "sys.h"
#include "delay.h"


//0x08000000 flash��ʼ��ַ
//0x2000 iap���볤��
//0x1ffc-0x1fff��Ϊ�̻����ô���

#define APP_CONFIG_ADDR 		0X08001FFC	//���õ�ַ
#define APP_CONFIG_SET_VALUE	0X5555	//����ֵ
#define APP_CONFIG_CLEAR_VALUE	0XFFFF	//����ֵ

void iap_down_s(void);

void iap_jump_app_s(void);

void iap_over_s(void);

//����app�̻�����
void iap_set_flag_s(void);

//���app�̻�����
void iap_clear_flag(void);


#endif


















