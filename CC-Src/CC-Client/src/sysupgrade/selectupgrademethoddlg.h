#ifndef SELECTUPGRADEMETHODDLG_H
#define SELECTUPGRADEMETHODDLG_H

#include <QWizardPage>

namespace Ui {
class SelectUpgradeMethodDlg;
}

class SelectUpgradeMethodDlg : public QWizardPage
{
    Q_OBJECT

public:
    explicit SelectUpgradeMethodDlg(QWidget *parent = nullptr);
    ~SelectUpgradeMethodDlg();

private:
    Ui::SelectUpgradeMethodDlg *ui;
};

#endif // SELECTUPGRADEMETHODDLG_H
