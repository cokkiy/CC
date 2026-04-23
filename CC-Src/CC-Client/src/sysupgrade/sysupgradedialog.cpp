#include "sysupgradedialog.h"
#include "ui_sysupgradedialog.h"
#include <QTimer>

SysUpgradeDialog::SysUpgradeDialog(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::SysUpgradeDialog) {
  ui->setupUi(this);
  setPixmap(QWizard::BannerPixmap, QPixmap(":/Icons/sysup.png"));
  setCommitPage(true);
}

SysUpgradeDialog::~SysUpgradeDialog() { delete ui; }

bool SysUpgradeDialog::validatePage() { return executed; }

bool SysUpgradeDialog::isComplete() const
{
  return executed;
}

void SysUpgradeDialog::on_upgradeButton_clicked() {
  t = new QTimer(this);
  t->callOnTimeout(this, &SysUpgradeDialog::executingUpgrade);
  t->start(30);
}

void SysUpgradeDialog::executingUpgrade()
{
  if (step < 100) {
    step++;
    ui->progressBar->setValue(step);
  } else {
    executed = true;
    t->stop();
    delete t;
    emit completeChanged();
  }
}
