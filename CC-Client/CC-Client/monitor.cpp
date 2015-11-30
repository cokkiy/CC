#include "monitor.h"
#include <QTimerEvent>
#include <QDateTime>
#include <time.h>


Monitor::Monitor(QObject *parent)
    : QThread(parent)
{    
}

Monitor::~Monitor()
{

}

//线程执行函数
//执行监视工作站状态任务
void Monitor::run()
{
    //启动工作站执行动作心跳计数线程    
    while (true)
    {
        //等待1秒
        QThread::sleep(2);
        //获取当前时间
        time_t now = time(NULL);
        for (auto& station : *pStations)
        {
            if (station.StationIsRunning())
            {
                if (now - station.lastTick >= 10)
                {
                    //10秒内没有收到状态,则转为未知状态
                    station.setState(StationInfo::Unkonown);
                }
                else if (now - station.lastTick >= 5)
                {
                    //如果大于5秒仍然没有收到新的状态,则报错
                    station.setState(StationInfo::Error);
                    station.setHint(QStringLiteral("5秒内没有收到该工作站状态,请检查"));
                }
                else if (now - station.lastTick >= 2)
                {
                    //如果大于2秒仍然没有收到新的状态,则报警
                    station.setState(StationInfo::Warning);
                    station.setHint(QStringLiteral("2秒内没有收到该工作站状态,请检查"));
                }
            }
        }
    }
}

/*!
设置工作站列表
@param StationList * pStations 工作站列表
@return void
作者：cokkiy（张立民)
创建时间：2015/11/12 20:51:47
*/
void Monitor::setStationList(StationList* pStations)
{
    this->pStations = pStations;
    this->pStations->subscribeAllStationsStateChangedEvent(this, SLOT(stationStateChanged(const QObject* )));
}

//工作站状态发生变化
void Monitor::stationStateChanged(const QObject* station)
{
    //如果是状态发生变化
    StationInfo* pStation = (StationInfo*)station;
    if (pStation->inExecutingState())
    {
        //在执行状态
        int id = startTimer(1000);
        if (id != 0)
        {
            stationInExecutingState.insert({ id,pStation });
        }
    }
}
//定时器事件,提供执行计数工作
void Monitor::timerEvent(QTimerEvent * event)
{
    int id = event->timerId();
    StationInfo* pStation = stationInExecutingState[id];
    if (pStation != NULL)
    {
        if (pStation->ExecuteCounting() >= pStation->PowerOnTimeout) //180秒
        {
            //超过3分钟,加电失败
            pStation->setState(StationInfo::PowerOnFailure);
            killTimer(id);
            stationInExecutingState.erase(id);
        }
        else
        {
            pStation->setExecuteCounting(pStation->ExecuteCounting() + 1);
        }
    }
}

