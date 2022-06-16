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
    @param const Option& option 选项
    @param QWidget * parent 
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/04 12:18:06
    */
    DefaultAppProcDialog(Option& option,QString title=QStringLiteral("设置新建工作站默认远程启动应用和监视进程"), QWidget *parent = 0);
    ~DefaultAppProcDialog();

    private slots:
    void on_addAppButton_clicked();
    void on_removeAppPushButton_clicked();
    void on_addProcButton_clicked();
    void on_removeProcPushButton_clicked();
    void on_OKPushButton_clicked();
    void on_canclePushButton_clicked();
private:
    Ui::DefaultAppProcDialog *ui;
    //存储的选项指针
    Option* option;
};

#endif // DEFAULTAPPPROCDIALOG_H
