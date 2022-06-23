#include "netconfigdlg.h"
#include "ui_netconfigdlg.h"

NetConfigDlg::NetConfigDlg(QWidget *parent)
    : QDialog(parent), ui(new Ui::NetConfigDlg) {
  ui->setupUi(this);
}

NetConfigDlg::~NetConfigDlg() { delete ui; }

void NetConfigDlg::on_staticIPCheckBox_stateChanged(int state) {
  if (state == Qt::Unchecked) {
    ui->ipGroupBox->setEnabled(false);
  } else {
    ui->ipGroupBox->setEnabled(true);
  }
}
