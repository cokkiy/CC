#include "createvnidialog.h"
#include "ui_createvnidialog.h"

CreateVNIDialog::CreateVNIDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateVNIDialog)
{
    ui->setupUi(this);
}

CreateVNIDialog::~CreateVNIDialog()
{
    delete ui;
}
