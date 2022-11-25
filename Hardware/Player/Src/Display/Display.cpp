/****************************************************************************
* @File name: Display.cpp
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: Display initialization
****************************************************************************/
#include "Display.h"
/*Physical layer pins*/
#include "../config.h"


/*backlight*/
int8_t Display::Light;
/*screen obj*/
screen Display::lcd(Pin_CS,Pin_DC,Pin_RST);

/*=====================================================PAGES===========================================*/

/*ҳ�湲������*/
Pages_Public_Info Page_dat;
/*ʵ����ҳ�������*/
PageManager Display::page(PAGE_MAX,PAGE_STACK_LEN);
/*ҳ��ע����*/
#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(uint8_t pageID);\
    PageRegister_##name(PAGE_##name);\
}while(0)
/**
  * @brief  ҳ���ʼ��
  * @param  ��
  * @retval ��
  */
static void Pages_Init()
{
    PAGE_REG(Displate);     //��������
    PAGE_REG(Master);       //������
    PAGE_REG(Level);        //�ؿ�
    PAGE_REG(Game);         //��Ϸ
    PAGE_REG(About);        //����
    Display::page.PagePush(PAGE_Displate);//�򿪿�������
}
/*=====================================================PAGES===========================================*/


/*=====================================================GAME============================================*/
void Draw_hook(const uint8_t* path,int x,int y,int w,int h)
{
    ai_load_picfile(path,x,y,w,h,1);
}
bool Load_hook(const uint8_t* path,void * ptr)
{
    return APP::SdSendToRam((const char*)path,ptr);
}
PTBox_Manager Box(Draw_hook,Load_hook);
/*=====================================================GAME============================================*/
/**
 * @brief Display init
 * 
 */
void Display::Display_Init()
{
    /*backlight Init*/
    PWM_Init(Pin_BLK,BacklightAdjust_MAX,Backlight_FRE);
    analogWrite(Pin_BLK,0);
    /*lcdӲ����ʼ��*/
    lcd.begin();
    lcd.fillScreen(screen::Black);
    /*ҳ���ʼ��*/
    Pages_Init();

#if (PRINT_INFO)
    LOG("->Lcd Load success\r\n");      
#endif
}

/**
 * @brief adjust backlight val
 * en : 1:adjust brighter  2: adjust darker
 * val : adjust val
 * tim : adjust tim (ms)
 */
void Display::Adjust_light(bool en,int8_t val,uint16_t tim)
{
    int i;
    val = (en) ? Display::Light + val : Display::Light - val;
    val = (val > BacklightAdjust_MAX ) ? BacklightAdjust_MAX : val;
    val = (val < 0) ? 0 : val;
    if(en){
        for (i = Display::Light; i <= val; i++)
        {
            analogWrite(Pin_BLK,i);
            delay_ms(tim);
        }     
        Display::Light = i - 1;   
    }
    else{
        for (i = Display::Light; i >= val; i--)
        {
            analogWrite(Pin_BLK,i);
            delay_ms(tim);
        }    
        Display::Light = i + 1;  
    }
#if (PRINT_INFO)
    LOG("->Adjust_light :%d\r\n",Display::Light);      
#endif
}

/**
 * @brief running to update screen
 * 
 */
void Display::Running()
{
    page.Running();
}









