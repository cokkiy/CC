#ifndef STATIONMANAGER_H
#define STATIONMANAGER_H

#include <QObject>
#include "StationList.h"
#include <QAbstractItemModel>
#include <Ice/Ice.h>
#include <QByteArray>

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
    StationManager(StationList* stations=NULL,  const QModelIndexList& indexs = QModelIndexList(), QObject *parent = NULL);
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
    @param int interval 监视间隔
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/01 11:42:42
    */
    void setInterval(int interval);

public:

	/*!
	开机指定工作站
	@param StationInfo * s 要开机的工作站
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/4/5 15:52:56
	*/
	void powerOn(StationInfo* s);
	
	/*!
	重启指定工作站
	@param StationInfo * s
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/4/5 15:53:20
	*/
	void reboot(StationInfo* s);

	/*!
	关闭指定工作站
	@param StationInfo * s
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/4/5 15:53:38
	*/
	void shutdown(StationInfo* s);

	/*!
	设置指定工作站监视间隔
	@param StationInfo * s
	@param int second 监视间隔（秒）
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/4/5 15:53:55
	*/
	void setInterval(StationInfo* s, int second);
	/*!
	选定工作站清屏
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/5/6 8:38:22
	*/
	void clearPage();
	/*!
	选定工作站全屏
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/5/6 8:38:39
	*/
	void fullScreen();
	/*!
	选定工作站切换到实时模式
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/5/6 8:39:05
	*/
	void realTime();
	/*!
	选定工作站切换到前一页
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/5/6 8:39:20
	*/
	void prevPage();
	/*!
	选定工作站切换到下一页
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/5/6 8:39:36
	*/
	void nextPage();
private:
	//监视间隔（秒）
	int interval;

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

    // restart specified station's app
    void restartApp(StationInfo* s);

	//mac 2 bytearray
	QByteArray MAC2Byte(QString mac);
	// 发送开机指令包
	void sendPowerOnPacket(StationInfo* s);
};

#endif // STATIONMANAGER_H
