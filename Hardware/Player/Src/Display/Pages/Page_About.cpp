#include "../Display.h"

static bool en = true;

static void Background()
{
    Display::lcd.fillScreen(screen::Black);
    Display::lcd.fillRoundRect(20,35,200,190,10,screen::White);
    Display::lcd.fillRoundRect(23,40,194,36,5,screen::Red);
    Display::lcd.fillRoundRect(23,85,194,132,5,screen::Red);
    Display::lcd.setTextColor(screen::White);
    Display::lcd.setTextSize(1);
    Display::lcd.setCursor(90, 14);
    Display::lcd.setFont(&FreeMonoBold9pt7b);
    Display::lcd.printf("About");   
    Display::lcd.setTextColor(screen::Black);
    if(en)
    {
        Display::lcd.setTextSize(1); 
        Display::lcd.setCursor(30,60);
        Display::lcd.printf("Volume : ON");

    }
    else
    {
        Display::lcd.setTextSize(1); 
        Display::lcd.setCursor(30,60);
        Display::lcd.printf("Volume : OFF");

    }
    Display::lcd.setFont(&FreeMono9pt7b);
    Display::lcd.setCursor(30,100);
    Display::lcd.println("Device :Player");    
    Display::lcd.setCursor(30,115);
    Display::lcd.println("Version:v0.0.3");
    Display::lcd.setCursor(30,130);
    Display::lcd.println("Auther :SunPeng");
    Display::lcd.setCursor(30,145);
    Display::lcd.println("Date   :22-11-24");
    Display::lcd.setCursor(30,160);
    Display::lcd.println("SD :29.7GB");
    ai_load_picfile(IMG_CAT_PATH,160,175,53,40,1);
    Display::lcd.drawRect(159,174,55,42,screen::Black);
}
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	/*Backlight on*/
  Background();
	Display::Adjust_light(true,BacklightAdjust_MAX/2,10);
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
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
      if(en)
      {
        APP::Audio_PlayMusic(BUTTON_MUSIC);
        Display::lcd.setTextColor(screen::Black);
        Display::lcd.setFont(&FreeMonoBold9pt7b);
        Display::lcd.setTextSize(1); 
        Display::lcd.setCursor(30,60);
        Display::lcd.fillRect(120,40,50,30,screen::Red);
        Display::lcd.printf("Volume : OFF");
        en = false;    
        APP::Buzz_SetEnable(false);      
      }

		}		
	}	
	if(btn == (ButtonEvent*)&HAL::BTNEXIT)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
      if(!en)
      {
        APP::Audio_PlayMusic(BUTTON_MUSIC);
        Display::lcd.setTextColor(screen::Black);
        Display::lcd.setFont(&FreeMonoBold9pt7b);
        Display::lcd.setTextSize(1); 
        Display::lcd.setCursor(30,60);
        Display::lcd.fillRect(120,40,50,30,screen::Red);
        Display::lcd.printf("Volume : ON");
        en = true;        
        APP::Buzz_SetEnable(true);        
      }
		}		
		if(event == ButtonEvent::EVENT_LONG_PRESSED)
		{
      APP::Audio_PlayMusic(BUTTON_LONG_MUSIC);
      Display::page.PagePop();
		}		
	}	
}


void PageRegister_About(uint8_t pageID)
{
    Display::page.PageRegister(pageID,Setup,NULL,Exit,Event);
}








