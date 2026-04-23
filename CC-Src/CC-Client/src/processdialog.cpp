#include "processdialog.h"
#include "ui_processdialog.h"
#include "StationInfo.h"
#include <QTableWidget>
#include <QMessageBox>

ProcessDialog::ProcessDialog(StationInfo* station, QWidget *parent)
	: QDialog(parent), station(station)
{
	ui = new Ui::ProcessDialog();
	ui->setupUi(this);
	getProcessInfo();
}

ProcessDialog::~ProcessDialog()
{
	delete ui;
}

int ProcessDialog::ProcessId()
{
	return processId;
}

QString ProcessDialog::ProcessName()
{
	return processName;
}

void ProcessDialog::getProcessInfo()
{
	if (station != NULL&&station->controlProxy != NULL)
	{
		station->controlProxy->begin_getAllProcessInfo(
			[=](const CC::ProcessInfos& processes)
		{
			//正常
			int rowCount = ui->tableWidget->rowCount();
			for (int row = 0; row < rowCount; row++)
			{
				for (int coloum = 0; coloum < 4; coloum++)
				{
					QTableWidgetItem* item = ui->tableWidget->takeItem(row, coloum);
					if (item != NULL)
						delete item;
				}
			}
			ui->tableWidget->setRowCount(0);

			int row = 0;

			for (CC::ProcessInfo info : processes)
			{
				ui->tableWidget->setRowCount(row + 1);
				QTableWidgetItem* item = new QTableWidgetItem(QStringLiteral("%1").arg(info.Id));
				ui->tableWidget->setItem(row, 0, item);
				item = new QTableWidgetItem(QString::fromStdWString(info.Name));
				ui->tableWidget->setItem(row, 1, item);
				item = new QTableWidgetItem(QString::fromStdString(info.Time));
				ui->tableWidget->setItem(row, 2, item);
				item = new QTableWidgetItem(QStringLiteral("%1").arg(info.MemorySize / 1024));
				ui->tableWidget->setItem(row, 3, item);
				row++;
			}
		},
			[=](const Ice::Exception& ex)
		{
			QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("发生错误：%1").arg(ex.what()));
		});
	}
}

void ProcessDialog::on_okPushButton_clicked()
{
	QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
	if (items.size() > 0)
	{
		processId = items[0]->text().toInt();
		processName = items[1]->text();
		this->accept();
	}
	else
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("必须选择一个进程。"));
	}
	
}

void ProcessDialog::on_cancelPushButton_clicked()
{
	this->reject();
}

