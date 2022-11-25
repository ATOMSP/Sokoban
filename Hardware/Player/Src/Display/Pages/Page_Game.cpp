#include "../Display.h"

/*note flag*/
static bool ISbusy = false;
static bool ISwin = false;

static void show_note()
{
	ai_load_picfile(IMG_GNOTE_PATH,40,80,160,80,1);
}


/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	ISbusy = false;
	ISwin = false;
	/*loading game photo*/
	if(!Page_dat.ISsave)
	{
		Box.Load((_Level_ID_)Page_dat.Level);
	}
	else
	{
		Page_dat.ISsave = false;
		Box.Load((_Level_ID_)SAVE_DAT);
	}
	/*Backlight on*/
	Display::Adjust_light(true,BacklightAdjust_MAX/2,10);
}


/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
	if(!ISwin)
	{
		if(!ISbusy)
		{
			if(Box.Running())
			{
					/*new game*/
					Page_dat.Level ++;
					if(Page_dat.Level >= _LEVEL_NUM_) 
					{
						LOG("you over : %d\r\n",Page_dat.Level);
						/*you win page*/
						if(!ISwin)
						{
							Display::Adjust_light(false,BacklightAdjust_MAX/2,4);
							APP::Audio_PlayMusic(WIN_MUSIC);
							ai_load_picfile(MATERIAL_IMG_WIN_PATH,0,0,LCD_W,LCD_H,1);
							Display::Adjust_light(true,BacklightAdjust_MAX/2,1);
							ISwin = true;						
						}
					}
					else
					{
						Display::Adjust_light(false,BacklightAdjust_MAX/2,4);
			#if (PRINT_INFO) 
						LOG("Next Level : %d\r\n",Page_dat.Level);
			#endif   
						Box.Load((_Level_ID_)Page_dat.Level);
						Display::Adjust_light(true,BacklightAdjust_MAX/2,1);
					}
			}		
		}		
	}
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
	if(!ISwin)
	{
		if(!ISbusy)
		{
			if(btn == (ButtonEvent*)&HAL::BTNUP)
			{
				if(event == ButtonEvent::EVENT_SHORT_CLICKED)
				{
					APP::Audio_PlayMusic(BUTTON_MUSIC);
					Box.Info.offest_x = -1 ;
					Box.Info.offest_y = 0;         
				}
			}
			if(btn == (ButtonEvent*)&HAL::BTNDOWN)
			{
				if(event == ButtonEvent::EVENT_SHORT_CLICKED)
				{
					APP::Audio_PlayMusic(BUTTON_MUSIC);
					Box.Info.offest_x = 1 ;
					Box.Info.offest_y = 0;  
				}
			}
			if(btn == (ButtonEvent*)&HAL::BTNLEFT)
			{
				if(event == ButtonEvent::EVENT_SHORT_CLICKED)
				{
					APP::Audio_PlayMusic(BUTTON_MUSIC);
					Box.Info.offest_x = 0 ;
					Box.Info.offest_y = -1;  
				}
			}
			if(btn == (ButtonEvent*)&HAL::BTNRIGHT)
			{
				if(event == ButtonEvent::EVENT_SHORT_CLICKED)
				{
					APP::Audio_PlayMusic(BUTTON_MUSIC);
					Box.Info.offest_x = 0 ;
					Box.Info.offest_y = 1;  
				}
			}
		}
/*长按exit弹出提示框 短按exit 保存退出 短按secl再逛逛 长按secl 直接退出*/
		if(btn == (ButtonEvent*)&HAL::BTNEXIT)
		{
			if(event == ButtonEvent::EVENT_LONG_PRESSED)
			{
				APP::Audio_PlayMusic(BUTTON_LONG_MUSIC);
				/*弹出提示框: 保存退出  直接退出 再逛逛*/
				ISbusy = true;
				show_note();
			}
			if(ISbusy)
			{
				if(event == ButtonEvent::EVENT_SHORT_CLICKED)
				{
					APP::Audio_PlayMusic(BUTTON_MUSIC);
					ISbusy = false;
					/*再逛逛 刷新screen go on play game*/
					for (int i = 0; i < MATERIAL_MAP_W; i++)
					{
						for (int k = 0; k < MATERIAL_MAP_H; k++)
						{
							Box.Draw_Single_Block(i,k);
						}
					}
				}					
			}

		}	
		
		if(btn == (ButtonEvent*)&HAL::BTNSECL)
		{
			/*active button*/
			if(ISbusy)
			{
				if(event == ButtonEvent::EVENT_SHORT_CLICKED)
				{
					APP::Audio_PlayMusic(BUTTON_MUSIC);
					/*直接退出：*/
					ISbusy = false;
					Display::page.PagePush(PAGE_Level);
				}
				if(event == ButtonEvent::EVENT_LONG_PRESSED)
				{
					APP::Audio_PlayMusic(BUTTON_LONG_MUSIC);
					/*保存退出： Isbuy = false 清屏 保存数据：地图+ 关卡*/
					ISbusy = false;
					Page_dat.Level = Page_dat.Level + '0';
					APP::File_Write(MATERIAL_LEVELDAT_PATH,(void*)&Page_dat.Level,1);
					APP::RamSendToSd(MATERIAL_MAP_SAVE_PATH,(void*)Box.Info.Game_MAP);
					Display::page.PagePush(PAGE_Level);

				}			
			}
		}
	}
	else
	{
		ISwin = false;
		Display::page.PagePush(PAGE_Master);
	}

}


void PageRegister_Game(uint8_t pageID)
{
    Display::page.PageRegister(pageID,Setup,Loop,Exit,Event);
}



