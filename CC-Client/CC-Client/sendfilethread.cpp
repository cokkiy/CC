#include "sendfilethread.h"
#include "StationInfo.h"
#include <Ice/Initialize.h>
#include <Ice/Stream.h>
#include <QDir>
#include <stdio.h>
using namespace std;

/*!
���������ļ��̶߳���
@param QStringList fileNames ��Ҫ���͵��ļ������б�
@param std::list<StationInfo * > stations ��Ҫ���͵��Ĺ���վ�б�
@param QObject * parent
@return �����ļ��̶߳���
���ߣ�cokkiy��������
����ʱ��:2016/3/24 9:31:41
*/
SendFileThread::SendFileThread(QStringList fileNames, QString dest, std::list<StationInfo*> stations,
	Ice::CommunicatorPtr communicator, QObject *parent)
	: QThread(parent), stations(stations), fileNames(fileNames), communicator(communicator), dest(dest)
{

}

SendFileThread::~SendFileThread()
{

}

//ʵ���ļ�����
void SendFileThread::run()
{
	for (StationInfo* s : stations)
	{
		if (s->fileProxy != NULL)
		{
			for (QString& file : fileNames)
			{
				QFileInfo fileInfo(file);
				//֪ͨ�ļ���С
				emit newFileSize(s, file, fileInfo.size());

				QString name = fileInfo.fileName();
				QString destFile = QStringLiteral("%1/%2").arg(dest).arg(name);
				try
				{							
					if (s->fileProxy->createFile(destFile.toStdWString()))
					{
						sendFile(file, s);

						//�ļ��������
						emit sendFileCompleted(s, file);
						s->fileProxy->closeFile();
					}
					else
					{
						//�����ļ�ʧ��
						emit failToSendFile(s, file, QStringLiteral("�޷������ļ�������վ�����ļ�ʧ�ܡ�"));
					}
				}
				catch (const CC::FileTransException& exception)
				{
					emit failToSendFile(s, file, QStringLiteral("�޷������ļ�������վ�����ļ�ʧ��:%1��")
						.arg(QString::fromStdWString(exception.Message)));
				}
				catch (const Ice::Exception& ex)
				{
					emit failToSendFile(s, file, QStringLiteral("�޷������ļ��������ļ������з����쳣:%1��").arg(ex.what()));
				}
				catch (...)
				{
					emit failToSendFile(s, file, QStringLiteral("�޷������ļ��������ļ������з���δ֪�쳣��"));
				}
			}
		}
		else
		{
			// �޷������ļ�
			emit failToSendFile(s, QStringLiteral("ȫ���ļ�"), QStringLiteral("�޷������ļ�������վ�����߻��пط���δ������"));
		}
	}
}

void SendFileThread::sendFile(QString &file, StationInfo* s)
{
	//�����ļ��ɹ�����ʼ�����ļ�����													 .
	std::vector< Ice::Byte > inParams, resultParams;
	size_t position = 0; //�ļ���ʼλ��
	int packetLength = 100 * 1024;//10K Bytes
	int maxPackets = 100;
	char* buf = new char[packetLength];
	//��binaryģʽ���ļ�
#ifdef Q_OS_WIN
	FILE* fp = _wfopen(file.toStdWString().c_str(), L"rb");
#else
	FILE* fp = fopen(file.toStdString().c_str(), "rb");
#endif
	list<tuple<Ice::AsyncResultPtr, QString&, size_t>> asyncResults;
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
			tuple<Ice::AsyncResultPtr, QString&, size_t> p(asyncResult, file, position);
			asyncResults.push_back(p);
			

			while (asyncResults.size() > maxPackets)
			{
				waitComplete(asyncResults, s, resultParams);
			}
		}

		//�ȴ��������ݷ������
		while (!asyncResults.empty())
		{
			waitComplete(asyncResults, s, resultParams);
		}

	}
	else
	{
		//�ļ�������
		emit failToSendFile(s, file, QStringLiteral("�ļ������ڡ�"));
	}
}

void SendFileThread::waitComplete(list <tuple<Ice::AsyncResultPtr, QString&, size_t>>& asyncResults, StationInfo* s, std::vector< Ice::Byte > resultParams)
{
	auto ar = asyncResults.front();
	Ice::AsyncResultPtr r = get<0>(ar);
	QString fileName = get<1>(ar);
	size_t position = get<2>(ar);
	asyncResults.pop_front();
	r->waitForCompleted();
	try
	{
		if (s->fileProxy->end_ice_invoke(resultParams, r))
		{
			//����ִ�����
			emit fileSended(s, fileName, (long long)position);
		}
		else
		{
			//�����û��Զ����쳣
			Ice::InputStreamPtr in = Ice::createInputStream(communicator, resultParams);
			in->startEncapsulation();
			Ice::BoolSeq result;
			in->read(result);
			CC::FileTransException ex;
			ex.__read(in);
			in->endEncapsulation();
			emit failToSendFile(s, fileName, QStringLiteral("����վ�޷�д���ļ�:%1").arg(QString::fromStdWString(ex.Message)));
		}
	}
	catch (const Ice::Exception& ex)
	{
		//����ϵͳ�쳣
		emit failToSendFile(s, fileName, QString::fromStdString(ex.what()));

	}
	catch (...)
	{
		//����ϵͳ�쳣
		emit failToSendFile(s, fileName, QStringLiteral("����δ֪�쳣��"));
	}
}
