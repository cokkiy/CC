#pragma once

#include <QThread>
#include <Ice/Communicator.h>
#include <QAbstractItemModel>
#include "FileVersion.h"
class StationInfo;
class UpgradePrepareThread : public QThread
{
	Q_OBJECT

private:
	std::list<StationInfo*> stations;
	Ice::CommunicatorPtr communicator;
	bool isUpgradeService;
	FileVersion version;
public:
	UpgradePrepareThread(std::list<StationInfo*> stations,
		Ice::CommunicatorPtr communicator, bool isUpgradeService = true,
		QObject *parent = NULL);
	~UpgradePrepareThread();

	virtual void run() override;

signals:
	void onPrepared(StationInfo* station, const std::wstring& path, const FileVersion& version);
	void onFail(StationInfo* station, const QString& msg);
};
