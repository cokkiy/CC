#pragma once

#include <QDialog>
#include <QAbstractItemModel>
#include <Ice/Communicator.h>
#include <list>
#include "FileVersion.h"
namespace Ui { class UpgradeStationServiceDlg; };

class StationList;
class StationInfo;
class StationBar;

class UpgradeStationServiceDlg : public QDialog
{
	Q_OBJECT

public:
	UpgradeStationServiceDlg(StationList* pStations, const QModelIndexList& selectedIndexs,
		bool allStations, Ice::CommunicatorPtr communicator, bool isService = true, QWidget *parent = Q_NULLPTR);
	~UpgradeStationServiceDlg();

private:
	Ui::UpgradeStationServiceDlg *ui;
	Ice::CommunicatorPtr communicator;
	QStringList fileNames;
	std::list<StationInfo*> stations;
	std::map<StationInfo*, StationBar*> station_bar;
	//要升级文件的版本
	FileVersion upgradingFileVersion;
	bool isUpgradeService;
	bool allStations;
	//创建布局
	void createLayout(StationList* pStations, const QModelIndexList& selectedIndexs, bool allStations);

	/// <summary>
	/// 把字节序列转化为Version结构
	/// </summary>
	/// <param name="versionArray"></param>
	/// <returns></returns>
	FileVersion toVersion(const QByteArray& versionArray);

	//信号处理函数
	void on_failToSendFile(StationInfo* station, QString fileName, QString message);
	void on_newFileSize(StationInfo* station, QString fileName, qint64 size);
	void on_sendFileCompleted(StationInfo* station, QString fileName);
	void on_fileSended(StationInfo* station, QString fileName, qint64 size);
	void on_allCompleted(StationInfo* station, int total, int skip);
	void on_skipFile(StationInfo* station, QString file);
	void on_ViewLog(StationInfo* station);
	void on_upgradePrepared(StationInfo* station, const std::wstring& path, const FileVersion& v);
	void on_prepareFailed(StationInfo* station, const QString& msg);
	//添加日志信息到工作站日志中
	void addToLog(StationInfo* station, QString message, bool isError = false);

	//槽函数
	private slots:
	//浏览要发送的文件
	void on_browserPushButton_clicked();
	//发送按钮按下
	void on_upgradePushButton_clicked();
};
