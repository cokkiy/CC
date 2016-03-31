#include "sendfiledialog.h"
#include "ui_sendfiledialog.h"
#include "StationList.h"
#include <QFileDialog>
#include <QBoxLayout>
#include "stationbar.h"
#include <QMessageBox>
#include "sendfilethread.h"

/*!
���������ļ��Ի���
@param StationList * pStations ȫ������վ�б�
@param const QModelIndexList & selectedIndexs ѡ���Ĺ���������
@param bool allStations �Ƿ���ȫ������վ
@param QWidget * parent
@return
���ߣ�cokkiy��������
����ʱ��:2016/3/23 10:59:55
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

//ѡ��Ҫ���͵��ļ�
void SendFileDialog::on_browserPushButton_clicked()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
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

//�����ļ�
void SendFileDialog::on_sendPushButton_clicked()
{
	if (fileNames.empty())
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("û��ѡ��Ҫ���͵��ļ���"));
		return;
	}

	if (ui->destLineEdit->text()=="")
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("������д�ļ�����λ�á�"));
		return;
	}

	//���������߳�
	SendFileThread* sendThread = new SendFileThread(fileNames, ui->destLineEdit->text(), stations, communicator);
	connect(sendThread, &SendFileThread::failToSendFile, this, &SendFileDialog::on_failToSendFile);
	connect(sendThread, &SendFileThread::newFileSize, this, &SendFileDialog::on_newFileSize);
	connect(sendThread, &SendFileThread::fileSended, this, &SendFileDialog::on_fileSended);
	connect(sendThread, &SendFileThread::sendFileCompleted, this, &SendFileDialog::on_sendFileCompleted);
	sendThread->start();
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
			stationBar->setTipText(QStringLiteral("׼�������ļ�..."));
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
			StationBar* stationBar = new StationBar(ui->scrollAreaWidgetContents);
			StationInfo* s = pStations->atCurrent(iter->row());
			stationBar->setTipText(QStringLiteral("׼�������ļ�..."));
			stationBar->setIsOnline(s->IsRunning());
			stationBar->setStationName(s->Name());
			verticalLayout->addWidget(stationBar);
			stations.push_back(s);
			station_bar[s] = stationBar;
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
	bar->setTipText(QStringLiteral("׼�������ļ�%1").arg(fileName));
	bar->setMaxPercent(size);
}

void SendFileDialog::on_sendFileCompleted(StationInfo* station, QString fileName)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("�ļ�%1������ϡ�").arg(fileName));
}

void SendFileDialog::on_fileSended(StationInfo* station, QString fileName, size_t size)
{
	StationBar* bar = station_bar[station];
	bar->setTipText(QStringLiteral("���ڷ����ļ�%1...").arg(fileName));
	bar->setPercent(size);
	update();
}

