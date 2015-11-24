#include "monitor.h"
#include <QTimerEvent>


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
        QThread::sleep(1);
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

