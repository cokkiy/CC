#ifndef CREATEVNIDIALOG_H
#define CREATEVNIDIALOG_H

#include <QDialog>

namespace Ui {
class CreateVNIDialog;
}

class CreateVNIDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateVNIDialog(QWidget *parent = nullptr);
    ~CreateVNIDialog();

private:
    Ui::CreateVNIDialog *ui;
};

#endif // CREATEVNIDIALOG_H
