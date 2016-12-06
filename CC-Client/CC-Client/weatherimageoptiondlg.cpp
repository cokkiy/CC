#include "weatherimageoptiondlg.h"
#include "ui_weatherimageoptiondlg.h"
#include <QMessageBox>

WeatherImageOptionDlg::WeatherImageOptionDlg(Option::WeatherImageDownloadOption* option, QWidget *parent)
	: QDialog(parent), option(option)
{
	ui = new Ui::WeatherImageOptionDlg();
	ui->setupUi(this);
	this->ui->ftpAddressLineEdit->setText(option->Url);
	this->ui->userNameLineEdit->setText(option->UserName);
	this->ui->passwordLineEdit->setText(option->Password);
	this->ui->lastHoursSpinBox->setValue(option->LastHours);
	this->ui->intervalSpinBox->setValue(option->Interval);
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
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("FTP��ַ������ftp://��ʼ"));
		return;
	}
	if (!this->ui->savePathForLinuxLineEdit->text().startsWith("/"))
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("Linuxϵͳ����λ�ñ�����'/'��ʼ��"));
		return;
	}

	if (this->ui->savePathForWindowsLineEdit->text() == "")
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("Windowsϵͳ����λ�ò���Ϊ�ա�"));
		return;
	}

	if (this->ui->userNameLineEdit->text() == "")
	{
		this->ui->userNameLineEdit->setText("anonymous");
	}


	option->Url = this->ui->ftpAddressLineEdit->text();
	option->UserName = this->ui->userNameLineEdit->text();
	option->Password = this->ui->passwordLineEdit->text();
	option->LastHours = this->ui->lastHoursSpinBox->value();
	option->Interval = this->ui->intervalSpinBox->value();
	option->SavePathForLinux = this->ui->savePathForLinuxLineEdit->text();
	option->SavePathForWindows = this->ui->savePathForWindowsLineEdit->text();
	this->done(QDialog::Accepted);
}

void WeatherImageOptionDlg::on_cancelPushButton_clicked()
{
	this->done(QDialog::Rejected);
}

