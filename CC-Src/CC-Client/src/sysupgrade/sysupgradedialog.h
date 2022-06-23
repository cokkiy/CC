#ifndef SYSUPGRADEDIALOG_H
#define SYSUPGRADEDIALOG_H

#include <QWizardPage>

namespace Ui {
class SysUpgradeDialog;
}
class QTimer;
class SysUpgradeDialog : public QWizardPage
{
    Q_OBJECT

public:
    explicit SysUpgradeDialog(QWidget *parent = nullptr);
    ~SysUpgradeDialog();
    virtual bool validatePage() override;
    virtual bool isComplete() const override;

  private:
    Ui::SysUpgradeDialog *ui;
    bool executed = false;
    QTimer *t;
    int step=0;
  private slots:
    void on_upgradeButton_clicked();
    void executingUpgrade();
};

#endif // SYSUPGRADEDIALOG_H
