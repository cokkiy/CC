#ifndef Q2WMAP_H
#define Q2WMAP_H

#include <QWidget>
#include <qglobal.h>
class Q2wmapPrivate;

//这是一个二维地图类
class Q2wmap : public QWidget
{
    Q_OBJECT

    //目标配置
    Q_PROPERTY(QString objectConfig READ getobj WRITE setobj RESET rstobj)

    //静态元素配置
    Q_PROPERTY(QString staticConfig READ getstatic WRITE setstatic RESET rststatic)

    //地图路径
    Q_PROPERTY(QPixmap backgroundImage READ getPixmap WRITE setPixmap)

    //地图经度下限
    Q_PROPERTY(double leftL READ getLLowLimit WRITE setLLowLimit)

    //地图经度上限
    Q_PROPERTY(double rightL READ getLUpLimit WRITE setLUpLimit)

    //地图纬度下限
    Q_PROPERTY(double lowB READ getBLowLimit WRITE setBLowLimit)

    //地图纬度上限
    Q_PROPERTY(double upB READ getBUpLimit WRITE setBUpLimit)

    //视窗起始点经度
    Q_PROPERTY(double viewOriginL READ getViewOriginL WRITE setViewOriginL)

    //视窗起始点纬度
    Q_PROPERTY(double viewOriginB READ getViewOriginB WRITE setViewOriginB)

    //坐标轴颜色
    Q_PROPERTY(QColor axisColor READ getAColor WRITE setAColor)

    //图元矩形
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)

public:
    Q2wmap(QWidget *parent = 0);
    virtual ~Q2wmap();

    //获取/设置目标
    QString getobj() const;
    void setobj(const QString str);
    void rstobj(){}

    //获取/设置静态元素
    QString getstatic() const;
    void setstatic(const QString str);
    void rststatic(){}

    //获取/设置地图路径
    QPixmap getPixmap() const;
    void setPixmap(const QPixmap mp);

    //获取/设置坐标轴轴颜色
    QColor getAColor()const;
    void setAColor(const QColor co);

    //获取/设置理论曲线文件路径
    QString getTFile() const;
    void setTFile(const QString file);

    //获取/设置地图经度下限
    double getLLowLimit() const;
    void setLLowLimit(const double jl);

    //获取/设置地图经度上限
    double getLUpLimit() const;
    void setLUpLimit(const double ju);

    //获取/设置地图纬度下限
    double getBLowLimit() const;
    void setBLowLimit(const double wl);

    //获取/设置地图纬度上限
    double getBUpLimit()  const;
    void setBUpLimit(const double wu);

    //获取/设置视窗起始点经纬度
    double getViewOriginL() const;
    void setViewOriginL(const double l);
    double getViewOriginB() const;
    void setViewOriginB(const double b);

    //右键菜单事件
    void contextMenuEvent(QContextMenuEvent*);

    //事件-鼠标按下
    void mousePressEvent(QMouseEvent *event);

    //事件-鼠标移动
    void mouseMoveEvent(QMouseEvent *event);

    //事件-鼠标移动
    void mouseReleaseEvent(QMouseEvent *event);

protected:

    //自定义动作 显示经纬度网格
    QAction* act_ShowLBGrid;

    //自定义动作 自动漫游-跟随
    QAction* act_AutoCruise_flw;

    //自定义动作 自动漫游-跳跃
    QAction* act_AutoCruise_jmp;

    //自定义动作 允许鼠标拖动
    QAction* act_EnableDrag;

    //鼠标左键是否按下(用于处理鼠标拖动)
    bool m_bLButtonPressed;

    //鼠标左键按下时的x，y(用于处理鼠标拖动)
    qint32 m_x;
    qint32 m_y;

    //事件-改变窗口大小
    void resizeEvent(QResizeEvent *event);

    //事件 绘制
    void paintEvent(QPaintEvent *event);

    QScopedPointer<Q2wmapPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Q2wmap)
    Q_DISABLE_COPY(Q2wmap)


    //自动漫游 参数type: 0=跟随 1=跳跃
    void AutoCruise(bool type);

public slots:

    //显示经纬度网格
    void ShowLBGrid();

    //自动漫游-跟随
    void AutoCruise_flw();

    //自动漫游-跳跃
    void AutoCruise_jmp();

    //允许鼠标拖动
    void EnableDrag();
};

#endif
