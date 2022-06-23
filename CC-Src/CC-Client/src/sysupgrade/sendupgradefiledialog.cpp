#include "sendupgradefiledialog.h"
#include "ui_sendupgradefiledialog.h"
#include <QFileDialog>
#include <QTimer>

SendUpgradeFileDialog::SendUpgradeFileDialog(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::SendUpgradeFileDialog) {
  ui->setupUi(this);
  setPixmap(QWizard::BannerPixmap, QPixmap(":/Icons/sysup.png"));
}

SendUpgradeFileDialog::~SendUpgradeFileDialog() { delete ui; }

bool SendUpgradeFileDialog::validatePage() { return fileSended; }

bool SendUpgradeFileDialog::isComplete() const { return fileSended; }

void SendUpgradeFileDialog::on_browserButton_clicked() {
  QFileDialog dialog(this, QStringLiteral("选择要下发的文件"));
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec() == QDialog::Accepted) {
    ui->fileNameEdit->setText(dialog.selectedFiles()[0]);
    ui->sendButton->setEnabled(true);
  }
}

void SendUpgradeFileDialog::sendFile() {
  if (step < 100) {
    step++;
    ui->progressBar->setValue(step);
  } else {
    fileSended = true;
    t->stop();
    delete t;
    emit completeChanged();
  }
}

void SendUpgradeFileDialog::on_sendButton_clicked()
{
  t = new QTimer(this);
  t->callOnTimeout(this, &SendUpgradeFileDialog::sendFile);
  t->start(30);
}
