#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QThread>
#include "Version.h"
#include <Ice/Communicator.h>
#include <QString>
#include <tuple>
class StationInfo;

//////////////////////////////////////////////////////////////////////////
//工作站更新状态
//////////////////////////////////////////////////////////////////////////
struct StationUpdateState
{
	/* 工作站 */
	StationInfo* Station = NULL;
	std::wstring servicePath;
	std::wstring proxyPath;
	bool serviceNeedUpdate = false;
	bool proxyNeedUpdate = false;
	bool serviceUpdated = false;
	bool proxyUpdated = false;
};
//////////////////////////////////////////////////////////////////////////
//自动更新管理类
//管理工作站中控服务和应用代理的自动升级更新工作
//////////////////////////////////////////////////////////////////////////
class UpdateManager : public QThread
{
	Q_OBJECT

public:
	UpdateManager(Ice::CommunicatorPtr communicator, QObject *parent = NULL);
	~UpdateManager();

	//线程函数
	virtual void run() override;

	//服务或者是代理
	enum ServiceOrProxy
	{
		Service,
		Proxy
	};

	/*!
	添加工作站的版本信息到管理器
	@param StationInfo * s 工作站
	@param const CC::ServerVersion & v 版本信息
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/5/11 9:07:44
	*/
	void addStationVersion(StationInfo* s,const CC::ServerVersion& v);
private:
	//Ice communicator对象
	Ice::CommunicatorPtr communicator;
	//工作站版本信息
	std::map<StationInfo*, CC::ServerVersion> stationsVersion;	
	//即将更新的工作站列表
	std::list<StationUpdateState> toBeUpdatingStations;
	//版本最高的工作站
	StationInfo* highestVersionedStation = NULL;

	//需要更新的数量
	int needUpdateCount = 2;

	//新版本文件已经被下载
	bool newVersionDownloaded = false;
	//服务是否已被下载
	bool serviceDownloaded = false;
	//代理是否已被下载
	bool proxyDownloaded = false;

	//获取新版文件 localFile：本地文件 remotePath：远程文件
	void getNewVersionFile(QString &localFile, std::wstring remotePath);
	//更新文件
	void UpdateFile(QString &localFile, ServiceOrProxy serviceorProxy);


signals:
	/*!
	更新进度报告事件
	@param int percent 完成度（0-100）
	@param QString message 消息
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/5/12 16:08:05
	*/
	void UpdatingProgressReport(int percent, QString message);
};

#endif // UPDATEMANAGER_H
