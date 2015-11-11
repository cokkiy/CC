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
    //原点的位置：左下、左上、右下、右上、上中、下中、正中
    QString m_OriginPlace_plot;
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
    //理论曲线名称
    QString m_LGraphName_plot ;
    //理论曲线宽度
    qint32  m_LGraphWidth_plot ;
    //理论曲线颜色
    QString m_strLgraphColor_plot;
    //实时曲线x参数
    QString m_xParam_plot;
    //实时曲线y参数
    QString m_yParam_plot;
    //理论曲线弹道文件
    QString m_Lgraphfile_plot;
    //
    qint32 m_XAxiswideth_plot;
    QString m_chooseXAxisColor_plot ;
    QString m_chooseXAxisLabelColor_plot ;
    bool m_chooseXAxislabeldisplay_plot ;
    QString m_chooseYAxisLabelColor_plot ;
    bool m_chooseYAxislabeldisplay_plot ;
    QString m_chooseXAxisScaleLabelColor_plot;
    qint32 m_numOfXAxisScale_plot ;//设置刻度数
    qint32 m_numOfYAxisScale_plot ;
    qint32 m_XAxisScaleRuler_plot ;
    qint32 m_XAxisScaleprecision_plot ;
    bool m_chooseXAxisScalelabeldisplay_plot ;
    QString m_chooseYAxisScaleLabelColor_plot;
    qint32 m_YAxisScaleRuler_plot ;
    qint32 m_YAxisScaleprecision_plot ;
    bool m_chooseYAxisScalelabeldisplay_plot ;
    bool m_chooseXAxisScaleTickdisplay_plot ;
    bool m_chooseYAxisScaleTickdisplay_plot ;
    QString m_XAxislabelFont_plot;
    QString m_XAxisScalelabelFont_plot;
    QString m_YAxisScalelabelFont_plot;
    QString m_XAxisScalelabeloffset_x_plot ;
    QString m_XAxisScalelabeloffset_y_plot ;
    QString m_YAxisScalelabeloffset_x_plot ;
    QString m_YAxisScalelabeloffset_y_plot ;

    //曲线图层指针
    //理论曲线图层
    QCPGraph* m_pLgraph;
    //实时曲线图层
    QCPGraph* m_pgraph;
    //实时曲线(最后收到的点)---闪烁点
    QCPGraph* m_pgraphLast;

    //曲线坐标轴指针
    QCPAxis* m_pXAxis;
    QCPAxis* m_pYAxis;

    //实时曲线数据 注意，更新数据后须使用setData方法才能更新曲线
    QVector<double> m_Cx, m_Cy,m_CxLast, m_CyLast;
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
    //设置初始化参数
    void setPlotOfInitialization();
    //设置画图更新的配置参数----根据多曲线配置对话框所作的坐标轴参数静态配置
    void setPlotOfAxis();
    //工作代码
    void setPlot();
    //即m_plot->replot()
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

    //页边距属性配置
    //获取/设置理论页边距属性
    //leftMargin
    qint32 m_leftmargin;
    qint32 getleftMargin()const
    {
        return m_leftmargin;
    }
    //rightMargin
    qint32 m_rightmargin;
    qint32 getrightMargin()const
    {
        return m_rightmargin;
    }
    //topMargin
    qint32 m_topmargin;
    qint32 gettopMargin()const
    {
        return m_topmargin;
    }
    //bottomMargin
    qint32 m_bottommargin;
    qint32 getbottomMargin()const
    {
        return m_bottommargin;
    }
    //统一设置margins
    void setleftMargin(const qint32 lm);
    void setrightMargin(const qint32 rm);
    void settopMargin(const qint32 tm);
    void setbottomMargin(const qint32 bm);
    qint32 m_riginplacenum;

    //多曲线配置
    QString m_textString;
    QString textString() const
    {
        return m_textString;
    }
    void setTextString(const QString string);

    //Json数据解析
    //Json解析与多曲线对象实例赋值（赋值给装有多个对象实例的数组（容器））
    void parseJsonData();


    //由原点类型获取轴类型
    qint32 m_AxesIndex;//第几套轴
    QCPAxis::AxisType m_xAxistype,m_yAxistype;
    QCPAxis::AxisType getxAxistype(const QString oxp);
    QCPAxis::AxisType getyAxistype(const QString oyp);

    //设置滚动模式
    void rollingmode(qint32 index,bool on);

    //设置当前改变（缩放、拖动后）的范围给plot
    QCPRange setcurrentrangex(double m_xoldvalue);
    QCPRange setcurrentrangey(double m_yoldvalue);
    //配置范围是不断变化的
    double XAxisTickRange,YAxisTickRange ;
    QCPRange m_xAxisrange,m_yAxisrange;
    //对于实时过程对于初始设置的改变，进行重新设置，并重绘
    void setplotchanged();
    QVector<double> m_vecxAxis;



public slots:




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
