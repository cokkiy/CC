#ifndef CONTROLANDMANAGER_H
#define CONTROLANDMANAGER_H

#include <QThread>
#include <Ice/Ice.h>
#include "StationList.h"

/*
 工作站控制和信息管理类,负责接收工作站发生的状态信息,
 并发送控制命令到工作站
 **/
class ControlAndManager : public QThread
{
    Q_OBJECT

public:
    ControlAndManager(StationList* pStations, QObject *parent);
    ~ControlAndManager();

    /*!
    线程核心函数,负责接收控制命令,并向工作站发送
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/15 15:56:35
    */
    virtual void run() override;
    /*!
    启动工作站状态接收并启动命令接收线程
    @param QThread::Priority priority   
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/15 15:58:22
    */
    void start(QThread::Priority priority = QThread::InheritPriority);
private:
    Ice::CommunicatorPtr communicator;
    Ice::ObjectAdapterPtr adapter;
    StationList* pStations;

public slots:
    void startApp(CC::AppStartParameters startParams, StationInfo *pStation);

};

#endif // CONTROLANDMANAGER_H
