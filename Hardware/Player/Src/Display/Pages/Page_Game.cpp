#include "../Display.h"

/*note flag*/
static bool ISbusy = false;
static bool ISwin = false;

static void show_note()
{
	ai_load_picfile(IMG_GNOTE_PATH,40,80,160,80,1);
}


/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
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
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
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
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
	/*Backlight off*/
	Display::Adjust_light(false,BacklightAdjust_MAX/2,5);
    Display::lcd.fillScreen(screen::Black);
}

/**
  * @brief  ҳ���¼�
  * @param  ��
  * @retval ��
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
/*����exit������ʾ�� �̰�exit �����˳� �̰�secl�ٹ�� ����secl ֱ���˳�*/
		if(btn == (ButtonEvent*)&HAL::BTNEXIT)
		{
			if(event == ButtonEvent::EVENT_LONG_PRESSED)
			{
				APP::Audio_PlayMusic(BUTTON_LONG_MUSIC);
				/*������ʾ��: �����˳�  ֱ���˳� �ٹ��*/
				ISbusy = true;
				show_note();
			}
			if(ISbusy)
			{
				if(event == ButtonEvent::EVENT_SHORT_CLICKED)
				{
					APP::Audio_PlayMusic(BUTTON_MUSIC);
					ISbusy = false;
					/*�ٹ�� ˢ��screen go on play game*/
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
					/*ֱ���˳���*/
					ISbusy = false;
					Display::page.PagePush(PAGE_Level);
				}
				if(event == ButtonEvent::EVENT_LONG_PRESSED)
				{
					APP::Audio_PlayMusic(BUTTON_LONG_MUSIC);
					/*�����˳��� Isbuy = false ���� �������ݣ���ͼ+ �ؿ�*/
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



