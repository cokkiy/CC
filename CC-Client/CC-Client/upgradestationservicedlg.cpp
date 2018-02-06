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
		ui->label->setText(QStringLiteral("选择中控服务所在文件夹："));
		this->setWindowTitle(QStringLiteral("升级中控服务"));
	}
	else
	{
		ui->label->setText(QStringLiteral("选择中控应用代理所在文件夹："));
		this->setWindowTitle(QStringLiteral("升级中控应用代理"));
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
			stationBar->setTipText(QStringLiteral("准备升级..."));
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
				stationBar->setTipText(QStringLiteral("准备升级..."));
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
			QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("选择的文件夹不正确，不包含要升级的文件。"));
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
				ui->versionLabel->setText(QStringLiteral("当前准备升级的版本：") + output);
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
		QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("没有选择升级文件。"));
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
	QString msg = QStringLiteral("准备发送文件%1").arg(fileName);
	bar->setTipText(msg);
	bar->setMaxPercent(size);
	addToLog(station, msg);
}

void UpgradeStationServiceDlg::on_sendFileCompleted(StationInfo * station, QString fileName)
{
	StationBar* bar = station_bar[station];
	QString msg = QStringLiteral("文件%1发送完毕。").arg(fileName);
	bar->setTipText(msg);
	addToLog(station, msg);
}

void UpgradeStationServiceDlg::on_fileSended(StationInfo * station, QString fileName, qint64 size)
{
	StationBar* bar = station_bar[station];
	QString msg = QStringLiteral("正在升级文件%1...").arg(fileName);
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
		msg = QStringLiteral("升级失败。");
	}
	else
	{
		msg = QStringLiteral("升级成功。共升级%1个文件。重启工作站使其生效。").arg(total);
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
	QString msg = QStringLiteral("准备完毕，工作站当前版本：%1。").arg(strVersion);
	bar->setTipText(msg, false, false);
	addToLog(station, msg);


	if (upgradingFileVersion > v)
	{
		//如果当前版本原始版本
		QString dst = QString::fromStdWString(path).replace("\\", "/");
		int index = dst.lastIndexOf("/");
		if (index != -1)
		{
			dst = dst.left(index);
		}	

		//启动发送线程
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
		QString msg = QStringLiteral("工作站当前版本：%1比要升级文件版本：%2高或一致，不需要升级。").arg(strVersion).arg(upgradingFileVersion.ProductVersion);
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

