#ifndef STATICGRAPHPRIVATE_H
#define STATICGRAPHPRIVATE_H


//#include <QtDesigner/QDataCenterNet>
#include <Net/NetComponents>
#include "qcustomplot.h"

class staticgraphPrivate :QObject
{
public:
    //构造函数，对于单参数，不允许隐式转换，强制显式转换
    explicit staticgraphPrivate(QWidget*);
    virtual ~staticgraphPrivate();
public:
    void setPlot();//理论弹道底图曲线
    void setPlot2();//实时曲线
    void update();
    //取数据
    //void getData();

    //取数据
    //graphdatatype 表示输入的曲线（数据）类型:
    //0---理论曲线
    //1---实时曲线
    void getData(qint32 graphdatatype);

    // 重设背景色
    void resetbackgroundColor();
    //重设背景图片
    void resetbackgroundImage();
public:

    //1、外框
    //图元的名字
    QString m_strName;
    QString getName() const
    {
        return m_strName;
    }
    void setName(const QString string){m_strName=string;}

    //图元背景颜色
    QColor m_backgroundColor;   //图元背景颜色
    QColor backgroundColor()const{return m_backgroundColor;}
    void setbackgroundColor(const QColor bb);
    //图元背景图片
    QPixmap m_backgroundImage;   //图元背景图片
    QPixmap backgroundImage()const{return m_backgroundImage;}
    void setbackgroundImage(const QPixmap qq);


    //2、坐标轴
    bool m_bShowGrid;//是否显示网格
    bool getShowGrid() const{ return m_bShowGrid; }
    void setShowGrid(const bool bShowGrid);    

    bool m_bShowLegend;//是否显示图例
    bool getShowLegend() const{ return m_bShowLegend; }
    void setShowLegend(const bool bShowLegend);


    //x轴刻度范围
    double m_xUp; // x轴最大值
    double xUp()const{return m_xUp;}
    void setXUp(const double xu);

    double m_xDown;// x轴最小值
    double xDown()const{return m_xDown;}
    void setXDown(const double xd);

    //y轴刻度范围
    double m_yUp;//y轴最大值
    double yUp()const{return m_yUp;}
    void setYUp(const double yu);

    double m_yDown; //y轴最小值
    double yDown()const{return m_yDown;}
    void setYDown(const double yd);

    //实时曲线参数x输入
    QString m_paramX;    //实时曲线输入单参数数据X变量
    QString paramX()const{return m_paramX;}
    void setParamX(const QString px){m_paramX=px;}
    //实时曲线参数y输入
    QString m_paramY;    //实时曲线输入单参数数据Y变量
    QString paramY()const{return m_paramY;}
    void setParamY(const QString py){m_paramY=py;}

    QColor m_graphColor;   //实时曲线颜色
    QColor graphColor()const{return m_graphColor;}
    void setgraphColor(const QColor cc);

    int m_graphWidth;      //实时曲线宽度
    int graphWidth()const{return m_graphWidth;}
    void setgraphWidth(const int cw);

    //理论曲线
    QString m_strTFile;//理论曲线文件路径
    QString getTFile()const{return m_strTFile;}
    void setTFile(const QString file);

    QColor m_LgraphColor;//理论曲线颜色
    QColor LgraphColor()const{return m_LgraphColor;}
    void setlgraphColor(const QColor lc);

    qint32 m_LgraphWidth;//理论曲线宽度
    qint32 LgraphWidth()const{return m_LgraphWidth;}
    void setLgraphWidth(const qint32 lw);

    bool ReadLLCv();//读取理论曲线文件，并生成理论曲线

private:
    //父窗口
    QWidget * parent;

    //画图基本工具类指针，QCustomPlot类继承自QWidget类
    QCustomPlot* m_plot;


    //数据接收类
    DataCenterInterface* m_dci_x;
    DataCenterInterface* m_dci_y;


    //理论曲线数据
    QVector<double> m_Lx, m_Ly;//容器存放接收数据，用来画图
    //实时曲线数据
    QVector<double> m_x, m_y;//容器存放接收数据，用来画图

    bool m_bAlready;
    int m_pos;


};


#endif
