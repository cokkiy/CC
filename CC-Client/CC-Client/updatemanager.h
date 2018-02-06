#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QThread>
#include "Version.h"
#include <Ice/Communicator.h>
#include <QString>
#include <tuple>
class StationInfo;

//////////////////////////////////////////////////////////////////////////
//����վ����״̬
//////////////////////////////////////////////////////////////////////////
struct StationUpdateState
{
	/* ����վ */
	StationInfo* Station = NULL;
	std::wstring servicePath;
	std::wstring proxyPath;
	bool serviceNeedUpdate = false;
	bool proxyNeedUpdate = false;
	bool serviceUpdated = false;
	bool proxyUpdated = false;
};
//////////////////////////////////////////////////////////////////////////
//�Զ����¹�����
//������վ�пط����Ӧ�ô�����Զ��������¹���
//////////////////////////////////////////////////////////////////////////
class UpdateManager : public QThread
{
	Q_OBJECT

public:
	UpdateManager(Ice::CommunicatorPtr communicator, QObject *parent = NULL);
	~UpdateManager();

	//�̺߳���
	virtual void run() override;

	//��������Ǵ���
	enum ServiceOrProxy
	{
		Service,
		Proxy
	};

	/*!
	��ӹ���վ�İ汾��Ϣ��������
	@param StationInfo * s ����վ
	@param const CC::ServerVersion & v �汾��Ϣ
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/5/11 9:07:44
	*/
	void addStationVersion(StationInfo* s,const CC::ServerVersion& v);
private:
	//Ice communicator����
	Ice::CommunicatorPtr communicator;
	//����վ�汾��Ϣ
	std::map<StationInfo*, CC::ServerVersion> stationsVersion;	
	//�������µĹ���վ�б�
	std::list<StationUpdateState> toBeUpdatingStations;
	//�汾��ߵĹ���վ
	StationInfo* highestVersionedStation = NULL;

	//��Ҫ���µ�����
	int needUpdateCount = 2;

	//�°汾�ļ��Ѿ�������
	bool newVersionDownloaded = false;
	//�����Ƿ��ѱ�����
	bool serviceDownloaded = false;
	//�����Ƿ��ѱ�����
	bool proxyDownloaded = false;

	//��ȡ�°��ļ� localFile�������ļ� remotePath��Զ���ļ�
	void getNewVersionFile(QString &localFile, std::wstring remotePath);
	//�����ļ�
	void UpdateFile(QString &localFile, ServiceOrProxy serviceorProxy);


signals:
	/*!
	���½��ȱ����¼�
	@param int percent ��ɶȣ�0-100��
	@param QString message ��Ϣ
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/5/12 16:08:05
	*/
	void UpdatingProgressReport(int percent, QString message);
};

#endif // UPDATEMANAGER_H
