#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QThread>
#include <list>
#include <QStringList>
#include <Ice/Communicator.h>
#include <mutex>
class StationInfo;
/************************************************************************/
/* �ļ������̣߳�ʵ�ֶ����̷߳����ļ�                                                                     */
/************************************************************************/
class SendFileThread : public QThread
{
	Q_OBJECT

public:
	/*!
	���������ļ��̶߳���
	@param QStringList fileNames ��Ҫ���͵��ļ������б�
	@param QString dest �ļ�����λ��
	@param std::list<StationInfo * > stations ��Ҫ���͵��Ĺ���վ�б�
	@param QObject * parent
	@return �����ļ��̶߳���
	���ߣ�cokkiy��������
	����ʱ��:2016/3/24 9:31:41
	*/
	SendFileThread(QStringList fileNames, QString dest, std::list<StationInfo*> stations,
		Ice::CommunicatorPtr communicator, QObject *parent = NULL);
	~SendFileThread();
	virtual void run() override;


private:
	QStringList fileNames;
	std::list<StationInfo*> stations;
	Ice::CommunicatorPtr communicator;
	QString dest;


	//��ָ������վ�����ļ�
	void sendFile(QString &file, StationInfo* s);
	//��ɷ��ͷ���
	void waitComplete(std::list < std::tuple<Ice::AsyncResultPtr, QString&, size_t>>& asyncResults, StationInfo* s, std::vector< Ice::Byte > resultParams);


signals:
	//�ź�
	/*!
	�޷������ļ��¼������ļ��޷�����ʱ����
	@param StationInfo * station ����վ��Ϣ
	@param QString fileName �ļ���
	@param QString message ��Ϣ
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/25 15:35:19
	*/
	void failToSendFile(StationInfo* station, QString fileName, QString message);

	/*!
	�ļ���������¼�
	@param StationInfo * station ����վ��Ϣ
	@param QString fileName �ļ���
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/25 15:53:37
	*/
	void sendFileCompleted(StationInfo* station, QString fileName);
	/*!
	���ļ���С�¼���֪ͨ�ļ���С
	@param StationInfo * station ����վ
	@param QString fileName �ļ���
	@param size_t size �ļ���С
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/25 16:00:41
	*/
	void newFileSize(StationInfo* station, QString fileName, long long size);

	/*!
	�ļ��ѷ���size�ֽ�����
	@param StationInfo * station ����վ
	@param QString fileName �ļ���
	@param size_t size ���ʹ�С
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/25 16:21:28
	*/
	void fileSended(StationInfo* station, QString fileName, long long size);
};

#endif // SENDFILETHREAD_H
