/****************************************************************************
* @File name: Game_box.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: 这是一个经典推箱子的游戏，开发者通过提供两个接口函数完成不同平台的移植
*               绘制贴图函数：（path:图片路径 x,y:图片坐标 w,h:图片宽高）注意保证你的
*               设备可以读取外部存储介质.
*               ->typedef void (*Draw_CallBack)(const uint8_t* path,int x,int y,int w,int y);
*               加载地图信息函数 （path:TXT文件路径 ptr:本库的GAME_MAP）
*               ->typedef bool (*LoadPicToRam)(const uint8_t* path,void * ptr);
*               确保以下宏被定义：{
*               地图尺寸
*                MATERIAL_MAP_W
*                MATERIAL_MAP_H
*               单个方块的尺寸
*                MATERIAL_Block_SIZE
*               地图信息TXT路径
*                MATERIAL_MAP_1_PATH   
*                MATERIAL_MAP_2_PATH   
*                MATERIAL_MAP_3_PATH   
*                MATERIAL_MAP_4_PATH   
*                MATERIAL_MAP_5_PATH   
*                MATERIAL_MAP_6_PATH   
*                MATERIAL_MAP_7_PATH   
*                MATERIAL_MAP_8_PATH
*               保存游戏数据地图（支持外部进行实时游戏保存）   
*                MATERIAL_MAP_SAVE_PATH 
*               方块贴图路径
*                MATERIAL_IMG_OTHER_PATH 
*                MATERIAL_IMG_WALL_PATH  
*                MATERIAL_IMG_BOX_PATH   
*                MATERIAL_IMG_END_PATH   
*                MATERIAL_IMG_FLOOR_PATH 
*                MATERIAL_IMG_PEOPLE_PATH
*                MATERIAL_IMG_BOD_PATH   }
*               update:准备增加存储flash的方式 以适配更多平台
****************************************************************************/
#include "Game_box.h"



/**
 * @brief 方块移动: 更新地图数组+更新人的位置 scan
 *  return : 1关卡通过 0关卡运行
 */
uint8_t PTBox_Manager::Running()
{
    uint8_t cnt = 0;
    /*更新人的位置，判断是否过关*/
    for (int i = 0; i < MAP_W; i++)
    {
        for (int k = 0; k < MAP_H; k++)
        {
            if(Info.Game_MAP[i][k]==PEOPLE || Info.Game_MAP[i][k]== PEOPLE_END)
            {
                Info.Pos_x = i;
                Info.Pos_y = k;
            }
            if(Info.Game_MAP[i][k]== BOD)
            {
                cnt++;
            }
        }
    }
#if (PRINT_INFO) 
        LOG("->Current end_num: %d\r\n",cnt); 
#endif   
    if(cnt == Info.End_Num)
    {
        Info.End_Flag = true;
#if (PRINT_INFO)
        LOG("->The Level is passed \r\n"); 
#endif
        return 1;
    }
    else
    {
        Info.End_Flag = false;
     }
    /*要改变人的位置*/
    if(Info.offest_x || Info.offest_y)
    {
        /*更新位置*/
        if(Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] == FLOOR || Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] == END)
        {
            Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] += PEOPLE;
            Info.Game_MAP[Info.Pos_x][Info.Pos_y ] -= PEOPLE; 
            /*绘制*/
            Draw_Single_Block(Info.Pos_x + Info.offest_x,Info.Pos_y + Info.offest_y);
            Draw_Single_Block(Info.Pos_x,Info.Pos_y);
        }
        else if(Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] == BOX || Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] == BOD)
        {
            if(Info.Game_MAP[Info.Pos_x + 2*Info.offest_x][Info.Pos_y + 2*Info.offest_y] == FLOOR || Info.Game_MAP[Info.Pos_x + 2*Info.offest_x][Info.Pos_y + 2 * Info.offest_y] == END)
            {
                Info.Game_MAP[Info.Pos_x + 2*Info.offest_x][Info.Pos_y + 2*Info.offest_y] += BOX;
                Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] +=  PEOPLE - BOX;
                Info.Game_MAP[Info.Pos_x][Info.Pos_y ] -= PEOPLE; 
                /*绘制*/
                Draw_Single_Block(Info.Pos_x + 2 * Info.offest_x,Info.Pos_y + 2 * Info.offest_y);
                Draw_Single_Block(Info.Pos_x + Info.offest_x,Info.Pos_y + Info.offest_y);
                Draw_Single_Block(Info.Pos_x,Info.Pos_y);
            }
        }
        Info.offest_x = 0;
        Info.offest_y = 0;   
    } 
    return 0;
}

/**
 * @brief Draw single block
 * 
 */
void PTBox_Manager::Draw_Single_Block(int i,int k)
{
    for (int j = 0; j < 8; j++)
    {
        if(Info.Game_MAP[i][k] == Iron[j].Id)
        {
            m_draw((const uint8_t*)Iron[j].pic,k*Block_SIZE,i*Block_SIZE,Block_SIZE,Block_SIZE);
        }
    }
}

/* 
@brief game Init
@param lev 关卡名
* bool : copy error
 */
bool PTBox_Manager::Load(_Level_ID_ lev)
{
    bool res;
    /*!!! clear the val*/
    Info.End_Num = 0;
    /*加载地图到内存*/
    for (int i = 0; i < _LEVEL_NUM_ + 1; i++)
    {
        if(lev == Map[i].lev_id)
        {
            res = m_copy((const uint8_t*)Map[i].pic,(void*)Info.Game_MAP);
            if(!res)
            {
#if (PRINT_INFO) 
                LOG("->Map Load error-copy error\r\n"); 
#endif            
                return res;    
            }
            break;
        }
    }
    /*记录更新数据： 人物位置 终点数 关卡数*/
    Info.level = lev;
    Info.End_Flag = false;     
    for(int i=0;i<MAP_W;i++)
    {
        for(int k=0;k<MAP_H;k++)
        {
           Draw_Single_Block(i,k);
            /*获取人物位置*/
            if(Info.Game_MAP[i][k] == PEOPLE|| Info.Game_MAP[i][k]== PEOPLE_END)
            {
                Info.Pos_x = i;
                Info.Pos_y = k;
            }
            /*获取关卡终点数*/
            if(Info.Game_MAP[i][k]==BOD || Info.Game_MAP[i][k]== END ||Info.Game_MAP[i][k]== PEOPLE_END)
            {
                Info.End_Num++;
            }
        }
    }   
#if (PRINT_INFO) 
        LOG("->New Load Dat: Level :%d End_NUM :%d\r\n",Info.level,Info.End_Num); 
#endif   
    return res;
}















