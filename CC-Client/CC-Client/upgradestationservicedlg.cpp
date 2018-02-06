#include "upgradestationservicedlg.h"
#include "ui_upgradestationservicedlg.h"
#include "StationList.h"
#include "stationbar.h"
#include <list>
#include "viewlogdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QProcess>
#include "sendfilethread.h"
#include "upgradepreparethread.h"
using namespace std;

UpgradeStationServiceDlg::UpgradeStationServiceDlg(StationList* pStations, const QModelIndexList& selectedIndexs,
	bool allStations, Ice::CommunicatorPtr communicator, bool isService, QWidget *parent)
	: QDialog(parent), isUpgradeService(isService),communicator(communicator),
	allStations(allStations)
{
	ui = new Ui::UpgradeStationServiceDlg();
	ui->setupUi(this);

	if (isService)
	{
		ui->label->setText(QStringLiteral("ѡ���пط��������ļ��У�"));
		this->setWindowTitle(QStringLiteral("�����пط���"));
	}
	else
	{
		ui->label->setText(QStringLiteral("ѡ���п�Ӧ�ô��������ļ��У�"));
		this->setWindowTitle(QStringLiteral("�����п�Ӧ�ô���"));
	}

	createLayout(pStations, selectedIndexs, allStations);
}

UpgradeStationServiceDlg::~UpgradeStationServiceDlg()
{
	delete ui;
}

void UpgradeStationServiceDlg::createLayout(StationList * pStations, const QModelIndexList & selectedIndexs, bool allStations)
{
	QVBoxLayout* verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);

	if (allStations)
	{
		for (auto iter = pStations->begin(); iter != pStations->end(); iter++)
		{
			StationBar* stationBar = new StationBar(&*iter, ui->scrollAreaWidgetContents);
			stationBar->setTipText(QStringLiteral("׼������..."));
			stationBar->setIsOnline(iter->IsRunning());
			stationBar->setStationName(iter->Name());
			verticalLayout->addWidget(stationBar);
			stations.push_back(&*iter);
			station_bar[&*iter] = stationBar;
			connect(stationBar, &StationBar::ViewLog, this, &UpgradeStationServiceDlg::on_ViewLog);
		}
	}
	else
	{
		for (auto iter = selectedIndexs.begin(); iter != selectedIndexs.end(); iter++)
		{
			StationInfo* s = pStations->atCurrent(iter->row());
			auto finded = find_if(stations.begin(), stations.end(), [=](StationInfo* station)
			{
				return station == s;
			});
			if (finded == stations.end())
			{
				StationBar* stationBar = new StationBar(s, ui->scrollAreaWidgetContents);
				stationBar->setTipText(QStringLiteral("׼������..."));
				stationBar->setIsOnline(s->IsRunning());
				stationBar->setStationName(s->Name());
				verticalLayout->addWidget(stationBar);
				stations.push_back(s);
				station_bar[s] = stationBar;
				connect(stationBar, &StationBar::ViewLog, this, &UpgradeStationServiceDlg::on_ViewLog);
			}
		}
	}

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 412, QSizePolicy::Minimum, QSizePolicy::Expanding);
	verticalLayout->addItem(verticalSpacer);
}

void UpgradeStationServiceDlg::on_browserPushButton_clicked()
{
	QFileDialog dialog(this);

	dialog.setFileMode(QFileDialog::Directory);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory(QDir::home());

	if (dialog.exec())
	{

		fileNames = dialog.selectedFiles();
		QDir dir(fileNames[0]);
		QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
		QString findFile = "CC-StationService.exe";
		if (!isUpgradeService)
		{
			findFile = "AppLuncher.exe";
		}
		auto result = find_if(entries.begin(), entries.end(), [&findFile](QFileInfo f) { return f.fileName() == findFile; });
		if (result == entries.end())
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("ѡ����ļ��в���ȷ��������Ҫ�������ļ���"));
		}
		else
		{
			QString program;
			QStringList args;
			if (qApp->platformName() == QStringLiteral("windows"))
			{
				program = result->absoluteFilePath();
				args << "-v";				
			}
			else
			{
				program = "/opt/mono/bin/mono";
				args << result->absoluteFilePath() << "-v";
			}

			QProcess* process = new QProcess(this);
			process->start(program, args);
			if (process->waitForReadyRead())
			{
				QByteArray output = process->readLine();
				upgradingFileVersion = toVersion(output);
				ui->versionLabel->setText(QStringLiteral("��ǰ׼�������İ汾��") + output);
			}

			QString name = "";
			for (QString file : fileNames)
			{
				name += file + ";";
			}
			ui->sendFileNamesPlainTextEdit->setPlainText(name);
			ui->upgradePushButton->setEnabled(true);
		}
	}
}

void UpgradeStationServiceDlg::on_upgradePushButton_clicked()
{
	if (fileNames.empty())
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("û��ѡ�������ļ���"));
		return;
	}

	for (auto s : stations)
	{
		s->clearSendFileLog();
	}

	UpgradePrepareThread* prepareThread = new UpgradePrepareThread(stations, communicator, isUpgradeService);
	connect(prepareThread, &UpgradePrepareThread::onFail, this, &UpgradeStationServiceDlg::on_prepareFailed);
	connect(prepareThread, &UpgradePrepareThread::onPrepared, this, &UpgradeStationServiceDlg::on_upgradePrepared);
	prepareThread->start();	
}

