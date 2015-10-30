#include "configshowtypedialog.h"
#include "ui_configshowtypedialog.h"
#include "../../../net/C3I/paramstyle/paramstyleconfig.h"
QConfigShowTypeDialog::QConfigShowTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QConfigShowTypeDialog)
{
    ui->setupUi(this);
    paramStyleInfor = NetComponents::getInforCenter();
    list<QParamStyle>* paramStyleList = paramStyleInfor->getParamStyleList();
    int a = 0;
}

QConfigShowTypeDialog::~QConfigShowTypeDialog()
{
    delete ui;
}
