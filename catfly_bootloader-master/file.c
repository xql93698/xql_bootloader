#include "file.h"
#include <string.h>

uint8_t read_bin_file(struct file_ops *fops,char *filename)    //读取 bin 文件
{
    //TODO assert file name and size

    if(filename == NULL)                                       //检查文件名是否为空，即是否存在 bin 文件
    {
        printf("no image file.\n");
        return -1;
    }

    fops->fp = fopen(filename,"r");                            //打开一个名为 filename 的文件，并将此文件返送给FILE类型的指针fp
    if(fops->fp<0)
    {
        printf("open the image file failed...\n");
        return -1;
    }

    //get file size
    fseek(fops->fp,0,SEEK_END);                                //将读写位置移动到文件末尾
    fops->fileSize = ftell(fops->fp);                          //与fseek共用，ftell获取文件读写指针的当前位置，返回文件的长度（以字节为单位）
    fseek(fops->fp,0,SEEK_SET);                                //把读写指针定位到文件的开头，开始读取文件

    //malloc memory
    fops->buf = malloc(fops->fileSize);                        //为 buf分配 fileSize的内存空间（以字节为单位），用于存放文件

    if(fops->buf == NULL)
    {
        printf("malloc buffer failed...\n");                   //需自行判断内存是否分配成功
        return -1;
    }

    if(fread(fops->buf,1,fops->fileSize,fops->fp) != fops->fileSize)  //从文件流 fp 中读取数据到 buf 中，fread的返回值为单元个数，若单元个数不相等，表示读取失败。
    {
        printf("read file failed...\n");
        return -1;
    }
    printf("read file SUCCESS...\n");                                 //读取成功！

    return 0;

}

uint16_t file_create(struct file_ops *fops,frame_file_t *file_frame)     //获取文件大小函数
{
    uint16_t len=MAX_FILE_LEN;                               //最大文件长度
    uint16_t i;

    if( fops->fileIndex*MAX_FILE_LEN > fops->fileSize)       //此时的fileIndex(文件索引号)=0, 判断是否存在 bin 文件
        return 0;

    if( (fops->fileIndex+1)*MAX_FILE_LEN > fops->fileSize)    //若文件长度在最大文件长度之内
        len = fops->fileSize - fops->fileIndex*MAX_FILE_LEN;  //将文件长度 赋给 len

	printf("len is %d index is %d",len,fops->fileIndex);
    file_frame->Sequence = fops->fileIndex;                  //文件序列号
    file_frame->length = len;                                //文件长度

    memcpy(file_frame->buf,fops->buf+fops->fileIndex*MAX_FILE_LEN,len); //将fops中的buf数组中的前len个字符的内容复制到file_frame中的buf数组中
    fops->fileIndex++;                                       //文件索引加1



    return sizeof(frame_file_t)+len-MAX_FILE_LEN;             //返回文件大小

}

void file_close(struct file_ops *fops)
{
    free(fops->buf);
    fops->buf=NULL;
    fclose(fops->fp);

}

file_ops __file_ops=
{
NULL,                                    //创建的文件流表示空指针
NULL,                                    //从文件流中读取数据，使之存放在 buf中，初始为空指针
0,                                       //fileSize 文件大小（字节为单位）
0,
read_bin_file,                           //打开 bin 文件
file_create,                             //如定义时所说
file_close
};