FileVersion UpgradeStationServiceDlg::toVersion(const QByteArray& versionArray)
{
	QString strVersion(versionArray);
	QStringList parts = strVersion.split('.', QString::SkipEmptyParts);
	if (parts.size() == 4)
	{
		return FileVersion(parts[0].toInt(), parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
	}

	return FileVersion();
}

void UpgradeStationServiceDlg::on_failToSendFile(StationInfo * station, QString fileName, QString message)
{
	QString msg = QStringLiteral("%1,%2").arg(fileName).arg(message);
	StationBar* bar = station_bar[station];
	bar->setTipText(msg, true);
	addToLog(station, msg, true);
}

void UpgradeStationServiceDlg::on_newFileSize(StationInfo * station, QString fileName, qint64 size)
{
	StationBar* bar = station_bar[station];
	QString msg = QStringLiteral("׼�������ļ�%1").arg(fileName);
	bar->setTipText(msg);
	bar->setMaxPercent(size);
	addToLog(station, msg);
}

void UpgradeStationServiceDlg::on_sendFileCompleted(StationInfo * station, QString fileName)
{
	StationBar* bar = station_bar[station];
	QString msg = QStringLiteral("�ļ�%1������ϡ�").arg(fileName);
	bar->setTipText(msg);
	addToLog(station, msg);
}

void UpgradeStationServiceDlg::on_fileSended(StationInfo * station, QString fileName, qint64 size)
{
	StationBar* bar = station_bar[station];
	QString msg = QStringLiteral("���������ļ�%1...").arg(fileName);
	bar->setTipText(msg);
	bar->setPercent(size);
	update();
}

void UpgradeStationServiceDlg::on_allCompleted(StationInfo * station, int total, int skip)
{
	StationBar* bar = station_bar[station];
	QString msg;

	if (station->getHasErrorWhenSendFile())
	{
		msg = QStringLiteral("����ʧ�ܡ�");
	}
	else
	{
		msg = QStringLiteral("�����ɹ���������%1���ļ�����������վʹ����Ч��").arg(total);
	}
	
	bar->setTipText(msg, station->getHasErrorWhenSendFile(), true);

	addToLog(station, msg);
}

void UpgradeStationServiceDlg::on_skipFile(StationInfo * station, QString file)
{
}

void UpgradeStationServiceDlg::on_ViewLog(StationInfo * station)
{
	list<QString> logs = station->getSendFileLog();
	ViewLogDialog dlg(logs, this);
	dlg.exec();
}

void UpgradeStationServiceDlg::on_upgradePrepared(StationInfo * station, const std::wstring & path, const FileVersion & v)
{
	StationBar* bar = station_bar[station];	
	QString strPath = QString::fromStdWString(path);
	QString strVersion = QStringLiteral("%1.%2.%3.%4").arg(v.Major).arg(v.Minor).arg(v.Build).arg(v.Private);
	QString msg = QStringLiteral("׼����ϣ�����վ��ǰ�汾��%1��").arg(strVersion);
	bar->setTipText(msg, false, false);
	addToLog(station, msg);


	if (upgradingFileVersion > v)
	{
		//�����ǰ�汾ԭʼ�汾
		QString dst = QString::fromStdWString(path).replace("\\", "/");
		int index = dst.lastIndexOf("/");
		if (index != -1)
		{
			dst = dst.left(index);
		}	

		//���������߳�
		std::list<StationInfo*> tmpList;
		tmpList.push_back(station);
		SendFileThread* sendThread = new SendFileThread(fileNames, dst, tmpList, communicator, false, true, true);
		connect(sendThread, &SendFileThread::failToSendFile, this, &UpgradeStationServiceDlg::on_failToSendFile);
		connect(sendThread, &SendFileThread::newFileSize, this, &UpgradeStationServiceDlg::on_newFileSize);
		connect(sendThread, &SendFileThread::fileSended, this, &UpgradeStationServiceDlg::on_fileSended);
		connect(sendThread, &SendFileThread::sendFileCompleted, this, &UpgradeStationServiceDlg::on_sendFileCompleted);
		connect(sendThread, &SendFileThread::allCompleted, this, &UpgradeStationServiceDlg::on_allCompleted);
		connect(sendThread, &SendFileThread::fileNoChange, this, &UpgradeStationServiceDlg::on_skipFile);
		sendThread->start();
	}
	else
	{
		QString msg = QStringLiteral("����վ��ǰ�汾��%1��Ҫ�����ļ��汾��%2�߻�һ�£�����Ҫ������").arg(strVersion).arg(upgradingFileVersion.ProductVersion);
		bar->setTipText(msg, false, true);
		addToLog(station, msg);
	}
	
}

void UpgradeStationServiceDlg::on_prepareFailed(StationInfo * station, const QString & msg)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(msg, true, true);
	addToLog(station, msg);
}

void UpgradeStationServiceDlg::addToLog(StationInfo * station, QString message, bool isError)
{
	QString log = QStringLiteral("%1 %2").arg(QDateTime::currentDateTime().toString()).arg(message);
	station->addSendFileLog(log, isError);
}

