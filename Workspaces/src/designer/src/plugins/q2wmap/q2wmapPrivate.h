#ifndef Q2WMAPPRIVATE_H
#define Q2WMAPPRIVATE_H
#include <Net/NetComponents>
#include "qcustomplot.h"

class Q2wmapPrivate : QObject
{
    Q_OBJECT

public:
    explicit Q2wmapPrivate(QWidget*);
    virtual ~Q2wmapPrivate();

    //取数据(测试)
    void TestgetData();

    //取数据
    void getData();

    //添加数据
    void addData(double x, double y);

    //设置曲线属性
    void setPlot();

    //视窗向地图左方移动
    void MoveLeft(qint32 pix);

    //视窗向地图右方移动
    void MoveRight(qint32 pix);

    //视窗向地图上方移动
    void MoveTop(qint32 pix);

    //视窗向地图下方移动
    void MoveBottom(qint32 pix);

    //视窗中心移动到指定的位置(地图经纬度)
    //L=经度
    //B=纬度
    void MoveToPointOfMapLB(double L, double B);

    //视窗中心移动到指定的位置(地图图像的x和y，单位是像素)
    void MoveToPointOfMap(qint32 x, qint32 y);

    //自动移动视窗，防止曲线当前点绘制到可见区域外
    //L=曲线当前点经度
    //B=曲线当前点纬度
    void AutoMoveView(const double L, const double B);

    //刷新
    void update();

    //计算视窗的经纬度上下限
    void CalcViewLB(double &VLL, double &VLU, double &VBL, double &VBU);

    //参数x的参数名
    QString m_paramX;
    QString getParamX()const{return m_paramX;}
    void setParamX(const QString px){m_paramX=px;}

    //参数y的参数名
    QString m_paramY;
    QString getParamY()const{return m_paramY;}
    void setParamY(const QString py){m_paramY=py;}

    //实时曲线颜色
    QColor m_curveColor;
    QColor getColor()const{return m_curveColor;}
    void setColor(const QColor cc);

    //实时曲线宽度
    qint32 m_curveWidth;
    qint32 getCurveWidth()const{return m_curveWidth;}
    void setCurveWidth(const qint32 cw);

    //理论曲线颜色
    QColor m_LcurveColor;
    QColor getLColor()const{return m_LcurveColor;}
    void setLColor(const QColor cc);

    //坐标轴颜色
    QColor m_AcurveColor;
    QColor getAColor()const{return m_AcurveColor;}
    void setAColor(const QColor cc);

    //理论曲线宽度
    qint32 m_LcurveWidth;
    qint32 getLCurveWidth()const{return m_LcurveWidth;}
    void setLCurveWidth(const qint32 cw);

    //地图经度下限
    double m_LLowLimit;
    double getLLowLimit() const{ return m_LLowLimit; }
    void setLLowLimit(const double jl);

    //地图经度上限
    double m_LUpLimit;
    double getLUpLimit() const{ return m_LUpLimit; }
    void setLUpLimit(const double ju);

    //地图纬度下限
    double m_BLowLimit;
    double getBLowLimit() const{ return m_BLowLimit; }
    void setBLowLimit(const double wl);

    //地图纬度上限
    double m_BUpLimit;
    double getBUpLimit() const{ return m_BUpLimit; }
    void setBUpLimit(const double wu);

    //地图路径
    QPixmap m_pixmap;
    QPixmap getPixmap()const{return m_pixmap;}
    void setPixmap(const QPixmap pixmap){m_pixmap = pixmap;}

    //理论曲线文件路径
    QString m_strTFile;
    QString getTFile()const{return m_strTFile;}
    void setTFile(const QString file);

    //地图是否显示网格
    bool m_bShowGrid;
    bool getShowGrid() const{ return m_bShowGrid; }
    void setShowGrid(const bool bShowGrid);

    //是否自动漫游
    bool m_bAutoCruise;

    //自动漫游类型 0=跟随 1=跳跃
    bool m_Cruisetype;
    bool getAutoCruise() const{ return m_bAutoCruise; }
    void setAutoCruise(const bool bAutoCruise, bool type);

    //地图是否可用鼠标拖动
    bool m_bEnableDrag;
    bool getEnableDrag() const{ return m_bEnableDrag; }
    void setEnableDrag(const bool bEnableDrag);

    //读取理论曲线文件，并生成理论曲线
    bool ReadLLCv();

    //鼠标拖动
    void Drag(qint32 from_x, qint32 from_y, qint32 to_x, qint32 to_y);

private:
    //父窗口
    QWidget * m_parent;

    //视窗左上角相对地图左上角的位移量x
    qint32 m_Pos_x;

    //视窗左上角相对地图左上角的位移量y
    qint32 m_Pos_y;

    //曲线
    QCustomPlot* m_plot;

    //图元的宽度
    qint32 m_width;

    //图元的高度
    qint32 m_height;

    //数据接收类
    DataCenterInterface* m_dci;

    //工作站类
    StationInterface* m_si;

    //定时器类
    TimerInterface* m_ti;

    //实时曲线数据
    QVector<double> m_x, m_y;

    //理论曲线数据
    QVector<double> m_Lx, m_Ly;

    //视窗的经度下限
    double m_ViewLLow;

    //视窗的经度上限
    double m_ViewLUp;

    //视窗的纬度下限
    double m_ViewBLow;

    //视窗的纬度上限
    double m_ViewBUp;

public slots:
    void timeEvent_100ms();

};

#endif // Q2WMAPPRIVATE_H
