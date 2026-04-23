#include "batchcapturedialog.h"
#include "ui_batchcapturedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include "StationList.h"
#include "batchcaputrethread.h"

BatchCaptureDialog::BatchCaptureDialog(StationList* pStations, const QModelIndexList& selectedIndexs, QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::BatchCaptureDialog();
	ui->setupUi(this);

	for (auto iter = selectedIndexs.begin();iter != selectedIndexs.end();iter++)
	{
		StationInfo* s = pStations->atCurrent(iter->row());
		auto finded = find_if(stations.begin(), stations.end(), [=](StationInfo* station)
		{
			return station == s;
		});
		if (finded == stations.end())
		{
			stations.push_back(s);
		}
	}
	ui->progressBar->setMaximum(stations.size());
}

BatchCaptureDialog::~BatchCaptureDialog()
{
	delete ui;
}

void BatchCaptureDialog::on_startPushButton_clicked()
{
	if (ui->saveDirLineEdit->text()==QStringLiteral(""))
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("必须指定截图保存位置。"));
		return;
	}
	BatchCaputreThread* captureThread = new BatchCaputreThread(stations, ui->saveDirLineEdit->text(), ui->saveToOneDirCheckBox->isChecked());
	connect(captureThread, &BatchCaputreThread::ScreeenCaptured, this, &BatchCaptureDialog::On_ScreenCaptured);
	captureThread->start();
	ui->startPushButton->setEnabled(false);

}

void BatchCaptureDialog::on_selectDirPushButton_clicked()
{
	QFileDialog dlg;
	dlg.setDirectory(QDir::home());	
	dlg.setFileMode(QFileDialog::Directory);
	if (dlg.exec() == QDialog::Accepted)
	{
		ui->saveDirLineEdit->setText(dlg.selectedFiles()[0]);
	}
}

void BatchCaptureDialog::On_ScreenCaptured(StationInfo* s, bool result, QString message)
{
	ui->progressBar->setValue(++captured);
	ui->infoLabel->setText(QStringLiteral("%1 %2%3").arg(s->Name(), result ? QStringLiteral("成功截屏。") : QStringLiteral("没有成功："), message));
	if (captured == stations.size())
	{
		QMessageBox::information(this, QStringLiteral("批量截图完毕"), QStringLiteral("图片已保存。"));
		ui->startPushButton->setEnabled(true);
	}
}

