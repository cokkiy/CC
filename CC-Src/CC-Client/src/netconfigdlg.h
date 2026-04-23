#ifndef NETCONFIGDLG_H
#define NETCONFIGDLG_H

#include <QDialog>

namespace Ui {
class NetConfigDlg;
}

class NetConfigDlg : public QDialog {
  Q_OBJECT

public:
  explicit NetConfigDlg(QWidget *parent = nullptr);
  ~NetConfigDlg();

private:
  Ui::NetConfigDlg *ui;
private slots:
  void on_staticIPCheckBox_stateChanged(int state);
};

#endif // NETCONFIGDLG_H
