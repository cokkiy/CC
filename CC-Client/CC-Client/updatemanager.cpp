#include "updatemanager.h"
#include "StationInfo.h"
#include <thread>
#include <QDir>
#include <Ice/Stream.h>
#include <Ice/Initialize.h>
#include <Ice/LocalException.h>
#include <list>
#include "sendfilethread.h"
using namespace std;

//文件最大大小 800K
#define  MaxFileSize 800*1024

UpdateManager::UpdateManager(Ice::CommunicatorPtr communicator, QObject *parent)
	: QThread(parent), communicator(communicator)
{
}

UpdateManager::~UpdateManager()
{

}

void UpdateManager::run()
{
	//开始升级
	if (highestVersionedStation != NULL && !toBeUpdatingStations.empty())
	{
		for (auto& ss : toBeUpdatingStations)
		{

			if (ss.Station->controlProxy != NULL)
			{
				try
				{
					ss.servicePath = ss.Station->controlProxy->getServicePath();
					ss.proxyPath = ss.Station->controlProxy->getLuncherProxyPath();
				}
				catch (...)
				{

				}
			}
		}

		if (highestVersionedStation->controlProxy != NULL)
		{
			emit UpdatingProgressReport(0, QStringLiteral("开始准备升级..."));
			try
			{
				wstring servicePath = highestVersionedStation->controlProxy->getServicePath();
				wstring proxyPath = highestVersionedStation->controlProxy->getLuncherProxyPath();
				if (highestVersionedStation->fileProxy != NULL)
				{
					QString tempPath = QDir::tempPath();
					QString localServiceFile = tempPath + "/" + "~CC-StationService.exe";
					getNewVersionFile(localServiceFile, servicePath);
					emit UpdatingProgressReport(1 /(float)newVersionDownloaded * 100, QStringLiteral("新版中控服务已准备好"));
					UpdateFile(localServiceFile, Service);

					QString localProxyFile = tempPath + "/" + "~AppLuncher.exe";
					if (proxyPath != L"")
					{
						getNewVersionFile(localProxyFile, proxyPath);
						UpdateFile(localProxyFile, Proxy);
					}

					for_each(toBeUpdatingStations.begin(), toBeUpdatingStations.end(),
						[](const StationUpdateState& ss)
					{
						if (ss.serviceUpdated)
						{
							ss.Station->setState(StationInfo::Rebooting);
							ss.Station->controlProxy->reboot(true);
						}
					});
				}
			}
			catch (CC::FileTransException)
			{

			}
			catch (Ice::Exception)
			{
			}
			catch (...)
			{
			}
		}
		else
		{
			emit UpdatingProgressReport(0, QStringLiteral("无法获取最新版文件"));
		}
	}
	else
	{
		emit UpdatingProgressReport(0, QStringLiteral("没有需要升级的工作站"));
	}
}

