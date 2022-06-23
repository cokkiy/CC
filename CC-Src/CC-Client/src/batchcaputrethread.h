#pragma once
#ifndef BATCHCAPUTRETHREAD_H
#define BATCHCAPUTRETHREAD_H

#include <QThread>
#include <list>
#include <Ice/Config.h>
class StationInfo;

#ifdef Q_OS_LINUX
using byte = unsigned char;
#endif

///////////////////////////////////////////////////////
//批量截图线程
///////////////////////////////////////////////////////
class BatchCaputreThread : public QThread
{
	Q_OBJECT

public:
	BatchCaputreThread(std::list<StationInfo*> stations,QString destDir,
		bool saveInOneDir = true, QObject *parent = NULL);
	~BatchCaputreThread();
	virtual void run() override;

private:
	std::list<StationInfo*> stations;
	bool saveInOnDirectory;
	QString destDir;
	std::map<StationInfo*, std::vector<byte>> StationBuffers;
	void getCaptureData(StationInfo* station, Ice::Long position);

	//计算文件名称
	QString getFileName(StationInfo* station, QString ext="png");

signals:
	/*!
	屏幕图像已截获通知事件
	@param StationInfo * 工作站
	@param bool 是否成功
	@param QString message 附加消息
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/4/19 16:04:14
	*/
	void ScreeenCaptured(StationInfo*, bool result = true, QString message = QStringLiteral(""));
};

#endif // BATCHCAPUTRETHREAD_H
