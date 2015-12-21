#ifndef EXECUTCOUNTING_H
#define EXECUTCOUNTING_H

#include <QThread>
#include "StationInfo.h"
#include <list>
#include <mutex>

/*
 执行计数线程类,完成执行动作计数
 **/
class ExecutCounting : public QThread
{
    Q_OBJECT

public:
    ExecutCounting(QObject *parent=NULL);
    ~ExecutCounting();

    //线程函数
    virtual void run() override;
    /*!
    添加执行状态工作站 
    @param StationInfo * 执行状态工作站 
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/14 17:54:41
    */
    void addExecutingStateStation(StationInfo*);

    /*!
    从执行状态列表移除工作站
    @param StationInfo * 要移除的工作站
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/14 18:00:03
    */
    void removeStationFromExecutingStationsList(StationInfo*);

    /*!
    停止执行,退出线程函数
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/14 21:24:11
    */
    void Stop();
private:
    /*
     *在执行状态的工作站列表
     **/
    std::list<StationInfo*> inExecutingStateStations;

    //同步变量
    std::mutex cvMutex;
    std::condition_variable cv;
    std::mutex mutexForRemove;
    //是否停止
    bool stop = false;
};

#endif // EXECUTCOUNTING_H
