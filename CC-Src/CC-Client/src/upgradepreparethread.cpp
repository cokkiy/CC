#include "upgradepreparethread.h"
#include "StationInfo.h"
#include "FileVersion.h"

UpgradePrepareThread::UpgradePrepareThread(std::list<StationInfo*> stations,
	Ice::CommunicatorPtr communicator, bool isUpgradeService, QObject *parent)
	: QThread(parent), stations(stations), communicator(communicator),
	isUpgradeService(isUpgradeService)
{
}

UpgradePrepareThread::~UpgradePrepareThread()
{
}

void UpgradePrepareThread::run()
{
	for (StationInfo* s : stations)
	{
		if (s->controlProxy != NULL)
		{
			try
			{				
				s->controlProxy->begin_getServerVersion([=](const CC::ServerVersion& v) {
					if (isUpgradeService)
					{		
						version = v.ServicesVersion;
						s->controlProxy->begin_getServicePath([=](const std::wstring& path) {
							emit onPrepared(s, path, version);
						},
							[=](const Ice::Exception& ex) {
							emit onFail(s, ex.what());
						});
					}
					else
					{
						FileVersion version(v.AppLuncherVersion);
						s->controlProxy->begin_getLuncherProxyPath([=, &version](const std::wstring& path) {
							emit onPrepared(s, path, version);
						},
							[=](const Ice::Exception& ex) {
							emit onFail(s, ex.what());
						});
					}
				},
					[=](const Ice::Exception& ex) {
					emit onFail(s, ex.what());
				});
			}
			catch (...)
			{
				emit onFail(s, QStringLiteral("获取对方信息失败，检查网络连接并确认中控服务正常运行。"));
			}
		}
		else
		{
			emit onFail(s, QStringLiteral("未连接，对方未开机或网络不同。"));
		}
	}
}
