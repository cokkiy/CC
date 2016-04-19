#include "recvfiledialog.h"
#include "ui_recvfiledialog.h"
#include <QFileDialog>
#include <Ice/Communicator.h>
#include "StationList.h"
#include <QAbstractItemModel>
#include <QMessageBox>
#include "stationbar.h"
#include "recvfilethread.h"
#include "filebrowserdialog.h"

RecvFileDialog::RecvFileDialog(StationList* pStations, const QModelIndexList& selectedIndexs,
	bool allStations, Ice::CommunicatorPtr communicator, QWidget *parent)
	: QDialog(parent),communicator(communicator)
{
	ui = new Ui::RecvFileDialog();
	ui->setupUi(this);
	createLayout(pStations, selectedIndexs, allStations);
}

RecvFileDialog::~RecvFileDialog()
{
	delete ui;
}

void RecvFileDialog::on_browsePushButton_clicked()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory(QDir::home());

	if (dialog.exec())
	{
		destDir = dialog.directory().absolutePath();
		ui->destLineEdit->setText(destDir);
	}
}

//接收文件
void RecvFileDialog::on_recvPushButton_clicked()
{
	if (ui->srcFileLineEdit->text() == "")
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("要接收的文件路径和名称不能为空。"));
		return;
	}

	if (ui->destLineEdit->text() == "")
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("文件存放位置不能为空。"));
		return;
	}

	//开始接收文件
	RecvFileThread* recvThread = new RecvFileThread(ui->srcFileLineEdit->text(), ui->destLineEdit->text(),
		communicator, stations, this);
	connect(recvThread, &RecvFileThread::createFileError, this, &RecvFileDialog::on_createFileError);
	connect(recvThread, &RecvFileThread::getDataError, this, &RecvFileDialog::on_getDataError);
	connect(recvThread, &RecvFileThread::notifyFileSize, this, &RecvFileDialog::on_notifyFileSize);
	connect(recvThread, &RecvFileThread::transFileComplete, this, &RecvFileDialog::on_transFileComplete);
	connect(recvThread, &RecvFileThread::fileGeted, this, &RecvFileDialog::on_fileGeted);

	recvThread->start();
}


//选择远程文件
void RecvFileDialog::on_selectFilePushButton_clicked()
{
	FileBrowserDialog dlg(*stations.begin(), false, ui->destLineEdit->text());
	if (dlg.exec() == QDialog::Accepted)
	{
		ui->srcFileLineEdit->setText(dlg.SelectedPath());
	}
}

void RecvFileDialog::on_notifyFileSize(StationInfo* station, long long size)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("开始接收文件%1").arg(ui->srcFileLineEdit->text()));
	bar->setMaxPercent(size);
}

void RecvFileDialog::on_getDataError(StationInfo* station, QString message)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(message);
}

void RecvFileDialog::on_createFileError(StationInfo* station, QString fileName, QString message)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(message + fileName);
}

void RecvFileDialog::on_transFileComplete(StationInfo *station)
{

	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("文件%1已接收完毕.").arg(ui->srcFileLineEdit->text()));
}

void RecvFileDialog::on_fileGeted(StationInfo* station, long long size)
{
	StationBar* bar = station_bar[station];
	bar->setPercent(size);
}

void RecvFileDialog::createLayout(StationList* pStations, const QModelIndexList& selectedIndexs, bool allStations)
{
	QVBoxLayout* verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);

	if (allStations)
	{
		for (auto iter = pStations->begin();iter != pStations->end();iter++)
		{
			StationBar* stationBar = new StationBar(ui->scrollAreaWidgetContents);
			stationBar->setTipText(QStringLiteral("准备接收文件..."));
			stationBar->setIsOnline(iter->IsRunning());
			stationBar->setStationName(iter->Name());
			verticalLayout->addWidget(stationBar);
			stations.push_back(&*iter);
			station_bar[&*iter] = stationBar;
		}
	}
	else
	{
		for (auto iter = selectedIndexs.begin();iter != selectedIndexs.end();iter++)
		{
			StationInfo* s = pStations->atCurrent(iter->row());
			auto finded = find_if(stations.begin(), stations.end(), [=](StationInfo* station)
			{
				return station == s;
			});
			if (finded == stations.end())
			{
				StationBar* stationBar = new StationBar(ui->scrollAreaWidgetContents);
				stationBar->setTipText(QStringLiteral("准备发送文件..."));
				stationBar->setIsOnline(s->IsRunning());
				stationBar->setStationName(s->Name());
				verticalLayout->addWidget(stationBar);
				stations.push_back(s);
				station_bar[s] = stationBar;
			}
		}
	}


	QSpacerItem* verticalSpacer = new QSpacerItem(20, 412, QSizePolicy::Minimum, QSizePolicy::Expanding);
	verticalLayout->addItem(verticalSpacer);
}

