#ifndef STATIONMANAGER_H
#define STATIONMANAGER_H

#include <QObject>
#include "StationList.h"
#include <QAbstractItemModel>
#include <Ice/Ice.h>

/************************************************************************
 @class StationManager:工作站管理类,管理工作站启动,关闭,重启,运行程序
 等动作
 ************************************************************************/
class StationManager : public QObject
{
    Q_OBJECT

public:
    /*!
    创建一个工作站管理类的实例
    @param StationList * stations 工作站列表
    @param const QModelIndexList & indexs 选择的工作站索引
    @param QObject * parent 
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:43:35
    */
    StationManager(StationList* stations,  const QModelIndexList& indexs = QModelIndexList(), QObject *parent = NULL);
    ~StationManager() = default;

    public slots:
    /*!
    全部开机
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:53:18
    */
    void powerOnAll();
    /*!
    开机选择的计算机
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:53:33
    */
    void powerOn();
    /*!
    重启选择的计算机
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:53:48
    */
    void reboot();
    
    /*!
    关闭选择的计算机
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:54:00
    */
    void shutdown();

    /*!
    重启全部的计算机
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:54:12
    */
    void rebootAll();
    /*!
    关闭全部的计算机
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:54:29
    */
    void shutdownAll();

    /*!
    重启程序（指显程序)
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:55:05
    */
    void restartApp();

    /*!
    重启全部工作站程序
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/16 18:10:25
    */
    void restartAllApp();

    /*!
    退出程序（指显程序)
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:55:19
    */
    void exitApp();

    /*!
    退出全部工作站指显程序
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:58:22
    */
    void exitAllApp();
    /*!
    启动程序
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:55:34
    */
    void startApp();

    /*!
    启动所有工作站指显程序
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:57:46
    */
    void startAllApp();
    /*!
    强制退出程序
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 15:56:02
    */
    void forceExitApp();  

    /*!
    设置监视间隔
    @param int interval 监视间隔（毫秒)
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/01 11:42:42
    */
    void setInterval(int interval);
private:
    //全部工作站列表
    StationList* pStations;
    //选择的工作站索引
    QModelIndexList stationIndexs;

    //ICE通信对象
    Ice::CommunicatorPtr  communicator;

    //start specified station's app
    void startApp(StationInfo* s);

    //exit specified station's app
    void exitApp(StationInfo* s);

    //reboot specified station
    void reboot(StationInfo* s);

    //shutdown specified station
    void shutdown(StationInfo* s);

    // restart specified station's app
    void restartApp(StationInfo* s);
};

#endif // STATIONMANAGER_H
