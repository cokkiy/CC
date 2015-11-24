#ifndef MONITOR_H
#define MONITOR_H

#include <QThread>
#include <QTimer>
#include "StationList.h"
#include <map>


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
    Monitor(QObject *parent = NULL);
    ~Monitor();
    //线程函数
    virtual void run() override;

    /*!
    设置工作站列表
    @param StationList * pStations 工作站列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/12 20:51:47
    */
    void setStationList(StationList* pStations);
private:
    //工作站列表
    StationList* pStations;
    //正在执行状态的工作站与对应计时器列表
    std::map<int, StationInfo*> stationInExecutingState;
    private slots:
    //工作站状态发生变化
    void stationStateChanged(const QObject* station);
    //定时器事件,提供执行计数工作
    virtual void timerEvent(QTimerEvent *) override;    

};

#endif // MONITOR_H
