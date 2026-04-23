#include "setintervaldialog.h"
#include "ui_setintervaldialog.h"

SetIntervalDialog::SetIntervalDialog(int interval, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui::SetIntervalDialog();
    ui->setupUi(this);
    ui->intervalSpinBox->setValue(interval);
}

SetIntervalDialog::~SetIntervalDialog()
{
    delete ui;
}

void SetIntervalDialog::on_okPushButton_clicked()
{
    setInterval(ui->intervalSpinBox->value());
    this->done(QDialog::Accepted);
}

void SetIntervalDialog::on_cancelPushButton_clicked()
{
    this->done(QDialog::Rejected);
}

