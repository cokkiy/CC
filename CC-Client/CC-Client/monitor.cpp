#include "monitor.h"
#include <QTimerEvent>
#include <QDateTime>
#include <time.h>


Monitor::Monitor(int interval, QObject *parent)
    : QThread(parent)
{
    this->interval = interval; //默认1秒
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
        QThread::sleep(interval);

        if(stop)
            break;

        //获取当前时间
        time_t now = time(NULL);
        for (auto& station : *pStations)
        {
            if (station.IsRunning())
            {
                if (now - station.lastTick >= 5 * interval)
                {
                    //如果大于5 * interval秒仍然没有收到新的状态,则设置为离线
                    station.setState(StationInfo::Unknown);
                }
                else if (now - station.lastTick >= 2 * interval)
                {
                    //如果大于2秒仍然没有收到新的状态,则报警
                    station.setState(StationInfo::NoHeartbeat, QStringLiteral("%1秒内没有收到该工作站状态,请检查").arg(2 * interval));
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
    this->pStations->subscribeStationAddedEvent(this, SLOT(stationAdded(StationInfo*)));
}

/*!
设置检测间隔
@param int interval 检测间隔
@return void
作者：cokkiy（张立民)
创建时间：2015/12/04 11:02:24
*/
void Monitor::setInterval(int interval)
{
    this->interval = interval;
}

//停止监听
void Monitor::Stop()
{
    this->stop = true;
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

//响应工作站被添加事件
void Monitor::stationAdded(StationInfo* addedStation)
{
    addedStation->subscribeStateChanged(this, SLOT(stationStateChanged(const QObject*)));
}

