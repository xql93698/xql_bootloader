#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "frame_phase.h"


typedef struct file_ops{
FILE* fp;                 //�����ļ��������ڶ�ȡ����
char* buf;                //���ļ����ж�ȡ���ݣ�ʹ֮����� buf ������ָ����
uint32_t fileSize;        //�ļ���С�����ֽ�Ϊ��λ
uint16_t fileIndex;       //�ļ������ţ���0��ʼ����������seq

uint8_t  (*open)(struct file_ops *fops,char *filename);     //���ڶ�ȡ bin �ļ�
uint16_t (*create)(struct file_ops *fops,frame_file_t *fileframe);//�ļ���С
void     (*close)(struct file_ops *fops);                         //�ر��ļ����ڳ�������ش�����֮��

}file_ops;

extern file_ops __file_ops;
#endif // FILE_H_INCLUDED
