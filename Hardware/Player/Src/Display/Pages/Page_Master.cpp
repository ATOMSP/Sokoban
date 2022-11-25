#include "../Display.h"

/**
 * @brief ѡ����ͷ����
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
 * @brief ����ѡ���
 * 
 */
static void Create_Option()
{
	ai_load_picfile(IMG_GDIR_PATH,SECL_X0,secl_y,SCEL_W,SECL_H,1);
}
/**
 * @brief ����ѡ���
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
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
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
				/*ֱ�ӽ�������Ϸ*/
				Page_dat.ISsave = false;
				/*����level*/		
				Display::page.PagePush(PAGE_Level);		
			}
			else if(secl_y == SECL_Y0 + SECL_STEP)
			{
				/*���������Ϸ*/
				Page_dat.ISsave = true;
				/*����֮ǰ��������ݣ� �ؿ�������ֵ����Ӧֵ*/
				APP::File_Read(MATERIAL_LEVELDAT_PATH,(uint8_t*)&Page_dat.Level,1);
				Page_dat.Level = Page_dat.Level - '0';
				LOG("Loading Level:%d\r\n",Page_dat.Level);
				/*����Game*/		
				Display::page.PagePush(PAGE_Game);				
			}
			else 
			{
				/*��������ҳ��*/		
				Display::page.PagePush(PAGE_About);		
			}
		}		
	}	
	if(btn == (ButtonEvent*)&HAL::BTNEXIT)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			/*ֱ���˳�*/
			Display::page.PagePush(PAGE_Displate);
		}	
	}		
}


void PageRegister_Master(uint8_t pageID)
{
    Display::page.PageRegister(pageID,Setup,NULL,Exit,Event);
}










