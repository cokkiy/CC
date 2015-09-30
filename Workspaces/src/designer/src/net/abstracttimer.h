#ifndef ABSTRACTTIMER_H
#define ABSTRACTTIMER_H
#include "net_global.h"
#include <QObject>
#include <QVector>


//定时器
class NETSHARED_EXPORT TimerInterface : public QObject
{
    Q_OBJECT
public:
    explicit TimerInterface(QObject *parent = 0);

    //定时器事件
    virtual void timerEvent_r() = 0;

signals:
    //时间信号，这里预定义了4种：100ms、200ms、500ms和1000ms，如有需要可以增加
    void timeout_100();
    void timeout_200();
    void timeout_500();
    void timeout_1000();

};

#endif // ABSTRACTTIMER_H
