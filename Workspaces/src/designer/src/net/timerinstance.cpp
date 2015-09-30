#include "timerinstance.h"
#include<iostream>
#include <QDebug>

TimerInstance::TimerInstance()
{
    //定时器事件触发的次数
    m_eventTimes = 0;
}

TimerInstance::~TimerInstance()
{

}

//定时器事件
void TimerInstance::timerEvent_r()
{
    //发送100ms信号
    if(m_eventTimes%10 == 0)
    {
        emit timeout_100();
    }

    //发送200ms信号
    if(m_eventTimes%20 == 0)
    {
        emit timeout_200();
    }

    //发送500ms信号
    if(m_eventTimes%50 == 0)
    {
        emit timeout_500();
    }

    //发送1s信号
    if(m_eventTimes%100 == 0)
    {
        emit timeout_1000();
    }

    //每次累加
    m_eventTimes++;

    //防止越界
    if(m_eventTimes >= 0xfffffff0)
    {
        m_eventTimes = 0;
    }

}
