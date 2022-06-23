#include "batchcaputrethread.h"
#include "StationInfo.h"
#include <QImage>
#include <QDateTime>
#include <QDir>
#include <QFile>
using namespace std;

BatchCaputreThread::BatchCaputreThread(std::list<StationInfo*> stations, QString destDir,
	bool saveInOneDir, QObject *parent)
	: QThread(parent), stations(stations), destDir(destDir), saveInOnDirectory(saveInOneDir)
{
	for (StationInfo* s : stations)
	{
		StationBuffers.insert(make_pair(s, vector<byte>()));
	}
}

BatchCaputreThread::~BatchCaputreThread()
{

}

void BatchCaputreThread::run()
{
	for (StationInfo* s : stations)
	{
		try
		{
			Ice::Long position = 0;
			if (s->IsRunning() && s->controlProxy != NULL)
			{
				getCaptureData(s, position);
			}
			else
			{
				emit ScreeenCaptured(s, false, QStringLiteral("未开机或中控服务不可用。"));
			}
		}
		catch (const Ice::Exception& ex)
		{
			emit ScreeenCaptured(s, false, ex.what());
		}
		
	}
}

void BatchCaputreThread::getCaptureData(StationInfo* station, Ice::Long position)
{
	station->controlProxy->begin_captureScreen(position,
		[=](bool eof, int length, const CC::ByteArray& data)
	{
		std::vector<byte>& buffer = StationBuffers[station];
		buffer.insert(buffer.end(), data.begin(), data.end());
		if (!eof)
		{
			getCaptureData(station, position + length);
		}
		else
		{
			//保存图片文件
			QString fileName = getFileName(station);

			QImage image;
			image.loadFromData(&buffer[0], buffer.size());
			image.save(fileName, "png");
			emit ScreeenCaptured(station);
		}
	},
		[=](const Ice::Exception& ex)
	{
		QString message;
		const CC::FileTransException* e = dynamic_cast<const CC::FileTransException*>(&ex);
		if (e != NULL)
		{
			message = QString::fromStdWString(e->Message);
		}
		else
		{
			message = ex.what();
		}
		//保存文本文件
		QString fileName = getFileName(station, "txt");
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		file.write(message.toStdString().c_str());
		emit ScreeenCaptured(station, false, QStringLiteral("发生错误，请查看保存的详细信息。"));
	});
}
//获取文件名
QString BatchCaputreThread::getFileName(StationInfo* station, QString ext)
{
	QDir dir(destDir);
	if (!dir.exists())
	{
		dir.mkdir(destDir);
	}
	destDir = destDir.replace("\\", "/");
	if (!destDir.endsWith("/"))
	{
		destDir += "/";
	}
	QString fileName = QStringLiteral("%1%2-%3.%4").arg(destDir)
		.arg(station->Name()).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd-HH.mm.ss"))
		.arg(ext);
	if (!saveInOnDirectory)
	{
		QString dest = destDir + station->Name() + "/";
		QDir newDir(dest);
		if (!newDir.exists())
		{
			newDir.mkdir(dest);
		}
		fileName = QStringLiteral("%1%2.%3")
			.arg(dest).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd-HH.mm.ss"))
			.arg(ext);
	}

	return fileName;
}
