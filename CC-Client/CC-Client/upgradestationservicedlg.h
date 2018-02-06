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
	//Ҫ�����ļ��İ汾
	FileVersion upgradingFileVersion;
	bool isUpgradeService;
	bool allStations;
	//��������
	void createLayout(StationList* pStations, const QModelIndexList& selectedIndexs, bool allStations);

	/// <summary>
	/// ���ֽ�����ת��ΪVersion�ṹ
	/// </summary>
	/// <param name="versionArray"></param>
	/// <returns></returns>
	FileVersion toVersion(const QByteArray& versionArray);

	//�źŴ�����
	void on_failToSendFile(StationInfo* station, QString fileName, QString message);
	void on_newFileSize(StationInfo* station, QString fileName, qint64 size);
	void on_sendFileCompleted(StationInfo* station, QString fileName);
	void on_fileSended(StationInfo* station, QString fileName, qint64 size);
	void on_allCompleted(StationInfo* station, int total, int skip);
	void on_skipFile(StationInfo* station, QString file);
	void on_ViewLog(StationInfo* station);
	void on_upgradePrepared(StationInfo* station, const std::wstring& path, const FileVersion& v);
	void on_prepareFailed(StationInfo* station, const QString& msg);
	//�����־��Ϣ������վ��־��
	void addToLog(StationInfo* station, QString message, bool isError = false);

	//�ۺ���
	private slots:
	//���Ҫ���͵��ļ�
	void on_browserPushButton_clicked();
	//���Ͱ�ť����
	void on_upgradePushButton_clicked();
};
