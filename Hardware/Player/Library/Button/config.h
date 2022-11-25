/****************************************************************************
* @File name: config.h
* @Author: sunpeng
* @Version: v0.0.1
* @Date: 2022.11.11
* @Description: ButtonEvent add other platform for users
****************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

/**
*   ButtonEvent 使用方法与注意事项
*   1、
*   2、
*   3、
*   4、
**/



/*config use arduino  yes(1) no(0)*/
#define USE_ARDUINO                 (1)     
#if (USE_ARDUINO)
#ifndef ARDUINO
#define ARDUINO
#endif
#ifdef  ARDUINO
#  include "Arduino.h"
#  define GET_TICK() millis()
#endif
#endif


/*config use user mills()  yes(1) no(0)*/
#define USE_USER                    (0)
#if (USE_USER)
#define USER_HEAD                   "demo.h"/*add user head file with mills()*/
#  include      USER_HEAD
#  define       GET_TICK() millis()
#endif


/*no func for mills()*/
#ifndef GET_TICK
#  error "Please define the GET_TICK() function to get the system time "
#endif

/*lib param*/

#  define UINT32_MAX  4294967295u



#endif 
