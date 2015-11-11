#ifndef SELECTCOMPUTERDIALOG_H
#define SELECTCOMPUTERDIALOG_H

#include <QDialog>

namespace Ui {
class selectComputerDialog;
}

class selectComputerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit selectComputerDialog(QWidget *parent = 0);
    ~selectComputerDialog();

private:
    Ui::selectComputerDialog *ui;
};

#endif // SELECTCOMPUTERDIALOG_H
