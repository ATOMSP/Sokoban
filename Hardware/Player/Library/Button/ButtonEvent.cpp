#include "ButtonEvent.h"
#include "config.h"


/**
  * @brief  �����¼����캯��
  * @param  LongPressTimeMs_Set: ��������������ʱ����
  * @param  LongPressTimeRepeatMs_Set: �����ظ�����ʱ��
  * @retval ��
  */
ButtonEvent::ButtonEvent(
    uint16_t longPressTime,
    uint16_t longPressTimeRepeat,
    uint16_t doubleClickTime
)
{
    memset(&priv, 0, sizeof(priv));

    priv.longPressTimeCfg = longPressTime;
    priv.longPressRepeatTimeCfg = longPressTimeRepeat;
    priv.doubleClickTimeCfg = doubleClickTime;

    priv.lastLongPressTime = priv.lastClickTime = priv.lastPressTime = 0;
    priv.isLongPressed = false;
    priv.nowState = STATE_NO_PRESS;

    priv.eventCallback = NULL;
}

/**
  * @brief  ��ȡ���ϴ�ʱ���ʱ���(��uint32���ʶ��)
  * @param  prevTick: �ϵ�ʱ���
  * @retval ʱ���
  */
uint32_t ButtonEvent::GetTickElaps(uint32_t prevTick)
{
    uint32_t actTime = GET_TICK();

    if(actTime >= prevTick)
    {
        prevTick = actTime - prevTick;
    }
    else
    {
        prevTick = UINT32_MAX - prevTick + 1;
        prevTick += actTime;
    }

    return prevTick;
}

/**
  * @brief  �����¼���
  * @param  function: �ص�����ָ��
  * @retval ��
  */
void ButtonEvent::EventAttach(FuncCallback_t function)
{
    priv.eventCallback = function;
}

/**
  * @brief  ����¼�������ɨ������10ms
  * @param  nowState: ��ǰ����״̬
  * @retval ��
  */
void ButtonEvent::EventMonitor(bool isPress)
{
    if(priv.eventCallback == NULL)
    {
        return;
    }

    if (isPress && priv.nowState == STATE_NO_PRESS)
    {
        priv.nowState = STATE_PRESS;

        IsPressed = true;
        priv.lastPressTime = GET_TICK();

        priv.eventCallback(this, EVENT_PRESSED);
        priv.eventCallback(this, EVENT_CHANGED);
    }

    if(priv.nowState == STATE_NO_PRESS)
    {
        return;
    }

    if(isPress)
    {
        priv.eventCallback(this, EVENT_PRESSING);
    }

    if (isPress && GetTickElaps(priv.lastPressTime) >= priv.longPressTimeCfg)
    {
        priv.nowState = STATE_LONG_PRESS;

        if(!priv.isLongPressed)
        {
            priv.eventCallback(this, EVENT_LONG_PRESSED);
            priv.lastLongPressTime = GET_TICK();
            IsLongPressed = priv.isLongPressed = true;
        }
        else if(GetTickElaps(priv.lastLongPressTime) >= priv.longPressRepeatTimeCfg)
        {
            priv.lastLongPressTime = GET_TICK();
            priv.eventCallback(this, EVENT_LONG_PRESSED_REPEAT);
        }
    }
    else if (!isPress)
    {
        priv.nowState = STATE_NO_PRESS;

        if(GetTickElaps(priv.lastClickTime) < priv.doubleClickTimeCfg)
        {
            priv.clickCnt++;
            priv.eventCallback(this, EVENT_DOUBLE_CLICKED);
        }

        if(priv.isLongPressed)
        {
            priv.eventCallback(this, EVENT_LONG_PRESSED_RELEASED);
        }

        priv.isLongPressed = false;
        IsClicked = true;
        priv.lastClickTime = GET_TICK();

        if(GetTickElaps(priv.lastPressTime) < priv.longPressTimeCfg)
        {
            priv.eventCallback(this, EVENT_SHORT_CLICKED);
        }

        priv.eventCallback(this, EVENT_CLICKED);
        priv.eventCallback(this, EVENT_RELEASED);
        priv.eventCallback(this, EVENT_CHANGED);
    }
}
