#include "viewlogdialog.h"
#include "ui_viewlogdialog.h"
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>

ViewLogDialog::ViewLogDialog(std::list<QString> logs, QWidget *parent)
	: QDialog(parent), logs(logs)
{
	ui = new Ui::ViewLogDialog();
	ui->setupUi(this);
	ui->listWidget->addItems(QStringList::fromStdList(logs));
	connect(ui->closePushButton, &QPushButton::clicked, this, &ViewLogDialog::close);
	connect(ui->saveLogPushButton, &QPushButton::clicked, this, &ViewLogDialog::saveLog);
}

ViewLogDialog::~ViewLogDialog()
{
	delete ui;
}

void ViewLogDialog::saveLog(bool checked)
{
	QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("保存日志"),
		QDir::home().absolutePath(), QStringLiteral("文本文件(*.txt)"));
	QFile data(fileName);
	if (data.open(QFile::WriteOnly | QFile::Truncate|QFile::Text)) {
		QTextStream out(&data);
		for (auto log : logs)
		{
			out << log << "\n" << flush;
		}
	}
}

