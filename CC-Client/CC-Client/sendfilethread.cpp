#include "sendfilethread.h"
#include "StationInfo.h"
#include <Ice/Initialize.h>
#include <Ice/Stream.h>
#include <QDir>
#include <stdio.h>
#include <QDebug>
using namespace std;

/*!
创建发送文件线程对象
@param QStringList fileNames 需要发送的文件名称列表
@param std::list<StationInfo * > stations 需要发送到的工作站列表
@param QObject * parent
@return 发送文件线程对象
作者：cokkiy（张立民）
创建时间:2016/3/24 9:31:41
*/
SendFileThread::SendFileThread(QStringList fileNames, QString dest, std::list<StationInfo*> stations,
	Ice::CommunicatorPtr communicator, bool skipUnchanged, bool renameBeforeSend,
	bool isUpgrading, QObject *parent)
	: QThread(parent), stations(stations), fileNames(fileNames),
	communicator(communicator), skipUnchanged(skipUnchanged),
	renameBeforeSend(renameBeforeSend), isUpgrading(isUpgrading)
{
	this->dest = dest.replace("\\", "/");
}

SendFileThread::~SendFileThread()
{

}

//实现文件发送
void SendFileThread::run()
{
	for (StationInfo* s : stations)
	{
		if (s->fileProxy != NULL)
		{
			for (QString& file : fileNames)
			{
				QFileInfo fileInfo(file);
				if (fileInfo.isDir())
				{
					soureDir = file.replace("\\", "/");
					QString srcFilePath = fileInfo.filePath().replace("\\", "/");
					QString lastSrcPath = srcFilePath.section("/", -1);
					QString lastDestPath = dest.section("/", -1);

					if (lastDestPath != lastSrcPath)
					{
						if (!dest.endsWith("/"))
						{
							dest += "/";
						}

						dest = QStringLiteral("%1%2").arg(dest).arg(lastSrcPath);
					}

					//如果是文件夹，则保持文件夹结构
					keepDirStructure = true;					
				}
				SendFilesInDir(s, file, fileInfo);
			}
		}
		else
		{
			// 无法发送文件
			emit failToSendFile(s, QStringLiteral("全部文件"), QStringLiteral("无法发送文件，工作站不在线或中控服务未启动。"));
		}
	}

	for (StationInfo* s : stations)
	{
		if (s->fileProxy != NULL)
		{
			emit allCompleted(s, total, skip);
		}
	}
    //qDebug() << QStringLiteral("共发送%1个文件").arg(total);
}

//发送文件夹中的所有文件
void SendFileThread::SendFilesInDir(StationInfo* s, const QString& fileName, QFileInfo &fileInfo)
{
	if (fileInfo.isDir())
	{
		//如果是目录
		QDir dir(fileInfo.filePath());
		QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::AllDirs |	QDir::NoDotAndDotDot);
		for (QFileInfo entry : entries)
		{
			if (entry.isDir())
			{
				//目录
				SendFilesInDir(s, entry.filePath(), entry);
                //qDebug() << QStringLiteral("进入文件夹%1").arg(entry.absolutePath());
			}
			else
			{
				//文件
				if (rename(s, entry))
				{				
					sendFile(s, entry.filePath(), entry);
				}	
				else
				{
					emit failToSendFile(s, entry.fileName(), QStringLiteral("文件重命名失败。"));
				}
				
			}
		}

	}
	else
	{
		//文件
		if (rename(s, fileInfo))
		{
			sendFile(s, fileName, fileInfo);
		}
		else
		{
			emit failToSendFile(s, fileName, QStringLiteral("文件重命名失败。"));
		}
	}
}

bool SendFileThread::rename(StationInfo * s, QFileInfo &entry)
{
	bool retflag = true;
	QString name = entry.fileName();
	QString destFile = QStringLiteral("%1/%2").arg(dest).arg(name);
	if (renameBeforeSend
		&& (name.endsWith(".dll") || name.endsWith(".exe") || name.endsWith(".pdb")))
	{
		retflag = false;
		QString suffex = QStringLiteral(".%1").arg(QDateTime::currentDateTime().toString("yyMMddHHmmsszzz"));
		wstring newName = destFile.toStdWString().append(suffex.toStdWString());

		try {
			if (s->controlProxy->renameFile(destFile.toStdWString(), newName))
			{
				retflag = true;
			}
		}
		catch (...)
		{
		}
	}
	return retflag;
}

