#ifndef CAPTUREPROGRESSDLG_H
#define CAPTUREPROGRESSDLG_H

#include <QDialog>

namespace Ui {
class CaptureProgressDlg;
}

class CaptureProgressDlg : public QDialog
{
    Q_OBJECT

public:
  explicit CaptureProgressDlg(QWidget *parent = nullptr,
                              Qt::WindowFlags f = Qt::WindowFlags());
  ~CaptureProgressDlg();

private:
  Ui::CaptureProgressDlg *ui;
private slots:
  void on_startPushButton_clicked();
  void on_analyzesPushButton_clicked();
  void handleResults(int progress);
};

#endif // CAPTUREPROGRESSDLG_H
