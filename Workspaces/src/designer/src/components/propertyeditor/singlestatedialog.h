#ifndef SINGLESTATEDIALOG_H
#define SINGLESTATEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDesignerFormEditorInterface>

namespace Ui {
class SingleStateDialog;
}

class SingleStateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SingleStateDialog(QDesignerFormEditorInterface *core,QString oldText,QWidget *parent = 0);
    void setupModel();   //初始化模型
    void setupTableview();   //初始化表格

    int rowValue;       //配置完成获得模型总行数

    int showDialog();     //打开对话框
    void setText(QString );    //解析数组字符串
    QString transArrayJson() const;     //传回Json字符串
    QString  strStateJson;   //表格配置好，确认按钮按下的字符串

    ~SingleStateDialog();

private slots:
    void on_addRowButton_clicked();    //增加行按钮
    void on_delRowButton_clicked();    //删除行按钮
    void on_delCurrentRowButton_clicked();     //删除指定行按钮
    void on_okButton_clicked();    //确定按钮
    void on_quitPushButton_clicked();    //放弃按钮

private:
    Ui::SingleStateDialog *ui;
    QStandardItemModel *model;
    QDesignerFormEditorInterface *m_core;
};

#endif // SINGLESTATEDIALOG_H
