/****************************************************************************
* @File name: main.cpp
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.23
* @Description: main
****************************************************************************/
#include "Display/Display.h"
#include "APP/APP.h"
#include "config.h"

/*任务调度器*/
static MillisTaskManager manager;
/*Button hook*/
void Button_Running()
{
    HAL::Button_Scan();
}
/*Interrupt music */
static void HAL_InterrputUpdate()
{
    HAL::Button_Scan();
    APP::Audio_Update();
}

/*初始化setup*/
void setup() 
{
#if (PRINT_INFO)
     LOG("->System Load...\r\n");
#endif

    /*SDFatfs Init*/
    HAL::SDfatfs_Init();
    /*Button Init*/
    HAL::Button_Init();
    /*Music init*/
    APP::Buzz_init();
    APP::Audio_Init();
    Timer_SetInterrupt(CONFIG_HAL_UPDATE_TIM, 10 * 1000, HAL_InterrputUpdate);
    TIM_Cmd(CONFIG_HAL_UPDATE_TIM, ENABLE);
    /*Display Init*/
    Display::Display_Init();
    /*register button task*/
//    manager.Register(Button_Running,10);
    APP::Audio_PlayMusic("Startup");
#if (PRINT_INFO)
     LOG("->System Load success!\r\n");
#endif
   
}


/*循环loop*/
void loop() 
{
    /*Button running*/
//    manager.Running(millis());
    /*Display running*/
    Display::Running();
}


int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    Serial.begin(Serial_Speed);
    setup();
    for(;;) loop();
}












