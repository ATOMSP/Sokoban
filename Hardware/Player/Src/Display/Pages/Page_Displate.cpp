#include "../Display.h"


/**
 * @brief 创建进度条
 * 
 */
static void Create_Bar(int x,int y,int len,uint16_t color,uint16_t tim,const char* str)
{
    Display::lcd.setCursor(x,y - 15);
    Display::lcd.setTextColor(screen::White);
    Display::lcd.setTextSize(1);
    Display::lcd.println(str);
    for (int i = 0; i < len; i++)
    {
        Display::lcd.fillRoundRect(x,y,i,5,3,color);
        delay_ms(tim);
    }       
} 
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*Backlight on*/
    Display::Adjust_light(true,BacklightAdjust_MAX/2,5);
    /*create the bar*/
    Create_Bar(30,190,180,screen::Red,10,"Loading......");
    /*Backlight off*/
	Display::Adjust_light(false,BacklightAdjust_MAX/2,5);
    /*Load the photo*/
    ai_load_picfile(IMG_GIRON_PATH,0,0,LCD_W,LCD_H,1);    
    /*Backlight on*/
	Display::Adjust_light(true,BacklightAdjust_MAX/2,5);

}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
	/*Backlight off*/
	Display::Adjust_light(false,BacklightAdjust_MAX/2,5);
    Display::lcd.fillScreen(screen::Black);
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(void* btn,int event)
{
	if(btn == (ButtonEvent*)&HAL::BTNSECL)
	{
		if(event == ButtonEvent::EVENT_LONG_PRESSED)
		{
			/*进入新页面*/
      APP::Audio_PlayMusic(BUTTON_LONG_MUSIC);
      Display::page.PagePush(PAGE_Master);
		}		
	}	
}


void PageRegister_Displate(uint8_t pageID)
{
    Display::page.PageRegister(pageID,Setup,NULL,Exit,Event);
}










