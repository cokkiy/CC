#include "recvfilethread.h"
#include "StationInfo.h"
#include <Ice/Initialize.h>
#include <Ice/Stream.h>
#include <vector>
#include <QDir>
#include <stdio.h>
using namespace std;

RecvFileThread::RecvFileThread(QString srcFile, QString destPath, Ice::CommunicatorPtr communicator, 
	std::list<StationInfo*> stations, QObject *parent)
	: QThread(parent), srcFile(srcFile), destPath(destPath),
	communicator(communicator), stations(stations)
{

}

RecvFileThread::~RecvFileThread()
{

}

void RecvFileThread::run()
{
	for (StationInfo* s : stations)
	{
		if (s->fileProxy != NULL)
		{
			try
			{
				QString path = destPath + "/" + s->Name();
				QDir dir(path);
				if (!dir.exists())
				{
					dir.mkdir(path);
				}
				QString fileName = path + "/" + QFileInfo(srcFile).fileName();

#ifdef Q_OS_WIN
				FILE* pf = _wfopen(fileName.toStdWString().c_str(), L"wb");
#else
				FILE* pf = fopen(fileName.toStdString().c_str(), "wb");
#endif
				if (pf != NULL)
				{
					size_t size = s->fileProxy->getSize(srcFile.toStdWString());
					emit notifyFileSize(s, size);
					getData(s, 0, pf);					
				}
				else
				{
					emit createFileError(s, fileName, QStringLiteral("�޷����������ļ���"));
				}
			}
			catch (const std::exception& ex)
			{
				const CC::FileTransException* exception = dynamic_cast<const CC::FileTransException*>(&ex);
				if (exception != NULL)
				{
					emit getDataError(s, QStringLiteral("����%1,%2").arg(srcFile).arg(QString::fromStdWString(exception->Message)));
				}
				else
				{
					emit getDataError(s, ex.what());
				}
			}
		}
		else
		{
			//���ܽ��գ�����վû������������
			emit getDataError(s, QStringLiteral("����վ�����߻��пط���δ������"));
		}
	}
}

//��ȡ����
void RecvFileThread::getData(StationInfo* s, size_t position, FILE* pf)
{
	std::vector< Ice::Byte > inParams;

	Ice::OutputStreamPtr out = Ice::createOutputStream(communicator);
	out->startEncapsulation();
	out->write(srcFile.toStdWString());
	out->write<Ice::Long>(position);
	out->endEncapsulation();
	out->finished(inParams);

	s->fileProxy->begin_ice_invoke("getData", Ice::Normal, inParams,
		[=](bool result, const CC::ByteArray& outDatas)
	{
		if (result)
		{
			Ice::InputStreamPtr in = Ice::wrapInputStream(communicator, outDatas);	
			in->startEncapsulation();
			int length;
			in->read(length);
			vector<Ice::Byte> data;
			in->read(data);

			bool eof;
			in->read(eof);
			in->endEncapsulation();

			//д���ݵ��ļ�
			fseek(pf, position, SEEK_SET);
			if (fwrite(&data[0], 1, length, pf) == length)
			{
				//��ȡʣ���ļ�
				if (!eof)
				{
					getData(s, position + length, pf);
				}
				else
				{
					//�ر��ļ�
					fclose(pf);
					emit transFileComplete(s);
				}
			}
			else
			{
				fclose(pf);

				emit getDataError(s, QStringLiteral("д���ļ�ʱ��������"));
			}

			emit fileGeted(s, position + length);

			
		}
		else
		{
			//�����쳣
			Ice::InputStreamPtr in = Ice::createInputStream(communicator, outDatas);
			in->startEncapsulation();
			CC::FileTransException ex;
			ex.__read(in);
			in->endEncapsulation();

			emit getDataError(s, QString::fromStdWString(ex.Message));
			fclose(pf);

		}
	},
		[=](const Ice::Exception& ex)
	{
		fclose(pf);

		emit getDataError(s, ex.what());
	});
}
