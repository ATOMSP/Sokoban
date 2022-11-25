#include "../Display.h"

/**
 * @brief 选项框箭头参数
 * 
 */
#define SCEL_W		(38)
#define SECL_H		(30)
#define SECL_X0		(47)
#define SECL_Y0		(77)
#define SECL_STEP	(37)
#define SECL_MAX	(SECL_Y0 + 2 * SECL_STEP)
static uint8_t secl_y;

/**
 * @brief 创建选项框
 * 
 */
static void Create_Option()
{
	ai_load_picfile(IMG_GDIR_PATH,SECL_X0,secl_y,SCEL_W,SECL_H,1);
}
/**
 * @brief 更新选项框
 * 
 */
static void Update_Option(bool dir)
{
	if((secl_y >= SECL_MAX && !dir)||(secl_y <= SECL_Y0 && dir)) 
	{
		return;
	}
	Display::lcd.fillRect(SECL_X0,secl_y,SCEL_W,SECL_H,screen::Black);
	secl_y = (dir) ? secl_y - SECL_STEP : secl_y + SECL_STEP;
	Create_Option();
}


/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	secl_y = SECL_Y0;
	/*draw matser*/
	ai_load_picfile(IMG_GMASTER_PATH,0,0,LCD_W,LCD_H,1);
	/*crtate option*/
	Create_Option();
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
	if(btn == (ButtonEvent*)&HAL::BTNUP)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			Update_Option(true);
		}
	}
	if(btn == (ButtonEvent*)&HAL::BTNDOWN)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			Update_Option(false);
		}
	}
	if(btn == (ButtonEvent*)&HAL::BTNSECL)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			if(secl_y == SECL_Y0)
			{
				/*直接进入新游戏*/
				Page_dat.ISsave = false;
				/*进入level*/		
				Display::page.PagePush(PAGE_Level);		
			}
			else if(secl_y == SECL_Y0 + SECL_STEP)
			{
				/*进入继续游戏*/
				Page_dat.ISsave = true;
				/*加载之前保存的数据： 关卡数：赋值给相应值*/
				APP::File_Read(MATERIAL_LEVELDAT_PATH,(uint8_t*)&Page_dat.Level,1);
				Page_dat.Level = Page_dat.Level - '0';
				LOG("Loading Level:%d\r\n",Page_dat.Level);
				/*进入Game*/		
				Display::page.PagePush(PAGE_Game);				
			}
			else 
			{
				/*进入设置页面*/		
				Display::page.PagePush(PAGE_About);		
			}
		}		
	}	
	if(btn == (ButtonEvent*)&HAL::BTNEXIT)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			/*直接退出*/
			Display::page.PagePush(PAGE_Displate);
		}	
	}		
}


void PageRegister_Master(uint8_t pageID)
{
    Display::page.PageRegister(pageID,Setup,NULL,Exit,Event);
}










