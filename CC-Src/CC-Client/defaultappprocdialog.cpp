#include "defaultappprocdialog.h"
#include "ui_defaultappprocdialog.h"
#include <QMessageBox>
#include <tuple>
using namespace std;

DefaultAppProcDialog::DefaultAppProcDialog(Option& option, QString title, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui::DefaultAppProcDialog();
    ui->setupUi(this);
	this->setWindowTitle(title);
    this->option = &option;
    ui->appTableWidget->setRowCount(this->option->StartApps.size());
    int row = 0;
    for (auto& param : this->option->StartApps)
    {
        QTableWidgetItem *pathItem = new QTableWidgetItem(QString::fromStdString(param.AppPath));
        ui->appTableWidget->setItem(row, 0, pathItem);
        QTableWidgetItem *argItem = new QTableWidgetItem(QString::fromStdString(param.Arguments));
        ui->appTableWidget->setItem(row, 1, argItem);
        QTableWidgetItem *proceNameItem = new QTableWidgetItem(QString::fromStdString(param.ProcessName));
        ui->appTableWidget->setItem(row, 2, proceNameItem);
        QTableWidgetItem *allowMultiInstanceItem = new QTableWidgetItem();
        allowMultiInstanceItem->setCheckState(param.AllowMultiInstance ? Qt::Checked : Qt::Unchecked);
        ui->appTableWidget->setItem(row, 3, allowMultiInstanceItem);
        row++;
    }
    ui->procListWidget->addItems(QStringList::fromStdList(this->option->MonitorProcesses));
}

DefaultAppProcDialog::~DefaultAppProcDialog()
{
    delete ui;
}

void DefaultAppProcDialog::on_addAppButton_clicked()
{
    if (ui->appLineEdit->text() == QStringLiteral(""))
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("必须填写需要启动程序完整路径和名称"));
        return;
    }
    if (ui->appProcNameLineEdit->text() == QStringLiteral(""))
    {
		QString name = ui->appLineEdit->text();
		int index = name.indexOf(QRegExp("\\w:\\\\", Qt::CaseInsensitive));
		if (index != -1)
		{
			// win likes system
			int pos = name.lastIndexOf(QRegExp("\\\\\\w+"));
			if (pos != -1)
			{
				name = name.mid(pos + 1);
				if (name.endsWith(".exe", Qt::CaseInsensitive))
				{
					name = name.mid(0, name.length() - 4);
				}
				ui->appProcNameLineEdit->setText(name);
			}
			else
			{
				ui->appProcNameLineEdit->setText(name);
			}
		}
		else
		{
			ui->appProcNameLineEdit->setText(name);
		}
    }
    int row = ui->appTableWidget->rowCount();
    ui->appTableWidget->setRowCount(row + 1);
    QTableWidgetItem *pathItem = new QTableWidgetItem(ui->appLineEdit->text());
    QTableWidgetItem *argItem= new QTableWidgetItem(ui->argumentLineEdit->text());
    QTableWidgetItem *procNameItem = new QTableWidgetItem(ui->appProcNameLineEdit->text());
    QTableWidgetItem *allowMultiInstanceItem = new QTableWidgetItem();
    allowMultiInstanceItem->setCheckState(ui->allowMultiInstanceCheckBox->checkState());
    ui->appTableWidget->setItem(row, 0, pathItem);
    ui->appTableWidget->setItem(row, 1, argItem);
    ui->appTableWidget->setItem(row, 2, procNameItem);
    ui->appTableWidget->setItem(row, 3, allowMultiInstanceItem);
    ui->appLineEdit->setText("");
    ui->argumentLineEdit->setText("");
    ui->appProcNameLineEdit->setText("");
}

void DefaultAppProcDialog::on_removeAppPushButton_clicked()
{
    auto index = ui->appTableWidget->currentIndex();
    if (index.isValid())
    {
        ui->appTableWidget->removeRow(index.row());
    }
    else
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("没有选择要删除的启动程序."));
    }
}

void DefaultAppProcDialog::on_addProcButton_clicked()
{
    if (ui->procLineEdit->text() == QStringLiteral(""))
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("必须填写需要监视进程名称"));
        return;
    }
    ui->procListWidget->addItem(ui->procLineEdit->text());
    ui->procLineEdit->setText("");
}

void DefaultAppProcDialog::on_removeProcPushButton_clicked()
{
    auto index = ui->procListWidget->currentIndex();
    if (index.isValid())
    {
        ui->procListWidget->takeItem(index.row());
    }
    else
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("没有选择要删除的监视进程."));
    }
}

void DefaultAppProcDialog::on_OKPushButton_clicked()
{
    option->StartApps.clear();
    option->MonitorProcesses.clear();
    for (int i = 0; i < ui->appTableWidget->rowCount(); i++)
    {
        CC::AppStartParameter param;
        param.AppPath = ui->appTableWidget->item(i, 0)->text().toStdString();
        param.Arguments = ui->appTableWidget->item(i, 1)->text().toStdString();
        param.ProcessName = ui->appTableWidget->item(i, 2)->text().toStdString();       
        param.AllowMultiInstance = ui->appTableWidget->item(i, 3)->checkState() == Qt::Checked;
        option->StartApps.push_back(param);
    }
    for (int i = 0; i < ui->procListWidget->count(); i++)
    {
        option->MonitorProcesses.push_back(ui->procListWidget->item(i)->text());
    }
    this->done(QDialog::Accepted);
}

void DefaultAppProcDialog::on_canclePushButton_clicked()
{
    this->done(QDialog::Rejected);
}

