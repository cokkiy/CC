#ifndef DEFAULTAPPPROCDIALOG_H
#define DEFAULTAPPPROCDIALOG_H

#include <QDialog>
#include "option.h"
namespace Ui {class DefaultAppProcDialog;};

class DefaultAppProcDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
    创建一个选项对话框
    @param Option option 选项
    @param QWidget * parent 
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/04 12:18:06
    */
    DefaultAppProcDialog(Option option, QWidget *parent = 0);
    ~DefaultAppProcDialog();

private:
    void on_addAppButton_clicked();
    void on_removeAppPushButton_clicked();
    void on_addProcButton_clicked();
    void on_removeProcPushButton_clicked();
    void on_OKPushButton_clicked();
    void on_canclePushButton_clicked();
private:
    Ui::DefaultAppProcDialog *ui;
};

#endif // DEFAULTAPPPROCDIALOG_H
