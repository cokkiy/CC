#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H



#include <QDialog>
#include <QDesignerFormEditorInterface>
#include "ui_graphdialog.h"
#include <Net/NetComponents>
#include "selectparamdialog.h"

QT_BEGIN_NAMESPACE



namespace Ui {
class GraphDialog;
}

class GraphDialog : public QDialog
{
    Q_OBJECT

public:

    explicit GraphDialog(QDesignerFormEditorInterface *core,QWidget *parent = 0);
    ~GraphDialog();

    int showDialog();
//    QString text() const;
//    void SetText(QString );

    QString TransStrStateJson() const;//传回Json字符串
    QString strStateJson;//生成的Json字符串

private slots:

    void on_pushButton_insertnewGraph_target_clicked();//增加曲线（插入新曲线）

    void on_pushButton_delchooseGraph_target_clicked();//删除选中曲线

    void on_pushButton_leftmovegraph_target_clicked();//左移曲线

    void on_pushButton_rightmovegraph_target_clicked();//右移曲线

    void on_lineEdit_Xmax_tab_textChanged(const QString &arg1);//输入X轴最大值

    void on_lineEdit_Xmin_tab_textChanged(const QString &arg1);//输入X轴最小值

    void on_lineEdit_Ymax_textChanged(const QString &arg1);//输入Y轴最大值

    void on_lineEdit_Ymin_textChanged(const QString &arg1);//输入Y轴最小值

    void on_lineEdit_wenbenpianyi_x_textChanged(const QString &arg1);//X轴文本偏移

    void on_lineEdit_wenbenpianyi_y_textChanged(const QString &arg1);//Y轴文本偏移

    void on_lineEdit_inputAxisLabel_x_textChanged(const QString &arg1);//X轴轴标签

    void on_lineEdit_inputAxisLabel_y_textChanged(const QString &arg1);//Y轴轴标签

    void on_lineEdit_inputGraphName_tab_textChanged(const QString &arg1);//曲线名称

    void on_lineEdit_GraphWidth_tab_textChanged(const QString &arg1);//曲线宽度

    void on_lineEdit_GraphBuffer_tab_textChanged(const QString &arg1);//曲线缓冲区

    void on_checkBox_chooseAxisdisplay_x_clicked(bool checked);//X轴显示

    void on_checkBox_chooseAxisdisplay_y_clicked(bool checked);//Y轴显示

    void on_pushButton_chooseGraphColor_tab_clicked();//曲线颜色

    void on_pushButton_chooseGraphbasemap_tab_clicked();//曲线底图（理论弹道）

    void on_pushButton_chooseXparam_tab_clicked();//选择X参数

    void on_pushButton_chooseYparam_tab_clicked();//选择Y参数

    void on_buttonBox_OKorCancel_target_accepted();//确认，传送 json

    void on_buttonBox_OKorCancel_target_rejected();//取消

    void on_comboBox_keduweizhi_x_currentTextChanged(const QString &arg1);//X的刻度位置（上中下）

    void on_comboBox_keduweizhi_y_currentTextChanged(const QString &arg1);//Y的刻度位置（左中右）

private:

    InformationInterface* net;


    Ui::GraphDialog *ui;
    QDesignerFormEditorInterface *m_core;

    qint32 numOfgraph;//曲线条数
    qint32 currentgraphindex;//当前曲线索引号,序号从0开始

    QWidget *tab;//默认的曲线1
    QWidget *tab_2;//添加曲线


    //定义多曲线配置的各种变量
    QString m_Xmax ;
    QString m_Xmin ;
    QString m_Ymax ;
    QString m_Ymin ;
    QString m_Xoffset ;
    QString m_Yoffset ;
    QString m_XAxisLabel ;
    QString m_YAxisLabel ;
    QString m_GraphName ;
    QString m_GraphWidth ;
    QString m_GraphBuffer;
    qint32 m_XAxisdisplay;
    qint32 m_YAxisdisplay;
    qint32 m_strgraphColor_RED ;
    qint32 m_strgraphColor_GREEN;
    qint32 m_strgraphColor_BLUE;
    QString m_keduweizhi_x;
    QString m_keduweizhi_y;
};


QT_END_NAMESPACE

#endif // GRAPHDIALOG_H
