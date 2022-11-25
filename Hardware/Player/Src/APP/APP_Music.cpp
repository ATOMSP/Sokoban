/****************************************************************************
* @File name: APP_Music.cpp
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.23
* @Description: music player APP
****************************************************************************/
/*APP_Music.h Header */
#include "APP_Music.h"
/*config */
#include "../config.h"

/*TonePlayer obj*/
static TonePlayer Player;
/*enable/disable the module*/
static bool IsEnable = true;


/**
 * @brief Buzz_init
 * 
 */
void APP::Buzz_init()
{
    pinMode(CONFIG_BUZZ_PIN, OUTPUT);
}


/**
 * @brief enable the music
 * en : true: enable  false : disable 
 */
void APP::Buzz_SetEnable(bool en)
{
    IsEnable = en;
}


/**
 * @brief Buzz callback
 * 
 */
void APP::Buzz_Tone(uint32_t freq, int32_t duration)
{
    if(!IsEnable)
        return;
    if(duration >= 0)
    {
        tone(CONFIG_BUZZ_PIN, freq, duration);
    }
    else
    {
        tone(CONFIG_BUZZ_PIN, freq);
    }
}

/**
 * @brief Tone Callback
 * 
 */
static void Tone_Callback(uint32_t freq, uint16_t volume)
{
    APP::Buzz_Tone(freq);
}


/**
 * @brief music init
 * 
 */
void APP::Audio_Init()
{
    Player.SetCallback(Tone_Callback);
}

/**
 * @brief music update is need 10ms
 * 
 */
void APP::Audio_Update()
{
    Player.Update(millis());
}

/**
 * @brief play music
 * name : music file
 */
bool APP::Audio_PlayMusic(const char* name)
{
    bool retval = false;
    for (int i = 0; i < sizeof(MusicList) / sizeof(MusicList[0]); i++)
    {
        if (strcmp(name, MusicList[i].name) == 0)
        {
            Player.Play(MusicList[i].mc, MusicList[i].length);
            retval = true;
            break;
        }
    }
    return retval;
}
