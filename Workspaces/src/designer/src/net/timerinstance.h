#ifndef TIMERINSTANCE_H
#define TIMERINSTANCE_H
#include "net_global.h"
#include "abstracttimer.h"
#include <QTimer>

class NETSHARED_EXPORT  TimerInstance: public TimerInterface
{
public:
    TimerInstance();

    //定时器事件
    void timerEvent_r();

private:
    ~TimerInstance();

    //定时器事件触发的次数(由此可得到需要的定时周期)
    quint32 m_eventTimes;
};

#endif // TIMERINSTANCE_H
