#ifndef Q2WMAPPRIVATE_H
#define Q2WMAPPRIVATE_H
#include <Net/NetComponents>
#include "qcustomplot.h"
#include <QVector>

//目标类
class Q2wmapObject
{

public:

    //目标名称
    QString m_strObjName;

    //是否主目标
    bool m_bMainObj;

    //参数x的参数名
    QString m_strParamX;

    //参数y的参数名
    QString m_strParamY;

    //实时曲线颜色
    QColor m_ccurveColor;

    //实时曲线宽度
    qint32 m_icurveWidth;

    //理论曲线颜色
    QColor m_cLcurveColor;

    //理论曲线宽度
    qint32 m_iLcurveWidth;

    //理论曲线文件
    QString m_strLFile;

    //理论曲线
    QCPGraph* m_pLcurve;

    //实时曲线
    QCPGraph* m_pcurve;

    //实时曲线(最后收到的点)
    QCPGraph* m_pcurveLast;

    //实时曲线数据 注意，更新数据后须使用setData方法才能更新曲线
    QVector<double> m_Cx, m_Cy, m_CxLast, m_CyLast;

    //理论曲线数据 注意，更新数据后须使用setData方法才能更新曲线
    QVector<double> m_Lx, m_Ly;

    //读取理论曲线文件，并生成理论曲线
    bool ReadLLCv();

    //数据接收类
    DataCenterInterface* m_dci;
};

//静态元素类
class Q2wmapStatic
{
public:

    //构造函数
    Q2wmapStatic()
    {
        m_pStaticGraph = NULL;
    }

    //析构函数
    ~Q2wmapStatic()
    {
    }

    //元素类型 Circle=圆形区域 Label=标签 Polygon=不规则区域
    QString m_strType;

    //元素名称
    QString m_strName;

    //边线颜色
    QColor m_Color;

    //边线粗细
    qint32 m_iWidth;

    //位置经度(仅圆形区域/标签 使用)
    double m_dbPosL;

    //位置纬度(仅圆形区域/标签 使用)
    double m_dbPosB;

    //半径(Km)(仅圆形区域使用)
    double m_dbRadius;

    //字体(仅标签使用)
    QString m_strFont;

    //绘制的图形
    QCPGraph* m_pStaticGraph;
};


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

    //为第index个目标的实时曲线添加数据：一个点（x，y）
    void addData(qint32 index, double x, double y);

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

    //目标属性
    QString m_strObj;
    QString getObj()const{return m_strObj;}
    void setObj(const QString str);

    //静态元素
    QString m_strStatic;
    QString getStatic()const{return m_strStatic;}
    void setStatic(const QString str);

    //坐标轴颜色
    QColor m_AcurveColor;
    QColor getAColor()const{return m_AcurveColor;}
    void setAColor(const QColor cc);

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

    //鼠标拖动
    void Drag(qint32 from_x, qint32 from_y, qint32 to_x, qint32 to_y);

private:
    //父窗口
    QWidget * m_parent;

    //目标数组
    QVector<Q2wmapObject> m_vctObj;

    //静态元素数组
    QVector<Q2wmapStatic> m_vctSta;

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

    //工作站类
    StationInterface* m_si;

    //定时器类
    TimerInterface* m_ti;

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
