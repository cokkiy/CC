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
    explicit SingleStateDialog(QDesignerFormEditorInterface *core,QWidget *parent = 0);
    int rowValue;//总行数
    int showDialog();//打开对话框
    QString TransStrStateJson() const;//传回Json字符串
    QString strStateJson;//生成的Json字符串
    ~SingleStateDialog();

private slots:
    void on_addRowButton_clicked();//增加行按钮

    void on_delRowButton_clicked();//删除行按钮

    void on_delCurrentRowButton_clicked();//删除指定行按钮

    void on_okButton_clicked();//确定按钮

    void on_quitPushButton_clicked();//放弃按钮

private:
    Ui::SingleStateDialog *ui;
    QStandardItemModel *model;
    QDesignerFormEditorInterface *m_core;

};

#endif // SINGLESTATEDIALOG_H
