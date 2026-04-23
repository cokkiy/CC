#ifndef STATIONDETAILDIALOG_H
#define STATIONDETAILDIALOG_H

#include <QDialog>
namespace Ui {class StationDetailDialog;};

class StationDetailDialog : public QDialog
{
    Q_OBJECT

public:
    StationDetailDialog(QWidget *parent = 0);
    ~StationDetailDialog();

private:
    Ui::StationDetailDialog *ui;
};

#endif // STATIONDETAILDIALOG_H