//发送文件
void SendFileThread::sendFile(StationInfo* s, const QString& file, QFileInfo &fileInfo)
{
	if (isUpgrading && (fileInfo.fileName() == "Config.Client" || fileInfo.fileName() == "Config.Server"))
	{
		//不发送配置文件
		return;
	}
	if (skipUnchanged && s->isSended(file, fileInfo.lastModified()))
	{
		emit fileNoChange(s, file);
		skip++;
		return;
	}
	//通知文件大小
	emit newFileSize(s, file, fileInfo.size());

	QString name = fileInfo.fileName();
	QString destFile = QStringLiteral("%1/%2").arg(dest).arg(name);

	if (keepDirStructure)
	{
		QString srcFilePath = fileInfo.filePath().replace("\\", "/");
		QString path = srcFilePath.remove(soureDir);
		destFile = QStringLiteral("%1%2").arg(dest).arg(path);
	}
	try
	{
		if (s->fileProxy->createFile(destFile.toStdWString()))
		{
			sendFileContents(file, s);
			s->fileProxy->closeFile();			
			s->addToSendedFileList(file, fileInfo.lastModified());
			//文件发送完毕
			emit sendFileCompleted(s, file);
			//qDebug() << QStringLiteral("文件%1发送完毕").arg(file);
			//qDebug() << QStringLiteral("%1").arg(file);
			total++;
		}
		else
		{
			//创建文件失败
			emit failToSendFile(s, file, QStringLiteral("无法发送文件，工作站创建文件失败。"));
		}
	}
	catch (const CC::FileTransException& exception)
	{
		emit failToSendFile(s, file, QStringLiteral("无法发送文件，工作站创建文件失败:%1。")
			.arg(QString::fromStdWString(exception.Message)));
	}
	catch (const Ice::Exception& ex)
	{
		emit failToSendFile(s, file, QStringLiteral("无法发送文件，发送文件过程中发生异常:%1。").arg(ex.what()));
	}
	catch (...)
	{
		emit failToSendFile(s, file, QStringLiteral("无法发送文件，发送文件过程中发生未知异常。"));
	}
}

void SendFileThread::sendFileContents(const QString &file, StationInfo* s)
{
	//创建文件成功，开始发送文件内容.													 
	std::vector< Ice::Byte > inParams, resultParams;
	qint64 position = 0; //文件起始位置
	int packetLength = 100 * 1024;//10K Bytes
	uint maxPackets = 100;
	char* buf = new char[packetLength];
	//以binary模式打开文件
#ifdef Q_OS_WIN
	FILE* fp = _wfopen(file.toStdWString().c_str(), L"rb");
#else
	FILE* fp = fopen(file.toStdString().c_str(), "rb");
#endif
	list<ResultTuple> asyncResults;
	if (fp != NULL)
	{
		while (!feof(fp))
		{
			size_t length = fread(buf, 1, packetLength, fp);
			Ice::OutputStreamPtr out = Ice::createOutputStream(communicator);
			out->startEncapsulation();
			out->write<Ice::Long>(position);
			out->write<Ice::Int>(length);
			out->write(buf, length);
			out->endEncapsulation();
			out->finished(inParams);
			Ice::AsyncResultPtr asyncResult = s->fileProxy->begin_ice_invoke("transData",
				Ice::Normal, inParams);
			asyncResult->waitForSent();
			position += length;
			ResultTuple p(asyncResult, file, position);
			asyncResults.push_back(p);			

			while (asyncResults.size() > maxPackets)
			{
				waitComplete(asyncResults, s, resultParams);
			}
		}

		//等待最后的数据发送完毕
		while (!asyncResults.empty())
		{
			waitComplete(asyncResults, s, resultParams);
		}

        fclose(fp);
	}
	else
	{
		//文件不存在
		emit failToSendFile(s, file, QStringLiteral("文件不存在。"));
        //qDebug() << QStringLiteral("%1文件不存在。").arg(file);
	}
}

void SendFileThread::waitComplete(list <ResultTuple>& asyncResults, StationInfo* s, 
	std::vector< Ice::Byte > resultParams)
{
	auto ar = asyncResults.front();
	Ice::AsyncResultPtr r = get<0>(ar);
	QString fileName = get<1>(ar);
	qint64 position = get<2>(ar);
	asyncResults.pop_front();
	r->waitForCompleted();
	try
	{
		if (s->fileProxy->end_ice_invoke(resultParams, r))
		{
			//正常执行完毕
			emit fileSended(s, fileName, (long long)position);
		}
		else
		{
			//发生用户自定义异常
			Ice::InputStreamPtr in = Ice::createInputStream(communicator, resultParams);
			in->startEncapsulation();
			Ice::BoolSeq result;
			in->read(result);
			CC::FileTransException ex;
			ex.__read(in);
			in->endEncapsulation();
			emit failToSendFile(s, fileName, QStringLiteral("工作站无法写入文件:%1").arg(QString::fromStdWString(ex.Message)));
		}
	}
	catch (const Ice::Exception& ex)
	{
		//发生系统异常
		emit failToSendFile(s, fileName, QString::fromStdString(ex.what()));
        //qDebug() << QStringLiteral("%1发送失败，原因:%2").arg(fileName).arg(ex.what());
	}
	catch (...)
	{
		//发生系统异常
		emit failToSendFile(s, fileName, QStringLiteral("发生未知异常。"));
        //qDebug() << QStringLiteral("%1发送失败，原因:发生未知异常").arg(fileName);
	}
}
