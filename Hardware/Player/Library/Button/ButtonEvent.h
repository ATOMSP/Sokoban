/**
  ******************************************************************************
  * @file    ButtonEvent.h
  * @author  FASTSHIFT
  * @version V1.6.0
  * @date    2018-12-23
  * @brief   ������ʽ�����¼������⣬֧�ֶ������������ͷš�״̬�ı��¼���ʶ��
  * @Upgrade 2019.6.18  ��Ӱ���˫���¼����������δ����¼���
  * @Upgrade 2019.8.26  ���GetClicked��GetPressed��GetLongPressed���¼�ģʽ֧�֡�
  * @Upgrade 2019.12.4  ʹ�����µ��¼��ص���ƣ������¼�ͳһ��EventAttach��
                        ���Click��LongPressed��LongPressRepeat�¼���
  * @Upgrade 2020.6.12  �ϲ�ButtonEvent_Type��ButtonEvent��
  * @Upgrade 2021.3.22  ���EVENT_SHORT_CLICKED��EVENT_PRESSING
                        �����������Ż�����Ҫ�ı�־λ
                        EventMonitor()�β�ʹ��bool���ͣ�ȥ��NoPressStateͳһ״̬
  * @Upgrade 2021.5.12  ���EventType.inc,�����ŵ� ö��+�ַ��� �Զ����ɷ�ʽ
                        FuncCallBack_t -> FuncCallback_t
  ******************************************************************************
  * @attention
  * ��Ҫ�ṩһ����ȷ�����뼶��ϵͳʱ�ӣ��û���Ҫ��ButtonEvent.cpp�ﶨ��GET_TIKC()
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_EVENT_H
#define __BUTTON_EVENT_H

#include <stdint.h>

class ButtonEvent
{
private:
    typedef void(*FuncCallback_t)(ButtonEvent* btn, int event);

public:
    typedef enum
    {
#       define EVENT_DEF(evt) evt
#       include "EventType.inc"
#       undef EVENT_DEF
        _EVENT_LAST
    } Event_t;

public:
    bool IsPressed;
    bool IsClicked;
    bool IsLongPressed;

public:
    ButtonEvent(
        uint16_t longPressTime = 500,
        uint16_t longPressTimeRepeat = 200,
        uint16_t doubleClickTime = 200
    );
    void EventAttach(FuncCallback_t function);
    void EventMonitor(bool isPress);

    const char* GetEventString(uint16_t event)
    {
        const char* eventStr[_EVENT_LAST] =
        {
#         define EVENT_DEF(evt) #evt
#         include "EventType.inc"
#         undef EVENT_DEF
        };

        return (event < _EVENT_LAST) ? eventStr[event] : "EVENT_NOT_FOUND";
    }

    inline uint16_t GetClickCnt()
    {
        uint16_t cnt = priv.clickCnt + 1;
        priv.clickCnt = 0;
        return cnt;
    }

    inline bool GetClicked()
    {
        bool n = IsClicked;
        IsClicked = false;
        return n;
    }

    inline bool GetPressed()
    {
        bool n = IsPressed;
        IsPressed = false;
        return n;
    }

    inline bool GetLongPressed()
    {
        bool n = IsLongPressed;
        IsLongPressed = false;
        return n;
    }

    operator uint8_t()
    {
        return priv.nowState;
    };


private:
    typedef enum
    {
        STATE_NO_PRESS,
        STATE_PRESS,
        STATE_LONG_PRESS
    } State_t;

private:
    struct
    {
        State_t nowState;
        uint16_t longPressTimeCfg;
        uint16_t longPressRepeatTimeCfg;
        uint16_t doubleClickTimeCfg;
        uint32_t lastLongPressTime;
        uint32_t lastPressTime;
        uint32_t lastClickTime;
        uint16_t clickCnt;
        bool isLongPressed;
        FuncCallback_t eventCallback;
    } priv;

private:
    uint32_t GetTickElaps(uint32_t prevTick);
};

#endif




