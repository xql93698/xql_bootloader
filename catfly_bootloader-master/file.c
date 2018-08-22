#include "file.h"
#include <string.h>

uint8_t read_bin_file(struct file_ops *fops,char *filename)    //��ȡ bin �ļ�
{
    //TODO assert file name and size

    if(filename == NULL)                                       //����ļ����Ƿ�Ϊ�գ����Ƿ���� bin �ļ�
    {
        printf("no image file.\n");
        return -1;
    }

    fops->fp = fopen(filename,"r");                            //��һ����Ϊ filename ���ļ����������ļ����͸�FILE���͵�ָ��fp
    if(fops->fp<0)
    {
        printf("open the image file failed...\n");
        return -1;
    }

    //get file size
    fseek(fops->fp,0,SEEK_END);                                //����дλ���ƶ����ļ�ĩβ
    fops->fileSize = ftell(fops->fp);                          //��fseek���ã�ftell��ȡ�ļ���дָ��ĵ�ǰλ�ã������ļ��ĳ��ȣ����ֽ�Ϊ��λ��
    fseek(fops->fp,0,SEEK_SET);                                //�Ѷ�дָ�붨λ���ļ��Ŀ�ͷ����ʼ��ȡ�ļ�

    //malloc memory
    fops->buf = malloc(fops->fileSize);                        //Ϊ buf���� fileSize���ڴ�ռ䣨���ֽ�Ϊ��λ�������ڴ���ļ�

    if(fops->buf == NULL)
    {
        printf("malloc buffer failed...\n");                   //�������ж��ڴ��Ƿ����ɹ�
        return -1;
    }

    if(fread(fops->buf,1,fops->fileSize,fops->fp) != fops->fileSize)  //���ļ��� fp �ж�ȡ���ݵ� buf �У�fread�ķ���ֵΪ��Ԫ����������Ԫ��������ȣ���ʾ��ȡʧ�ܡ�
    {
        printf("read file failed...\n");
        return -1;
    }
    printf("read file SUCCESS...\n");                                 //��ȡ�ɹ���

    return 0;

}

uint16_t file_create(struct file_ops *fops,frame_file_t *file_frame)     //��ȡ�ļ���С����
{
    uint16_t len=MAX_FILE_LEN;                               //����ļ�����
    uint16_t i;

    if( fops->fileIndex*MAX_FILE_LEN > fops->fileSize)       //��ʱ��fileIndex(�ļ�������)=0, �ж��Ƿ���� bin �ļ�
        return 0;

    if( (fops->fileIndex+1)*MAX_FILE_LEN > fops->fileSize)    //���ļ�����������ļ�����֮��
        len = fops->fileSize - fops->fileIndex*MAX_FILE_LEN;  //���ļ����� ���� len

	printf("len is %d index is %d",len,fops->fileIndex);
    file_frame->Sequence = fops->fileIndex;                  //�ļ����к�
    file_frame->length = len;                                //�ļ�����

    memcpy(file_frame->buf,fops->buf+fops->fileIndex*MAX_FILE_LEN,len); //��fops�е�buf�����е�ǰlen���ַ������ݸ��Ƶ�file_frame�е�buf������
    fops->fileIndex++;                                       //�ļ�������1



    return sizeof(frame_file_t)+len-MAX_FILE_LEN;             //�����ļ���С

}

void file_close(struct file_ops *fops)
{
    free(fops->buf);
    fops->buf=NULL;
    fclose(fops->fp);

}

file_ops __file_ops=
{
NULL,                                    //�������ļ�����ʾ��ָ��
NULL,                                    //���ļ����ж�ȡ���ݣ�ʹ֮����� buf�У���ʼΪ��ָ��
0,                                       //fileSize �ļ���С���ֽ�Ϊ��λ��
0,
read_bin_file,                           //�� bin �ļ�
file_create,                             //�綨��ʱ��˵
file_close
};
