#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "frame_phase.h"


typedef struct file_ops{
FILE* fp;                 //创建文件流，用于读取数据
char* buf;                //从文件流中读取数据，使之存放在 buf 的数组指针中
uint32_t fileSize;        //文件大小，以字节为单位
uint16_t fileIndex;       //文件索引号（从0开始），类似于seq

uint8_t  (*open)(struct file_ops *fops,char *filename);     //用于读取 bin 文件
uint16_t (*create)(struct file_ops *fops,frame_file_t *fileframe);//文件大小
void     (*close)(struct file_ops *fops);                         //关闭文件（在超过最大重传次数之后）

}file_ops;

extern file_ops __file_ops;
#endif // FILE_H_INCLUDED
