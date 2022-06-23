#ifndef AUTONETCFGDIALOG_H
#define AUTONETCFGDIALOG_H

#include <QDialog>

namespace Ui {
class AutoNetCfgDialog;
}

class AutoNetCfgDialog : public QDialog {
  Q_OBJECT

public:
  explicit AutoNetCfgDialog(QWidget *parent = nullptr);
  ~AutoNetCfgDialog();

private:
  Ui::AutoNetCfgDialog *ui;
private slots:
  void on_autoConfigCheckBox_stateChanged(int state);
};

#endif // AUTONETCFGDIALOG_H
