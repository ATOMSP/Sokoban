#include "../Display.h"

/*ѡ������ò���*/
#define SECL_X0		(38)
#define SECL_Y0		(109)
#define SECL_STEP_X	(53)
#define SECL_STEP_X_MAX	(SECL_X0 + SECL_STEP_X * 3)
#define SECL_STEP_Y	(52)
#define SECL_STEP_Y_MAX	(SECL_Y0 + SECL_STEP_Y)
#define SECL_R		(13)
static uint8_t secl_x;
static uint8_t secl_y;
#define UP			(0)
#define DOWN		(1)
#define LEFT		(2)
#define RIGHT		(3)

/**
 * @brief ����ѡ���
 * 
 */
static void Create_Option()
{
	for (int i = 0; i <= 4; i++)
	{
		Display::lcd.drawCircle(secl_x,secl_y,SECL_R + i,screen::Red);
	}
}
/**
 * @brief ����ѡ���
 * 
 */
static void Update_Option(uint8_t dir)
{
	if((secl_x >= SECL_STEP_X_MAX && dir== RIGHT) || (secl_x <= SECL_X0 && dir == LEFT)||(secl_y >= SECL_STEP_Y_MAX && dir == DOWN)|| (secl_y <= SECL_Y0 && dir == UP))
	{
		return;
	}
	for (int i = 0; i <= 4; i++)
	{
		Display::lcd.drawCircle(secl_x,secl_y,SECL_R + i,screen::White);
	}	
	if(dir == UP)
		secl_y -= SECL_STEP_Y;
	else if(dir ==DOWN)
		secl_y += SECL_STEP_Y;
	else if(dir == LEFT)
		secl_x -= SECL_STEP_X;
	else if(dir == RIGHT)
		secl_x += SECL_STEP_X;
	Create_Option();	
}


/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
	secl_x = SECL_X0;
	secl_y = SECL_Y0;
	/*draw*/
	ai_load_picfile(IMG_GLEVEL_PATH,0,0,LCD_W,LCD_H,1);
	/*����ѡ���*/
	for (int i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			for (size_t k = 0; k <= 4; k++)
			{
				Display::lcd.drawCircle(secl_x + j * SECL_STEP_X,secl_y + i * SECL_STEP_Y,SECL_R + k,screen::White);
			}
		}
	}	
	Create_Option();
	Display::Adjust_light(true,BacklightAdjust_MAX/2,10);
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
   			Update_Option(UP);
		}
	}
	if(btn == (ButtonEvent*)&HAL::BTNDOWN)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			Update_Option(DOWN);
		}
	}
	if(btn == (ButtonEvent*)&HAL::BTNLEFT)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			Update_Option(LEFT);
		}
	}
	if(btn == (ButtonEvent*)&HAL::BTNRIGHT)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			Update_Option(RIGHT);
		}
	}
	if(btn == (ButtonEvent*)&HAL::BTNEXIT)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{

			/*ֱ���˳�*/
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			Display::page.PagePush(PAGE_Master);
		}	
	}	
	if(btn == (ButtonEvent*)&HAL::BTNSECL)
	{
		if(event == ButtonEvent::EVENT_SHORT_CLICKED)
		{
			/*�ڶ���*/
			APP::Audio_PlayMusic(BUTTON_MUSIC);
			if(secl_y - SECL_Y0)
			{
				Page_dat.Level = (secl_x - SECL_X0)/SECL_STEP_X + 1 + 4;
			}
			else
			{
				Page_dat.Level = (secl_x - SECL_X0)/SECL_STEP_Y + 1;
			}
			/*������Ϸҳ��*/
			Page_dat.ISsave = false;
			Display::page.PagePush(PAGE_Game);
		}		
	}	
}


void PageRegister_Level(uint8_t pageID)
{
    Display::page.PageRegister(pageID,Setup,NULL,Exit,Event);
}




