/****************************************************************************
* @File name: APP_Sdfatfs.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: File system application layer interface for use by other programs
****************************************************************************/
#ifndef APP_SDFATFS_H__
#define APP_SDFATFS_H__


/*HAL  */
#include "../HAL/HAL.h"
/*extern libraries*/
#include "ExternLib.h"

typedef const TCHAR* Path;


namespace APP
{
    /*status v*/
    extern UINT Byte_Read;
    extern UINT Byte_written;
    bool File_Read(Path path,uint8* buf,UINT len);
    bool File_Write(Path path,void* buf,UINT len);
    void RamSendToSd(const char* path,void * ptr);
    bool SdSendToRam(const char* path,void * ptr);
}


















#endif



