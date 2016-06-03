#include "sendfiledialog.h"
#include "ui_sendfiledialog.h"
#include "StationList.h"
#include <QFileDialog>
#include <QBoxLayout>
#include "stationbar.h"
#include <QMessageBox>
#include "sendfilethread.h"
#include "filebrowserdialog.h"

/*!
创建发送文件对话框
@param StationList * pStations 全部工作站列表
@param const QModelIndexList & selectedIndexs 选定的工作扎索引
@param bool allStations 是否是全部工作站
@param QWidget * parent
@return
作者：cokkiy（张立民）
创建时间:2016/3/23 10:59:55
*/
SendFileDialog::SendFileDialog(StationList* pStations, const QModelIndexList& selectedIndexs,
	bool allStations/*=true*/, Ice::CommunicatorPtr communicator, QWidget *parent /*= 0*/)
	:QDialog(parent), communicator(communicator)
{
	ui = new Ui::SendFileDialog();
	ui->setupUi(this);
	//this->pStations = pStations;
	//this->allStations = allStations;
	//this->selectedIndex = selectedIndex;
	//this->communicator = communicator;
	createLayout(pStations, selectedIndexs, allStations);
}

SendFileDialog::~SendFileDialog()
{
	delete ui;
}


//选择要发送的文件
void SendFileDialog::on_browserPushButton_clicked()
{
	QFileDialog dialog(this);
	if (ui->sendDirCheckBox->isChecked())
	{
		dialog.setFileMode(QFileDialog::Directory);
	}
	else
	{
		dialog.setFileMode(QFileDialog::ExistingFiles);
	}
	
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory(QDir::home());
	
	if (dialog.exec())
	{
		fileNames = dialog.selectedFiles();
		QString name = "";
		for (QString file : fileNames)
		{
			name += file + ";";
		}
		ui->sendFileNamesPlainTextEdit->setPlainText(name);
		ui->sendPushButton->setEnabled(true);
	}
}

//发送文件
void SendFileDialog::on_sendPushButton_clicked()
{
	if (fileNames.empty())
	{
		QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("没有选择要发送的文件。"));
		return;
	}

	if (ui->destLineEdit->text()=="")
	{
		QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("必须填写文件保存位置。"));
		return;
	}

	//启动发送线程
	SendFileThread* sendThread = new SendFileThread(fileNames, ui->destLineEdit->text(), stations, communicator);
	connect(sendThread, &SendFileThread::failToSendFile, this, &SendFileDialog::on_failToSendFile);
	connect(sendThread, &SendFileThread::newFileSize, this, &SendFileDialog::on_newFileSize);
	connect(sendThread, &SendFileThread::fileSended, this, &SendFileDialog::on_fileSended);
	connect(sendThread, &SendFileThread::sendFileCompleted, this, &SendFileDialog::on_sendFileCompleted);
	connect(sendThread, &SendFileThread::allCompleted, this, &SendFileDialog::on_allCompleted);
	sendThread->start();
}

//选择目标文件夹
void SendFileDialog::on_selectDestPushButton_clicked()
{
	FileBrowserDialog dlg(*(stations.begin()), true, ui->destLineEdit->text());
	if (dlg.exec() == QDialog::Accepted)
	{
		ui->destLineEdit->setText(dlg.SelectedPath());
	}
}

void SendFileDialog::createLayout(StationList* pStations, const QModelIndexList& selectedIndexs, bool allStations)
{
	QVBoxLayout* verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);

	if (allStations)
	{
		for (auto iter = pStations->begin();iter != pStations->end();iter++)
		{
			StationBar* stationBar = new StationBar(ui->scrollAreaWidgetContents);
			stationBar->setTipText(QStringLiteral("准备发送文件..."));
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
			auto finded=find_if(stations.begin(), stations.end(), [=](StationInfo* station) 
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

void SendFileDialog::on_failToSendFile(StationInfo* station, QString fileName, QString message)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("%1,%2").arg(fileName).arg(message));
}

void SendFileDialog::on_newFileSize(StationInfo* station, QString fileName, size_t size)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("准备发送文件%1").arg(fileName));
	bar->setMaxPercent(size);
}

void SendFileDialog::on_sendFileCompleted(StationInfo* station, QString fileName)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("文件%1发送完毕。").arg(fileName));
}

void SendFileDialog::on_fileSended(StationInfo* station, QString fileName, size_t size)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("正在发送文件%1...").arg(fileName));
	bar->setPercent(size);
	update();
}

void SendFileDialog::on_allCompleted(StationInfo* station, int total)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("发送完毕,共发送%1个文件。").arg(total));
}

