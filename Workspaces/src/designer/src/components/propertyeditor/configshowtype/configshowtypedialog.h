#ifndef CONFIGSHOWTYPEDIALOG_H
#define CONFIGSHOWTYPEDIALOG_H
#include <Net/NetComponents>
#include <QDialog>

namespace Ui {
class QConfigShowTypeDialog;
}

class QConfigShowTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QConfigShowTypeDialog(QWidget *parent = 0);
    ~QConfigShowTypeDialog();

private:
    Ui::QConfigShowTypeDialog *ui;
    //信息约定表参数接口
    InformationInterface* paramStyleInfor;
};

#endif // CONFIGSHOWTYPEDIALOG_H
