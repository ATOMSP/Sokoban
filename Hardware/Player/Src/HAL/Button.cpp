/****************************************************************************
* @File name: Button.cpp
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: The underlying driver of the key, using the key management 
* library, provides a callback function for the key, and the key must share
* all resources
****************************************************************************/
/*button header*/
#include "Button.h"
/*Physical layer pins*/
#include "../config.h"

/*Display to provide pages*/
#include "../Display/Display.h"

/*按键实例化*/
ButtonEvent HAL::BTNUP;
ButtonEvent HAL::BTNDOWN;
ButtonEvent HAL::BTNLEFT;
ButtonEvent HAL::BTNRIGHT;
ButtonEvent HAL::BTNSECL;
ButtonEvent HAL::BTNEXIT;
/*按键实例化*/

/**
 * @brief button callback
 * 
 */
void Button_CallBack(ButtonEvent* btn, int event)
{
    Display::page.PageEventTransmit(btn,event);
}


/**
 * @brief Button HAL Init 
 * 
 */
void HAL::Button_Init(void)
{
#if (PRINT_INFO)
    LOG("->Button_Init ......\r\n");
#endif 
    pinMode(BUTTON_UP,INPUT_PULLUP);
    pinMode(BUTTON_DOWN,INPUT_PULLUP);
    pinMode(BUTTON_LEFT,INPUT_PULLUP);
    pinMode(BUTTON_RIGHT,INPUT_PULLUP);
    pinMode(BUTTON_SECL,INPUT_PULLUP);
    pinMode(BUTTON_EXIT,INPUT_PULLUP);
    BTNUP.EventAttach(Button_CallBack);
    BTNDOWN.EventAttach(Button_CallBack);
    BTNLEFT.EventAttach(Button_CallBack);
    BTNRIGHT.EventAttach(Button_CallBack);
    BTNSECL.EventAttach(Button_CallBack);  
    BTNEXIT.EventAttach(Button_CallBack);  
#if (PRINT_INFO)
    LOG("->Button_Init success\r\n");
#endif 
    
}

/**
 * @brief Button scan (10ms is needed)
 * 
 */
void HAL::Button_Scan(void)
{
    BTNSECL.EventMonitor((bool)(digitalRead_FAST(BUTTON_SECL)   == LOW));
    BTNUP.EventMonitor((bool)(digitalRead_FAST(BUTTON_UP)       == LOW));
    BTNDOWN.EventMonitor((bool)(digitalRead_FAST(BUTTON_DOWN)   == LOW));
    BTNLEFT.EventMonitor((bool)(digitalRead_FAST(BUTTON_LEFT)   == LOW));
    BTNRIGHT.EventMonitor((bool)(digitalRead_FAST(BUTTON_RIGHT) == LOW));
    BTNEXIT.EventMonitor((bool)(digitalRead_FAST(BUTTON_EXIT)   == LOW));
}















