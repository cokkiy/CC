#include "captureprogressdlg.h"
#include "ui_captureprogressdlg.h"
#include "fakecapturedata.h"
CaptureProgressDlg::CaptureProgressDlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent,f), ui(new Ui::CaptureProgressDlg) {
  ui->setupUi(this);
}

CaptureProgressDlg::~CaptureProgressDlg()
{
    delete ui;
}

void CaptureProgressDlg::on_startPushButton_clicked() {
  FakeCaptureData *workerThread = new FakeCaptureData(this);
  connect(workerThread, &FakeCaptureData::progress, this,
          &CaptureProgressDlg::handleResults);
  connect(workerThread, &FakeCaptureData::finished, workerThread,
          &QObject::deleteLater);
  workerThread->start();
}

void CaptureProgressDlg::on_analyzesPushButton_clicked() {
  this->accept();
}

void CaptureProgressDlg::handleResults(int progress)
{
  if (progress == 100) {
    ui->startPushButton->setEnabled(false);
    ui->analyzesPushButton->setEnabled(true);
  }
  ui->progressBar->setValue(progress);
}