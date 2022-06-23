#include "selectupgrademethoddlg.h"
#include "ui_selectupgrademethoddlg.h"

SelectUpgradeMethodDlg::SelectUpgradeMethodDlg(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::SelectUpgradeMethodDlg)
{
  ui->setupUi(this);
  setPixmap(QWizard::BannerPixmap, QPixmap(":/Icons/sysup.png"));
}

SelectUpgradeMethodDlg::~SelectUpgradeMethodDlg()
{
    delete ui;
}
