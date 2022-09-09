#ifndef MONITOR_H
#define MONITOR_H

#include <QThread>
#include <QTimer>
#include "StationList.h"
#include <map>
#include <thread>
#include <mutex>
#include "executcounting.h"


/*!
 * \class Monitor
 *
 * \brief 状态监视线程类,实现所有工作站状态监视和
 * 判断,并在执行状态下,为工作站提供执行计数工作
 * \author 张立民(cokkiy)
 * \date 十一月 2015
 */
class Monitor : public QThread
{
    Q_OBJECT

public:
    /*!
    创建监视对象
    @param int interval 监视间隔,默认为1秒
    @param QObject * parent 
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/04 11:04:04
    */
    Monitor(int interval = 1, QObject *parent = NULL);
    //~Monitor();

    //线程核心函数
    virtual void run() override;

    /*!
    开始运行
    @param QThread::Priority priority 线程优先级
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/14 21:22:45
    */
    virtual void start(QThread::Priority priority = QThread::NormalPriority);

    /*!
    设置工作站列表
    @param StationList * pStations 工作站列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/12 20:51:47
    */
    void setStationList(StationList* pStations);
    
    /*!
    设置检测间隔
    @param int interval 检测间隔
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/04 11:02:24
    */
    void setInterval(int interval);
    /*!
    停止监听
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/04 11:13:42
    */
    void Stop();
private:
    //工作站列表
    StationList* pStations;
    //正在执行状态的工作站与对应计时器列表
    std::map<int, StationInfo*> stationInExecutingState;
    //正在执行状态的工作站<->对应计时器列表
    std::map<void*, int> stationsTimerId;
    //检测间隔
    int interval;
    //是否停止
    bool stop = false;
    //执行动作计数线程
    ExecutCounting* executCounting;

    private slots:
    //工作站状态发生变化
    void stationStateChanged(const QObject* station);

    //工作站被添加
    void stationAdded(StationInfo* addedStation);
};



#endif // MONITOR_H
