#ifndef Q2WMAPSTATICELEMENTDLG_H
#define Q2WMAPSTATICELEMENTDLG_H

#include <QDialog>

namespace Ui {
class Q2wmapStaticElementDlg;
}

class Q2wmapStaticElementDlg : public QDialog
{
    Q_OBJECT

public:
    explicit Q2wmapStaticElementDlg(QWidget *parent = 0);
    ~Q2wmapStaticElementDlg();

    QString transText();

    //输出的json字符串
    QByteArray m_jsonStr;

    //以字符串初始化控件
    void InitControl(QString str);

    void Show();

private slots:
    void on_pushButtonCircle_clicked();

    void on_pushButtonPolygon_clicked();

    void on_pushButtonLabel_clicked();

    void on_pushButtonDel_clicked();

    void on_pushButtonOK_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::Q2wmapStaticElementDlg *ui;
};

#endif // Q2WMAPSTATICELEMENTDLG_H
