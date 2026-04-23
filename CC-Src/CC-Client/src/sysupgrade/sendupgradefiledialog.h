#ifndef SENDUPGRADEFILEDIALOG_H
#define SENDUPGRADEFILEDIALOG_H

#include <QWizardPage>

namespace Ui {
class SendUpgradeFileDialog;
}

class QTimer;
class SendUpgradeFileDialog : public QWizardPage {
  Q_OBJECT

public:
    explicit SendUpgradeFileDialog(QWidget *parent = nullptr);
    ~SendUpgradeFileDialog();
    virtual bool validatePage() override;
    virtual bool isComplete() const override;

  private:
    Ui::SendUpgradeFileDialog *ui;
    bool fileSended = false;
    QTimer *t;
    int step=0;
  private slots:
    void on_browserButton_clicked();
    void sendFile();
    void on_sendButton_clicked();
};

#endif // SENDUPGRADEFILEDIALOG_H