void UpdateManager::UpdateFile(QString &localFile, ServiceOrProxy serviceorProxy)
{
#ifdef Q_OS_WIN
	FILE* fp = _wfopen(localFile.toStdWString().c_str(), L"rb");
#else
	FILE* fp = fopen(localFile.toStdString().c_str(), "rb");
#endif
	char* buf = new char[MaxFileSize]; //假定程序大小不超过512k
	long long int length = fread(buf, 1, MaxFileSize, fp);
	CC::ByteArray bytes;
	bytes.insert(bytes.begin(), buf, buf + length);
	if (!feof(fp))
	{
		emit UpdatingProgressReport(0, QStringLiteral("文件太大，无法自动更新。"));
		throw;
	}
	int count = 1;
	wstring tmpName = L"~CC-StationService.exe";
	if (serviceorProxy == Proxy)
		tmpName = L"~AppLuncher.exe";
	for_each(toBeUpdatingStations.begin(), toBeUpdatingStations.end(),
		[&](StationUpdateState& ss)
	{
		wstring remotePath;
		if (serviceorProxy == Service)
			remotePath = ss.servicePath;
		else
			remotePath = ss.proxyPath;
		if (remotePath != L"")
		{
			if (serviceorProxy == Service ? ss.serviceNeedUpdate : ss.proxyNeedUpdate)
			{
				size_t pos = remotePath.find_last_of('/');
				if (pos == wstring::npos)
					pos = remotePath.find_last_of('\\');
				size_t number = remotePath.length() - pos - 1;
				wstring newServicePath = remotePath;
				newServicePath = newServicePath.replace(pos + 1, number, tmpName);
				try
				{
					if (ss.Station->fileProxy->createFile(newServicePath))
					{
						if (ss.Station->fileProxy->transData(0, length, bytes))
						{
							if (ss.Station->fileProxy->closeFile())
							{
								wstring oldServiceTmpName = remotePath;
								oldServiceTmpName = oldServiceTmpName.replace(pos + 1, number, L"~" + tmpName);
								if (ss.Station->controlProxy->renameFile(remotePath, oldServiceTmpName))
								{
									if (ss.Station->controlProxy->renameFile(newServicePath, remotePath))
									{
										if (serviceorProxy == Service)
										{
											ss.serviceUpdated = true;
											emit UpdatingProgressReport(++count/ (float)needUpdateCount * 100, QStringLiteral("%1已完成中控服务更新。").arg(ss.Station->Name()));
										}
										else
										{
											ss.proxyUpdated = true;
											emit UpdatingProgressReport(++count / (float)needUpdateCount * 100, QStringLiteral("%1已完成应用代理更新。").arg(ss.Station->Name()));
										}										
									}
									else
									{
										ss.Station->controlProxy->renameFile(oldServiceTmpName, remotePath);
									}
								}
								else
								{
									if (serviceorProxy == Service)
									{
										emit UpdatingProgressReport(++count / (float)needUpdateCount * 100, QStringLiteral("%1无法更新应用代理。").arg(ss.Station->Name()));
									}
									else
									{
										emit UpdatingProgressReport(++count / (float)needUpdateCount * 100, QStringLiteral("%1无法更新应用代理。").arg(ss.Station->Name()));
									}
								}
							}
						}
					}
				}
				catch (const Ice::OperationNotExistException&)
				{
					emit UpdatingProgressReport(++count / (float)needUpdateCount * 100, QStringLiteral("%1无法更新,该工作站版本太低，不支持自动更新。").arg(ss.Station->Name()));
				}
				catch (...)
				{
					emit UpdatingProgressReport(++count / (float)needUpdateCount * 100, QStringLiteral("%1无法更新,发生未知错误，请手动升级。").arg(ss.Station->Name()));
				}
			}
		}
		else
		{
			emit UpdatingProgressReport(++count / (float)needUpdateCount * 100, QStringLiteral("%1无法更新,该工作站版本太低，不支持自动更新。").arg(ss.Station->Name()));
		}
	});
}

void UpdateManager::getNewVersionFile(QString &localFile, wstring remotePath)
{
#ifdef Q_OS_WIN
	FILE* pf = _wfopen(localFile.toStdWString().c_str(), L"wb");
#else
	FILE* pf = fopen(localFile.toStdString().c_str(), "wb");
#endif

	long long int positon = 0;
	int bytesReturn = 0;
	CC::ByteArray data;
	bool eof = highestVersionedStation->fileProxy->getData(remotePath, positon, bytesReturn, data);
	if (bytesReturn > 0)
	{
		fwrite(&data[0], 1, bytesReturn, pf);
	}
	while (!eof)
	{
		positon += bytesReturn;
		eof = highestVersionedStation->fileProxy->getData(remotePath, positon, bytesReturn, data);
		if (bytesReturn > 0)
		{
			fwrite(&data[0], 1, bytesReturn, pf);
		}
	}
	fclose(pf);
}


void UpdateManager::addStationVersion(StationInfo* s, const CC::ServerVersion& v)
{
	stationsVersion[s] = v;
	if (stationsVersion.size() >= 2)
	{
		auto max = max_element(stationsVersion.begin(), stationsVersion.end(),
            [](pair<StationInfo*,CC::ServerVersion> v1, pair<StationInfo*,CC::ServerVersion> v2)
		{
			return v1.second < v2.second;
		});

		highestVersionedStation = max->first;

		for_each(stationsVersion.begin(), stationsVersion.end(),
			[this, max](pair<StationInfo*, CC::ServerVersion> item)
		{
			if (max->first != item.first && max->second != item.second)
			{
				if (!item.first->NeedUpdate()) //只添加一次
				{
					item.first->setNeedUpdate(true);
					StationUpdateState state;
					state.Station = item.first;
					state.serviceNeedUpdate = true;
					needUpdateCount++;
					if (max->second.AppLuncherVersion>item.second.AppLuncherVersion)
					{
						state.proxyNeedUpdate = true;
						needUpdateCount++;
					}
					else
					{
						state.proxyNeedUpdate = false;
					}
					toBeUpdatingStations.push_back(state);
				}
			}
		});
	}
}


