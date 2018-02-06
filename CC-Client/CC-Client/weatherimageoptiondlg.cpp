#include "weatherimageoptiondlg.h"
#include "ui_weatherimageoptiondlg.h"
#include <QMessageBox>

WeatherImageOptionDlg::WeatherImageOptionDlg(Option::WeatherImageDownloadOption* option, QWidget *parent)
	: QDialog(parent), option(option)
{
	ui = new Ui::WeatherImageOptionDlg();	
	ui->setupUi(this);
	this->ui->buttonGroup->setId(this->ui->radioButtonAllDownload, 0);
	this->ui->buttonGroup->setId(this->ui->radioButtonAllNoDownload, 1);
	this->ui->buttonGroup->setId(this->ui->radioButtonSelfSetDownload, 2);
	if (option->Download > 2 || option->Download < 0)
		option->Download = 2;

	this->ui->buttonGroup->button(option->Download)->setChecked(true);
	this->ui->ftpAddressLineEdit->setText(option->Url);
	this->ui->userNameLineEdit->setText(option->UserName);
	this->ui->passwordLineEdit->setText(option->Password);
	this->ui->lastHoursSpinBox->setValue(option->LastHours);
	this->ui->intervalSpinBox->setValue(option->Interval);
	this->ui->deleteFileCheckBox->setChecked(option->DeletePreviousFiles);
	this->ui->deleteFileSpinBox->setValue(option->DeleteHowHoursAgo);
	this->ui->subDirLineEdit->setText(option->SubDirectory);
	this->ui->savePathForLinuxLineEdit->setText(option->SavePathForLinux);
	this->ui->savePathForWindowsLineEdit->setText(option->SavePathForWindows);
}

WeatherImageOptionDlg::~WeatherImageOptionDlg()
{
	delete ui;
}

void WeatherImageOptionDlg::on_okPushButton_clicked()
{
	if (!this->ui->ftpAddressLineEdit->text().startsWith("ftp://"))
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("FTP地址必须以ftp://开始"));
		return;
	}
	if (!this->ui->savePathForLinuxLineEdit->text().startsWith("/"))
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("Linux系统保存位置必须以'/'开始。"));
		return;
	}

	if (this->ui->savePathForWindowsLineEdit->text() == "")
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("Windows系统保存位置不能为空。"));
		return;
	}

	if (this->ui->userNameLineEdit->text() == "")
	{
		this->ui->userNameLineEdit->setText("anonymous");
	}

	option->Download = this->ui->buttonGroup->checkedId();
	option->Url = this->ui->ftpAddressLineEdit->text();
	option->UserName = this->ui->userNameLineEdit->text();
	option->Password = this->ui->passwordLineEdit->text();
	option->LastHours = this->ui->lastHoursSpinBox->value();
	option->Interval = this->ui->intervalSpinBox->value();
	option->DeletePreviousFiles = this->ui->deleteFileCheckBox->isChecked();
	option->DeleteHowHoursAgo = this->ui->deleteFileSpinBox->value();
	option->SubDirectory = this->ui->subDirLineEdit->text();
	option->SavePathForLinux = this->ui->savePathForLinuxLineEdit->text();
	option->SavePathForWindows = this->ui->savePathForWindowsLineEdit->text();

	this->done(QDialog::Accepted);
}

void WeatherImageOptionDlg::on_cancelPushButton_clicked()
{
	this->done(QDialog::Rejected);
}

