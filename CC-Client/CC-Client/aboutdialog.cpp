#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QFile>
#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::AboutDialog();
	ui->setupUi(this);
	ui->versionLabel->setText(QStringLiteral("1.1"));
	QString dir = QApplication::applicationDirPath();
	QFile file(dir + "/version.txt");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString version = file.readLine();
		ui->versionLabel->setText(version.trimmed());
		QString info = file.readAll();
		//info.replace("\r", "<p\>");
		ui->infoTextBrowser->setText(info);
	}
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

