#include "setintervaldialog.h"
#include "ui_setintervaldialog.h"

SetIntervalDialog::SetIntervalDialog(QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui::SetIntervalDialog();
    ui->setupUi(this);
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

