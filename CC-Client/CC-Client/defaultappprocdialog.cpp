#include "defaultappprocdialog.h"
#include "ui_defaultappprocdialog.h"
#include <QMessageBox>

DefaultAppProcDialog::DefaultAppProcDialog(Option option, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui::DefaultAppProcDialog();
    ui->setupUi(this);
}

DefaultAppProcDialog::~DefaultAppProcDialog()
{
    delete ui;
}

void DefaultAppProcDialog::on_addAppButton_clicked()
{
    if (ui->appLineEdit->text() == QStringLiteral(""))
    {
        QMessageBox::warning(NULL, QStringLiteral("错误"), QStringLiteral("必须填写需要启动程序完整名称和参数"));
        return;
    }
    ui->appListWidget->addItem(ui->appLineEdit->text());
    ui->appLineEdit->setText("");
}

void DefaultAppProcDialog::on_removeAppPushButton_clicked()
{
    auto index = ui->appListWidget->currentIndex();
    if (index.isValid())
    {
        ui->appListWidget->takeItem(index.row());
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
    this->done(QDialog::Accepted);
}

void DefaultAppProcDialog::on_canclePushButton_clicked()
{
    this->done(QDialog::Rejected);
}

