#ifndef SENDFILETHREAD_H
#define SENDFILETHREAD_H

#include <QThread>
#include <list>
#include <QStringList>
#include <Ice/Communicator.h>
#include <mutex>
#include <QFileInfo>
class StationInfo;
/************************************************************************/
/* 文件发送线程，实现独立线程发送文件                                         */
/************************************************************************/
using ResultTuple = std::tuple<Ice::AsyncResultPtr, const QString&, size_t>;

class SendFileThread : public QThread
{
	Q_OBJECT

public:
	/*!
	创建发送文件线程对象
	@param QStringList fileNames 需要发送的文件名称列表
	@param QString dest 文件保存位置
	@param std::list<StationInfo * > stations 需要发送到的工作站列表
	@param QObject * parent
	@return 发送文件线程对象
	作者：cokkiy（张立民）
	创建时间:2016/3/24 9:31:41
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
	QString soureDir;
	//保持文件夹结构
	bool keepDirStructure = false;

	//向指定工作站发送文件内容
	void sendFileContents(const QString &file, StationInfo* s);
	//完成发送方法
	void waitComplete(std::list <ResultTuple>& asyncResults, StationInfo* s, std::vector< Ice::Byte > resultParams);
	//发送文件夹中的所有文件
	void SendFilesInDir(StationInfo* s, const QString& fileName, QFileInfo &fileInfo);
	//发送文件
	void sendFile(StationInfo* s, const QString& file, QFileInfo &fileInfo);

signals:
	//信号
	/*!
	无法发送文件事件，当文件无法发送时触发
	@param StationInfo * station 工作站信息
	@param QString fileName 文件名
	@param QString message 消息
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/25 15:35:19
	*/
	void failToSendFile(StationInfo* station, QString fileName, QString message);

	/*!
	文件传送完毕事件
	@param StationInfo * station 工作站信息
	@param QString fileName 文件名
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/25 15:53:37
	*/
	void sendFileCompleted(StationInfo* station, QString fileName);
	/*!
	新文件大小事件，通知文件大小
	@param StationInfo * station 工作站
	@param QString fileName 文件名
	@param size_t size 文件大小
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/25 16:00:41
	*/
	void newFileSize(StationInfo* station, QString fileName, long long size);

	/*!
	文件已发送size字节内容
	@param StationInfo * station 工作站
	@param QString fileName 文件名
	@param size_t size 发送大小
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/25 16:21:28
	*/
	void fileSended(StationInfo* station, QString fileName, long long size);
};

#endif // SENDFILETHREAD_H
