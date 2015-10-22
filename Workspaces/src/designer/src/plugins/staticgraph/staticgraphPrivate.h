#ifndef STATICGRAPHPRIVATE_H
#define STATICGRAPHPRIVATE_H

#include <Net/NetComponents>
#include "qcustomplot.h"
#include <QVector>

//目标类---保存自多曲线配置对话框解析来的数据
class PlotGraphObject
{

public:

    //x轴最大值
    double m_Xmax_plot ;
    //x轴最小值
    double m_Xmin_plot ;
    //y轴最大值
    double m_Ymax_plot ;
    //y轴最小值
    double m_Ymin_plot ;
    //x轴标签文本偏移
    qint32 m_Xoffset_plot ;
    //y轴标签文本偏移
    qint32 m_Yoffset_plot ;
    //x轴标签
    QString m_XAxisLabel_plot ;
    //y轴标签
    QString m_YAxisLabel_plot ;
    //x轴刻度位置
    QString m_Scaleplace_x_plot;
    //y轴刻度位置
    QString m_Scaleplace_y_plot;
    //是否显示x轴
    bool m_XAxisdisplay_plot;
    //是否显示y轴
    bool m_YAxisdisplay_plot;

    //实时曲线名称
    QString m_GraphName_plot ;
    //实时曲线宽度
    qint32  m_GraphWidth_plot ;
    //实时曲线颜色
    QString m_strgraphColor_plot;
    //实时曲线缓冲区大小
    double  m_GraphBuffer_plot;

    //理论曲线名称
    QString m_LGraphName_plot ;
    //理论曲线宽度
    qint32  m_LGraphWidth_plot ;
    //理论曲线颜色
    QString m_strLgraphColor_plot;
    //理论曲线缓冲区大小
    double  m_LGraphBuffer_plot;

    //实时曲线x参数
    QString m_xParam_plot;
    //实时曲线y参数
    QString m_yParam_plot;

    //理论曲线弹道文件
    QString m_Lgraphfile_plot;//理论曲线文件(路径)

    //理论曲线图层
    QCPGraph* m_pLgraph;
    //实时曲线图层
    QCPGraph* m_pgraph;

    //实时曲线数据 注意，更新数据后须使用setData方法才能更新曲线
    QVector<double> m_Cx, m_Cy;
    //理论曲线数据 注意，更新数据后须使用setData方法才能更新曲线
    QVector<double> m_Lx, m_Ly;
    //读取理论曲线文件，并生成理论曲线
    bool ReadLLGraph();
};


class staticgraphPrivate :QObject
{
public:
    //构造函数，对于单参数，不允许隐式转换，强制显式转换
    explicit staticgraphPrivate(QWidget*);
    virtual ~staticgraphPrivate();
public:
    void setPlot();

    void update();


    //取自己设计的测试数据数据
    void getData_Test();
    //取从外部net模块接收的数据
    void getData();

    // 重设背景色
    void resetbackgroundColor();
    //重设背景图片
    void resetbackgroundImage();
public:

     //共性部分

    //图元背景颜色
    QColor m_backgroundColor;
    QColor backgroundColor()const{return m_backgroundColor;}
    void setbackgroundColor(const QColor bb);

    //图元背景图片
    QPixmap m_backgroundImage;
    QPixmap backgroundImage()const{return m_backgroundImage;}
    void setbackgroundImage(const QPixmap qq);

    //是否显示网格
    bool m_bShowGrid;
    bool getShowGrid() const{ return m_bShowGrid; }
    void setShowGrid(const bool bShowGrid);

    //是否显示图例
    bool m_bShowLegend;
    bool getShowLegend() const{ return m_bShowLegend; }
    void setShowLegend(const bool bShowLegend);




    //个性部分

    //多曲线配置
    QString m_textString;
    QString textString() const
    {
        return m_textString;
    }
    void setTextString(const QString string);

    //Json数据解析
    void parseJsonData();


private:
    //父窗口
    QWidget * parent;

    //目标数组
    QVector<PlotGraphObject> m_vctgraphObj;

    //画图基本工具类指针，QCustomPlot类继承自QWidget类
    QCustomPlot* m_plot;


    //数据接收类
    DataCenterInterface* m_dci_x;
    DataCenterInterface* m_dci_y;


};


#endif
