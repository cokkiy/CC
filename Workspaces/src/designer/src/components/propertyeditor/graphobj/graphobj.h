#ifndef GRAPHOBJ_H
#define GRAPHOBJ_H
#include <QWidget>



QT_BEGIN_NAMESPACE

namespace Ui {
class GraphObj;
}

class GraphObj : public QDialog
{
    Q_OBJECT

public:
    explicit GraphObj();
    ~GraphObj();

    QString transText();

    //输出的json字符串
    QByteArray m_jsonStr;

    //以字符串初始化控件
    void InitControl(QString str);

    void Show();

private slots:

    //增加新曲线
    void on_pushButton_AddNewObj_clicked();
    //删除选中曲线
    void on_pushButton_DelCurrentObj_clicked();

//    //左移曲线
//    void on_pushButton_leftmove_clicked();
//    //右移曲线
//    void on_pushButton_rightmove_clicked();

    //保存退出
    void on_pushButton_SaveQuit_clicked();
    //取消退出
    void on_pushButton_UnsaveQuit_clicked();




private:
    Ui::GraphObj *ui;


};

QT_END_NAMESPACE

#endif // GRAPHOBJ_H
