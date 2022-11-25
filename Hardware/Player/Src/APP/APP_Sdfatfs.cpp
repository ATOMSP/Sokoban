/****************************************************************************
* @File name: APP_Sdfatfs.cpp
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: File system application layer interface for use by other programs
****************************************************************************/



/*header*/
#include "APP_Sdfatfs.h"
/*Physical layer pins*/
#include "../config.h"
UINT APP::Byte_Read;
UINT APP::Byte_written;

/* 
 @brief 读文件
 @param path 文件绝对路径
 @param buf  文件内容
 @param len  读取长度
 @return 
  */
bool APP::File_Read(Path path,uint8_t* buf,UINT len)
{
    FRESULT res;
    res = f_open(&HAL::f,path,FA_READ);
    if(res)
    {
#if (PRINT_INFO)
        LOG("->Open File Failed-Error Code: %d\r\n",res);
#endif
        return false;
    }
    res = f_read(&HAL::f,(uint8_t*)buf,len,&APP::Byte_Read);
    if(res)
    {
#if (PRINT_INFO)
        LOG("->Read File Failed-Error Code: %d\r\n",res);
#endif
        return false;
    }
#if (PRINT_INFO)
    LOG("->Read success! \r\nBytes :%d\r\n",(int)APP::Byte_Read); 
#endif
    f_close(&HAL::f);
    return true;
}
/* 
 @brief 读文件
 @param path 文件绝对路径
 @param buf 文件写入内容
 @param len 内容长度
 @return 
  */
bool APP::File_Write(Path path,void* buf,UINT len)
{
    FRESULT res;
    res = f_open(&HAL::f,path,FA_WRITE|FA_OPEN_ALWAYS);
    if(res)
    {
#if (PRINT_INFO)
        LOG("->Open File Failed-Error Code: %d\r\n",res);
#endif
        return false;
    }
    res = f_write(&HAL::f,(const void*)buf,len,&APP::Byte_written);
    if(res)
    {
#if (PRINT_INFO)
        LOG("->Write File Failed-Error Code: %d\r\n",res);
#endif
        return false;
    }
#if (PRINT_INFO)
    LOG("Written success! \r\nBytes :%d\r\n",(int)APP::Byte_written);
#endif
    f_close(&HAL::f);
    return true;
}



/**
 * @brief copy dat from sd to ram
 * path  : sd path
 * ptr   : ram addr
 */
bool APP::SdSendToRam(const char* path,void * ptr)
{
    uint8_t buf[100];
    uint8_t *p = buf;
    APP::File_Read(path,(uint8_t*)p,100);  
    for(int i = 0; i < 100; i++)
    {   /*出现非法字符*/
        if((uint8_t)(*(p + i)) == 0)
        {
#if (PRINT_INFO)
            LOG("SD_COPY Failed-Code Error\r\n");
#endif
            return false;
        }
        *((uint8_t *)ptr+i) = (uint8_t)(*(p + i) - '0');
    }
    return true;
   
}

/**
 * @brief copy dat from ram to sd
 * path  : sd path
 * ptr   : ram addr
 */
void APP::RamSendToSd(const char* path,void * ptr)
{
    char buff[100];
    char *p = buff;
    for (int i = 0; i < 100; i++)
    {
        sprintf(((char*)p+i),"%d",*((uint8_t*)ptr+i));
    }
    APP::File_Write(path,(void*)p,100);  
}






