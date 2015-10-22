#ifndef Q2WMAPOBJ_H
#define Q2WMAPOBJ_H
#include <QWidget>

QT_BEGIN_NAMESPACE

namespace Ui {
class Q2wmapObj;
}

class Q2wmapObj : public QDialog
{
    Q_OBJECT

public:
    explicit Q2wmapObj();
    ~Q2wmapObj();

    QString transText();

    //输出的json字符串
    QByteArray m_jsonStr;

    //以字符串初始化控件
    void InitControl(QString str);

    void Show();

private slots:

    void on_pushButton_DelCurrentObj_clicked();

    void on_pushButton_AddNewObj_clicked();

    void on_pushButton_SaveQuit_clicked();

    void on_pushButton_UnsaveQuit_clicked();

private:
    Ui::Q2wmapObj *ui;

};

QT_END_NAMESPACE

#endif // Q2WMAPOBJ_H
