#include "defaultappprocdialog.h"
#include "ui_defaultappprocdialog.h"
#include <QMessageBox>

DefaultAppProcDialog::DefaultAppProcDialog(Option& option, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui::DefaultAppProcDialog();
    ui->setupUi(this);
    this->option = &option;
    QStringList header;
    header<< QStringLiteral("路径") << QStringLiteral("参数");
    ui->appTableWidget->setHorizontalHeaderLabels(header);
    ui->appTableWidget->setRowCount(this->option->StartApps.size());
    int row = 0;
    for (auto& param : this->option->StartApps)
    {
        QTableWidgetItem *pathItem = new QTableWidgetItem(param.first);
        ui->appTableWidget->setItem(row, 0, pathItem);
        QTableWidgetItem *argItem = new QTableWidgetItem(param.second);
        ui->appTableWidget->setItem(row, 1, argItem);
        row++;
    }
    ui->procListWidget->addItems(QStringList::fromStdList(this->option->MonitorProcesses));
    ui->autoAddStartAppToMonitorListCheckBox->setChecked(option.AutoMonitorRemoteStartApp);
}

DefaultAppProcDialog::~DefaultAppProcDialog()
{
    delete ui;
}

void DefaultAppProcDialog::on_addAppButton_clicked()
{
    if (ui->appLineEdit->text() == QStringLiteral(""))
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("必须填写需要启动程序完整名称"));
        return;
    }
    int row = ui->appTableWidget->rowCount();
    ui->appTableWidget->setRowCount(row + 1);
    QTableWidgetItem *pathItem = new QTableWidgetItem(ui->appLineEdit->text());
    QTableWidgetItem *argItem= new QTableWidgetItem(ui->argumentLineEdit->text());
    ui->appTableWidget->setItem(row, 0, pathItem);
    ui->appTableWidget->setItem(row, 1, argItem);
    ui->appLineEdit->setText("");
    ui->argumentLineEdit->setText("");
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
    option->AutoMonitorRemoteStartApp = ui->autoAddStartAppToMonitorListCheckBox->isChecked();
    option->MonitorProcesses.clear();
    for (int i = 0; i < ui->appTableWidget->rowCount(); i++)
    {
        option->StartApps.push_back({ ui->appTableWidget->item(i,0)->text(), ui->appTableWidget->item(i,1)->text() });
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

