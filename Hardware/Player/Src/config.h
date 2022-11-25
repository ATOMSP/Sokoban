/****************************************************************************
* @File name: config.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: system config header
****************************************************************************/

/*===================Platform dat =======================*/
#include <stdint.h>
#include "Arduino.h"
typedef unsigned char	_BYTE;
typedef short			SHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;
typedef int				INT;
typedef unsigned int	UINT;
typedef long			LONG;
typedef unsigned long	DWORD;


/*Debug*/
#define PRINT_INFO      (0)
#define Serial_Speed    (9600)
#define LOG(TXT,...)    Serial.printf(TXT,##__VA_ARGS__)


/*music*/
#define CONFIG_HAL_UPDATE_TIM   TIM4
#define CONFIG_BUZZ_PIN         (PC6)
#define BUTTON_MUSIC         "SingleButton"
#define BUTTON_LONG_MUSIC    "BattChargeEnd"
#define WIN_MUSIC           "Connect"

/*Button Pin config*/
#define BUTTON_UP       (PC7)
#define BUTTON_DOWN     (PB7) 
#define BUTTON_LEFT     (PA8)
#define BUTTON_RIGHT    (PB6)
#define BUTTON_SECL     (PE9)
#define BUTTON_EXIT     (PE8)

/*SD Pin config*/
#define SDIO_D0         (PC8)
#define SDIO_D1         (PC9)
#define SDIO_D2         (PC10)
#define SDIO_D3         (PC11)
#define SDIO_CLK        (PC12)
#define SDIO_CMD        (PD2)

/*LCD config*/
#define LCD_W               (240)
#define LCD_H               (240)
#define BacklightAdjust_MAX (100)
#define Backlight_FRE       (20000)
#define Pin_CS              (0)
#define Pin_DC              (PA4)
#define Pin_RST             (PA6)
#define Pin_BLK             (PB1)
/*page manager stack len*/
#define PAGE_STACK_LEN  (40)

/*Game materials*/
#define MATERIAL_Block_SIZE (24)
#define MATERIAL_MAP_W      (10)
#define MATERIAL_MAP_H      (10)

/*Map info*/
#define MATERIAL_LEVELDAT_PATH   "0:/LevelDAT.txt"
#define MATERIAL_MAP_1_PATH      "0:/Map1.txt"
#define MATERIAL_MAP_2_PATH      "0:/Map2.txt"
#define MATERIAL_MAP_3_PATH      "0:/Map3.txt"
#define MATERIAL_MAP_4_PATH      "0:/Map4.txt"
#define MATERIAL_MAP_5_PATH      "0:/Map5.txt"
#define MATERIAL_MAP_6_PATH      "0:/Map6.txt"
#define MATERIAL_MAP_7_PATH      "0:/Map7.txt"
#define MATERIAL_MAP_8_PATH      "0:/Map8.txt"
#define MATERIAL_MAP_SAVE_PATH   "0:/MapSave.txt"
/*iron photos*/
#define MATERIAL_IMG_OTHER_PATH   "0:/Grass.jpg"
#define MATERIAL_IMG_WALL_PATH    "0:/Wall.jpg"
#define MATERIAL_IMG_BOX_PATH     "0:/Box.jpg"
#define MATERIAL_IMG_END_PATH     "0:/End.jpg"
#define MATERIAL_IMG_FLOOR_PATH   "0:/Floor.jpg"
#define MATERIAL_IMG_PEOPLE_PATH  "0:/People.jpg"
#define MATERIAL_IMG_BOD_PATH     "0:/Bod.jpg"
/*Page material*/
#define IMG_GLEVEL_PATH           "0:/Glevel.jpg"
#define IMG_GIRON_PATH            "0:/GIron.jpg"
#define IMG_GMASTER_PATH          "0:/Gmaster.jpg"
/*160 80*/
#define IMG_GNOTE_PATH            "0:/Gnote.jpg"
#define MATERIAL_IMG_WIN_PATH     "0:/Gwin.jpg"
/*38 30*/
#define IMG_GDIR_PATH            "0:/Gdir.jpg"

#define IMG_CAT_PATH     "0:/Cat.jpg"









