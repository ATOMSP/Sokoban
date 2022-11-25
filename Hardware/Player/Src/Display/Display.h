/****************************************************************************
* @File name: Display.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: Display initialization
****************************************************************************/
#ifndef DISPLAY_H__
#define DISPLAY_H__

/*ArduinoAPI */
#include "Arduino.h"
/*extern libraries*/
#include "ExternLib.h"
/*APP API*/
#include "../APP/APP.h"
/*=========================Page=========================*/
typedef enum
{
    /*����*/
    PAGE_NONE,
    /*�û�ҳ��*/
    PAGE_Displate,      /*��������Ļ*/
    PAGE_Master,        /*��Ϸ������*/
    PAGE_Level,         /*��Ϸ�ؿ�*/
    PAGE_Game,          /*��Ϸ����*/
    PAGE_About,         /*��Ϸ����*/
    /*����*/
    PAGE_MAX
} Page_Type;

typedef struct
{
    /*current level*/
    uint8_t Level;
    /* is load saved */
    bool ISsave;
}Pages_Public_Info;
extern Pages_Public_Info Page_dat;
/*=========================Page=========================*/

/*=========================GAMES========================*/
extern PTBox_Manager Box;
/*=========================GAMES========================*/

namespace Display
{
    /*backlight*/
    extern int8_t Light;
    /*screen obj*/
    extern screen lcd;
    /*ʵ����ҳ�������*/
    extern PageManager page;
    /*Display Init */
    void Display_Init();
    /*adjust light*/
    void Adjust_light(bool en,int8_t val,uint16_t tim);
    /*running*/
    void Running();
}



#endif // !DISPLAY_H__
