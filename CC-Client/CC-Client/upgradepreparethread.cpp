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
				emit onFail(s, QStringLiteral("��ȡ�Է���Ϣʧ�ܣ�����������Ӳ�ȷ���пط����������С�"));
			}
		}
		else
		{
			emit onFail(s, QStringLiteral("δ���ӣ��Է�δ���������粻ͬ��"));
		}
	}
}
