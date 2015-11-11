#include "selectcomputerdialog.h"
#include "ui_selectcomputerdialog.h"

selectComputerDialog::selectComputerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectComputerDialog)
{
    ui->setupUi(this);
}

selectComputerDialog::~selectComputerDialog()
{
    delete ui;
}
