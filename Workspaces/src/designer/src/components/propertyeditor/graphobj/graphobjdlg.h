#ifndef GRAPHOBJDLG_H
#define GRAPHOBJDLG_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace Ui {
class GraphObjDlg;
}

class GraphObjDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GraphObjDlg(QJsonObject jobj, QWidget *parent = 0);
    ~GraphObjDlg();

    //目标名称
//    QString m_strName;

    //定义多曲线配置的各种变量
    double m_Xmax;
    double m_Xmin;
    double m_Ymax;
    double m_Ymin;
    qint32 m_Xoffset;
    qint32 m_Yoffset;
    QString m_XAxisLabel;
    QString m_YAxisLabel;
    bool m_XAxisdisplay;
    bool m_YAxisdisplay;
    QString m_Scaleplace_x;
    QString m_Scaleplace_y;

    QString m_GraphName;
    qint32  m_GraphWidth;
    double  m_GraphBuffer;
    QString m_strgraphColor;

    qint32  m_LGraphWidth;
    double  m_LGraphBuffer;
    QString m_strLgraphColor;

    QString m_xParam;
    QString m_yParam;
    QString m_Lgraphfile;//理论曲线文件(路径)

    //设置控件
    void setControls();

private slots:

    void on_lineEdit_Xmax_tab_textChanged(const QString &arg1);//输入X轴最大值

    void on_lineEdit_Xmin_tab_textChanged(const QString &arg1);//输入X轴最小值

    void on_lineEdit_Ymax_tab_textChanged(const QString &arg1);//输入Y轴最大值

    void on_lineEdit_Ymin_tab_textChanged(const QString &arg1);//输入Y轴最小值

    void on_lineEdit_Xoffset_tab_textChanged(const QString &arg1);//X轴文本偏移

    void on_lineEdit_Yoffset_tab_textChanged(const QString &arg1);//Y轴文本偏移

    void on_lineEdit_XAxisLabel_tab_textChanged(const QString &arg1);//X轴轴标签

    void on_lineEdit_YAxisLabel_tab_textChanged(const QString &arg1);//Y轴轴标签

    void on_checkBox_chooseAxisdisplay_x_clicked(bool checked);//X轴显示

    void on_checkBox_chooseAxisdisplay_y_clicked(bool checked);//Y轴显示

    void on_comboBox_Scaleplace_x_currentTextChanged(const QString &arg1);//X的刻度位置（上中下）

    void on_comboBox_Scaleplace_y_currentTextChanged(const QString &arg1);//Y的刻度位置（左中右）



    void on_lineEdit_GraphName_tab_textChanged(const QString &arg1);//曲线名称

    void on_lineEdit_GraphWidth_tab_textChanged(const QString &arg1);//实时曲线宽度

    void on_lineEdit_GraphBuffer_tab_textChanged(const QString &arg1);//实时曲线缓冲区

    void on_pushButton_chooseGraphColor_tab_clicked();//实时曲线颜色

    void on_pushButton_Graphmode_tab_clicked(); //实时曲线样式


    void on_lineEdit_LGraphWidth_tab_textChanged(const QString &arg1);//理论曲线宽度

    void on_lineEdit_LGraphBuffer_tab_textChanged(const QString &arg1);//理论曲线缓冲区

    void on_pushButton_chooseLGraphColor_tab_clicked();//理论曲线颜色


    void on_pushButton_chooseGraphbasemap_tab_clicked();//曲线底图（理论弹道）

    void on_pushButton_chooseXparam_tab_clicked();//选择X参数

    void on_pushButton_chooseYparam_tab_clicked();//选择Y参数

private:
    Ui::GraphObjDlg *ui;
};

#endif // GRAPHOBJDLG_H
