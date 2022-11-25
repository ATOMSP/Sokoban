/****************************************************************************
* @File name: APP_Music.h
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.23
* @Description: 
****************************************************************************/

#ifndef APP_MUSIC_H__
#define APP_MUSIC_H__


/*HAL  */
#include "../HAL/HAL.h"
/*extern libraries*/
#include "ExternLib.h"


namespace APP
{
    void Buzz_init();
    void Buzz_Tone(uint32_t freq, int32_t duration = -1);
    void Audio_Init();
    bool Audio_PlayMusic(const char* name);
    void Audio_Update();
    void Buzz_SetEnable(bool en);
}


/*音符频率对应表*/
enum ToneMap
{
    L1 = 262,
    L1h = 277,
    L2 = 294,
    L2h = 311,
    L3 = 330,
    L4 = 349,
    L4h = 370,
    L5 = 392,
    L5h = 415,
    L6 = 440,
    L6h = 466,
    L7 = 494,
    M1 = 523,
    M1h = 554,
    M2 = 587,
    M2h = 622,
    M3 = 659,
    M4 = 698,
    M4h = 740,
    M5 = 784,
    M5h = 831,
    M6 = 880,
    M6h = 932,
    M7 = 988,
    H1 = 1046,
    H1h = 1109,
    H2 = 1175,
    H2h = 1245,
    H3 = 1318,
    H4 = 1397,
    H4h = 1480,
    H5 = 1568,
    H5h = 1661,
    H6 = 1760,
    H6h = 1865,
    H7 = 1976,
};

/*music files*/
#define MUSIC_DEF(name) static const TonePlayer::MusicNode_t Music_##name[] =

/*开机音*/
MUSIC_DEF(Startup)
{
    {M1, 80},
    {M6, 80},
    {M3, 80},
};

/*关机音*/
MUSIC_DEF(Shutdown)
{
    {M3, 80},
    {M6, 80},
    {M1, 80},
};

/*错误提示音*/
MUSIC_DEF(Error)
{
    {100, 80},
    {0,   80},
    {100, 80},
};

/*连接成功音*/
MUSIC_DEF(Connect)
{
    {H1, 80},
    {H2, 80},
    {H3, 80},
};

/*断开连接音*/
MUSIC_DEF(Disconnect)
{
    {H3, 80},
    {H2, 80},
    {H1, 80},
};

/*信号不稳定提示音*/
MUSIC_DEF(UnstableConnect)
{
    {H1, 80},
    {0, 80},
    {H1, 80},
};

/*充电启动提示音*/
MUSIC_DEF(BattChargeStart)
{
    {L1, 80},
    {L3, 80},
};

/*按键单击提示音*/
MUSIC_DEF(SingleButton)
{
    {L3, 80},
};

/*充电关闭提示音*/
MUSIC_DEF(BattChargeEnd)
{
    {L3, 80},
    {L1, 80},
};

/*设备插入提示音*/
MUSIC_DEF(DeviceInsert)
{
    /*C4,A3,F3,F4*/
    {M1, 180},
    {L6, 80},
    {L4, 80},
    {M4, 160},
};

/*设备拔出提示音*/
MUSIC_DEF(DevicePullout)
{
    /*A4,F4,E4*/
    {L6, 80},
    {L4, 80},
    {L3, 80},
};

/*无操作提示音*/
MUSIC_DEF(NoOperationWarning) 
{
    {4000, 40},
    {0, 80},
    {4000, 40},
    {0, 80},
    {4000, 40},
};

/*列表数据类型定义*/
typedef struct
{
    const TonePlayer::MusicNode_t* mc;
    uint16_t length;
    const char* name;
} MusicList_t;


#define ADD_MUSIC(mc) {Music_##mc,(sizeof(Music_##mc) / sizeof(Music_##mc[0])), #mc}

/*提示音地址储存列表*/
static const MusicList_t MusicList[] =
{
    ADD_MUSIC(Startup),
    ADD_MUSIC(Shutdown),
    ADD_MUSIC(Error),
    ADD_MUSIC(Connect),
    ADD_MUSIC(Disconnect),
    ADD_MUSIC(UnstableConnect),
    ADD_MUSIC(BattChargeStart),
    ADD_MUSIC(BattChargeEnd),
    ADD_MUSIC(DeviceInsert),
    ADD_MUSIC(DevicePullout),
    ADD_MUSIC(NoOperationWarning),
    ADD_MUSIC(SingleButton),
};



#endif // !APP_MUSIC_H__








