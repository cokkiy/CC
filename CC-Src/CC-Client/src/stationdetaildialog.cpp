#include "stationdetaildialog.h"
#include "ui_stationdetaildialog.h"

StationDetailDialog::StationDetailDialog(QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui::StationDetailDialog();
    ui->setupUi(this);
}

StationDetailDialog::~StationDetailDialog()
{
    delete ui;
}

