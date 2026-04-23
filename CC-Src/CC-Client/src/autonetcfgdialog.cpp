#include "autonetcfgdialog.h"
#include "ui_autonetcfgdialog.h"

AutoNetCfgDialog::AutoNetCfgDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AutoNetCfgDialog) {
  ui->setupUi(this);
}

AutoNetCfgDialog::~AutoNetCfgDialog() { delete ui; }

void AutoNetCfgDialog::on_autoConfigCheckBox_stateChanged(int state) {
  if (state == Qt::Unchecked) {
    ui->ipGroupBox->setEnabled(false);
  } else {
    ui->ipGroupBox->setEnabled(true);
  }
}
