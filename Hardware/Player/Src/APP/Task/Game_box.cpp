/****************************************************************************
* @File name: Game_box.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: ����һ�����������ӵ���Ϸ��������ͨ���ṩ�����ӿں�����ɲ�ͬƽ̨����ֲ
*               ������ͼ��������path:ͼƬ·�� x,y:ͼƬ���� w,h:ͼƬ��ߣ�ע�Ᵽ֤���
*               �豸���Զ�ȡ�ⲿ�洢����.
*               ->typedef void (*Draw_CallBack)(const uint8_t* path,int x,int y,int w,int y);
*               ���ص�ͼ��Ϣ���� ��path:TXT�ļ�·�� ptr:�����GAME_MAP��
*               ->typedef bool (*LoadPicToRam)(const uint8_t* path,void * ptr);
*               ȷ�����º걻���壺{
*               ��ͼ�ߴ�
*                MATERIAL_MAP_W
*                MATERIAL_MAP_H
*               ��������ĳߴ�
*                MATERIAL_Block_SIZE
*               ��ͼ��ϢTXT·��
*                MATERIAL_MAP_1_PATH   
*                MATERIAL_MAP_2_PATH   
*                MATERIAL_MAP_3_PATH   
*                MATERIAL_MAP_4_PATH   
*                MATERIAL_MAP_5_PATH   
*                MATERIAL_MAP_6_PATH   
*                MATERIAL_MAP_7_PATH   
*                MATERIAL_MAP_8_PATH
*               ������Ϸ���ݵ�ͼ��֧���ⲿ����ʵʱ��Ϸ���棩   
*                MATERIAL_MAP_SAVE_PATH 
*               ������ͼ·��
*                MATERIAL_IMG_OTHER_PATH 
*                MATERIAL_IMG_WALL_PATH  
*                MATERIAL_IMG_BOX_PATH   
*                MATERIAL_IMG_END_PATH   
*                MATERIAL_IMG_FLOOR_PATH 
*                MATERIAL_IMG_PEOPLE_PATH
*                MATERIAL_IMG_BOD_PATH   }
*               update:׼�����Ӵ洢flash�ķ�ʽ ���������ƽ̨
****************************************************************************/
#include "Game_box.h"



/**
 * @brief �����ƶ�: ���µ�ͼ����+�����˵�λ�� scan
 *  return : 1�ؿ�ͨ�� 0�ؿ�����
 */
uint8_t PTBox_Manager::Running()
{
    uint8_t cnt = 0;
    /*�����˵�λ�ã��ж��Ƿ����*/
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
    /*Ҫ�ı��˵�λ��*/
    if(Info.offest_x || Info.offest_y)
    {
        /*����λ��*/
        if(Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] == FLOOR || Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] == END)
        {
            Info.Game_MAP[Info.Pos_x + Info.offest_x][Info.Pos_y + Info.offest_y] += PEOPLE;
            Info.Game_MAP[Info.Pos_x][Info.Pos_y ] -= PEOPLE; 
            /*����*/
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
                /*����*/
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
@param lev �ؿ���
* bool : copy error
 */
bool PTBox_Manager::Load(_Level_ID_ lev)
{
    bool res;
    /*!!! clear the val*/
    Info.End_Num = 0;
    /*���ص�ͼ���ڴ�*/
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
    /*��¼�������ݣ� ����λ�� �յ��� �ؿ���*/
    Info.level = lev;
    Info.End_Flag = false;     
    for(int i=0;i<MAP_W;i++)
    {
        for(int k=0;k<MAP_H;k++)
        {
           Draw_Single_Block(i,k);
            /*��ȡ����λ��*/
            if(Info.Game_MAP[i][k] == PEOPLE|| Info.Game_MAP[i][k]== PEOPLE_END)
            {
                Info.Pos_x = i;
                Info.Pos_y = k;
            }
            /*��ȡ�ؿ��յ���*/
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















