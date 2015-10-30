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
    QString m_OriginPlace;
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
    qint32 m_XAxiswideth;
    QString m_chooseXAxisColor;
    bool m_chooseXAxislabeldisplay;
    bool m_chooseYAxislabeldisplay;
    bool m_chooseXAxisScalelabeldisplay;
    bool m_chooseYAxisScalelabeldisplay;
    bool m_chooseXAxisScaleTickdisplay;
    bool m_chooseYAxisScaleTickdisplay;
    qint32 m_numOfXAxisScale;//刻度数的设置
    qint32 m_numOfYAxisScale;
    qint32 m_XAxisScaleRuler;//刻度小线段长度的设置
    qint32 m_YAxisScaleRuler;//刻度小线段长度的设置
    qint32 m_XAxisScaleprecision;
    qint32 m_YAxisScaleprecision;

    QString m_XAxislabelFont;
    QString m_XAxisScalelabelFont;
    QString m_YAxisScalelabelFont;

    QString m_XAxisScalelabeloffset_x;
    QString m_XAxisScalelabeloffset_y;
    QString m_YAxisScalelabeloffset_x;
    QString m_YAxisScalelabeloffset_y;


    //设置控件
    void setControls();

signals:






private slots:




    
    void on_lineEdit_Xmax_tab_textChanged(const QString &arg1);//输入X轴最大值

    void on_lineEdit_Xmin_tab_textChanged(const QString &arg1);//输入X轴最小值

    void on_lineEdit_Ymax_tab_textChanged(const QString &arg1);//输入Y轴最大值

    void on_lineEdit_Ymin_tab_textChanged(const QString &arg1);//输入Y轴最小值

    void on_lineEdit_XAxisLabel_tab_textChanged(const QString &arg1);//X轴轴标签

    void on_lineEdit_YAxisLabel_tab_textChanged(const QString &arg1);//Y轴轴标签

    void on_checkBox_chooseAxisdisplay_x_clicked(bool checked);//X轴显示

    void on_checkBox_chooseAxisdisplay_y_clicked(bool checked);//Y轴显示

    void on_comboBox_Scaleplace_x_currentTextChanged(const QString &arg1);//x的刻度位置（上中下）

    void on_comboBox_Scaleplace_y_currentTextChanged(const QString &arg1);//y的刻度位置（左中右）

    void on_lineEdit_GraphName_tab_textChanged(const QString &arg1);//曲线名称

    void on_lineEdit_GraphWidth_tab_textChanged(const QString &arg1);//实时曲线宽度


    void on_pushButton_chooseGraphColor_tab_clicked();//实时曲线颜色

    void on_pushButton_Graphmode_tab_clicked(); //实时曲线样式


    void on_lineEdit_LGraphWidth_tab_textChanged(const QString &arg1);//理论曲线宽度

    void on_pushButton_chooseLGraphColor_tab_clicked();//理论曲线颜色


    void on_pushButton_chooseGraphbasemap_tab_clicked();//曲线底图（理论弹道）

    void on_pushButton_chooseXparam_tab_clicked();//选择X参数

    void on_pushButton_chooseYparam_tab_clicked();//选择Y参数


    
    void on_lineEdit_XAxiswideth_tab_textChanged(const QString &arg1);

    void on_lineEdit_numOfXAxisScale_tab_textChanged(const QString &arg1);
    void on_lineEdit_numOfYAxisScale_tab_textChanged(const QString &arg1);
    void on_lineEdit_XAxisScaleRuler_tab_textChanged(const QString &arg1);
    void on_lineEdit_YAxisScaleRuler_tab_textChanged(const QString &arg1);
    void on_lineEdit_XAxisScaleprecision_tab_textChanged(const QString &arg1);
    void on_lineEdit_YAxisScaleprecision_tab_textChanged(const QString &arg1);



    void on_pushButton_chooseXAxisColor_tab_clicked();



    void on_checkBox_chooseXAxislabeldisplay_tab_clicked(bool checked);
    void on_checkBox_chooseYAxislabeldisplay_tab_clicked(bool checked);
    void on_checkBox_chooseXAxisScalelabeldisplay_tab_clicked(bool checked);
    void on_checkBox_chooseYAxisScalelabeldisplay_tab_clicked(bool checked);
    void on_checkBox_chooseXAxisScaleTickdisplay_tab_clicked(bool checked);
    void on_checkBox_chooseYAxisScaleTickdisplay_tab_clicked(bool checked);

    void on_pushButton_XAxislabelFont_tab_clicked();

    void on_pushButton_XAxisScalelabelFont_tab_clicked();
    void on_pushButton_YAxisScalelabelFont_tab_clicked();

    void on_lineEdit_XAxisScalelabeloffset_x_tab_textChanged(const QString &arg1);
    void on_lineEdit_XAxisScalelabeloffset_y_tab_textChanged(const QString &arg1);
    void on_lineEdit_YAxisScalelabeloffset_x_tab_textChanged(const QString &arg1);
    void on_lineEdit_YAxisScalelabeloffset_y_tab_textChanged(const QString &arg1);











    void on_comboBox_OriginPlace_currentTextChanged(const QString &arg1);

private:
    Ui::GraphObjDlg *ui;
};

#endif // GRAPHOBJDLG_H
