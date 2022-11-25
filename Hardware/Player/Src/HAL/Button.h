/****************************************************************************
* @File name: Button.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: The underlying driver of the key, using the key management 
* library, provides a callback function for the key, and the key must share
* all resources
****************************************************************************/
#ifndef BUTTON_H__
#define BUTTON_H__

/*ArduinoAPI */
#include "Arduino.h"
/*extern libraries*/
#include "ExternLib.h"


/*Hal button API*/
namespace HAL 
{
    extern ButtonEvent BTNUP;
    extern ButtonEvent BTNDOWN;
    extern ButtonEvent BTNLEFT;
    extern ButtonEvent BTNRIGHT;
    extern ButtonEvent BTNSECL;
    extern ButtonEvent BTNEXIT;
    void Button_Init();
    void Button_Scan();
}




#endif // !BUTTON_H__

