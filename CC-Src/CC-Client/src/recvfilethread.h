#ifndef RECVFILETHREAD_H
#define RECVFILETHREAD_H

#include <QThread>
#include <Ice/Communicator.h>
#include "StationInfo.h"
#include <list>

class RecvFileThread : public QThread
{
	Q_OBJECT

public:
	RecvFileThread(QString srcFile, QString destPath,Ice::CommunicatorPtr communicator, 
		std::list<StationInfo*> stations, QObject *parent=NULL);
	~RecvFileThread();
	virtual void run() override;

private:
	//源文件
	QString srcFile;
	//目的地址
	QString destPath;
	Ice::CommunicatorPtr communicator;
	std::list<StationInfo*> stations;

	//获取数据
	void getData(StationInfo* s, qint64 position, FILE* pf);

signals:
	/*!
	通知文件大小事件
	@param StationInfo * station 工作站
	@param long long size 文件大小
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/28 16:49:16
	*/
	void notifyFileSize(StationInfo* station, long long size);

	/*!
	通知获取数据错误
	@param StationInfo * station
	@param QString message 错误消息
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/28 17:07:08
	*/
	void getDataError(StationInfo* station, QString message);

	/*!
	创建文件失败通知
	@param StationInfo * station
	@param QString fileName
	@param QString message
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/29 10:00:03
	*/
	void createFileError(StationInfo* station, QString fileName, QString message);

	/*!
	文件传送完毕事件
	@param StationInfo * station 
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/29 12:06:07
	*/
	void transFileComplete(StationInfo* station);


	/*!
	文件已获取大小事件
	@param StationInfo * station
	@param long long length 已获取长度
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/29 12:11:38
	*/
	void fileGeted(StationInfo* station, long long length);
};

#endif // RECVFILETHREAD_H
