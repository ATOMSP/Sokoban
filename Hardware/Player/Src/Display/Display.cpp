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

/*页面共享数据*/
Pages_Public_Info Page_dat;
/*实例化页面调度器*/
PageManager Display::page(PAGE_MAX,PAGE_STACK_LEN);
/*页面注册器*/
#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(uint8_t pageID);\
    PageRegister_##name(PAGE_##name);\
}while(0)
/**
  * @brief  页面初始化
  * @param  无
  * @retval 无
  */
static void Pages_Init()
{
    PAGE_REG(Displate);     //开机界面
    PAGE_REG(Master);       //主界面
    PAGE_REG(Level);        //关卡
    PAGE_REG(Game);         //游戏
    PAGE_REG(About);        //关于
    Display::page.PagePush(PAGE_Displate);//打开开机界面
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
    /*lcd硬件初始化*/
    lcd.begin();
    lcd.fillScreen(screen::Black);
    /*页面初始化*/
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









