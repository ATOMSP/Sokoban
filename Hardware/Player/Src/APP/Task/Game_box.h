/****************************************************************************
* @File name: Game_box.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: ???????????????????????????????????
*               ????????path:???? x,y:???? w,h:???????????
*               ????????????.
*               ->typedef void (*Draw_CallBack)(const uint8_t* path,int x,int y,int w,int y);
*               ???????? ?path:TXT???? ptr:???GAME_MAP?
*               ->typedef bool (*LoadPicToRam)(const uint8_t* path,void * ptr);
*               ?????????{
*               ????
*                MATERIAL_MAP_W
*                MATERIAL_MAP_H
*               ???????
*                MATERIAL_Block_SIZE
*               ????TXT??
*                MATERIAL_MAP_1_PATH   
*                MATERIAL_MAP_2_PATH   
*                MATERIAL_MAP_3_PATH   
*                MATERIAL_MAP_4_PATH   
*                MATERIAL_MAP_5_PATH   
*                MATERIAL_MAP_6_PATH   
*                MATERIAL_MAP_7_PATH   
*                MATERIAL_MAP_8_PATH
*               ??????????????????????   
*                MATERIAL_MAP_SAVE_PATH 
*               ??????
*                MATERIAL_IMG_OTHER_PATH 
*                MATERIAL_IMG_WALL_PATH  
*                MATERIAL_IMG_BOX_PATH   
*                MATERIAL_IMG_END_PATH   
*                MATERIAL_IMG_FLOOR_PATH 
*                MATERIAL_IMG_PEOPLE_PATH
*                MATERIAL_IMG_BOD_PATH   }
*               update:??????flash??? ???????
****************************************************************************/
#ifndef __GAME_BOX_H__
#define __GAME_BOX_H__


/*==============================Put your config file here======================================*/
#include "../../config.h"
/*==============================Put your config file here======================================*/

#include <stdint.h>


/*Map info path*/
#define MAP_1_PATH       MATERIAL_MAP_1_PATH   
#define MAP_2_PATH       MATERIAL_MAP_2_PATH   
#define MAP_3_PATH       MATERIAL_MAP_3_PATH   
#define MAP_4_PATH       MATERIAL_MAP_4_PATH   
#define MAP_5_PATH       MATERIAL_MAP_5_PATH   
#define MAP_6_PATH       MATERIAL_MAP_6_PATH   
#define MAP_7_PATH       MATERIAL_MAP_7_PATH   
#define MAP_8_PATH       MATERIAL_MAP_8_PATH   
#define MAP_SAVE_PATH    MATERIAL_MAP_SAVE_PATH 
/*iron photos path*/
#define IMG_OTHER_PATH   MATERIAL_IMG_OTHER_PATH 
#define IMG_WALL_PATH    MATERIAL_IMG_WALL_PATH  
#define IMG_BOX_PATH     MATERIAL_IMG_BOX_PATH   
#define IMG_END_PATH     MATERIAL_IMG_END_PATH   
#define IMG_FLOOR_PATH   MATERIAL_IMG_FLOOR_PATH 
#define IMG_PEOPLE_PATH  MATERIAL_IMG_PEOPLE_PATH
#define IMG_BOD_PATH     MATERIAL_IMG_BOD_PATH   
/*=====================================PTbox_Manager ==========================*/


/*Map W+H*/
#define MAP_W           MATERIAL_MAP_W
#define MAP_H           MATERIAL_MAP_H
/*Block Size*/
#define Block_SIZE      MATERIAL_Block_SIZE

/*Block Kinds*/
#define FLOOR       (0)
#define WALL        (1)
#define END         (2)
#define BOX         (3)
#define PEOPLE      (4)
#define BOD         (5)
#define PEOPLE_END  (6)
#define OTHER       (7)



typedef enum
{
    SAVE_DAT = 0,
    Level_1 ,
    Level_2 ,        
    Level_3 ,
    Level_4 ,
    Level_5 ,        
    Level_6 ,
    Level_7 ,
    Level_8 ,
    _LEVEL_NUM_
    
}_Level_ID_;
/*Í¼±êËØ²Ä*/
typedef struct 
{
    uint8_t Id;
    const char* pic;
}Iron_Info_t;
const Iron_Info_t Iron[] = { 

    {FLOOR,IMG_FLOOR_PATH},
    {WALL ,IMG_WALL_PATH},
    {END  ,IMG_END_PATH},
    {BOX  , IMG_BOX_PATH},
    {PEOPLE,IMG_PEOPLE_PATH},
    {BOD  ,IMG_BOD_PATH},
    {PEOPLE_END,IMG_PEOPLE_PATH},
    {OTHER,IMG_OTHER_PATH}
};
/*µØÍ¼ËØ²Ä*/
typedef struct 
{
    uint8_t lev_id;
    const char* pic;
}Map_Info_t;
const Map_Info_t Map[] ={

    {Level_1,MAP_1_PATH},
    {Level_2,MAP_2_PATH},
    {Level_3,MAP_3_PATH},
    {Level_4,MAP_4_PATH},
    {Level_5,MAP_5_PATH},
    {Level_6,MAP_6_PATH},
    {Level_7,MAP_7_PATH},
    {Level_8,MAP_8_PATH},
    {SAVE_DAT,MAP_SAVE_PATH}
};

class PTBox_Manager
{
public:
/*Display hook*/
    typedef void (*Draw_CallBack)(const uint8_t*,int,int,int,int);
    typedef bool (*LoadPicToRam)(const uint8_t*,void *);
/*Button hook*/
    PTBox_Manager(Draw_CallBack disp,LoadPicToRam copy)
    {
        m_draw = disp;
        m_copy = copy;
    }
    typedef struct
    {
        /*Map*/
        bool    End_Flag;
        uint8_t End_Num;
        /*current level*/
        _Level_ID_ level;
        /*pos offest*/
        int8_t offest_x;
        int8_t offest_y;
        /*pos people*/
        uint8_t Pos_x;
        uint8_t Pos_y;
        uint8_t Game_MAP[MAP_W][MAP_H];

    }PTBox_Info_t;
    PTBox_Info_t Info;
public: 
    uint8_t Running(void);
    bool Load(_Level_ID_ lev);
    void Draw_Single_Block(int i,int k);    
private:
    
    Draw_CallBack m_draw;
    LoadPicToRam  m_copy;

};


#endif












