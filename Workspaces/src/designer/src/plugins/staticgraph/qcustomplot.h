/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011, 2012, 2013, 2014 Emanuel Eichhammer               **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 27.12.14                                             **
**          Version: 1.3.0                                                **
****************************************************************************/

#ifndef QCUSTOMPLOT_H
#define QCUSTOMPLOT_H

#include <QObject>
#include <QPointer>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QMultiMap>
#include <QFlags>
#include <QDebug>
#include <QVector2D>
#include <QStack>
#include <QCache>
#include <QMargins>
#include <qmath.h>
#include <limits>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#  include <qnumeric.h>
#  include <QPrinter>
#  include <QPrintEngine>
#else
#  include <QtNumeric>
#  include <QtPrintSupport>
#endif

//画图类，继承自QPainter类
class QCPPainter;
//画图工具的基本类，继承自QWidget类
class QCustomPlot;
//图层类,继承自QObject类
//可分层的对象，是所有可绘制到屏幕上的对象的类的基类
class QCPLayerable;

//QCPLayerable类的子类之1---QCPLayoutElement类
//可布局的元素类，其子类都可通过QCP布局系统，像QT中的布局那样，使它们组织得更有条理
class QCPLayoutElement;
//QCPLayoutElement类的子类QCPLayout类
class QCPLayout;

//QCPLayerable类的子类之2---QCPAxis类
//坐标轴类
class QCPAxis;
//QCPLayoutElement类的子类QCPAxisRect
//轴长方形区，用于存放轴
class QCPAxisRect;
//轴的私有画图类
class QCPAxisPainterPrivate;

//QCPLayerable类的子类之3---QCPAbstractPlottable类
//抽象类，其子类是可绘制的图形（表示数据系列的图线）
class QCPAbstractPlottable;
//派生自QCPAbstractPlottable类，曲线类，1个x值对应1个y值
class QCPGraph;
//派生自QCPAbstractPlottable类，弧线类，1个x值对应2个y值
//class QCP_LIB_DECL QCPCurve : public QCPAbstractPlottable

//QCPLayerable类的子类之4---QCPAbstractItem类
//抽象类，其子类是用于显示一些特殊的图形，如放上一张图片，加载一个带箭头的直线等
class QCPAbstractItem;
//Position位置，Anchor锚定
//用于与鼠标、键盘定位相关的类
class QCPItemPosition;
//Layer，阶层
//阶层类
class QCPLayer;
//QCPLayoutElement类的子类QCPPlotTitle
//图表标题，即最顶上标题
class QCPPlotTitle;
//class QCP_LIB_DECL QCPLegend : public QCPLayoutGrid
//图例类，即图表右下角的大框
class QCPLegend;
//QCPLayoutElement类的子类QCPAbstractLegendItem
//抽象类，其子类主要用于图例中间显示一些特殊的图形
class QCPAbstractLegendItem;
//QCPAbstractPlottable类的子类QCPColorMap
//色图类,就是一个大的正方形有很多小色块，可以用笔选其中的小色块
class QCPColorMap;

//QCPAbstractPlottable类的子类QCPStatisticalBox
//统计学箱类
//class QCP_LIB_DECL QCPStatisticalBox : public QCPAbstractPlottable

//Scale,刻度,衡量,比例,数值范围,比例尺,天平,等级
//class QCP_LIB_DECL QCPColorScale : public QCPLayoutElement
//颜色刻度类
class QCPColorScale;
//QCPAbstractPlottable类的子类QCPBars
//柱状图类
class QCPBars;

//QCPLayerable类的子类之5---QCPGrid类
// 网格类，背景虚线网格
//class QCP_LIB_DECL QCPGrid :public QCPLayerable
//范围类
//class QCP_LIB_DECL QCPRange


/*! \file */


// decl definitions for shared library compilation/usage:
#if defined(QCUSTOMPLOT_COMPILE_LIBRARY)
#  define QCP_LIB_DECL Q_DECL_EXPORT
#elif defined(QCUSTOMPLOT_USE_LIBRARY)
#  define QCP_LIB_DECL Q_DECL_IMPORT
#else
#  define QCP_LIB_DECL
#endif

/*!
  The QCP Namespace contains general enums and QFlags used throughout the QCustomPlot library
*/

//QCP名空间
//包含了在整个the QCustomPlot library使用的，常用的枚举enums和标志QFlags
namespace QCP
{
/*!
  Defines the sides of a rectangular entity to which margins can be applied.

  \see QCPLayoutElement::setAutoMargins, QCPAxisRect::setAutoMargins
*/
//页边距（页边的空白）参数
enum MarginSide { msLeft     = 0x01 ///< <tt>0x01</tt> left margin
                  ,msRight   = 0x02 ///< <tt>0x02</tt> right margin
                  ,msTop     = 0x04 ///< <tt>0x04</tt> top margin
                  ,msBottom  = 0x08 ///< <tt>0x08</tt> bottom margin
                  ,msAll     = 0xFF ///< <tt>0xFF</tt> all margins
                  ,msNone    = 0x00 ///< <tt>0x00</tt> no margin
                };
Q_DECLARE_FLAGS(MarginSides, MarginSide)

/*!
  Defines what objects of a plot can be forcibly drawn antialiased/not antialiased. If an object is
  neither forcibly drawn antialiased nor forcibly drawn not antialiased, it is up to the respective
  element how it is drawn. Typically it provides a \a setAntialiased function for this.

  \c AntialiasedElements is a flag of or-combined elements of this enum type.

  \see QCustomPlot::setAntialiasedElements, QCustomPlot::setNotAntialiasedElements
*/

//抗锯齿的元素
//为了边缘光滑
enum AntialiasedElement { aeAxes           = 0x0001 ///< <tt>0x0001</tt> Axis base line and tick marks 轴的基本线和记号
                          ,aeGrid          = 0x0002 ///< <tt>0x0002</tt> Grid lines 网格线
                          ,aeSubGrid       = 0x0004 ///< <tt>0x0004</tt> Sub grid lines 子网格线
                          ,aeLegend        = 0x0008 ///< <tt>0x0008</tt> Legend box 图例盒
                          ,aeLegendItems   = 0x0010 ///< <tt>0x0010</tt> Legend items 图例项
                          ,aePlottables    = 0x0020 ///< <tt>0x0020</tt> Main lines of plottables (excluding error bars, see element \ref aeErrorBars) 图表主要的线
                          ,aeItems         = 0x0040 ///< <tt>0x0040</tt> Main lines of items 主要的线和项目
                          ,aeScatters      = 0x0080 ///< <tt>0x0080</tt> Scatter symbols of plottables (excluding scatter symbols of type ssPixmap) 图表的散点记号
                          ,aeErrorBars     = 0x0100 ///< <tt>0x0100</tt> Error bars 误差条、错误条
                          ,aeFills         = 0x0200 ///< <tt>0x0200</tt> Borders of fills (e.g. under or between graphs)  画出的图的边界边框
                          ,aeZeroLine      = 0x0400 ///< <tt>0x0400</tt> Zero-lines, see \ref QCPGrid::setZeroLinePen  0线
                          ,aeAll           = 0xFFFF ///< <tt>0xFFFF</tt> All elements  所有元素
                          ,aeNone          = 0x0000 ///< <tt>0x0000</tt> No elements   没有元素
                        };
Q_DECLARE_FLAGS(AntialiasedElements, AntialiasedElement)

/*!
  Defines plotting hints that control various aspects of the quality and speed of plotting.

  \see QCustomPlot::setPlottingHints
*/

//画图的提示
enum PlottingHint { phNone            = 0x000 ///< <tt>0x000</tt> No hints are set  没设提示
                    ,phFastPolylines  = 0x001 ///< <tt>0x001</tt> Graph/Curve lines are drawn with a faster method. This reduces the quality
                                              ///<                especially of the line segment joins. (Only relevant for solid line pens.)
                                              ///<                最快的画曲线/弧线方式，简化线条的一些特别的设置，如仅设置为实线画笔
                    ,phForceRepaint   = 0x002 ///< <tt>0x002</tt> causes an immediate repaint() instead of a soft update() when QCustomPlot::replot() is called with parameter \ref QCustomPlot::rpHint.
                                              ///<                This is set by default to prevent the plot from freezing on fast consecutive replots (e.g. user drags ranges with mouse).
                                              ///<                强制重绘，取代温柔的update()
                    ,phCacheLabels    = 0x004 ///< <tt>0x004</tt> axis (tick) labels will be cached as pixmaps, increasing replot performance.
                                              ///<                隐藏标签，轴的标签被隐藏作为象素映射，增加重绘执行
                  };
Q_DECLARE_FLAGS(PlottingHints, PlottingHint)

/*!
  Defines the mouse interactions possible with QCustomPlot.

  \c Interactions is a flag of or-combined elements of this enum type.

  \see QCustomPlot::setInteractions
*/

//交互性
enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref QCPAxisRect::setRangeDrag, \ref QCPAxisRect::setRangeDragAxes) 轴的拖动范围
                   ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref QCPAxisRect::setRangeZoom, \ref QCPAxisRect::setRangeZoomAxes) 用鼠标缩放轴的范围
                   ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref QCustomPlot::setMultiSelectModifier while clicking 复选
                   ,iSelectPlottables = 0x008 ///< <tt>0x008</tt> Plottables are selectable (e.g. graphs, curves, bars,... see QCPAbstractPlottable) 选定图表
                   ,iSelectAxes       = 0x010 ///< <tt>0x010</tt> Axes are selectable (or parts of them, see QCPAxis::setSelectableParts) 轴被选定
                   ,iSelectLegend     = 0x020 ///< <tt>0x020</tt> Legends are selectable (or their child items, see QCPLegend::setSelectableParts)  图例被选定
                   ,iSelectItems      = 0x040 ///< <tt>0x040</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref QCPAbstractItem) 项目（控件）被选定
                   ,iSelectOther      = 0x080 ///< <tt>0x080</tt> All other objects are selectable (e.g. your own derived layerables, the plot title,...) 其他对象被选定
                 };
Q_DECLARE_FLAGS(Interactions, Interaction)

/*! \internal

  Returns whether the specified \a value is considered an invalid data value for plottables (i.e.
  is \e nan or \e +/-inf). This function is used to check data validity upon replots, when the
  compiler flag \c QCUSTOMPLOT_CHECK_DATA is set.
*/

//Invalid无效性检查
//用于检查数据重绘时，当编译器标志被设置时，其数据有效性（合法性）
inline bool isInvalidData(double value)
{
  return qIsNaN(value) || qIsInf(value);
}

/*! \internal
  \overload

  Checks two arguments instead of one.
*/
inline bool isInvalidData(double value1, double value2)
{
  return isInvalidData(value1) || isInvalidData(value2);
}

/*! \internal

  Sets the specified \a side of \a margins to \a value

  \see getMarginValue
*/

//设置留白（页边距等）
//QMargins实际上就是画四种矩形
inline void setMarginValue(QMargins &margins, QCP::MarginSide side, int value)
{
    //
  switch (side)
  {
    case QCP::msLeft: margins.setLeft(value); break;//左边留白
    case QCP::msRight: margins.setRight(value); break;//右边留白
    case QCP::msTop: margins.setTop(value); break;//上边留白
    case QCP::msBottom: margins.setBottom(value); break;//下边留白
    case QCP::msAll: margins = QMargins(value, value, value, value); break;//设置所有边
    default: break;
  }
}

/*! \internal

  Returns the value of the specified \a side of \a margins. If \a side is \ref QCP::msNone or
  \ref QCP::msAll, returns 0.

  \see setMarginValue
*/

//获取留白值（页边距等）
//QMargins实际上就是画四种矩形
inline int getMarginValue(const QMargins &margins, QCP::MarginSide side)
{
  switch (side)
  {
    case QCP::msLeft: return margins.left();//获取左边留白值
    case QCP::msRight: return margins.right();//获取右边留白值
    case QCP::msTop: return margins.top();//获取上边留白值
    case QCP::msBottom: return margins.bottom();//获取下边留白值
    default: break;
  }
  return 0;
}

} // end of namespace QCP

Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::AntialiasedElements)//抗锯齿的元素，为了边缘光滑
Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::PlottingHints)//画图的提示
Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::MarginSides)//设置留白（页边距等）
Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::Interactions)//交互性, 主要是选定图元的某个对象（如图表、轴、边框等）


//散点类型（样式）类
class QCP_LIB_DECL QCPScatterStyle
{
  Q_GADGET
public:
  /*!
    Defines the shape used for scatter points.

    On plottables/items that draw scatters, the sizes of these visualizations (with exception of
    \ref ssDot and \ref ssPixmap) can be controlled with the \ref setSize function. Scatters are
    drawn with the pen and brush specified with \ref setPen and \ref setBrush.
  */

  //散点的形状，枚举了各种类型
  Q_ENUMS(ScatterShape)
  enum ScatterShape { ssNone       ///< no scatter symbols are drawn (e.g. in QCPGraph, data only represented with lines)
                      ,ssDot       ///< \enumimage{ssDot.png} a single pixel (use \ref ssDisc or \ref ssCircle if you want a round shape with a certain radius)
                      ,ssCross     ///< \enumimage{ssCross.png} a cross
                      ,ssPlus      ///< \enumimage{ssPlus.png} a plus
                      ,ssCircle    ///< \enumimage{ssCircle.png} a circle
                      ,ssDisc      ///< \enumimage{ssDisc.png} a circle which is filled with the pen's color (not the brush as with ssCircle)
                      ,ssSquare    ///< \enumimage{ssSquare.png} a square
                      ,ssDiamond   ///< \enumimage{ssDiamond.png} a diamond
                      ,ssStar      ///< \enumimage{ssStar.png} a star with eight arms, i.e. a combination of cross and plus
                      ,ssTriangle  ///< \enumimage{ssTriangle.png} an equilateral triangle, standing on baseline
                      ,ssTriangleInverted ///< \enumimage{ssTriangleInverted.png} an equilateral triangle, standing on corner
                      ,ssCrossSquare      ///< \enumimage{ssCrossSquare.png} a square with a cross inside
                      ,ssPlusSquare       ///< \enumimage{ssPlusSquare.png} a square with a plus inside
                      ,ssCrossCircle      ///< \enumimage{ssCrossCircle.png} a circle with a cross inside
                      ,ssPlusCircle       ///< \enumimage{ssPlusCircle.png} a circle with a plus inside
                      ,ssPeace     ///< \enumimage{ssPeace.png} a circle, with one vertical and two downward diagonal lines
                      ,ssPixmap    ///< a custom pixmap specified by \ref setPixmap, centered on the data point coordinates
                      ,ssCustom    ///< custom painter operations are performed per scatter (As QPainterPath, see \ref setCustomPath)
                    };

  //构造函数
  QCPScatterStyle();
  QCPScatterStyle(ScatterShape shape, double size=6);
  QCPScatterStyle(ScatterShape shape, const QColor &color, double size);
  QCPScatterStyle(ScatterShape shape, const QColor &color, const QColor &fill, double size);
  QCPScatterStyle(ScatterShape shape, const QPen &pen, const QBrush &brush, double size);
  QCPScatterStyle(const QPixmap &pixmap);
  QCPScatterStyle(const QPainterPath &customPath, const QPen &pen, const QBrush &brush=Qt::NoBrush, double size=6);

  // getters:
  double size() const { return mSize; }//散点大小
  ScatterShape shape() const { return mShape; }//散点形状
  QPen pen() const { return mPen; }//画笔
  QBrush brush() const { return mBrush; }//画刷
  QPixmap pixmap() const { return mPixmap; }//象素映射
  QPainterPath customPath() const { return mCustomPath; }//规定的路径

  // setters:
  void setSize(double size);//设置散点大小
  void setShape(ScatterShape shape);//设置散点形状
  void setPen(const QPen &pen);//设置画笔
  void setBrush(const QBrush &brush);//设置画刷
  void setPixmap(const QPixmap &pixmap);//设置象素映射
  void setCustomPath(const QPainterPath &customPath);//设置规定的路径

  // non-property methods:
  bool isNone() const { return mShape == ssNone; }//没有散点类型
  bool isPenDefined() const { return mPenDefined; }//画笔定义
  void applyTo(QCPPainter *painter, const QPen &defaultPen) const;//应用缺省画笔
  void drawShape(QCPPainter *painter, QPointF pos) const;//画点
  void drawShape(QCPPainter *painter, double x, double y) const;//画点

protected:
  // property members:
  double mSize;//设置散点大小
  ScatterShape mShape;//设置散点形状
  QPen mPen;//设置画笔
  QBrush mBrush;//设置画刷
  QPixmap mPixmap;//设置象素映射
  QPainterPath mCustomPath;//设置规定的路径

  // non-property members:
  bool mPenDefined;//画笔定义
};
Q_DECLARE_TYPEINFO(QCPScatterStyle, Q_MOVABLE_TYPE);

//画图类，继承自QPainter类
class QCP_LIB_DECL QCPPainter : public QPainter
{
  Q_GADGET
public:
  /*!
    Defines special modes the painter can operate in. They disable or enable certain subsets of features/fixes/workarounds,
    depending on whether they are wanted on the respective output device.
  */
  enum PainterMode { pmDefault       = 0x00   ///< <tt>0x00</tt> Default mode for painting on screen devices 缺省模式
                     ,pmVectorized   = 0x01   ///< <tt>0x01</tt> Mode for vectorized painting (e.g. PDF export). For example, this prevents some antialiasing fixes.矢量模式
                     ,pmNoCaching    = 0x02   ///< <tt>0x02</tt> Mode for all sorts of exports (e.g. PNG, PDF,...). For example, this prevents using cached pixmap labels 缓冲模式
                     ,pmNonCosmetic  = 0x04   ///< <tt>0x04</tt> Turns pen widths 0 to 1, i.e. disables cosmetic pens. (A cosmetic pen is always drawn with width 1 pixel
                     ///<                           in the vector image/pdf viewer, independent of zoom.)  去装饰模式，只保留线宽为0-1之间的画笔
                   };
  Q_FLAGS(PainterMode PainterModes)
  Q_DECLARE_FLAGS(PainterModes, PainterMode)

  //构造函数
  QCPPainter();
  QCPPainter(QPaintDevice *device);
  ~QCPPainter();

  // getters:
  bool antialiasing() const { return testRenderHint(QPainter::Antialiasing); }//抗锯齿
  PainterModes modes() const { return mModes; } //画图模式

  // setters:
  void setAntialiasing(bool enabled);//设置抗锯齿
  void setMode(PainterMode mode, bool enabled=true);//设置画图模式
  void setModes(PainterModes modes);

  // methods hiding non-virtual base class functions (QPainter bug workarounds):
  bool begin(QPaintDevice *device);//初始化绘图设备类
  void setPen(const QPen &pen);// 设置画笔
  void setPen(const QColor &color);
  void setPen(Qt::PenStyle penStyle);
  void drawLine(const QLineF &line);//画线
  void drawLine(const QPointF &p1, const QPointF &p2) {drawLine(QLineF(p1, p2));}
  void save();//保存，抗锯齿
  void restore();//重新恢复

  // non-virtual methods:
  void makeNonCosmetic();//去装饰模式，只保留线宽为0-1之间的画笔

protected:
  // property members:
  PainterModes mModes;//画图模式,枚举变量，如：mModes=pmVectorized，表示矢量模式
  bool mIsAntialiasing;//抗锯齿

  // non-property members:
  QStack<bool> mAntialiasingStack;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCPPainter::PainterModes)



//Layer，阶层
//阶层类
class QCP_LIB_DECL QCPLayer : public QObject
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCustomPlot* parentPlot READ parentPlot)//画图类的父指针parentPlot
  Q_PROPERTY(QString name READ name)//名称
  Q_PROPERTY(int index READ index)//索引
  Q_PROPERTY(QList<QCPLayerable*> children READ children)//可分层的对象列表children（）函数
  Q_PROPERTY(bool visible READ visible WRITE setVisible)//可见的
  /// \endcond
public:
  //构造函数
  QCPLayer(QCustomPlot* parentPlot, const QString &layerName);//画图类的父指针parentPlot
  ~QCPLayer();

  // getters:
  QCustomPlot *parentPlot() const { return mParentPlot; }//设置画图类的父指针parentPlot
  QString name() const { return mName; }//设置名称
  int index() const { return mIndex; }//设置索引
  QList<QCPLayerable*> children() const { return mChildren; }//设置可分层的对象列表children（）函数
  bool visible() const { return mVisible; }//设置可见性

  // setters:
  void setVisible(bool visible);

protected:
  // property members:
  QCustomPlot *mParentPlot;//画图类的父指针parentPlot
  QString mName;//名称
  int mIndex;//索引
  QList<QCPLayerable*> mChildren;//可分层的对象列表
  bool mVisible;//可见性

  // non-virtual methods:
  void addChild(QCPLayerable *layerable, bool prepend);//增加可分层的对象列表
  void removeChild(QCPLayerable *layerable);//移除可分层的对象列表

private:
  Q_DISABLE_COPY(QCPLayer)

  friend class QCustomPlot;//友元类QCustomPlot
  friend class QCPLayerable;//友元类QCPLayerable
};

//图层类,继承自QObject类
//可分层的对象，是所有可绘制到屏幕上的对象的类的基类
class QCP_LIB_DECL QCPLayerable : public QObject
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(bool visible READ visible WRITE setVisible)//设置可见性
  Q_PROPERTY(QCustomPlot* parentPlot READ parentPlot)//画图类的父指针parentPlot
  Q_PROPERTY(QCPLayerable* parentLayerable READ parentLayerable)//可分层的对象类的指针parentLayerable
  Q_PROPERTY(QCPLayer* layer READ layer WRITE setLayer NOTIFY layerChanged)//阶层类QCPLayer的指针
  Q_PROPERTY(bool antialiased READ antialiased WRITE setAntialiased)//抗锯齿
  /// \endcond
public:
  QCPLayerable(QCustomPlot *plot, QString targetLayer=QString(), QCPLayerable *parentLayerable=0);
  ~QCPLayerable();

  // getters:
  bool visible() const { return mVisible; }//设置可见性
  QCustomPlot *parentPlot() const { return mParentPlot; }//画图类的父指针parentPlot
  QCPLayerable *parentLayerable() const { return mParentLayerable.data(); }//可分层的对象类的指针parentLayerable
  QCPLayer *layer() const { return mLayer; }//阶层类QCPLayer的指针
  bool antialiased() const { return mAntialiased; }//抗锯齿

  // setters:
  void setVisible(bool on);//设置可见性
  Q_SLOT bool setLayer(QCPLayer *layer);//设置阶层类QCPLayer的指针
  bool setLayer(const QString &layerName);
  void setAntialiased(bool enabled); //设置抗锯齿

  // introduced virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  // non-property methods:
  bool realVisibility() const;//实时可见性

signals:
  void layerChanged(QCPLayer *newLayer);//阶层改变

protected:
  // property members:
  bool mVisible;//设置可见性
  QCustomPlot *mParentPlot;//画图类的父指针parentPlot
  QPointer<QCPLayerable> mParentLayerable;//可分层的对象类的指针parentLayerable
  QCPLayer *mLayer;//阶层类QCPLayer的指针
  bool mAntialiased;//抗锯齿

  // introduced virtual methods:
  virtual void parentPlotInitialized(QCustomPlot *parentPlot);//画图类的父指针parentPlot初始化
  virtual QCP::Interaction selectionCategory() const;//交互性，你到定选中啥
  virtual QRect clipRect() const;//夹住的区域
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const = 0;//应用缺省的抗锯齿提示
  virtual void draw(QCPPainter *painter) = 0;//画图
  // events:
  //选择事件
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  //未选择事件
  virtual void deselectEvent(bool *selectionStateChanged);

  // non-property methods:
  void initializeParentPlot(QCustomPlot *parentPlot);//初始化画图类的父指针
  void setParentLayerable(QCPLayerable* parentLayerable);//设置可分层对象的指针
  bool moveToLayer(QCPLayer *layer, bool prepend);//移动到某阶层
  //应用缺省的抗锯齿提示
  void applyAntialiasingHint(QCPPainter *painter, bool localAntialiased, QCP::AntialiasedElement overrideElement) const;

private:
  Q_DISABLE_COPY(QCPLayerable)

  friend class QCustomPlot;
  friend class QCPAxisRect;//友元类轴长方形区类（用于存放轴）
};

//范围类
class QCP_LIB_DECL QCPRange
{
public:
  double lower, upper;

  QCPRange();
  QCPRange(double lower, double upper);

  bool operator==(const QCPRange& other) const { return lower == other.lower && upper == other.upper; }
  bool operator!=(const QCPRange& other) const { return !(*this == other); }

  QCPRange &operator+=(const double& value) { lower+=value; upper+=value; return *this; }
  QCPRange &operator-=(const double& value) { lower-=value; upper-=value; return *this; }
  QCPRange &operator*=(const double& value) { lower*=value; upper*=value; return *this; }
  QCPRange &operator/=(const double& value) { lower/=value; upper/=value; return *this; }
  friend inline const QCPRange operator+(const QCPRange&, double);
  friend inline const QCPRange operator+(double, const QCPRange&);
  friend inline const QCPRange operator-(const QCPRange& range, double value);
  friend inline const QCPRange operator*(const QCPRange& range, double value);
  friend inline const QCPRange operator*(double value, const QCPRange& range);
  friend inline const QCPRange operator/(const QCPRange& range, double value);

  double size() const;//大小
  double center() const;//中心点
  void normalize();//规格化
  void expand(const QCPRange &otherRange);//扩张
  QCPRange expanded(const QCPRange &otherRange) const;
  QCPRange sanitizedForLogScale() const;
  QCPRange sanitizedForLinScale() const;
  bool contains(double value) const;

  static bool validRange(double lower, double upper);//有效的范围
  static bool validRange(const QCPRange &range);
  static const double minRange; //1e-280;
  static const double maxRange; //1e280;

};
Q_DECLARE_TYPEINFO(QCPRange, Q_MOVABLE_TYPE);

/* documentation of inline functions */

/*! \fn QCPRange &QCPRange::operator+=(const double& value)

  Adds \a value to both boundaries of the range.
*/

/*! \fn QCPRange &QCPRange::operator-=(const double& value)

  Subtracts \a value from both boundaries of the range.
*/

/*! \fn QCPRange &QCPRange::operator*=(const double& value)

  Multiplies both boundaries of the range by \a value.
*/

/*! \fn QCPRange &QCPRange::operator/=(const double& value)

  Divides both boundaries of the range by \a value.
*/

/* end documentation of inline functions */

/*!
  Adds \a value to both boundaries of the range.
*/
inline const QCPRange operator+(const QCPRange& range, double value)
{
  QCPRange result(range);
  result += value;
  return result;
}

/*!
  Adds \a value to both boundaries of the range.
*/
inline const QCPRange operator+(double value, const QCPRange& range)
{
  QCPRange result(range);
  result += value;
  return result;
}

/*!
  Subtracts \a value from both boundaries of the range.
*/
inline const QCPRange operator-(const QCPRange& range, double value)
{
  QCPRange result(range);
  result -= value;
  return result;
}

/*!
  Multiplies both boundaries of the range by \a value.
*/
inline const QCPRange operator*(const QCPRange& range, double value)
{
  QCPRange result(range);
  result *= value;
  return result;
}

/*!
  Multiplies both boundaries of the range by \a value.
*/
inline const QCPRange operator*(double value, const QCPRange& range)
{
  QCPRange result(range);
  result *= value;
  return result;
}

/*!
  Divides both boundaries of the range by \a value.
*/
inline const QCPRange operator/(const QCPRange& range, double value)
{
  QCPRange result(range);
  result /= value;
  return result;
}


class QCP_LIB_DECL QCPMarginGroup : public QObject
{
  Q_OBJECT
public:
  QCPMarginGroup(QCustomPlot *parentPlot);
  ~QCPMarginGroup();

  // non-virtual methods:
  QList<QCPLayoutElement*> elements(QCP::MarginSide side) const { return mChildren.value(side); }
  bool isEmpty() const;
  void clear();

protected:
  // non-property members:
  QCustomPlot *mParentPlot;
  QHash<QCP::MarginSide, QList<QCPLayoutElement*> > mChildren;

  // non-virtual methods:
  int commonMargin(QCP::MarginSide side) const;
  void addChild(QCP::MarginSide side, QCPLayoutElement *element);
  void removeChild(QCP::MarginSide side, QCPLayoutElement *element);

private:
  Q_DISABLE_COPY(QCPMarginGroup)

  friend class QCPLayoutElement;
};

//QCPLayerable类的子类之1---QCPLayoutElement类
//可布局的元素类，其子类都可通过QCP布局系统，像QT中的布局那样，使它们组织得更有条理
class QCP_LIB_DECL QCPLayoutElement : public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCPLayout* layout READ layout)
  Q_PROPERTY(QRect rect READ rect)
  Q_PROPERTY(QRect outerRect READ outerRect WRITE setOuterRect)
  Q_PROPERTY(QMargins margins READ margins WRITE setMargins)
  Q_PROPERTY(QMargins minimumMargins READ minimumMargins WRITE setMinimumMargins)
  Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
  Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
  /// \endcond
public:
  /*!
    Defines the phases of the update process, that happens just before a replot. At each phase,
    \ref update is called with the according UpdatePhase value.
  */
  enum UpdatePhase { upPreparation ///< Phase used for any type of preparation that needs to be done before margin calculation and layout
                     ,upMargins    ///< Phase in which the margins are calculated and set
                     ,upLayout     ///< Final phase in which the layout system places the rects of the elements
                   };
  Q_ENUMS(UpdatePhase)

  explicit QCPLayoutElement(QCustomPlot *parentPlot=0);
  virtual ~QCPLayoutElement();

  // getters:
  QCPLayout *layout() const { return mParentLayout; }
  QRect rect() const { return mRect; }
  QRect outerRect() const { return mOuterRect; }
  QMargins margins() const { return mMargins; }
  QMargins minimumMargins() const { return mMinimumMargins; }
  QCP::MarginSides autoMargins() const { return mAutoMargins; }
  QSize minimumSize() const { return mMinimumSize; }
  QSize maximumSize() const { return mMaximumSize; }
  QCPMarginGroup *marginGroup(QCP::MarginSide side) const { return mMarginGroups.value(side, (QCPMarginGroup*)0); }
  QHash<QCP::MarginSide, QCPMarginGroup*> marginGroups() const { return mMarginGroups; }

  // setters:
  void setOuterRect(const QRect &rect);
  void setMargins(const QMargins &margins);
  void setMinimumMargins(const QMargins &margins);
  void setAutoMargins(QCP::MarginSides sides);
  void setMinimumSize(const QSize &size);
  void setMinimumSize(int width, int height);
  void setMaximumSize(const QSize &size);
  void setMaximumSize(int width, int height);
  void setMarginGroup(QCP::MarginSides sides, QCPMarginGroup *group);

  // introduced virtual methods:
  virtual void update(UpdatePhase phase);
  virtual QSize minimumSizeHint() const;
  virtual QSize maximumSizeHint() const;
  virtual QList<QCPLayoutElement*> elements(bool recursive) const;

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

protected:
  // property members:
  QCPLayout *mParentLayout;
  QSize mMinimumSize, mMaximumSize;
  QRect mRect, mOuterRect;
  QMargins mMargins, mMinimumMargins;
  QCP::MarginSides mAutoMargins;
  QHash<QCP::MarginSide, QCPMarginGroup*> mMarginGroups;

  // introduced virtual methods:
  virtual int calculateAutoMargin(QCP::MarginSide side);
  // events:
  virtual void mousePressEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void mouseMoveEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void mouseReleaseEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void mouseDoubleClickEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void wheelEvent(QWheelEvent *event) {Q_UNUSED(event)}

  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const { Q_UNUSED(painter) }
  virtual void draw(QCPPainter *painter) { Q_UNUSED(painter) }
  virtual void parentPlotInitialized(QCustomPlot *parentPlot);

private:
  Q_DISABLE_COPY(QCPLayoutElement)

  friend class QCustomPlot;
  friend class QCPLayout;
  friend class QCPMarginGroup;
};

//QCPLayoutElement类的子类QCPLayout类
class QCP_LIB_DECL QCPLayout : public QCPLayoutElement
{
  Q_OBJECT
public:
  explicit QCPLayout();

  // reimplemented virtual methods:
  virtual void update(UpdatePhase phase);
  virtual QList<QCPLayoutElement*> elements(bool recursive) const;

  // introduced virtual methods:
  virtual int elementCount() const = 0;
  virtual QCPLayoutElement* elementAt(int index) const = 0;
  virtual QCPLayoutElement* takeAt(int index) = 0;
  virtual bool take(QCPLayoutElement* element) = 0;
  virtual void simplify();

  // non-virtual methods:
  bool removeAt(int index);
  bool remove(QCPLayoutElement* element);
  void clear();

protected:
  // introduced virtual methods:
  virtual void updateLayout();

  // non-virtual methods:
  void sizeConstraintsChanged() const;
  void adoptElement(QCPLayoutElement *el);
  void releaseElement(QCPLayoutElement *el);
  QVector<int> getSectionSizes(QVector<int> maxSizes, QVector<int> minSizes, QVector<double> stretchFactors, int totalSize) const;

private:
  Q_DISABLE_COPY(QCPLayout)
  friend class QCPLayoutElement;
};


class QCP_LIB_DECL QCPLayoutGrid : public QCPLayout
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(int rowCount READ rowCount)//排计数
  Q_PROPERTY(int columnCount READ columnCount)//列计数
  Q_PROPERTY(QList<double> columnStretchFactors READ columnStretchFactors WRITE setColumnStretchFactors)//列伸展因子
  Q_PROPERTY(QList<double> rowStretchFactors READ rowStretchFactors WRITE setRowStretchFactors)//排伸展因子
  Q_PROPERTY(int columnSpacing READ columnSpacing WRITE setColumnSpacing)//列间距
  Q_PROPERTY(int rowSpacing READ rowSpacing WRITE setRowSpacing)//排间距
  /// \endcond
public:
  explicit QCPLayoutGrid();
  virtual ~QCPLayoutGrid();

  // getters:
  int rowCount() const;//排计数
  int columnCount() const;//列计数
  QList<double> columnStretchFactors() const { return mColumnStretchFactors; }//列伸展因子
  QList<double> rowStretchFactors() const { return mRowStretchFactors; }//排伸展因子
  int columnSpacing() const { return mColumnSpacing; }//列间距
  int rowSpacing() const { return mRowSpacing; }//排间距

  // setters:
  void setColumnStretchFactor(int column, double factor);//设置列伸展因子
  void setColumnStretchFactors(const QList<double> &factors);
  void setRowStretchFactor(int row, double factor);//设置排伸展因子
  void setRowStretchFactors(const QList<double> &factors);
  void setColumnSpacing(int pixels);//设置列间距
  void setRowSpacing(int pixels);//设置排间距

  // reimplemented virtual methods:  重写虚函数
  virtual void updateLayout(); //更新布局
  virtual int elementCount() const;
  virtual QCPLayoutElement* elementAt(int index) const;//元素在
  virtual QCPLayoutElement* takeAt(int index);//元素获取在
  virtual bool take(QCPLayoutElement* element);//获取
  virtual QList<QCPLayoutElement*> elements(bool recursive) const;//元素列表
  virtual void simplify();//简化，全压扁装空元素
  virtual QSize minimumSizeHint() const;//最小化提示
  virtual QSize maximumSizeHint() const;//最大化提示

  // non-virtual methods:非虚方法
  QCPLayoutElement *element(int row, int column) const;//元素
  bool addElement(int row, int column, QCPLayoutElement *element);//增加元素
  bool hasElement(int row, int column);//有元素
  void expandTo(int newRowCount, int newColumnCount);//扩张到哪个元素计数
  void insertRow(int newIndex);//插入元素到排
  void insertColumn(int newIndex);//插入元素到列

protected:
  // property members:
  QList<QList<QCPLayoutElement*> > mElements;//元素列表
  QList<double> mColumnStretchFactors;//列伸展因子
  QList<double> mRowStretchFactors;//排伸展因子
  int mColumnSpacing, mRowSpacing;//列间距，排间距

  // non-virtual methods:非虚方法
  //获取最小化的排、列的尺寸
  void getMinimumRowColSizes(QVector<int> *minColWidths, QVector<int> *minRowHeights) const;
  //获取最大化的排、列的尺寸
  void getMaximumRowColSizes(QVector<int> *maxColWidths, QVector<int> *maxRowHeights) const;

private:
  Q_DISABLE_COPY(QCPLayoutGrid)
};


class QCP_LIB_DECL QCPLayoutInset : public QCPLayout
{
  Q_OBJECT
public:
  /*!
    Defines how the placement and sizing is handled for a certain element in a QCPLayoutInset.
  */
  enum InsetPlacement { ipFree            ///< The element may be positioned/sized arbitrarily, see \ref setInsetRect
                        ,ipBorderAligned  ///< The element is aligned to one of the layout sides, see \ref setInsetAlignment
                      };

  explicit QCPLayoutInset();
  virtual ~QCPLayoutInset();

  // getters:
  InsetPlacement insetPlacement(int index) const;
  Qt::Alignment insetAlignment(int index) const;
  QRectF insetRect(int index) const;

  // setters:
  void setInsetPlacement(int index, InsetPlacement placement);
  void setInsetAlignment(int index, Qt::Alignment alignment);
  void setInsetRect(int index, const QRectF &rect);

  // reimplemented virtual methods:
  virtual void updateLayout();
  virtual int elementCount() const;
  virtual QCPLayoutElement* elementAt(int index) const;
  virtual QCPLayoutElement* takeAt(int index);
  virtual bool take(QCPLayoutElement* element);
  virtual void simplify() {}
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  // non-virtual methods:
  void addElement(QCPLayoutElement *element, Qt::Alignment alignment);
  void addElement(QCPLayoutElement *element, const QRectF &rect);

protected:
  // property members:
  QList<QCPLayoutElement*> mElements;
  QList<InsetPlacement> mInsetPlacement;
  QList<Qt::Alignment> mInsetAlignment;
  QList<QRectF> mInsetRect;

private:
  Q_DISABLE_COPY(QCPLayoutInset)
};


class QCP_LIB_DECL QCPLineEnding
{
  Q_GADGET
public:
  /*!
    Defines the type of ending decoration for line-like items, e.g. an arrow.

    \image html QCPLineEnding.png

    The width and length of these decorations can be controlled with the functions \ref setWidth
    and \ref setLength. Some decorations like \ref esDisc, \ref esSquare, \ref esDiamond and \ref esBar only
    support a width, the length property is ignored.

    \see QCPItemLine::setHead, QCPItemLine::setTail, QCPItemCurve::setHead, QCPItemCurve::setTail, QCPAxis::setLowerEnding, QCPAxis::setUpperEnding
  */
  Q_ENUMS(EndingStyle)
  enum EndingStyle { esNone          ///< No ending decoration
                     ,esFlatArrow    ///< A filled arrow head with a straight/flat back (a triangle)
                     ,esSpikeArrow   ///< A filled arrow head with an indented back
                     ,esLineArrow    ///< A non-filled arrow head with open back
                     ,esDisc         ///< A filled circle
                     ,esSquare       ///< A filled square
                     ,esDiamond      ///< A filled diamond (45° rotated square)
                     ,esBar          ///< A bar perpendicular to the line
                     ,esHalfBar      ///< A bar perpendicular to the line, pointing out to only one side (to which side can be changed with \ref setInverted)
                     ,esSkewedBar    ///< A bar that is skewed (skew controllable via \ref setLength)
                   };

  QCPLineEnding();
  QCPLineEnding(EndingStyle style, double width=8, double length=10, bool inverted=false);

  // getters:
  EndingStyle style() const { return mStyle; }
  double width() const { return mWidth; }
  double length() const { return mLength; }
  bool inverted() const { return mInverted; }

  // setters:
  void setStyle(EndingStyle style);
  void setWidth(double width);
  void setLength(double length);
  void setInverted(bool inverted);

  // non-property methods:
  double boundingDistance() const;
  double realLength() const;
  void draw(QCPPainter *painter, const QVector2D &pos, const QVector2D &dir) const;
  void draw(QCPPainter *painter, const QVector2D &pos, double angle) const;

protected:
  // property members:
  EndingStyle mStyle;
  double mWidth, mLength;
  bool mInverted;
};
Q_DECLARE_TYPEINFO(QCPLineEnding, Q_MOVABLE_TYPE);


//QCPLayerable类的子类之5---QCPGrid类
// 网格类，背景虚线网格
class QCP_LIB_DECL QCPGrid :public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(bool subGridVisible READ subGridVisible WRITE setSubGridVisible)//子网格可见性
  Q_PROPERTY(bool antialiasedSubGrid READ antialiasedSubGrid WRITE setAntialiasedSubGrid)//抗锯齿子网格
  Q_PROPERTY(bool antialiasedZeroLine READ antialiasedZeroLine WRITE setAntialiasedZeroLine)//抗锯齿零线
  Q_PROPERTY(QPen pen READ pen WRITE setPen)//画笔
  Q_PROPERTY(QPen subGridPen READ subGridPen WRITE setSubGridPen)//子网格画笔
  Q_PROPERTY(QPen zeroLinePen READ zeroLinePen WRITE setZeroLinePen)//零线画笔
  /// \endcond
public:
  //构造函数
  QCPGrid(QCPAxis *parentAxis);//轴的指针

  // getters:
  bool subGridVisible() const { return mSubGridVisible; }//子网格可见性
  bool antialiasedSubGrid() const { return mAntialiasedSubGrid; }//抗锯齿子网格
  bool antialiasedZeroLine() const { return mAntialiasedZeroLine; }//抗锯齿零线
  QPen pen() const { return mPen; }//画笔
  QPen subGridPen() const { return mSubGridPen; }//子网格画笔
  QPen zeroLinePen() const { return mZeroLinePen; }//零线画笔

  // setters:
  void setSubGridVisible(bool visible);//设置子网格可见性
  void setAntialiasedSubGrid(bool enabled);//设置抗锯齿子网格
  void setAntialiasedZeroLine(bool enabled);//设置抗锯齿零线
  void setPen(const QPen &pen);//设置画笔
  void setSubGridPen(const QPen &pen);//设置子网格画笔
  void setZeroLinePen(const QPen &pen);//设置零线画笔

protected:
  // property members: 属性成员
  bool mSubGridVisible;//子网格可见性
  bool mAntialiasedSubGrid, mAntialiasedZeroLine;//抗锯齿子网格，抗锯齿零线
  QPen mPen, mSubGridPen, mZeroLinePen;//画笔
  // non-property members:
  QCPAxis *mParentAxis;//轴的指针

  // reimplemented virtual methods:重写虚方法
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;//应用缺省的抗锯齿
  virtual void draw(QCPPainter *painter);//画

  // non-virtual methods: 非虚方法
  void drawGridLines(QCPPainter *painter) const;//画网格线
  void drawSubGridLines(QCPPainter *painter) const;//画子网格线

  friend class QCPAxis;//友元类坐标轴类
};

//QCPLayerable类的子类之2---QCPAxis类
//坐标轴类
class QCP_LIB_DECL QCPAxis : public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(AxisType axisType READ axisType)//轴的类型axisTyp（）
  Q_PROPERTY(QCPAxisRect* axisRect READ axisRect)//获取轴长方形区的指针axisRect()
  Q_PROPERTY(ScaleType scaleType READ scaleType WRITE setScaleType NOTIFY scaleTypeChanged)//刻度类型
  Q_PROPERTY(double scaleLogBase READ scaleLogBase WRITE setScaleLogBase)
  Q_PROPERTY(QCPRange range READ range WRITE setRange NOTIFY rangeChanged)//设置范围
  Q_PROPERTY(bool rangeReversed READ rangeReversed WRITE setRangeReversed)//设置刻度反过来
  Q_PROPERTY(bool autoTicks READ autoTicks WRITE setAutoTicks)//自动刻度
  Q_PROPERTY(int autoTickCount READ autoTickCount WRITE setAutoTickCount)//自动刻度计数
  Q_PROPERTY(bool autoTickLabels READ autoTickLabels WRITE setAutoTickLabels)//自动刻度标签
  Q_PROPERTY(bool autoTickStep READ autoTickStep WRITE setAutoTickStep)//自动刻度梯级
  Q_PROPERTY(bool autoSubTicks READ autoSubTicks WRITE setAutoSubTicks)//自动刻度子刻度
  Q_PROPERTY(bool ticks READ ticks WRITE setTicks)//刻度
  Q_PROPERTY(bool tickLabels READ tickLabels WRITE setTickLabels)//刻度标签
  Q_PROPERTY(int tickLabelPadding READ tickLabelPadding WRITE setTickLabelPadding)//刻度标签填充
  Q_PROPERTY(LabelType tickLabelType READ tickLabelType WRITE setTickLabelType) //刻度标签类型
  Q_PROPERTY(QFont tickLabelFont READ tickLabelFont WRITE setTickLabelFont)//刻度标签字体
  Q_PROPERTY(QColor tickLabelColor READ tickLabelColor WRITE setTickLabelColor)//刻度标签颜色
  Q_PROPERTY(double tickLabelRotation READ tickLabelRotation WRITE setTickLabelRotation)//刻度标签旋转
  Q_PROPERTY(LabelSide tickLabelSide READ tickLabelSide WRITE setTickLabelSide)//刻度标签侧面
  Q_PROPERTY(QString dateTimeFormat READ dateTimeFormat WRITE setDateTimeFormat)//日期时间格式
  Q_PROPERTY(Qt::TimeSpec dateTimeSpec READ dateTimeSpec WRITE setDateTimeSpec)//日期时间规格
  Q_PROPERTY(QString numberFormat READ numberFormat WRITE setNumberFormat)//数字格式
  Q_PROPERTY(int numberPrecision READ numberPrecision WRITE setNumberPrecision)//数字精度
  Q_PROPERTY(double tickStep READ tickStep WRITE setTickStep)//台阶记号
  Q_PROPERTY(QVector<double> tickVector READ tickVector WRITE setTickVector)//矢量记号
  Q_PROPERTY(QVector<QString> tickVectorLabels READ tickVectorLabels WRITE setTickVectorLabels)//矢量记号标签
  Q_PROPERTY(int tickLengthIn READ tickLengthIn WRITE setTickLengthIn)//内侧的刻度长度
  Q_PROPERTY(int tickLengthOut READ tickLengthOut WRITE setTickLengthOut)//内侧的刻度长度
  Q_PROPERTY(int subTickCount READ subTickCount WRITE setSubTickCount)//子刻度计数
  Q_PROPERTY(int subTickLengthIn READ subTickLengthIn WRITE setSubTickLengthIn)//内侧的子刻度长度
  Q_PROPERTY(int subTickLengthOut READ subTickLengthOut WRITE setSubTickLengthOut)//外侧的子刻度长度
  Q_PROPERTY(QPen basePen READ basePen WRITE setBasePen)//基本画笔
  Q_PROPERTY(QPen tickPen READ tickPen WRITE setTickPen)//刻度用画笔
  Q_PROPERTY(QPen subTickPen READ subTickPen WRITE setSubTickPen)//子刻度用画笔
  Q_PROPERTY(QFont labelFont READ labelFont WRITE setLabelFont)//标签字体
  Q_PROPERTY(QColor labelColor READ labelColor WRITE setLabelColor)//标签颜色
  Q_PROPERTY(QString label READ label WRITE setLabel) //标签
  Q_PROPERTY(int labelPadding READ labelPadding WRITE setLabelPadding)//标签填充
  Q_PROPERTY(int padding READ padding WRITE setPadding)//填充,衬垫
  Q_PROPERTY(int offset READ offset WRITE setOffset)//偏移量
  Q_PROPERTY(SelectableParts selectedParts READ selectedParts WRITE setSelectedParts NOTIFY selectionChanged)//可选择的部分
  Q_PROPERTY(SelectableParts selectableParts READ selectableParts WRITE setSelectableParts NOTIFY selectableChanged)
  Q_PROPERTY(QFont selectedTickLabelFont READ selectedTickLabelFont WRITE setSelectedTickLabelFont) //选择的刻度标签字体
  Q_PROPERTY(QFont selectedLabelFont READ selectedLabelFont WRITE setSelectedLabelFont)//选择的标签字体
  Q_PROPERTY(QColor selectedTickLabelColor READ selectedTickLabelColor WRITE setSelectedTickLabelColor)//选择的刻度标签颜色
  Q_PROPERTY(QColor selectedLabelColor READ selectedLabelColor WRITE setSelectedLabelColor)//选择的标签颜色
  Q_PROPERTY(QPen selectedBasePen READ selectedBasePen WRITE setSelectedBasePen)//选择的基本画笔
  Q_PROPERTY(QPen selectedTickPen READ selectedTickPen WRITE setSelectedTickPen)//选择的刻度画笔
  Q_PROPERTY(QPen selectedSubTickPen READ selectedSubTickPen WRITE setSelectedSubTickPen)//选择的子刻度画笔
  Q_PROPERTY(QCPLineEnding lowerEnding READ lowerEnding WRITE setLowerEnding)//下限
  Q_PROPERTY(QCPLineEnding upperEnding READ upperEnding WRITE setUpperEnding)//上限
  Q_PROPERTY(QCPGrid* grid READ grid)//网格
  /// \endcond
public:
  /*!
    Defines at which side of the axis rect the axis will appear. This also affects how the tick
    marks are drawn, on which side the labels are placed etc.
  */
  enum AxisType { atLeft    = 0x01  ///< <tt>0x01</tt> Axis is vertical and on the left side of the axis rect 左边轴
                  ,atRight  = 0x02  ///< <tt>0x02</tt> Axis is vertical and on the right side of the axis rect 右边轴
                  ,atTop    = 0x04  ///< <tt>0x04</tt> Axis is horizontal and on the top side of the axis rect 上边轴
                  ,atBottom = 0x08  ///< <tt>0x08</tt> Axis is horizontal and on the bottom side of the axis rect 下边轴
                };
  Q_FLAGS(AxisType AxisTypes)
  Q_DECLARE_FLAGS(AxisTypes, AxisType)
  /*!
    When automatic tick label generation is enabled (\ref setAutoTickLabels), defines how the
    coordinate of the tick is interpreted, i.e. translated into a string.

    \see setTickLabelType
  */
  enum LabelType { ltNumber    ///< Tick coordinate is regarded as normal number and will be displayed as such. (see \ref setNumberFormat)
                   ,ltDateTime ///< Tick coordinate is regarded as a date/time (seconds since 1970-01-01T00:00:00 UTC) and will be displayed and formatted as such. (for details, see \ref setDateTimeFormat)
                 };
  Q_ENUMS(LabelType)
  /*!
    Defines on which side of the axis the tick labels (numbers) shall appear.

    \see setTickLabelSide
  */
  enum LabelSide { lsInside    ///< Tick labels will be displayed inside the axis rect and clipped to the inner axis rect  标签在内侧
                   ,lsOutside  ///< Tick labels will be displayed outside the axis rect  标签在外侧
                 };
  Q_ENUMS(LabelSide)
  /*!
    Defines the scale of an axis.
    \see setScaleType
  */
  enum ScaleType { stLinear       ///< Linear scaling  直线型
                   ,stLogarithmic ///< Logarithmic scaling with correspondingly transformed plots and (major) tick marks at every base power (see \ref setScaleLogBase).对数型
                 };
  Q_ENUMS(ScaleType)
  /*!
    Defines the selectable parts of an axis.
    \see setSelectableParts, setSelectedParts
  */
  enum SelectablePart { spNone        = 0      ///< None of the selectable parts  没有可选部分
                        ,spAxis       = 0x001  ///< The axis backbone and tick marks 轴的骨干
                        ,spTickLabels = 0x002  ///< Tick labels (numbers) of this axis (as a whole, not individually) 轴的刻度标签
                        ,spAxisLabel  = 0x004  ///< The axis label 轴的标签
                      };
  Q_FLAGS(SelectablePart SelectableParts)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)

  //构造函数
  //定义为不可隐式转换，只能显式的强制转换构造
  explicit QCPAxis(QCPAxisRect *parent, AxisType type);//包括轴长方形区的指针和轴的类型
  virtual ~QCPAxis();//析构函数

  // getters:
  AxisType axisType() const { return mAxisType; }//轴类型
  QCPAxisRect *axisRect() const { return mAxisRect; }//轴长方形区的指针
  ScaleType scaleType() const { return mScaleType; }//刻度类型
  double scaleLogBase() const { return mScaleLogBase; }//对数刻度类型
  const QCPRange range() const { return mRange; }//范围
  bool rangeReversed() const { return mRangeReversed; }//反转的范围
  bool autoTicks() const { return mAutoTicks; } //自动刻度
  int autoTickCount() const { return mAutoTickCount; }//自动刻度计数
  bool autoTickLabels() const { return mAutoTickLabels; }//自动刻度标签
  bool autoTickStep() const { return mAutoTickStep; }//自动刻度台阶
  bool autoSubTicks() const { return mAutoSubTicks; }//自动子刻度
  bool ticks() const { return mTicks; }//刻度
  bool tickLabels() const { return mTickLabels; } //刻度标签
  int tickLabelPadding() const;//刻度标签衬垫
  LabelType tickLabelType() const { return mTickLabelType; }//刻度标签类型
  QFont tickLabelFont() const { return mTickLabelFont; }//刻度标签字体
  QColor tickLabelColor() const { return mTickLabelColor; }//刻度标签颜色
  //在这里从界面设置获取刻度标签的位移量（单位为象素），再在主类中调用私有类指针将位移量传输到私有类中
  // !!!!!这个特别重要！！！！
  //QCPAxisPainterPrivate *mAxisPainter;//画轴的父类指针
  double getXAxisTickLabeloffset_x() const
  {
      return m_XAxisTickLabeloffset_x;
  }
  double getXAxisTickLabeloffset_y() const
  {
      return m_XAxisTickLabeloffset_y;
  }
  double getYAxisTickLabeloffset_x() const
  {
      return m_YAxisTickLabeloffset_x;
  }
  double getYAxisTickLabeloffset_y() const
  {
      return m_YAxisTickLabeloffset_y;
  }

  double tickLabelRotation() const;//刻度标签颜色旋转
  LabelSide tickLabelSide() const;//刻度标签在哪边
  QString dateTimeFormat() const { return mDateTimeFormat; }//日期时间格式
  Qt::TimeSpec dateTimeSpec() const { return mDateTimeSpec; }//日期时间规格
  QString numberFormat() const;//数字格式
  int numberPrecision() const { return mNumberPrecision; }//数字精度
  double tickStep() const { return mTickStep; }//刻度台阶
  QVector<double> tickVector() const { return mTickVector; }//刻度矢量
  QVector<QString> tickVectorLabels() const { return mTickVectorLabels; }//刻度矢量标签
  int tickLengthIn() const;//内侧的刻度长度
  int tickLengthOut() const;//外侧的刻度长度
  int subTickCount() const { return mSubTickCount; }//子刻度计数
  int subTickLengthIn() const;//内侧的子刻度长度
  int subTickLengthOut() const;//外侧的子刻度长度
  QPen basePen() const { return mBasePen; }//基本画笔
  QPen tickPen() const { return mTickPen; }//刻度的画笔
  QPen subTickPen() const { return mSubTickPen; }//子刻度的画笔
  QFont labelFont() const { return mLabelFont; } //标签字体
  QColor labelColor() const { return mLabelColor; }//标签颜色
  QString label() const { return mLabel; }//标签
  int labelPadding() const;//标签衬垫
  int padding() const { return mPadding; }//衬垫
  int offset() const;//位移
  SelectableParts selectedParts() const { return mSelectedParts; }//可选择的部分
  SelectableParts selectableParts() const { return mSelectableParts; }
  QFont selectedTickLabelFont() const { return mSelectedTickLabelFont; }//选择的刻度标签字体
  QFont selectedLabelFont() const { return mSelectedLabelFont; }//选择的标签字体
  QColor selectedTickLabelColor() const { return mSelectedTickLabelColor; }//选择的刻度标签颜色
  QColor selectedLabelColor() const { return mSelectedLabelColor; }//选择的标签颜色
  QPen selectedBasePen() const { return mSelectedBasePen; }//选择的基本画笔
  QPen selectedTickPen() const { return mSelectedTickPen; }//选择的刻度画笔
  QPen selectedSubTickPen() const { return mSelectedSubTickPen; }//选择的子刻度画笔
  QCPLineEnding lowerEnding() const;//下限
  QCPLineEnding upperEnding() const;//上限
  QCPGrid *grid() const { return mGrid; }//网格

  // setters:
  Q_SLOT void setScaleType(QCPAxis::ScaleType type);//设置刻度类型
  void setScaleLogBase(double base);//设置对数刻度类型
  Q_SLOT void setRange(const QCPRange &range);//设置范围
  void setRange(double lower, double upper);
  void setRange(double position, double size, Qt::AlignmentFlag alignment);
  void setRangeLower(double lower);
  void setRangeUpper(double upper);
  void setRangeReversed(bool reversed);
  void setAutoTicks(bool on); //设置自动刻度
  void setAutoTickCount(int approximateCount);//设置自动刻度计数
  void setAutoTickLabels(bool on);//设置自动刻度标签
  void setAutoTickStep(bool on);//设置自动刻度台阶
  void setAutoSubTicks(bool on);//设置自动子刻度标签
  void setTicks(bool show);//设置刻度
  void setTickLabels(bool show);//设置刻度标签
  void setTickLabelPadding(int padding);//设置刻度标签衬垫
  void setTickLabelType(LabelType type);//设置刻度标签类型
  void setTickLabelFont(const QFont &font);//设置刻度标签字体
  void setTickLabelColor(const QColor &color);//设置刻度标签颜色
  //设置刻度标签位移（单位为象素）
  //在这里从界面设置获取刻度标签的位移量（单位为象素），再在主类中调用私有类指针将位移量传输到私有类中
  // !!!!!这个特别重要！！！！
  //QCPAxisPainterPrivate *mAxisPainter;//画轴的父类指针
  void setXAxisTickLabeloffset_x(double X_x,qint32 numxofall_x);
  void setXAxisTickLabeloffset_y(double X_y,qint32 numxofall_y);
  void setYAxisTickLabeloffset_x(double Y_x,qint32 numyofall_x);
  void setYAxisTickLabeloffset_y(double Y_y,qint32 numyofall_y);

  void setTickLabelRotation(double degrees);//设置刻度标签旋转
  void setTickLabelSide(LabelSide side);//设置刻度标签在哪边
  void setDateTimeFormat(const QString &format);//设置日期时间格式
  void setDateTimeSpec(const Qt::TimeSpec &timeSpec);//设置日期时间规格
  void setNumberFormat(const QString &formatCode); //设置数字格式
  void setNumberPrecision(int precision);//设置数字精度
  void setTickStep(double step); //设置刻度间距
  void setTickVector(const QVector<double> &vec);//设置刻度矢量
  void setTickVectorLabels(const QVector<QString> &vec);//设置刻度矢量标签
  void setTickLength(int inside, int outside=0);//设置刻度长度
  void setTickLengthIn(int inside);
  void setTickLengthOut(int outside);
  void setSubTickCount(int count);//设置子刻度长度
  void setSubTickLength(int inside, int outside=0);
  void setSubTickLengthIn(int inside);
  void setSubTickLengthOut(int outside);
  void setBasePen(const QPen &pen);//设置基本画笔
  void setTickPen(const QPen &pen);//设置刻度画笔
  void setSubTickPen(const QPen &pen);//设置子刻度画笔
  void setLabelFont(const QFont &font);//设置标签字体
  void setLabelColor(const QColor &color);//设置标签颜色
  void setLabel(const QString &str);//设置标签
  void setLabelPadding(int padding);//设置标签衬垫
  void setPadding(int padding); //设置衬垫
  void setOffset(int offset);//设置位移量
  void setSelectedTickLabelFont(const QFont &font);//设置选择的刻度标签字体
  void setSelectedLabelFont(const QFont &font);//设置选择的标签字体
  void setSelectedTickLabelColor(const QColor &color);//设置选择的刻度标签颜色
  void setSelectedLabelColor(const QColor &color);//设置选择的标签颜色
  void setSelectedBasePen(const QPen &pen);//设置选择的基本画笔
  void setSelectedTickPen(const QPen &pen);//设置选择的刻度画笔
  void setSelectedSubTickPen(const QPen &pen);//设置选择的子刻度画笔
  Q_SLOT void setSelectableParts(const QCPAxis::SelectableParts &selectableParts);//设置选择的部分
  Q_SLOT void setSelectedParts(const QCPAxis::SelectableParts &selectedParts);
  void setLowerEnding(const QCPLineEnding &ending);//设置下限
  void setUpperEnding(const QCPLineEnding &ending);//设置上限

  // reimplemented virtual methods: 重载的虚方法
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  // non-property methods:   非属性的方法
  Qt::Orientation orientation() const { return mOrientation; }//方向
  void moveRange(double diff);//移动范围
  void scaleRange(double factor, double center);//刻度范围
  void setScaleRatio(const QCPAxis *otherAxis, double ratio=1.0);//设置刻度比率
  void rescale(bool onlyVisiblePlottables=false); // 重新调节
  double pixelToCoord(double value) const;//象素到坐标
  double coordToPixel(double value) const;//坐标到象素
  SelectablePart getPartAt(const QPointF &pos) const;//获取选择的部分的坐标
  QList<QCPAbstractPlottable*> plottables() const;//画的图表的指针列表
  QList<QCPGraph*> graphs() const;//画的曲线的指针列表
  QList<QCPAbstractItem*> items() const;//项目的指针列表

  //哪一边的轴的类型
  static AxisType marginSideToAxisType(QCP::MarginSide side);
  //轴的方向
  static Qt::Orientation orientation(AxisType type) { return type==atBottom||type==atTop ? Qt::Horizontal : Qt::Vertical; }
  //相反的轴的类型
  static AxisType opposite(AxisType type);

signals:
  void ticksRequest();//刻度请求
  void rangeChanged(const QCPRange &newRange);//范围改变
  void rangeChanged(const QCPRange &newRange, const QCPRange &oldRange);
  void scaleTypeChanged(QCPAxis::ScaleType scaleType);//刻度类型改变
  void selectionChanged(const QCPAxis::SelectableParts &parts); //选择的部分的改变
  void selectableChanged(const QCPAxis::SelectableParts &parts);

protected:
  // property members:
  // axis base:
  AxisType mAxisType;//轴类型
  QCPAxisRect *mAxisRect;//轴长方形区的指针
  //int mOffset; // in QCPAxisPainter
  int mPadding;//衬垫
  Qt::Orientation mOrientation;//轴的方向
  SelectableParts mSelectableParts, mSelectedParts;//选择的部分
  QPen mBasePen, mSelectedBasePen;//设置画笔
  //QCPLineEnding mLowerEnding, mUpperEnding; // in QCPAxisPainter
  // axis label:
  //int mLabelPadding; // in QCPAxisPainter
  QString mLabel;//标签
  QFont mLabelFont, mSelectedLabelFont;//标签字体
  QColor mLabelColor, mSelectedLabelColor;//选择的标签颜色
  // tick labels:
  //int mTickLabelPadding; // in QCPAxisPainter
  bool mTickLabels, mAutoTickLabels;//刻度标签，自动刻度标签
  //double mTickLabelRotation; // in QCPAxisPainter
  LabelType mTickLabelType;//刻度标签类型
  QFont mTickLabelFont, mSelectedTickLabelFont;//刻度标签字体，选择的刻度标签字体
  QColor mTickLabelColor, mSelectedTickLabelColor;//刻度标签颜色，选择的刻度标签颜色
  double m_XAxisTickLabeloffset_x;//存放设置的刻度标签位移量
  double m_XAxisTickLabeloffset_y;
  double m_YAxisTickLabeloffset_x;
  double m_YAxisTickLabeloffset_y;
  qint32 m_numxofall_x;//存放获得设置刻度标签数字的有效位数，以对刻度文本偏移的自动设置提供帮助
  qint32 m_numxofall_y;
  qint32 m_numyofall_x;
  qint32 m_numyofall_y;
  QString mDateTimeFormat;//日期时间格式
  Qt::TimeSpec mDateTimeSpec;//日期时间规格
  int mNumberPrecision;//数字精度
  QLatin1Char mNumberFormatChar;//数字格式字符
  bool mNumberBeautifulPowers;//数字的漂亮开关
  //bool mNumberMultiplyCross; // QCPAxisPainter
  // ticks and subticks:
  bool mTicks; //刻度
  double mTickStep;//刻度间距（比例尺）
  int mSubTickCount, mAutoTickCount;//子刻度计数，自动刻度计数
  bool mAutoTicks, mAutoTickStep, mAutoSubTicks;//自动刻度，自动刻度台阶，自动子刻度
  //int mTickLengthIn, mTickLengthOut, mSubTickLengthIn, mSubTickLengthOut; // QCPAxisPainter
  QPen mTickPen, mSelectedTickPen;//刻度画笔，选择的刻度画笔
  QPen mSubTickPen, mSelectedSubTickPen;//子刻度画笔，选择的子刻度画笔
  // scale and range:
  QCPRange mRange;//范围
  bool mRangeReversed;//反转的范围
  ScaleType mScaleType;//刻度类型
  double mScaleLogBase, mScaleLogBaseLogInv;//基本的对数刻度，

  // non-property members:没有属性化的成员
  QCPGrid *mGrid;//网格
  QCPAxisPainterPrivate *mAxisPainter;//画轴的父类指针
  int mLowestVisibleTick, mHighestVisibleTick;//最低的可见刻度，最高的可见刻度
  QVector<double> mTickVector;//刻度容器
  QVector<QString> mTickVectorLabels;//刻度容器标签
  QVector<double> mSubTickVector;//子刻度容器标签
  bool mCachedMarginValid;//页边隐藏的有效性
  int mCachedMargin;//页边隐藏

  // introduced virtual methods:   传入的虚方法
  virtual void setupTickVectors(); //安装刻度容器
  virtual void generateAutoTicks();//产生自动刻度
  virtual int calculateAutoSubTickCount(double tickStep) const;//计算自动子刻度计数
  virtual int calculateMargin();//计算页边

  // reimplemented virtual methods: 重载的虚方法
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;//应用缺省的抗锯齿
  virtual void draw(QCPPainter *painter);//画
  virtual QCP::Interaction selectionCategory() const;//交互性
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);//选择事件
  virtual void deselectEvent(bool *selectionStateChanged);//取消选择事件

  // non-virtual methods: 非虚方法
  void visibleTickBounds(int &lowIndex, int &highIndex) const;//可见的刻度范围：最小索引、最大索引
  double baseLog(double value) const;//基本对数
  double basePow(double value) const;//幂函数
  QPen getBasePen() const;//获取基本画笔
  QPen getTickPen() const;//获取刻度画笔
  QPen getSubTickPen() const;//获取子刻度画笔
  QFont getTickLabelFont() const;//获取刻度标签字体
  QFont getLabelFont() const;//获取标签字体
  QColor getTickLabelColor() const;//获取刻度标签颜色
  QColor getLabelColor() const;//获取标签颜色

private:
  Q_DISABLE_COPY(QCPAxis)

  friend class QCustomPlot;//友元类QCustomPlot（画图工具的基本类）
  friend class QCPGrid;//友元类网格类
  friend class QCPAxisRect; //友元类轴长方区类
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCPAxis::SelectableParts)//刻度的操作声明：选择的哪部分
Q_DECLARE_OPERATORS_FOR_FLAGS(QCPAxis::AxisTypes)//刻度的操作声明：轴的类型
Q_DECLARE_METATYPE(QCPAxis::SelectablePart)//刻度的伴型声明：选择的哪部分

//轴的私有画图类
class QCPAxisPainterPrivate
{
public:
  //单参数的构造建议加上关键字explicit，不允许隐式的转换，避免二意义性
  explicit QCPAxisPainterPrivate(QCustomPlot *parentPlot);
  virtual ~QCPAxisPainterPrivate();

  virtual void draw(QCPPainter *painter);
  virtual int size() const;//尺寸
  void clearCache();//清缓冲

  QRect axisSelectionBox() const { return mAxisSelectionBox; }//轴的选择框
  QRect tickLabelsSelectionBox() const { return mTickLabelsSelectionBox; }//刻度的选择框
  QRect labelSelectionBox() const { return mLabelSelectionBox; }//标签的选择框

  // public property members:
  QCPAxis::AxisType type;//坐标类型
  QPen basePen;//基本画笔
  QCPLineEnding lowerEnding, upperEnding; // directly accessed by QCPAxis setters/getters 下限，上限
  int labelPadding; // directly accessed by QCPAxis setters/getters 标签衬垫
  QFont labelFont; //标签字体
  QColor labelColor;//标签颜色
  QString label;//标签
  int tickLabelPadding; // directly accessed by QCPAxis setters/getters 刻度标签衬垫
  double tickLabelRotation; // directly accessed by QCPAxis setters/getters  刻度标签旋转
  QCPAxis::LabelSide tickLabelSide; // directly accessed by QCPAxis setters/getters 刻度标签在哪边
  bool substituteExponent;//子stitute的解释者
  bool numberMultiplyCross; // directly accessed by QCPAxis setters/getters 数字交叉增加
  int tickLengthIn, tickLengthOut, subTickLengthIn, subTickLengthOut; // directly accessed by QCPAxis setters/getters
  QPen tickPen, subTickPen; //刻度画笔，子刻度画笔
  QFont tickLabelFont;//刻度标签字体
  QColor tickLabelColor;//刻度标签颜色
  QRect axisRect, viewportRect;//轴长方形区，视口区
  double offset; // directly accessed by QCPAxis setters/getters 位移量
  bool abbreviateDecimalPowers;//简化的十进制幂函数
  bool reversedEndings;//反转终点

  QVector<double> subTickPositions;//子刻度坐标容器
  QVector<double> tickPositions;//刻度坐标容器
  QVector<QString> tickLabels;//刻度标签容器

  double m_XAxisTickLabeloffset_x;
  double m_XAxisTickLabeloffset_y;
  double m_YAxisTickLabeloffset_x;
  double m_YAxisTickLabeloffset_y;

protected:
  struct CachedLabel//隐藏的标签
  {
    QPointF offset;
    QPixmap pixmap;
  };
  struct TickLabelData //刻度标签数据
  {
    QString basePart, expPart;
    QRect baseBounds, expBounds, totalBounds, rotatedTotalBounds;
    QFont baseFont, expFont;
  };
  QCustomPlot *mParentPlot;//画图工具类的父指针
  //决定是否需要预期改变标签参数的缓冲
  QByteArray mLabelParameterHash; // to determine whether mLabelCache needs to be cleared due to changed parameters
  QCache<QString, CachedLabel> mLabelCache;
  QRect mAxisSelectionBox, mTickLabelsSelectionBox, mLabelSelectionBox;//轴的选择框，刻度标签的选择框，标签选择框

  virtual QByteArray generateLabelParameterHash() const;//通常的标签参数缓存
  //放置刻度标签
  virtual void placeTickLabel(QCPPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize);
  //放置刻度标签 zjb add :在这里根据不同的轴计算出不同的刻度标签位移量 2015-10-23
  virtual void placeTickLabel(QCPPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize,
          double XAxisTickLabeloffset_x,double XAxisTickLabeloffset_y,double YAxisTickLabeloffset_x,double YAxisTickLabeloffset_y);
  //绘制刻度标签
  virtual void drawTickLabel(QCPPainter *painter, double x, double y, const TickLabelData &labelData) const;
  //刻度标签数据
  virtual TickLabelData getTickLabelData(const QFont &font, const QString &text) const;
  //获取标志标签的绘制位移量
  virtual QPointF getTickLabelDrawOffset(const TickLabelData &labelData) const;
  //获取最大的标志标签尺寸
  virtual void getMaxTickLabelSize(const QFont &font, const QString &text, QSize *tickLabelsSize) const;
};

//QCPLayerable类的子类之3---QCPAbstractPlottable类
//抽象类，其子类是可绘制的图形（表示数据系列的图线）
class QCP_LIB_DECL QCPAbstractPlottable : public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString name READ name WRITE setName)//名称
  Q_PROPERTY(bool antialiasedFill READ antialiasedFill WRITE setAntialiasedFill)//抗锯齿的填充
  Q_PROPERTY(bool antialiasedScatters READ antialiasedScatters WRITE setAntialiasedScatters)//抗锯齿的散点
  Q_PROPERTY(bool antialiasedErrorBars READ antialiasedErrorBars WRITE setAntialiasedErrorBars)//抗锯齿的误差条
  Q_PROPERTY(QPen pen READ pen WRITE setPen)//画笔
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)//选择的画笔
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)//画刷
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)//选择的画刷
  Q_PROPERTY(QCPAxis* keyAxis READ keyAxis WRITE setKeyAxis)//坐标轴类指针
  Q_PROPERTY(QCPAxis* valueAxis READ valueAxis WRITE setValueAxis)//坐标轴类指针
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)//选择
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectionChanged)//被选择
  /// \endcond
public:
  //构造函数
  QCPAbstractPlottable(QCPAxis *keyAxis, QCPAxis *valueAxis);

  // getters:
  QString name() const { return mName; }//名称
  bool antialiasedFill() const { return mAntialiasedFill; }//抗锯齿的填充
  bool antialiasedScatters() const { return mAntialiasedScatters; }//抗锯齿的散点
  bool antialiasedErrorBars() const { return mAntialiasedErrorBars; }//抗锯齿的误差条
  QPen pen() const { return mPen; }//画笔
  QPen selectedPen() const { return mSelectedPen; }//选择的画笔
  QBrush brush() const { return mBrush; }//画刷
  QBrush selectedBrush() const { return mSelectedBrush; }//选择的画刷
  QCPAxis *keyAxis() const { return mKeyAxis.data(); }//坐标轴类指针
  QCPAxis *valueAxis() const { return mValueAxis.data(); }//坐标轴类指针
  bool selectable() const { return mSelectable; }//选择
  bool selected() const { return mSelected; }//被选择

  // setters:
  void setName(const QString &name);//设置名称
  void setAntialiasedFill(bool enabled);//设置抗锯齿的填充
  void setAntialiasedScatters(bool enabled);//设置抗锯齿的散点
  void setAntialiasedErrorBars(bool enabled);//设置抗锯齿的误差条
  void setPen(const QPen &pen);//设置画笔
  void setSelectedPen(const QPen &pen);//设置选择的画笔
  void setBrush(const QBrush &brush);//设置画刷
  void setSelectedBrush(const QBrush &brush);//设置选择的画刷
  void setKeyAxis(QCPAxis *axis);//设置坐标轴类指针
  void setValueAxis(QCPAxis *axis);//设置坐标轴类指针
  Q_SLOT void setSelectable(bool selectable);//设置选择
  Q_SLOT void setSelected(bool selected);//设置被选择

  // introduced virtual methods:
  virtual void clearData() = 0;//清数据，纯虚函数
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const = 0;
  virtual bool addToLegend();//增加到图例
  virtual bool removeFromLegend() const;//从图例中移除

  // non-property methods:非属性的方法
  void rescaleAxes(bool onlyEnlarge=false) const;//缩放相关
  void rescaleKeyAxis(bool onlyEnlarge=false) const;
  void rescaleValueAxis(bool onlyEnlarge=false) const;

signals:
  void selectionChanged(bool selected);//选择状态发生改变
  void selectableChanged(bool selectable);//被选择的状态发生改变

protected:
  /*!
    Represents negative and positive sign domain for passing to \ref getKeyRange and \ref getValueRange.
  */
  enum SignDomain { sdNegative  ///< The negative sign domain, i.e. numbers smaller than zero 负的
                    ,sdBoth     ///< Both sign domains, including zero, i.e. all (rational) numbers 两边
                    ,sdPositive ///< The positive sign domain, i.e. numbers greater than zero 正的
                  };

  // property members:
  QString mName;//设置名称
  bool mAntialiasedFill, mAntialiasedScatters, mAntialiasedErrorBars;//设置抗锯齿的填充
  QPen mPen, mSelectedPen;//设置画笔
  QBrush mBrush, mSelectedBrush;//设置画刷
  QPointer<QCPAxis> mKeyAxis, mValueAxis;//设置坐标轴类指针
  bool mSelectable, mSelected;//设置选择，被选择

  // reimplemented virtual methods: 重写虚方法
  virtual QRect clipRect() const;//夹住的区域
  virtual void draw(QCPPainter *painter) = 0;//画
  virtual QCP::Interaction selectionCategory() const;//选择的种类
  void applyDefaultAntialiasingHint(QCPPainter *painter) const;//应用缺省的抗锯齿
  // events:
  //选择事件
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  //取消选择事件
  virtual void deselectEvent(bool *selectionStateChanged);

  // introduced virtual methods:
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const = 0;//画图例图标
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const = 0;//获取键的范围
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const = 0;//获取值的范围

  // non-virtual methods: 非虚方法
  void coordsToPixels(double key, double value, double &x, double &y) const;//坐标到象素
  const QPointF coordsToPixels(double key, double value) const;
  void pixelsToCoords(double x, double y, double &key, double &value) const;//象素到坐标
  void pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const;
  QPen mainPen() const;//主要画笔
  QBrush mainBrush() const;//主要画刷
  void applyFillAntialiasingHint(QCPPainter *painter) const;//应用填充抗锯齿
  void applyScattersAntialiasingHint(QCPPainter *painter) const;//应用散点抗锯齿
  void applyErrorBarsAntialiasingHint(QCPPainter *painter) const;//应用误差条抗锯齿
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;//

private:
  Q_DISABLE_COPY(QCPAbstractPlottable)

  friend class QCustomPlot;//友元类画图工具类的基本类
  friend class QCPAxis;//友元类坐标轴类
  friend class QCPPlottableLegendItem;//友元类图表图例项目类
};


class QCP_LIB_DECL QCPItemAnchor
{
public:
  QCPItemAnchor(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name, int anchorId=-1);
  virtual ~QCPItemAnchor();

  // getters:
  QString name() const { return mName; }
  virtual QPointF pixelPoint() const;

protected:
  // property members:
  QString mName;

  // non-property members:
  QCustomPlot *mParentPlot;
  QCPAbstractItem *mParentItem;
  int mAnchorId;
  QSet<QCPItemPosition*> mChildrenX, mChildrenY;

  // introduced virtual methods:
  virtual QCPItemPosition *toQCPItemPosition() { return 0; }

  // non-virtual methods:
  void addChildX(QCPItemPosition* pos); // called from pos when this anchor is set as parent
  void removeChildX(QCPItemPosition *pos); // called from pos when its parent anchor is reset or pos deleted
  void addChildY(QCPItemPosition* pos); // called from pos when this anchor is set as parent
  void removeChildY(QCPItemPosition *pos); // called from pos when its parent anchor is reset or pos deleted

private:
  Q_DISABLE_COPY(QCPItemAnchor)

  friend class QCPItemPosition;
};


//Position位置，Anchor锚定
//用于与鼠标、键盘定位相关的类
class QCP_LIB_DECL QCPItemPosition : public QCPItemAnchor
{
public:
  /*!
    Defines the ways an item position can be specified. Thus it defines what the numbers passed to
    \ref setCoords actually mean.

    \see setType
  */
  enum PositionType { ptAbsolute        ///< Static positioning in pixels, starting from the top left corner of the viewport/widget.
                      ,ptViewportRatio  ///< Static positioning given by a fraction of the viewport size. For example, if you call setCoords(0, 0), the position will be at the top
                                        ///< left corner of the viewport/widget. setCoords(1, 1) will be at the bottom right corner, setCoords(0.5, 0) will be horizontally centered and
                                        ///< vertically at the top of the viewport/widget, etc.
                      ,ptAxisRectRatio  ///< Static positioning given by a fraction of the axis rect size (see \ref setAxisRect). For example, if you call setCoords(0, 0), the position will be at the top
                                        ///< left corner of the axis rect. setCoords(1, 1) will be at the bottom right corner, setCoords(0.5, 0) will be horizontally centered and
                                        ///< vertically at the top of the axis rect, etc. You can also go beyond the axis rect by providing negative coordinates or coordinates larger than 1.
                      ,ptPlotCoords     ///< Dynamic positioning at a plot coordinate defined by two axes (see \ref setAxes).
                    };

  QCPItemPosition(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name);
  virtual ~QCPItemPosition();

  // getters:
  PositionType type() const { return typeX(); }
  PositionType typeX() const { return mPositionTypeX; }
  PositionType typeY() const { return mPositionTypeY; }
  QCPItemAnchor *parentAnchor() const { return parentAnchorX(); }
  QCPItemAnchor *parentAnchorX() const { return mParentAnchorX; }
  QCPItemAnchor *parentAnchorY() const { return mParentAnchorY; }
  double key() const { return mKey; }
  double value() const { return mValue; }
  QPointF coords() const { return QPointF(mKey, mValue); }
  QCPAxis *keyAxis() const { return mKeyAxis.data(); }
  QCPAxis *valueAxis() const { return mValueAxis.data(); }
  QCPAxisRect *axisRect() const;
  virtual QPointF pixelPoint() const;

  // setters:
  void setType(PositionType type);
  void setTypeX(PositionType type);
  void setTypeY(PositionType type);
  bool setParentAnchor(QCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  bool setParentAnchorX(QCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  bool setParentAnchorY(QCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  void setCoords(double key, double value);
  void setCoords(const QPointF &coords);
  void setAxes(QCPAxis* keyAxis, QCPAxis* valueAxis);
  void setAxisRect(QCPAxisRect *axisRect);
  void setPixelPoint(const QPointF &pixelPoint);

protected:
  // property members:
  PositionType mPositionTypeX, mPositionTypeY;
  QPointer<QCPAxis> mKeyAxis, mValueAxis;
  QPointer<QCPAxisRect> mAxisRect;
  double mKey, mValue;
  QCPItemAnchor *mParentAnchorX, *mParentAnchorY;

  // reimplemented virtual methods:
  virtual QCPItemPosition *toQCPItemPosition() { return this; }

private:
  Q_DISABLE_COPY(QCPItemPosition)

};


//QCPLayerable类的子类之4---QCPAbstractItem类
//抽象类，其子类是用于显示一些特殊的图形,如放上一张图片，加载一个带箭头的直线等
class QCP_LIB_DECL QCPAbstractItem : public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(bool clipToAxisRect READ clipToAxisRect WRITE setClipToAxisRect)
  Q_PROPERTY(QCPAxisRect* clipAxisRect READ clipAxisRect WRITE setClipAxisRect)
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectionChanged)
  /// \endcond
public:
  QCPAbstractItem(QCustomPlot *parentPlot);
  virtual ~QCPAbstractItem();

  // getters:
  bool clipToAxisRect() const { return mClipToAxisRect; }
  QCPAxisRect *clipAxisRect() const;
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }

  // setters:
  void setClipToAxisRect(bool clip);
  void setClipAxisRect(QCPAxisRect *rect);
  Q_SLOT void setSelectable(bool selectable);
  Q_SLOT void setSelected(bool selected);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const = 0;

  // non-virtual methods:
  QList<QCPItemPosition*> positions() const { return mPositions; }
  QList<QCPItemAnchor*> anchors() const { return mAnchors; }
  QCPItemPosition *position(const QString &name) const;
  QCPItemAnchor *anchor(const QString &name) const;
  bool hasAnchor(const QString &name) const;

signals:
  void selectionChanged(bool selected);
  void selectableChanged(bool selectable);

protected:
  // property members:
  bool mClipToAxisRect;
  QPointer<QCPAxisRect> mClipAxisRect;
  QList<QCPItemPosition*> mPositions;
  QList<QCPItemAnchor*> mAnchors;
  bool mSelectable, mSelected;

  // reimplemented virtual methods:
  virtual QCP::Interaction selectionCategory() const;
  virtual QRect clipRect() const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter) = 0;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);

  // introduced virtual methods:
  virtual QPointF anchorPixelPoint(int anchorId) const;

  // non-virtual methods:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;
  double rectSelectTest(const QRectF &rect, const QPointF &pos, bool filledRect) const;
  QCPItemPosition *createPosition(const QString &name);
  QCPItemAnchor *createAnchor(const QString &name, int anchorId);

private:
  Q_DISABLE_COPY(QCPAbstractItem)

  friend class QCustomPlot;
  friend class QCPItemAnchor;
};

//画图工具的基本类，继承自QWidget类
class QCP_LIB_DECL QCustomPlot : public QWidget
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QRect viewport READ viewport WRITE setViewport)//视图矩形
  Q_PROPERTY(QPixmap background READ background WRITE setBackground)//背景
  Q_PROPERTY(bool backgroundScaled READ backgroundScaled WRITE setBackgroundScaled)//背景缩放
  Q_PROPERTY(Qt::AspectRatioMode backgroundScaledMode READ backgroundScaledMode WRITE setBackgroundScaledMode)//背景缩放模式
  Q_PROPERTY(QCPLayoutGrid* plotLayout READ plotLayout)//图层
  Q_PROPERTY(bool autoAddPlottableToLegend READ autoAddPlottableToLegend WRITE setAutoAddPlottableToLegend)//自动增加图表到图例
  Q_PROPERTY(int selectionTolerance READ selectionTolerance WRITE setSelectionTolerance)//已选择的公差
  Q_PROPERTY(bool noAntialiasingOnDrag READ noAntialiasingOnDrag WRITE setNoAntialiasingOnDrag)//非抗锯齿的拖放
  Q_PROPERTY(Qt::KeyboardModifier multiSelectModifier READ multiSelectModifier WRITE setMultiSelectModifier)//复选修改者
  /// \endcond
public:
  /*!
    Defines how a layer should be inserted relative to an other layer.

    \see addLayer, moveLayer
  */
  //定义一个图层相对于另一个图层怎么插入
  enum LayerInsertMode { limBelow  ///< Layer is inserted below other layer  插入下面
                         ,limAbove ///< Layer is inserted above other layer  插入上面
                       };
  Q_ENUMS(LayerInsertMode)

  /*!
    Defines with what timing the QCustomPlot surface is refreshed after a replot.

    \see replot
  */
  //定义在replot（重画）命令后图层如何刷新
  enum RefreshPriority { rpImmediate ///< The QCustomPlot surface is immediately refreshed, by calling QWidget::repaint() after the replot
                         ,rpQueued   ///< Queues the refresh such that it is performed at a slightly delayed point in time after the replot, by calling QWidget::update() after the replot
                         ,rpHint     ///< Whether to use immediate repaint or queued update depends on whether the plotting hint \ref QCP::phForceRepaint is set, see \ref setPlottingHints.
                       };

  //构造函数
  explicit QCustomPlot(QWidget *parent = 0);
  virtual ~QCustomPlot();

  // getters:
  QRect viewport() const { return mViewport; }//视图矩形
  QPixmap background() const { return mBackgroundPixmap; }//背景图片
  bool backgroundScaled() const { return mBackgroundScaled; }//背景缩放
  Qt::AspectRatioMode backgroundScaledMode() const { return mBackgroundScaledMode; }//背景缩放模式
  //布局网格类，图例类的父类
  QCPLayoutGrid *plotLayout() const { return mPlotLayout; }
  QCP::AntialiasedElements antialiasedElements() const { return mAntialiasedElements; }//抗锯齿
  QCP::AntialiasedElements notAntialiasedElements() const { return mNotAntialiasedElements; }//非抗锯齿
  bool autoAddPlottableToLegend() const { return mAutoAddPlottableToLegend; }//自动增加图表到图例
  const QCP::Interactions interactions() const { return mInteractions; }//交互性：包括选定哪之类
  int selectionTolerance() const { return mSelectionTolerance; }//已选择的公差
  bool noAntialiasingOnDrag() const { return mNoAntialiasingOnDrag; }//非抗锯齿的拖放
  QCP::PlottingHints plottingHints() const { return mPlottingHints; }//图提示
  Qt::KeyboardModifier multiSelectModifier() const { return mMultiSelectModifier; }//复选修改者

  // setters:
  void setViewport(const QRect &rect);//设置视图矩形
  void setBackground(const QPixmap &pm);//设置背景图片
  void setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
  void setBackground(const QBrush &brush);
  void setBackgroundScaled(bool scaled);//设置背景缩放
  void setBackgroundScaledMode(Qt::AspectRatioMode mode);//设置背景缩放模式
  void setAntialiasedElements(const QCP::AntialiasedElements &antialiasedElements);//设置抗锯齿
  void setAntialiasedElement(QCP::AntialiasedElement antialiasedElement, bool enabled=true);
  void setNotAntialiasedElements(const QCP::AntialiasedElements &notAntialiasedElements);//设置非抗锯齿
  void setNotAntialiasedElement(QCP::AntialiasedElement notAntialiasedElement, bool enabled=true);
  void setAutoAddPlottableToLegend(bool on);//设置自动增加图表到图例
  void setInteractions(const QCP::Interactions &interactions);//设置交互性：包括选定哪之类
  void setInteraction(const QCP::Interaction &interaction, bool enabled=true);
  void setSelectionTolerance(int pixels);//设置已选择的公差
  void setNoAntialiasingOnDrag(bool enabled);//设置非抗锯齿的拖放
  void setPlottingHints(const QCP::PlottingHints &hints);//设置图提示
  void setPlottingHint(QCP::PlottingHint hint, bool enabled=true);
  void setMultiSelectModifier(Qt::KeyboardModifier modifier);//设置复选修改者

  // non-property methods: 非属性方法
  // plottable interface: 图表接口
  QCPAbstractPlottable *plottable(int index);//数据系列绘制的图表
  QCPAbstractPlottable *plottable();
  bool addPlottable(QCPAbstractPlottable *plottable);//增加数据系列绘制的图表
  bool removePlottable(QCPAbstractPlottable *plottable);//移除数据系列绘制的图表
  bool removePlottable(int index);
  int clearPlottables();//清除数据系列绘制的图表
  int plottableCount() const;//数据系列绘制的图表计数
  QList<QCPAbstractPlottable*> selectedPlottables() const;//已被选择的图表
  QCPAbstractPlottable *plottableAt(const QPointF &pos, bool onlySelectable=false) const;//图表在
  bool hasPlottable(QCPAbstractPlottable *plottable) const;//有图表

  //*************特别注意*****************
  //*************特别注意*****************
  //*************特别注意*****************
  // specialized interface for QCPGraph: 为曲线类的专门接口
  QCPGraph *graph(int index) const;//曲线指针 zjb add!!!!!!
  QCPGraph *graph() const;
  //添加数据曲线
  //如：graph(0)可以获取某个数据曲线（按添加先后顺序）
  //例程： CstaticcurvePrivate::CstaticcurvePrivate(QWidget*wgt)
  //      parent = wgt;
  //      m_plot = new QCustomPlot(parent);
  //      m_plot->graph(0)->setData(x,y)
  //      即customPlot->graph(0)->setData(x,y)表示为数据曲线关联数据
  QCPGraph *addGraph(QCPAxis *keyAxis=0, QCPAxis *valueAxis=0);//增加曲线
  bool removeGraph(QCPGraph *graph);//移除曲线
  bool removeGraph(int index);
  int clearGraphs();//清除曲线
  int graphCount() const;//曲线计数
  QList<QCPGraph*> selectedGraphs() const;//已选择的曲线

  // item interface: 特殊图形的项目类的接口
  QCPAbstractItem *item(int index) const;//特殊图形的项目类
  QCPAbstractItem *item() const;
  bool addItem(QCPAbstractItem* item);//增加项目
  bool removeItem(QCPAbstractItem *item);//移除项目
  bool removeItem(int index);
  int clearItems();//清除项目
  int itemCount() const;//项目计数
  QList<QCPAbstractItem*> selectedItems() const;//已选择的项目
  QCPAbstractItem *itemAt(const QPointF &pos, bool onlySelectable=false) const;//项目在
  bool hasItem(QCPAbstractItem *item) const;//有项目

  // layer interface: 图层的接口
  QCPLayer *layer(const QString &name) const;//图层
  QCPLayer *layer(int index) const;
  QCPLayer *currentLayer() const;//当前图层
  bool setCurrentLayer(const QString &name);//设置当前图层
  bool setCurrentLayer(QCPLayer *layer);
  int layerCount() const;//图层计数
  bool addLayer(const QString &name, QCPLayer *otherLayer=0, LayerInsertMode insertMode=limAbove);//增加图层
  bool removeLayer(QCPLayer *layer);//移除图层
  bool moveLayer(QCPLayer *layer, QCPLayer *otherLayer, LayerInsertMode insertMode=limAbove);//移动图层

  // axis rect/layout interface: 轴长方形区类的接口
  int axisRectCount() const;//轴长方形区计数
  QCPAxisRect* axisRect(int index=0) const;
  QList<QCPAxisRect*> axisRects() const;//轴长方形区列表
  QCPLayoutElement* layoutElementAt(const QPointF &pos) const;//可布局的元素类（轴长方形区类的父类）在
  Q_SLOT void rescaleAxes(bool onlyVisiblePlottables=false); //重新缩放轴

  QList<QCPAxis*> selectedAxes() const;//已选择的轴列表
  QList<QCPLegend*> selectedLegends() const;//已选择的图列列表
  Q_SLOT void deselectAll();//取消所有选择

  //保存文件
  bool savePdf(const QString &fileName, bool noCosmeticPen=false, int width=0, int height=0, const QString &pdfCreator=QString(), const QString &pdfTitle=QString());
  bool savePng(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveJpg(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveBmp(const QString &fileName, int width=0, int height=0, double scale=1.0);
  bool saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality=-1);
  QPixmap toPixmap(int width=0, int height=0, double scale=1.0);
  void toPainter(QCPPainter *painter, int width=0, int height=0);
  //重绘
  Q_SLOT void replot(QCustomPlot::RefreshPriority refreshPriority=QCustomPlot::rpHint);

  //为坐标轴添加标签
  //如：customPlot->xAxis->setLabel("x");
  //如：customPlot->yAxis->setLabel("y");
  //设置坐标轴的范围
  //如：customPlot->xAxis->setRange(-1,1);
  //如：customPlot->yAxis->setRange(0,1);
  //重画图像：customPlot->replot();
  QCPAxis *xAxis, *yAxis, *xAxis2, *yAxis2;//下面的X轴，左边的Y轴，上面的X轴，右边的Y轴
  QCPLegend *legend;//图例

signals:
  void mouseDoubleClick(QMouseEvent *event);//鼠标双击
  void mousePress(QMouseEvent *event);//鼠标按下
  void mouseMove(QMouseEvent *event);//鼠标移动
  void mouseRelease(QMouseEvent *event);//鼠标松开
  void mouseWheel(QWheelEvent *event);//鼠标滚动

  void plottableClick(QCPAbstractPlottable *plottable, QMouseEvent *event);//图表点击
  void plottableDoubleClick(QCPAbstractPlottable *plottable, QMouseEvent *event);//图表双击
  void itemClick(QCPAbstractItem *item, QMouseEvent *event);//项目点击
  void itemDoubleClick(QCPAbstractItem *item, QMouseEvent *event);//项目双击
  void axisClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);//轴点击
  void axisDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);//轴双击
  void legendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);//图例点击
  void legendDoubleClick(QCPLegend *legend,  QCPAbstractLegendItem *item, QMouseEvent *event);//图例双击
  void titleClick(QMouseEvent *event, QCPPlotTitle *title);//标题点击
  void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);//标题双击

  void selectionChangedByUser();//已选的被用户改变
  void beforeReplot();//重绘前
  void afterReplot();//重绘后

protected:
  // property members: 属性成员
  QRect mViewport;//视图矩形区
  QCPLayoutGrid *mPlotLayout;//图例类父类
  bool mAutoAddPlottableToLegend;//设置自动增加图表到图例
  QList<QCPAbstractPlottable*> mPlottables;//数据序列图表列表
  QList<QCPGraph*> mGraphs; // extra list of plottables also in mPlottables that are of type QCPGraph 曲线列表
  QList<QCPAbstractItem*> mItems;//特殊图形列表
  QList<QCPLayer*> mLayers;//图层
  QCP::AntialiasedElements mAntialiasedElements, mNotAntialiasedElements;//抗锯齿，非抗锯齿
  QCP::Interactions mInteractions;//交互性
  int mSelectionTolerance;//设置已选择的公差
  bool mNoAntialiasingOnDrag;//设置非抗锯齿的拖放
  QBrush mBackgroundBrush;//背景画刷
  QPixmap mBackgroundPixmap;//背景图片
  QPixmap mScaledBackgroundPixmap;//缩放背景图片
  bool mBackgroundScaled;//背景缩放
  Qt::AspectRatioMode mBackgroundScaledMode;//背景的缩放模式
  QCPLayer *mCurrentLayer;//当前图层
  QCP::PlottingHints mPlottingHints;//设置图提示
  Qt::KeyboardModifier mMultiSelectModifier;//复选修改者

  // non-property members: 非属性成员
  QPixmap mPaintBuffer;//图片
  QPoint mMousePressPos;//鼠标按下处坐标
  QPointer<QCPLayoutElement> mMouseEventElement;//鼠标事件元素
  bool mReplotting;//重绘

  // reimplemented virtual methods: 重写的虚方法
  virtual QSize minimumSizeHint() const;//最小化
  virtual QSize sizeHint() const;//尺寸
  virtual void paintEvent(QPaintEvent *event);//绘图事件
  virtual void resizeEvent(QResizeEvent *event);//重绘事件
  virtual void mouseDoubleClickEvent(QMouseEvent *event);//鼠标双击事件
  virtual void mousePressEvent(QMouseEvent *event);//鼠标按下事件
  virtual void mouseMoveEvent(QMouseEvent *event);//鼠标移动事件
  virtual void mouseReleaseEvent(QMouseEvent *event);//鼠标放开事件
  virtual void wheelEvent(QWheelEvent *event);//鼠标滚动事件

  // introduced virtual methods: 传入的虚方法
  virtual void draw(QCPPainter *painter); //画
  virtual void axisRemoved(QCPAxis *axis);//轴移除
  virtual void legendRemoved(QCPLegend *legend);//图例移除

  // non-virtual methods: 非虚方法
  void updateLayerIndices() const;//更新图层索引
  QCPLayerable *layerableAt(const QPointF &pos, bool onlySelectable, QVariant *selectionDetails=0) const;//图层在
  void drawBackground(QCPPainter *painter);//绘制背景

  friend class QCPLegend;//友元类图例类
  friend class QCPAxis;//友元类坐标轴类
  friend class QCPLayer;//友元类图层类
  friend class QCPAxisRect;//友元类轴长方形区类
};


class QCP_LIB_DECL QCPColorGradient
{
  Q_GADGET
public:
  /*!
    Defines the color spaces in which color interpolation between gradient stops can be performed.

    \see setColorInterpolation
  */
  enum ColorInterpolation { ciRGB  ///< Color channels red, green and blue are linearly interpolated
                            ,ciHSV ///< Color channels hue, saturation and value are linearly interpolated (The hue is interpolated over the shortest angle distance)
                          };
  Q_ENUMS(ColorInterpolation)

  /*!
    Defines the available presets that can be loaded with \ref loadPreset. See the documentation
    there for an image of the presets.
  */
  enum GradientPreset { gpGrayscale  ///< Continuous lightness from black to white (suited for non-biased data representation)
                        ,gpHot       ///< Continuous lightness from black over firey colors to white (suited for non-biased data representation)
                        ,gpCold      ///< Continuous lightness from black over icey colors to white (suited for non-biased data representation)
                        ,gpNight     ///< Continuous lightness from black over weak blueish colors to white (suited for non-biased data representation)
                        ,gpCandy     ///< Blue over pink to white
                        ,gpGeography ///< Colors suitable to represent different elevations on geographical maps
                        ,gpIon       ///< Half hue spectrum from black over purple to blue and finally green (creates banding illusion but allows more precise magnitude estimates)
                        ,gpThermal   ///< Colors suitable for thermal imaging, ranging from dark blue over purple to orange, yellow and white
                        ,gpPolar     ///< Colors suitable to emphasize polarity around the center, with blue for negative, black in the middle and red for positive values
                        ,gpSpectrum  ///< An approximation of the visible light spectrum (creates banding illusion but allows more precise magnitude estimates)
                        ,gpJet       ///< Hue variation similar to a spectrum, often used in numerical visualization (creates banding illusion but allows more precise magnitude estimates)
                        ,gpHues      ///< Full hue cycle, with highest and lowest color red (suitable for periodic data, such as angles and phases, see \ref setPeriodic)
                      };
  Q_ENUMS(GradientPreset)

  QCPColorGradient(GradientPreset preset=gpCold);
  bool operator==(const QCPColorGradient &other) const;
  bool operator!=(const QCPColorGradient &other) const { return !(*this == other); }

  // getters:
  int levelCount() const { return mLevelCount; }
  QMap<double, QColor> colorStops() const { return mColorStops; }
  ColorInterpolation colorInterpolation() const { return mColorInterpolation; }
  bool periodic() const { return mPeriodic; }

  // setters:
  void setLevelCount(int n);
  void setColorStops(const QMap<double, QColor> &colorStops);
  void setColorStopAt(double position, const QColor &color);
  void setColorInterpolation(ColorInterpolation interpolation);
  void setPeriodic(bool enabled);

  // non-property methods:
  void colorize(const double *data, const QCPRange &range, QRgb *scanLine, int n, int dataIndexFactor=1, bool logarithmic=false);
  QRgb color(double position, const QCPRange &range, bool logarithmic=false);
  void loadPreset(GradientPreset preset);
  void clearColorStops();
  QCPColorGradient inverted() const;

protected:
  void updateColorBuffer();

  // property members:
  int mLevelCount;
  QMap<double, QColor> mColorStops;
  ColorInterpolation mColorInterpolation;
  bool mPeriodic;

  // non-property members:
  QVector<QRgb> mColorBuffer;
  bool mColorBufferInvalidated;
};

//QCPLayoutElement类的子类QCPAxisRect
//轴长方形区，用于存放轴
class QCP_LIB_DECL QCPAxisRect : public QCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPixmap background READ background WRITE setBackground)//背景图片
  Q_PROPERTY(bool backgroundScaled READ backgroundScaled WRITE setBackgroundScaled)//背景缩放
  //背景缩放模式
  Q_PROPERTY(Qt::AspectRatioMode backgroundScaledMode READ backgroundScaledMode WRITE setBackgroundScaledMode)
  Q_PROPERTY(Qt::Orientations rangeDrag READ rangeDrag WRITE setRangeDrag)//拖动方向
  Q_PROPERTY(Qt::Orientations rangeZoom READ rangeZoom WRITE setRangeZoom)//范围放大的方向
  /// \endcond
public:
  //构造函数
  explicit QCPAxisRect(QCustomPlot *parentPlot, bool setupDefaultAxes=true);//轴长方形区：画图父指针，设置缺省的轴
  virtual ~QCPAxisRect();

  // getters:
  QPixmap background() const { return mBackgroundPixmap; }//背景图片
  bool backgroundScaled() const { return mBackgroundScaled; }//背景缩放
  Qt::AspectRatioMode backgroundScaledMode() const { return mBackgroundScaledMode; }//背景缩放模式
  Qt::Orientations rangeDrag() const { return mRangeDrag; }//拖动方向
  Qt::Orientations rangeZoom() const { return mRangeZoom; }//范围放大的方向
  QCPAxis *rangeDragAxis(Qt::Orientation orientation);//往某方向拖动轴的范围
  QCPAxis *rangeZoomAxis(Qt::Orientation orientation);//往某方向放大轴的范围
  double rangeZoomFactor(Qt::Orientation orientation);//往某方向放大因子的范围

  // setters:
  void setBackground(const QPixmap &pm);//设置背景图片
  void setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
  void setBackground(const QBrush &brush);
  void setBackgroundScaled(bool scaled);//背景缩放
  void setBackgroundScaledMode(Qt::AspectRatioMode mode);//背景缩放模式
  void setRangeDrag(Qt::Orientations orientations);//设置拖动方向
  void setRangeZoom(Qt::Orientations orientations);//设置范围放大的方向
  void setRangeDragAxes(QCPAxis *horizontal, QCPAxis *vertical);//设置往某方向拖动轴的范围
  void setRangeZoomAxes(QCPAxis *horizontal, QCPAxis *vertical);//设置往某方向放大轴的范围
  void setRangeZoomFactor(double horizontalFactor, double verticalFactor);//设置往某方向放大因子的范围
  void setRangeZoomFactor(double factor);

  // non-property methods:非属性函数
  int axisCount(QCPAxis::AxisType type) const;//轴计算
  QCPAxis *axis(QCPAxis::AxisType type, int index=0) const;//轴
  QList<QCPAxis*> axes(QCPAxis::AxisTypes types) const;
  QList<QCPAxis*> axes() const;
  QCPAxis *addAxis(QCPAxis::AxisType type, QCPAxis *axis=0);//增加轴
  QList<QCPAxis*> addAxes(QCPAxis::AxisTypes types);
  bool removeAxis(QCPAxis *axis);//移除轴
  QCPLayoutInset *insetLayout() const { return mInsetLayout; }//内部设置布局

  void setupFullAxesBox(bool connectRanges=false);//安装完全的轴框
  QList<QCPAbstractPlottable*> plottables() const;//图表列表
  QList<QCPGraph*> graphs() const;//曲线列表
  QList<QCPAbstractItem*> items() const;//特殊图形的项目列表

  // read-only interface imitating a QRect:
  int left() const { return mRect.left(); }//轴长方形区的左边
  int right() const { return mRect.right(); }//轴长方形区的右边
  int top() const { return mRect.top(); }//轴长方形区的顶端
  int bottom() const { return mRect.bottom(); }//轴长方形区的底部
  int width() const { return mRect.width(); }//轴长方形区的宽
  int height() const { return mRect.height(); }//轴长方形区的高
  QSize size() const { return mRect.size(); }//轴长方形区的尺寸
  QPoint topLeft() const { return mRect.topLeft(); }//轴长方形区的左上坐标
  QPoint topRight() const { return mRect.topRight(); }//轴长方形区的右上坐标
  QPoint bottomLeft() const { return mRect.bottomLeft(); }//轴长方形区的左下坐标
  QPoint bottomRight() const { return mRect.bottomRight(); }//轴长方形区的右下坐标
  QPoint center() const { return mRect.center(); }//轴长方形区的中心坐标

  // reimplemented virtual methods:  重写的虚方法
  virtual void update(UpdatePhase phase);//更新
  virtual QList<QCPLayoutElement*> elements(bool recursive) const;//可布局的元素类的列表

protected:
  // property members:
  QBrush mBackgroundBrush; //背景画刷
  QPixmap mBackgroundPixmap;//背景图片
  QPixmap mScaledBackgroundPixmap;//背景图片缩放
  bool mBackgroundScaled;//背景缩放
  Qt::AspectRatioMode mBackgroundScaledMode;//背景缩放模式
  QCPLayoutInset *mInsetLayout;//内部设置布局
  Qt::Orientations mRangeDrag, mRangeZoom;//拖动范围的方向，放大范围的方向
  //拖动横轴、纵轴，放大横轴、纵轴
  QPointer<QCPAxis> mRangeDragHorzAxis, mRangeDragVertAxis, mRangeZoomHorzAxis, mRangeZoomVertAxis;
  double mRangeZoomFactorHorz, mRangeZoomFactorVert;//横轴放大因子，纵轴放大因子
  // non-property members:
  QCPRange mDragStartHorzRange, mDragStartVertRange;//拖动横轴的开始范围，拖动纵轴的开始范围
  QCP::AntialiasedElements mAADragBackup, mNotAADragBackup;//拖动备份，拖动不备份
  QPoint mDragStart;//拖动开始点
  bool mDragging;//是否拖动
  QHash<QCPAxis::AxisType, QList<QCPAxis*> > mAxes;//轴列表

  // reimplemented virtual methods:  重写的虚方法
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;//应用缺省的抗锯齿
  virtual void draw(QCPPainter *painter);//画
  virtual int calculateAutoMargin(QCP::MarginSide side);//计算自动的页边留白
  // events: 事件
  virtual void mousePressEvent(QMouseEvent *event);//鼠标按下事件
  virtual void mouseMoveEvent(QMouseEvent *event);//鼠标移动事件
  virtual void mouseReleaseEvent(QMouseEvent *event);//鼠标释放事件
  virtual void wheelEvent(QWheelEvent *event);//鼠标滚动事件

  // non-property methods:
  void drawBackground(QCPPainter *painter);//画背景
  void updateAxesOffset(QCPAxis::AxisType type);//更新轴的位移量

private:
  Q_DISABLE_COPY(QCPAxisRect)

  friend class QCustomPlot;//友元类为画图工具类的基本类
};


//QCPLayoutElement类的子类QCPAbstractLegendItem
//抽象类，其子类主要用于图例中间显示一些特殊的图形
class QCP_LIB_DECL QCPAbstractLegendItem : public QCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCPLegend* parentLegend READ parentLegend)
  Q_PROPERTY(QFont font READ font WRITE setFont)
  Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)
  Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor)
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectionChanged)
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectableChanged)
  /// \endcond
public:
  explicit QCPAbstractLegendItem(QCPLegend *parent);

  // getters:
  QCPLegend *parentLegend() const { return mParentLegend; }
  QFont font() const { return mFont; }
  QColor textColor() const { return mTextColor; }
  QFont selectedFont() const { return mSelectedFont; }
  QColor selectedTextColor() const { return mSelectedTextColor; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }

  // setters:
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);
  Q_SLOT void setSelectable(bool selectable);
  Q_SLOT void setSelected(bool selected);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

signals:
  void selectionChanged(bool selected);
  void selectableChanged(bool selectable);

protected:
  // property members:
  QCPLegend *mParentLegend;
  QFont mFont;
  QColor mTextColor;
  QFont mSelectedFont;
  QColor mSelectedTextColor;
  bool mSelectable, mSelected;

  // reimplemented virtual methods:
  virtual QCP::Interaction selectionCategory() const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);

private:
  Q_DISABLE_COPY(QCPAbstractLegendItem)

  friend class QCPLegend;
};

//QCPAbstractLegendItem类的子类QCPPlottableLegendItem
//主要用于图例中间显示一些特殊的图形
class QCP_LIB_DECL QCPPlottableLegendItem : public QCPAbstractLegendItem
{
  Q_OBJECT
public:
  QCPPlottableLegendItem(QCPLegend *parent, QCPAbstractPlottable *plottable);

  // getters:
  QCPAbstractPlottable *plottable() { return mPlottable; }

protected:
  // property members:
  QCPAbstractPlottable *mPlottable;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual QSize minimumSizeHint() const;

  // non-virtual methods:
  QPen getIconBorderPen() const;
  QColor getTextColor() const;
  QFont getFont() const;
};


//class QCP_LIB_DECL QCPLegend : public QCPLayoutGrid
//图例类，即图表右下角的大框
class QCP_LIB_DECL QCPLegend : public QCPLayoutGrid
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen borderPen READ borderPen WRITE setBorderPen)//边框画笔
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)//画刷
  Q_PROPERTY(QFont font READ font WRITE setFont)//字体
  Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)//文本颜色
  Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)//图标尺寸
  Q_PROPERTY(int iconTextPadding READ iconTextPadding WRITE setIconTextPadding)//图标文本垫衬
  Q_PROPERTY(QPen iconBorderPen READ iconBorderPen WRITE setIconBorderPen) //图标边框画笔
  Q_PROPERTY(SelectableParts selectableParts READ selectableParts WRITE setSelectableParts NOTIFY selectionChanged)//可选择的部分
  Q_PROPERTY(SelectableParts selectedParts READ selectedParts WRITE setSelectedParts NOTIFY selectableChanged)//已选择的部分
  Q_PROPERTY(QPen selectedBorderPen READ selectedBorderPen WRITE setSelectedBorderPen)//已选择的边框画笔
  Q_PROPERTY(QPen selectedIconBorderPen READ selectedIconBorderPen WRITE setSelectedIconBorderPen)//已选择的图标边框画笔
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)//已选择的画刷
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)//已选择的字体
  Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor)//已选择的文本字体颜色
  /// \endcond
public:
  /*!
    Defines the selectable parts of a legend

    \see setSelectedParts, setSelectableParts
  */
  enum SelectablePart { spNone       = 0x000  ///< <tt>0x000</tt> None 没有
                        ,spLegendBox  = 0x001 ///< <tt>0x001</tt> The legend box (frame) 图例框
                        ,spItems      = 0x002 ///< <tt>0x002</tt> Legend items individually (see \ref selectedItems) 图例项目
                      };
  Q_FLAGS(SelectablePart SelectableParts)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)

  //构造函数，单参数
  explicit QCPLegend();
  virtual ~QCPLegend();

  // getters:
  QPen borderPen() const { return mBorderPen; }//边框画笔
  QBrush brush() const { return mBrush; }//画刷
  QFont font() const { return mFont; }//字体
  QColor textColor() const { return mTextColor; }//文本颜色
  QSize iconSize() const { return mIconSize; }//图标尺寸
  int iconTextPadding() const { return mIconTextPadding; }//图标文本垫衬
  QPen iconBorderPen() const { return mIconBorderPen; }//图标边框画笔
  SelectableParts selectableParts() const { return mSelectableParts; }//可选择的部分
  SelectableParts selectedParts() const;//已选择的部分
  QPen selectedBorderPen() const { return mSelectedBorderPen; }//已选择的边框画笔
  QPen selectedIconBorderPen() const { return mSelectedIconBorderPen; }//已选择的图标边框画笔
  QBrush selectedBrush() const { return mSelectedBrush; }//已选择的画刷
  QFont selectedFont() const { return mSelectedFont; }//已选择的字体
  QColor selectedTextColor() const { return mSelectedTextColor; }//已选择的文本字体颜色

  // setters:
  void setBorderPen(const QPen &pen);//设置边框画笔
  void setBrush(const QBrush &brush);//设置画刷
  void setFont(const QFont &font);//设置字体
  void setTextColor(const QColor &color);//设置文本颜色
  void setIconSize(const QSize &size);//设置图标尺寸
  void setIconSize(int width, int height);
  void setIconTextPadding(int padding);//设置图标文本垫衬
  void setIconBorderPen(const QPen &pen);//设置图标边框画笔
  Q_SLOT void setSelectableParts(const SelectableParts &selectableParts);//设置可选择的部分
  Q_SLOT void setSelectedParts(const SelectableParts &selectedParts);//设置已选择的部分
  void setSelectedBorderPen(const QPen &pen);//设置已选择的边框画笔
  void setSelectedIconBorderPen(const QPen &pen);//设置已选择的图标边框画笔
  void setSelectedBrush(const QBrush &brush);//设置已选择的画刷
  void setSelectedFont(const QFont &font);//设置已选择的字体
  void setSelectedTextColor(const QColor &color);//设置已选择的文本字体颜色

  // reimplemented virtual methods: 重写的虚方法
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  // non-virtual methods: 非虚方法
  //特殊图形显示的项目
  QCPAbstractLegendItem *item(int index) const;
  QCPPlottableLegendItem *itemWithPlottable(const QCPAbstractPlottable *plottable) const;//图表图例项目
  int itemCount() const;//项目计算
  bool hasItem(QCPAbstractLegendItem *item) const;//有项目
  bool hasItemWithPlottable(const QCPAbstractPlottable *plottable) const;//有图表项目
  bool addItem(QCPAbstractLegendItem *item);//增加项目
  bool removeItem(int index);//移除项目
  bool removeItem(QCPAbstractLegendItem *item);
  void clearItems();//清除项目
  QList<QCPAbstractLegendItem*> selectedItems() const;//已选择的项目

signals:
  void selectionChanged(QCPLegend::SelectableParts parts);//已选择的状态改变
  void selectableChanged(QCPLegend::SelectableParts parts);//可选择性状态改变

protected:
  // property members:
  QPen mBorderPen, mIconBorderPen; //画笔
  QBrush mBrush;//画刷
  QFont mFont;//字体
  QColor mTextColor;//文本颜色
  QSize mIconSize;//图标尺寸
  int mIconTextPadding;//图标文本垫衬
  SelectableParts mSelectedParts, mSelectableParts;//可选择的部分
  QPen mSelectedBorderPen, mSelectedIconBorderPen;//已选择的边框画笔，图标边框画笔
  QBrush mSelectedBrush;//已选择的画刷
  QFont mSelectedFont;//已选择的字体
  QColor mSelectedTextColor;//已选择的文本颜色

  // reimplemented virtual methods: 重写的虚方法
  virtual void parentPlotInitialized(QCustomPlot *parentPlot);//画图类的基本类指针初始化
  virtual QCP::Interaction selectionCategory() const;//选择种类的交互性
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const; //应用缺省的抗锯齿性
  virtual void draw(QCPPainter *painter);//画
  // events: 事件
  //选择事件
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  //取消选择事件
  virtual void deselectEvent(bool *selectionStateChanged);

  // non-virtual methods: 非虚方法
  QPen getBorderPen() const; //获取边框画笔
  QBrush getBrush() const;   //获取画刷

private:
  Q_DISABLE_COPY(QCPLegend)

  friend class QCustomPlot;//友元类画图类的基本类QCustomPlot
  friend class QCPAbstractLegendItem;//友元类抽象的图例项目类QCPAbstractLegendItem
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCPLegend::SelectableParts)
Q_DECLARE_METATYPE(QCPLegend::SelectablePart)


//QCPLayoutElement类的子类QCPPlotTitle
//图表标题，即最顶上标题
class QCP_LIB_DECL QCPPlotTitle : public QCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString text READ text WRITE setText)//文本读写
  Q_PROPERTY(QFont font READ font WRITE setFont)//文本字体
  Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)//文本颜色
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)//被选择的（文本）字体
  Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor)//被选择的（文本）颜色
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)//选择的状态
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectionChanged)//被选择的状态
  /// \endcond
public:
  //构造函数
  explicit QCPPlotTitle(QCustomPlot *parentPlot);//只可显示转换，画图工具类的基本类的父指针
  explicit QCPPlotTitle(QCustomPlot *parentPlot, const QString &text);

  // getters:
  QString text() const { return mText; }//文本读写
  QFont font() const { return mFont; }//文本字体
  QColor textColor() const { return mTextColor; }//文本颜色
  QFont selectedFont() const { return mSelectedFont; }//被选择的（文本）字体
  QColor selectedTextColor() const { return mSelectedTextColor; }//被选择的（文本）颜色
  bool selectable() const { return mSelectable; }//选择的状态
  bool selected() const { return mSelected; }//被选择的状态

  // setters:
  void setText(const QString &text);//设置文本
  void setFont(const QFont &font);//设置字体
  void setTextColor(const QColor &color);//设置文本颜色
  void setSelectedFont(const QFont &font);//设置选择的（文本）字体
  void setSelectedTextColor(const QColor &color);//设置选择的（文本）颜色
  Q_SLOT void setSelectable(bool selectable);//设置可选择性
  Q_SLOT void setSelected(bool selected);//设置被选择的


  // reimplemented virtual methods: 重写虚方法
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

signals:
  void selectionChanged(bool selected);//被选择的状态改变
  void selectableChanged(bool selectable);//可选择性的状态改变

protected:
  // property members:
  QString mText;//设置文本
  QFont mFont;//设置字体
  QColor mTextColor;//设置文本颜色
  QFont mSelectedFont;//设置选择的（文本）字体
  QColor mSelectedTextColor;//设置选择的（文本）颜色
  QRect mTextBoundingRect;//设置文本边界区域
  bool mSelectable, mSelected;//设置可选择性，被选择的

  // reimplemented virtual methods: 重写函数
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;//应用缺省的抗锯齿
  virtual void draw(QCPPainter *painter);//画
  virtual QSize minimumSizeHint() const;//最小尺寸
  virtual QSize maximumSizeHint() const;//最大尺寸
  // events:事件
  //选择事件
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  //取消选择事件
  virtual void deselectEvent(bool *selectionStateChanged);

  // non-virtual methods:非虚方法
  QFont mainFont() const;//主要字体
  QColor mainTextColor() const;//主要文本颜色

private:
  Q_DISABLE_COPY(QCPPlotTitle)
};


class QCPColorScaleAxisRectPrivate : public QCPAxisRect
{
  Q_OBJECT
public:
  explicit QCPColorScaleAxisRectPrivate(QCPColorScale *parentColorScale);
protected:
  QCPColorScale *mParentColorScale;
  QImage mGradientImage;
  bool mGradientImageInvalidated;
  // re-using some methods of QCPAxisRect to make them available to friend class QCPColorScale
  using QCPAxisRect::calculateAutoMargin;
  using QCPAxisRect::mousePressEvent;
  using QCPAxisRect::mouseMoveEvent;
  using QCPAxisRect::mouseReleaseEvent;
  using QCPAxisRect::wheelEvent;
  using QCPAxisRect::update;
  virtual void draw(QCPPainter *painter);
  void updateGradientImage();
  Q_SLOT void axisSelectionChanged(QCPAxis::SelectableParts selectedParts);
  Q_SLOT void axisSelectableChanged(QCPAxis::SelectableParts selectableParts);
  friend class QCPColorScale;
};


//Scale,刻度,衡量,比例,数值范围,比例尺,天平,等级
//颜色刻度类
class QCP_LIB_DECL QCPColorScale : public QCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCPAxis::AxisType type READ type WRITE setType)
  Q_PROPERTY(QCPRange dataRange READ dataRange WRITE setDataRange NOTIFY dataRangeChanged)
  Q_PROPERTY(QCPAxis::ScaleType dataScaleType READ dataScaleType WRITE setDataScaleType NOTIFY dataScaleTypeChanged)
  Q_PROPERTY(QCPColorGradient gradient READ gradient WRITE setGradient NOTIFY gradientChanged)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(int barWidth READ barWidth WRITE setBarWidth)
  Q_PROPERTY(bool rangeDrag READ rangeDrag WRITE setRangeDrag)
  Q_PROPERTY(bool rangeZoom READ rangeZoom WRITE setRangeZoom)
  /// \endcond
public:
  explicit QCPColorScale(QCustomPlot *parentPlot);
  virtual ~QCPColorScale();

  // getters:
  QCPAxis *axis() const { return mColorAxis.data(); }
  QCPAxis::AxisType type() const { return mType; }
  QCPRange dataRange() const { return mDataRange; }
  QCPAxis::ScaleType dataScaleType() const { return mDataScaleType; }
  QCPColorGradient gradient() const { return mGradient; }
  QString label() const;
  int barWidth () const { return mBarWidth; }
  bool rangeDrag() const;
  bool rangeZoom() const;

  // setters:
  void setType(QCPAxis::AxisType type);
  Q_SLOT void setDataRange(const QCPRange &dataRange);
  Q_SLOT void setDataScaleType(QCPAxis::ScaleType scaleType);
  Q_SLOT void setGradient(const QCPColorGradient &gradient);
  void setLabel(const QString &str);
  void setBarWidth(int width);
  void setRangeDrag(bool enabled);
  void setRangeZoom(bool enabled);

  // non-property methods:
  QList<QCPColorMap*> colorMaps() const;
  void rescaleDataRange(bool onlyVisibleMaps);

  // reimplemented virtual methods:
  virtual void update(UpdatePhase phase);

signals:
  void dataRangeChanged(QCPRange newRange);
  void dataScaleTypeChanged(QCPAxis::ScaleType scaleType);
  void gradientChanged(QCPColorGradient newGradient);

protected:
  // property members:
  QCPAxis::AxisType mType;
  QCPRange mDataRange;
  QCPAxis::ScaleType mDataScaleType;
  QCPColorGradient mGradient;
  int mBarWidth;

  // non-property members:
  QPointer<QCPColorScaleAxisRectPrivate> mAxisRect;
  QPointer<QCPAxis> mColorAxis;

  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  // events:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);

private:
  Q_DISABLE_COPY(QCPColorScale)

  friend class QCPColorScaleAxisRectPrivate;
};


/*! \file */



class QCP_LIB_DECL QCPData
{
public:
  QCPData();
  QCPData(double key, double value);
  double key, value;
  double keyErrorPlus, keyErrorMinus;
  double valueErrorPlus, valueErrorMinus;
};
Q_DECLARE_TYPEINFO(QCPData, Q_MOVABLE_TYPE);

/*! \typedef QCPDataMap
  Container for storing \ref QCPData items in a sorted fashion. The key of the map
  is the key member of the QCPData instance.

  This is the container in which QCPGraph holds its data.
  \see QCPData, QCPGraph::setData
*/
typedef QMap<double, QCPData> QCPDataMap;
typedef QMapIterator<double, QCPData> QCPDataMapIterator;
typedef QMutableMapIterator<double, QCPData> QCPDataMutableMapIterator;

//派生自QCPAbstractPlottable类，曲线类，1个x值对应1个y值
class QCP_LIB_DECL QCPGraph : public QCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)//直线类型
  Q_PROPERTY(QCPScatterStyle scatterStyle READ scatterStyle WRITE setScatterStyle)//散点类型
  Q_PROPERTY(ErrorType errorType READ errorType WRITE setErrorType)//误差类型
  Q_PROPERTY(QPen errorPen READ errorPen WRITE setErrorPen)//误差画笔
  Q_PROPERTY(double errorBarSize READ errorBarSize WRITE setErrorBarSize)//误差条尺寸
  Q_PROPERTY(bool errorBarSkipSymbol READ errorBarSkipSymbol WRITE setErrorBarSkipSymbol)//误差条跳跃符号
  Q_PROPERTY(QCPGraph* channelFillGraph READ channelFillGraph WRITE setChannelFillGraph)//曲线，填充曲线路线
  Q_PROPERTY(bool adaptiveSampling READ adaptiveSampling WRITE setAdaptiveSampling)//自适应的取样
  /// \endcond
public:
  /*!
    Defines how the graph's line is represented visually in the plot. The line is drawn with the
    current pen of the graph (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone        ///< data points are not connected with any lines (e.g. data only represented 没直线
                                 ///< with symbols according to the scatter style, see \ref setScatterStyle)
                   ,lsLine       ///< data points are connected by a straight line  直线连接
                   ,lsStepLeft   ///< line is drawn as steps where the step height is the value of the left data point 左侧数据点
                   ,lsStepRight  ///< line is drawn as steps where the step height is the value of the right data point 右侧数据点
                   ,lsStepCenter ///< line is drawn as steps where the step is in between two data points 在2个数据点中间
                   ,lsImpulse    ///< each data point is represented by a line parallel to the value axis, which reaches from the data point to the zero-value-line 脉冲
                 };
  Q_ENUMS(LineStyle)
  /*!
    Defines what kind of error bars are drawn for each data point
  */
  enum ErrorType { etNone   ///< No error bars are shown  没误差条
                   ,etKey   ///< Error bars for the key dimension of the data point are shown 键上的数据点
                   ,etValue ///< Error bars for the value dimension of the data point are shown 值上的数据点
                   ,etBoth  ///< Error bars for both key and value dimensions of the data point are shown 键上和值上的数据点
                 };
  Q_ENUMS(ErrorType)

  //构造函数
  //曲线
  explicit QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPGraph();

  // getters:  获得者
  QCPDataMap *data() const { return mData; }//数据文本
  LineStyle lineStyle() const { return mLineStyle; }//直线形式
  QCPScatterStyle scatterStyle() const { return mScatterStyle; }//散点形式
  ErrorType errorType() const { return mErrorType; }//误差类型
  QPen errorPen() const { return mErrorPen; }//误差画笔
  double errorBarSize() const { return mErrorBarSize; }//误差条尺寸
  bool errorBarSkipSymbol() const { return mErrorBarSkipSymbol; }//误差条跳跃符号
  QCPGraph *channelFillGraph() const { return mChannelFillGraph.data(); }//填充曲线
  bool adaptiveSampling() const { return mAdaptiveSampling; }//自适应取样

  // setters: 设置者
  void setData(QCPDataMap *data, bool copy=false);//设置数据文本
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError);//设置数据键误差
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError);//设置数据值误差
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError);//设置数据键和值误差
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setLineStyle(LineStyle ls);//设置直线形式
  void setScatterStyle(const QCPScatterStyle &style);//设置散点形式
  void setErrorType(ErrorType errorType);//设置误差类型
  void setErrorPen(const QPen &pen);//设置误差画笔
  void setErrorBarSize(double size);//设置误差条尺寸
  void setErrorBarSkipSymbol(bool enabled);//设置误差条跳跃符号
  void setChannelFillGraph(QCPGraph *targetGraph);//设置填充曲线
  void setAdaptiveSampling(bool enabled);//设置自适应取样

  // non-property methods: 非属性的方法
  void addData(const QCPDataMap &dataMap);//增加数据
  void addData(const QCPData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);//在某键值前移除数据
  void removeDataAfter(double key);//在某键值后移除数据
  void removeData(double fromKey, double toKey);//从某键值移除数据
  void removeData(double key);

  // reimplemented virtual methods:
  virtual void clearData();//清数据
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;//选择检验
  using QCPAbstractPlottable::rescaleAxes;//缩放轴相关
  using QCPAbstractPlottable::rescaleKeyAxis;
  using QCPAbstractPlottable::rescaleValueAxis;
  void rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  void rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  void rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface

protected:
  // property members:  属性的成员
  QCPDataMap *mData;//数据文本
  QPen mErrorPen;//误差笔
  LineStyle mLineStyle;//直线类型
  QCPScatterStyle mScatterStyle;//缩放类型
  ErrorType mErrorType;//误差类型
  double mErrorBarSize;//误差条尺寸
  bool mErrorBarSkipSymbol;//误差条跳跃符号
  QPointer<QCPGraph> mChannelFillGraph;//填充曲线
  bool mAdaptiveSampling;//设置自适应取样

  // reimplemented virtual methods: 重写的虚方法
  virtual void draw(QCPPainter *painter);//画
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;//画图例图标
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;//获取键范围
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;//获取值范围
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface

  // introduced virtual methods: 传入的虚方法
  virtual void drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const; //画填充
  virtual void drawScatterPlot(QCPPainter *painter, QVector<QCPData> *scatterData) const;//画散点
  virtual void drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;//画直线
  virtual void drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;//画脉冲

  // non-virtual methods:非虚方法
  void getPreparedData(QVector<QCPData> *lineData, QVector<QCPData> *scatterData) const;//获取预备的数据
  void getPlotData(QVector<QPointF> *lineData, QVector<QCPData> *scatterData) const;//获取画图数据
  void getScatterPlotData(QVector<QCPData> *scatterData) const;//获取散点画图数据
  void getLinePlotData(QVector<QPointF> *linePixelData, QVector<QCPData> *scatterData) const;//获取直线画图数据
  void getStepLeftPlotData(QVector<QPointF> *linePixelData, QVector<QCPData> *scatterData) const;//获取台阶左边画图数据
  void getStepRightPlotData(QVector<QPointF> *linePixelData, QVector<QCPData> *scatterData) const;//获取台阶右边画图数据
  void getStepCenterPlotData(QVector<QPointF> *linePixelData, QVector<QCPData> *scatterData) const;//获取台阶中间画图数据
  void getImpulsePlotData(QVector<QPointF> *linePixelData, QVector<QCPData> *scatterData) const;//获取脉冲画图数据
  void drawError(QCPPainter *painter, double x, double y, const QCPData &data) const;//画误差
  void getVisibleDataBounds(QCPDataMap::const_iterator &lower, QCPDataMap::const_iterator &upper) const;//获取可见的数据界限
  int countDataInBounds(const QCPDataMap::const_iterator &lower, const QCPDataMap::const_iterator &upper, int maxCount) const;//计算边界的数据
  void addFillBasePoints(QVector<QPointF> *lineData) const;//增加填充基本点
  void removeFillBasePoints(QVector<QPointF> *lineData) const;//移除增加填充基本点
  QPointF lowerFillBasePoint(double lowerKey) const;//往下填充基本点
  QPointF upperFillBasePoint(double upperKey) const;//往上填充基本点
  const QPolygonF getChannelFillPolygon(const QVector<QPointF> *lineData) const;//获取路线填充多边形
  int findIndexBelowX(const QVector<QPointF> *data, double x) const;//查找x下的索引
  int findIndexAboveX(const QVector<QPointF> *data, double x) const;//查找x上的索引
  int findIndexBelowY(const QVector<QPointF> *data, double y) const;//查找y下的索引
  int findIndexAboveY(const QVector<QPointF> *data, double y) const;//查找y上的索引
  double pointDistance(const QPointF &pixelPoint) const;//点的距离

  friend class QCustomPlot;//友元类画图工具类的基本类
  friend class QCPLegend;//友元类图例类
};


/*! \file */



class QCP_LIB_DECL QCPCurveData
{
public:
  QCPCurveData();
  QCPCurveData(double t, double key, double value);
  double t, key, value;
};
Q_DECLARE_TYPEINFO(QCPCurveData, Q_MOVABLE_TYPE);

/*! \typedef QCPCurveDataMap
  Container for storing \ref QCPCurveData items in a sorted fashion. The key of the map
  is the t member of the QCPCurveData instance.

  This is the container in which QCPCurve holds its data.
  \see QCPCurveData, QCPCurve::setData
*/

typedef QMap<double, QCPCurveData> QCPCurveDataMap;
typedef QMapIterator<double, QCPCurveData> QCPCurveDataMapIterator;
typedef QMutableMapIterator<double, QCPCurveData> QCPCurveDataMutableMapIterator;


//派生自QCPAbstractPlottable类，弧线类，1个x值对应2个y值
class QCP_LIB_DECL QCPCurve : public QCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCPScatterStyle scatterStyle READ scatterStyle WRITE setScatterStyle)
  Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)
  /// \endcond
public:
  /*!
    Defines how the curve's line is represented visually in the plot. The line is drawn with the
    current pen of the curve (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone  ///< No line is drawn between data points (e.g. only scatters)
                   ,lsLine ///< Data points are connected with a straight line
                 };
  explicit QCPCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPCurve();

  // getters:
  QCPCurveDataMap *data() const { return mData; }
  QCPScatterStyle scatterStyle() const { return mScatterStyle; }
  LineStyle lineStyle() const { return mLineStyle; }

  // setters:
  void setData(QCPCurveDataMap *data, bool copy=false);
  void setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setScatterStyle(const QCPScatterStyle &style);
  void setLineStyle(LineStyle style);

  // non-property methods:
  void addData(const QCPCurveDataMap &dataMap);
  void addData(const QCPCurveData &data);
  void addData(double t, double key, double value);
  void addData(double key, double value);
  void addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double t);
  void removeDataAfter(double t);
  void removeData(double fromt, double tot);
  void removeData(double t);

  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

protected:
  // property members:
  QCPCurveDataMap *mData;
  QCPScatterStyle mScatterStyle;
  LineStyle mLineStyle;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;

  // introduced virtual methods:
  virtual void drawScatterPlot(QCPPainter *painter, const QVector<QPointF> *pointData) const;

  // non-virtual methods:
  void getCurveData(QVector<QPointF> *lineData) const;
  int getRegion(double x, double y, double rectLeft, double rectTop, double rectRight, double rectBottom) const;
  QPointF getOptimizedPoint(int prevRegion, double prevKey, double prevValue, double key, double value, double rectLeft, double rectTop, double rectRight, double rectBottom) const;
  QVector<QPointF> getOptimizedCornerPoints(int prevRegion, int currentRegion, double prevKey, double prevValue, double key, double value, double rectLeft, double rectTop, double rectRight, double rectBottom) const;
  bool mayTraverse(int prevRegion, int currentRegion) const;
  bool getTraverse(double prevKey, double prevValue, double key, double value, double rectLeft, double rectTop, double rectRight, double rectBottom, QPointF &crossA, QPointF &crossB) const;
  void getTraverseCornerPoints(int prevRegion, int currentRegion, double rectLeft, double rectTop, double rectRight, double rectBottom, QVector<QPointF> &beforeTraverse, QVector<QPointF> &afterTraverse) const;
  double pointDistance(const QPointF &pixelPoint) const;

  friend class QCustomPlot;
  friend class QCPLegend;
};


/*! \file */



class QCP_LIB_DECL QCPBarsGroup : public QObject
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(SpacingType spacingType READ spacingType WRITE setSpacingType)
  Q_PROPERTY(double spacing READ spacing WRITE setSpacing)
  /// \endcond
public:
  /*!
    Defines the ways the spacing between bars in the group can be specified. Thus it defines what
    the number passed to \ref setSpacing actually means.

    \see setSpacingType, setSpacing
  */
  enum SpacingType { stAbsolute       ///< Bar spacing is in absolute pixels
                     ,stAxisRectRatio ///< Bar spacing is given by a fraction of the axis rect size
                     ,stPlotCoords    ///< Bar spacing is in key coordinates and thus scales with the key axis range
                 };
  QCPBarsGroup(QCustomPlot *parentPlot);
  ~QCPBarsGroup();

  // getters:
  SpacingType spacingType() const { return mSpacingType; }
  double spacing() const { return mSpacing; }

  // setters:
  void setSpacingType(SpacingType spacingType);
  void setSpacing(double spacing);

  // non-virtual methods:
  QList<QCPBars*> bars() const { return mBars; }
  QCPBars* bars(int index) const;
  int size() const { return mBars.size(); }
  bool isEmpty() const { return mBars.isEmpty(); }
  void clear();
  bool contains(QCPBars *bars) const { return mBars.contains(bars); }
  void append(QCPBars *bars);
  void insert(int i, QCPBars *bars);
  void remove(QCPBars *bars);

protected:
  // non-property members:
  QCustomPlot *mParentPlot;
  SpacingType mSpacingType;
  double mSpacing;
  QList<QCPBars*> mBars;

  // non-virtual methods:
  void registerBars(QCPBars *bars);
  void unregisterBars(QCPBars *bars);

  // virtual methods:
  double keyPixelOffset(const QCPBars *bars, double keyCoord);
  double getPixelSpacing(const QCPBars *bars, double keyCoord);

private:
  Q_DISABLE_COPY(QCPBarsGroup)

  friend class QCPBars;
};


class QCP_LIB_DECL QCPBarData
{
public:
  QCPBarData();
  QCPBarData(double key, double value);
  double key, value;
};
Q_DECLARE_TYPEINFO(QCPBarData, Q_MOVABLE_TYPE);

/*! \typedef QCPBarDataMap
  Container for storing \ref QCPBarData items in a sorted fashion. The key of the map
  is the key member of the QCPBarData instance.

  This is the container in which QCPBars holds its data.
  \see QCPBarData, QCPBars::setData
*/
typedef QMap<double, QCPBarData> QCPBarDataMap;
typedef QMapIterator<double, QCPBarData> QCPBarDataMapIterator;
typedef QMutableMapIterator<double, QCPBarData> QCPBarDataMutableMapIterator;


//QCPAbstractPlottable类的子类QCPBars
//柱状图类
class QCP_LIB_DECL QCPBars : public QCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(WidthType widthType READ widthType WRITE setWidthType)
  Q_PROPERTY(QCPBarsGroup* barsGroup READ barsGroup WRITE setBarsGroup)
  Q_PROPERTY(double baseValue READ baseValue WRITE setBaseValue)
  Q_PROPERTY(QCPBars* barBelow READ barBelow)
  Q_PROPERTY(QCPBars* barAbove READ barAbove)
  /// \endcond
public:
  /*!
    Defines the ways the width of the bar can be specified. Thus it defines what the number passed
    to \ref setWidth actually means.

    \see setWidthType, setWidth
  */
  enum WidthType { wtAbsolute       ///< Bar width is in absolute pixels
                   ,wtAxisRectRatio ///< Bar width is given by a fraction of the axis rect size
                   ,wtPlotCoords    ///< Bar width is in key coordinates and thus scales with the key axis range
                 };
   Q_ENUMS(WidthType)

  explicit QCPBars(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPBars();

  // getters:
  double width() const { return mWidth; }
  WidthType widthType() const { return mWidthType; }
  QCPBarsGroup *barsGroup() const { return mBarsGroup; }
  double baseValue() const { return mBaseValue; }
  QCPBars *barBelow() const { return mBarBelow.data(); }
  QCPBars *barAbove() const { return mBarAbove.data(); }
  QCPBarDataMap *data() const { return mData; }

  // setters:
  void setWidth(double width);
  void setWidthType(WidthType widthType);
  void setBarsGroup(QCPBarsGroup *barsGroup);
  void setBaseValue(double baseValue);
  void setData(QCPBarDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);

  // non-property methods:
  void moveBelow(QCPBars *bars);
  void moveAbove(QCPBars *bars);
  void addData(const QCPBarDataMap &dataMap);
  void addData(const QCPBarData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);

  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

protected:
  // property members:
  QCPBarDataMap *mData;
  double mWidth;
  WidthType mWidthType;
  QCPBarsGroup *mBarsGroup;
  double mBaseValue;
  QPointer<QCPBars> mBarBelow, mBarAbove;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;

  // non-virtual methods:
  void getVisibleDataBounds(QCPBarDataMap::const_iterator &lower, QCPBarDataMap::const_iterator &upperEnd) const;
  QPolygonF getBarPolygon(double key, double value) const;
  void getPixelWidth(double key, double &lower, double &upper) const;
  double getStackedBaseValue(double key, bool positive) const;
  static void connectBars(QCPBars* lower, QCPBars* upper);

  friend class QCustomPlot;
  friend class QCPLegend;
  friend class QCPBarsGroup;
};


/*! \file */


//QCPAbstractPlottable类的子类QCPStatisticalBox
//统计学箱类
class QCP_LIB_DECL QCPStatisticalBox : public QCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(double key READ key WRITE setKey)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double lowerQuartile READ lowerQuartile WRITE setLowerQuartile)
  Q_PROPERTY(double median READ median WRITE setMedian)
  Q_PROPERTY(double upperQuartile READ upperQuartile WRITE setUpperQuartile)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(QVector<double> outliers READ outliers WRITE setOutliers)
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(double whiskerWidth READ whiskerWidth WRITE setWhiskerWidth)
  Q_PROPERTY(QPen whiskerPen READ whiskerPen WRITE setWhiskerPen)
  Q_PROPERTY(QPen whiskerBarPen READ whiskerBarPen WRITE setWhiskerBarPen)
  Q_PROPERTY(QPen medianPen READ medianPen WRITE setMedianPen)
  Q_PROPERTY(QCPScatterStyle outlierStyle READ outlierStyle WRITE setOutlierStyle)
  /// \endcond
public:
  explicit QCPStatisticalBox(QCPAxis *keyAxis, QCPAxis *valueAxis);

  // getters:
  double key() const { return mKey; }
  double minimum() const { return mMinimum; }
  double lowerQuartile() const { return mLowerQuartile; }
  double median() const { return mMedian; }
  double upperQuartile() const { return mUpperQuartile; }
  double maximum() const { return mMaximum; }
  QVector<double> outliers() const { return mOutliers; }
  double width() const { return mWidth; }
  double whiskerWidth() const { return mWhiskerWidth; }
  QPen whiskerPen() const { return mWhiskerPen; }
  QPen whiskerBarPen() const { return mWhiskerBarPen; }
  QPen medianPen() const { return mMedianPen; }
  QCPScatterStyle outlierStyle() const { return mOutlierStyle; }

  // setters:
  void setKey(double key);
  void setMinimum(double value);
  void setLowerQuartile(double value);
  void setMedian(double value);
  void setUpperQuartile(double value);
  void setMaximum(double value);
  void setOutliers(const QVector<double> &values);
  void setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum);
  void setWidth(double width);
  void setWhiskerWidth(double width);
  void setWhiskerPen(const QPen &pen);
  void setWhiskerBarPen(const QPen &pen);
  void setMedianPen(const QPen &pen);
  void setOutlierStyle(const QCPScatterStyle &style);

  // non-property methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

protected:
  // property members:
  QVector<double> mOutliers;
  double mKey, mMinimum, mLowerQuartile, mMedian, mUpperQuartile, mMaximum;
  double mWidth;
  double mWhiskerWidth;
  QPen mWhiskerPen, mWhiskerBarPen, mMedianPen;
  QCPScatterStyle mOutlierStyle;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;

  // introduced virtual methods:
  virtual void drawQuartileBox(QCPPainter *painter, QRectF *quartileBox=0) const;
  virtual void drawMedian(QCPPainter *painter) const;
  virtual void drawWhiskers(QCPPainter *painter) const;
  virtual void drawOutliers(QCPPainter *painter) const;

  friend class QCustomPlot;
  friend class QCPLegend;
};


class QCP_LIB_DECL QCPColorMapData
{
public:
  QCPColorMapData(int keySize, int valueSize, const QCPRange &keyRange, const QCPRange &valueRange);
  ~QCPColorMapData();
  QCPColorMapData(const QCPColorMapData &other);
  QCPColorMapData &operator=(const QCPColorMapData &other);

  // getters:
  int keySize() const { return mKeySize; }
  int valueSize() const { return mValueSize; }
  QCPRange keyRange() const { return mKeyRange; }
  QCPRange valueRange() const { return mValueRange; }
  QCPRange dataBounds() const { return mDataBounds; }
  double data(double key, double value);
  double cell(int keyIndex, int valueIndex);

  // setters:
  void setSize(int keySize, int valueSize);
  void setKeySize(int keySize);
  void setValueSize(int valueSize);
  void setRange(const QCPRange &keyRange, const QCPRange &valueRange);
  void setKeyRange(const QCPRange &keyRange);
  void setValueRange(const QCPRange &valueRange);
  void setData(double key, double value, double z);
  void setCell(int keyIndex, int valueIndex, double z);

  // non-property methods:
  void recalculateDataBounds();
  void clear();
  void fill(double z);
  bool isEmpty() const { return mIsEmpty; }
  void coordToCell(double key, double value, int *keyIndex, int *valueIndex) const;
  void cellToCoord(int keyIndex, int valueIndex, double *key, double *value) const;

protected:
  // property members:
  int mKeySize, mValueSize;
  QCPRange mKeyRange, mValueRange;
  bool mIsEmpty;
  // non-property members:
  double *mData;
  QCPRange mDataBounds;
  bool mDataModified;

  friend class QCPColorMap;
};


//QCPAbstractPlottable类的子类QCPColorMap
//色图类,就是一个大的正方形有很多小色块，可以用笔选其中的小色块
class QCP_LIB_DECL QCPColorMap : public QCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCPRange dataRange READ dataRange WRITE setDataRange NOTIFY dataRangeChanged)
  Q_PROPERTY(QCPAxis::ScaleType dataScaleType READ dataScaleType WRITE setDataScaleType NOTIFY dataScaleTypeChanged)
  Q_PROPERTY(QCPColorGradient gradient READ gradient WRITE setGradient NOTIFY gradientChanged)
  Q_PROPERTY(bool interpolate READ interpolate WRITE setInterpolate)
  Q_PROPERTY(bool tightBoundary READ tightBoundary WRITE setTightBoundary)
  Q_PROPERTY(QCPColorScale* colorScale READ colorScale WRITE setColorScale)
  /// \endcond
public:
  explicit QCPColorMap(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPColorMap();

  // getters:
  QCPColorMapData *data() const { return mMapData; }
  QCPRange dataRange() const { return mDataRange; }
  QCPAxis::ScaleType dataScaleType() const { return mDataScaleType; }
  bool interpolate() const { return mInterpolate; }
  bool tightBoundary() const { return mTightBoundary; }
  QCPColorGradient gradient() const { return mGradient; }
  QCPColorScale *colorScale() const { return mColorScale.data(); }

  // setters:
  void setData(QCPColorMapData *data, bool copy=false);
  Q_SLOT void setDataRange(const QCPRange &dataRange);
  Q_SLOT void setDataScaleType(QCPAxis::ScaleType scaleType);
  Q_SLOT void setGradient(const QCPColorGradient &gradient);
  void setInterpolate(bool enabled);
  void setTightBoundary(bool enabled);
  void setColorScale(QCPColorScale *colorScale);

  // non-property methods:
  void rescaleDataRange(bool recalculateDataBounds=false);
  Q_SLOT void updateLegendIcon(Qt::TransformationMode transformMode=Qt::SmoothTransformation, const QSize &thumbSize=QSize(32, 18));

  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

signals:
  void dataRangeChanged(QCPRange newRange);
  void dataScaleTypeChanged(QCPAxis::ScaleType scaleType);
  void gradientChanged(QCPColorGradient newGradient);

protected:
  // property members:
  QCPRange mDataRange;
  QCPAxis::ScaleType mDataScaleType;
  QCPColorMapData *mMapData;
  QCPColorGradient mGradient;
  bool mInterpolate;
  bool mTightBoundary;
  QPointer<QCPColorScale> mColorScale;
  // non-property members:
  QImage mMapImage;
  QPixmap mLegendIcon;
  bool mMapImageInvalidated;

  // introduced virtual methods:
  virtual void updateMapImage();

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;

  friend class QCustomPlot;
  friend class QCPLegend;
};


/*! \file */



class QCP_LIB_DECL QCPFinancialData
{
public:
  QCPFinancialData();
  QCPFinancialData(double key, double open, double high, double low, double close);
  double key, open, high, low, close;
};
Q_DECLARE_TYPEINFO(QCPFinancialData, Q_MOVABLE_TYPE);

/*! \typedef QCPFinancialDataMap
  Container for storing \ref QCPFinancialData items in a sorted fashion. The key of the map
  is the key member of the QCPFinancialData instance.

  This is the container in which QCPFinancial holds its data.
  \see QCPFinancial, QCPFinancial::setData
*/
typedef QMap<double, QCPFinancialData> QCPFinancialDataMap;
typedef QMapIterator<double, QCPFinancialData> QCPFinancialDataMapIterator;
typedef QMutableMapIterator<double, QCPFinancialData> QCPFinancialDataMutableMapIterator;


class QCP_LIB_DECL QCPFinancial : public QCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(ChartStyle chartStyle READ chartStyle WRITE setChartStyle)
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(bool twoColored READ twoColored WRITE setTwoColored)
  Q_PROPERTY(QBrush brushPositive READ brushPositive WRITE setBrushPositive)
  Q_PROPERTY(QBrush brushNegative READ brushNegative WRITE setBrushNegative)
  Q_PROPERTY(QPen penPositive READ penPositive WRITE setPenPositive)
  Q_PROPERTY(QPen penNegative READ penNegative WRITE setPenNegative)
  /// \endcond
public:
  /*!
    Defines the possible representations of OHLC data in the plot.

    \see setChartStyle
  */
  enum ChartStyle { csOhlc         ///< Open-High-Low-Close bar representation
                   ,csCandlestick  ///< Candlestick representation
                  };
  Q_ENUMS(ChartStyle)

  explicit QCPFinancial(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPFinancial();

  // getters:
  QCPFinancialDataMap *data() const { return mData; }
  ChartStyle chartStyle() const { return mChartStyle; }
  double width() const { return mWidth; }
  bool twoColored() const { return mTwoColored; }
  QBrush brushPositive() const { return mBrushPositive; }
  QBrush brushNegative() const { return mBrushNegative; }
  QPen penPositive() const { return mPenPositive; }
  QPen penNegative() const { return mPenNegative; }


  // setters:
  void setData(QCPFinancialDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close);
  void setChartStyle(ChartStyle style);
  void setWidth(double width);
  void setTwoColored(bool twoColored);
  void setBrushPositive(const QBrush &brush);
  void setBrushNegative(const QBrush &brush);
  void setPenPositive(const QPen &pen);
  void setPenNegative(const QPen &pen);

  // non-property methods:
  void addData(const QCPFinancialDataMap &dataMap);
  void addData(const QCPFinancialData &data);
  void addData(double key, double open, double high, double low, double close);
  void addData(const QVector<double> &key, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);

  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  // static methods:
  static QCPFinancialDataMap timeSeriesToOhlc(const QVector<double> &time, const QVector<double> &value, double timeBinSize, double timeBinOffset = 0);

protected:
  // property members:
  QCPFinancialDataMap *mData;
  ChartStyle mChartStyle;
  double mWidth;
  bool mTwoColored;
  QBrush mBrushPositive, mBrushNegative;
  QPen mPenPositive, mPenNegative;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
  virtual QCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual QCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;

  // non-virtual methods:
  void drawOhlcPlot(QCPPainter *painter, const QCPFinancialDataMap::const_iterator &begin, const QCPFinancialDataMap::const_iterator &end);
  void drawCandlestickPlot(QCPPainter *painter, const QCPFinancialDataMap::const_iterator &begin, const QCPFinancialDataMap::const_iterator &end);
  double ohlcSelectTest(const QPointF &pos, const QCPFinancialDataMap::const_iterator &begin, const QCPFinancialDataMap::const_iterator &end) const;
  double candlestickSelectTest(const QPointF &pos, const QCPFinancialDataMap::const_iterator &begin, const QCPFinancialDataMap::const_iterator &end) const;
  void getVisibleDataBounds(QCPFinancialDataMap::const_iterator &lower, QCPFinancialDataMap::const_iterator &upper) const;

  friend class QCustomPlot;
  friend class QCPLegend;
};


class QCP_LIB_DECL QCPItemStraightLine : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  /// \endcond
public:
  QCPItemStraightLine(QCustomPlot *parentPlot);
  virtual ~QCPItemStraightLine();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const point1;
  QCPItemPosition * const point2;

protected:
  // property members:
  QPen mPen, mSelectedPen;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);

  // non-virtual methods:
  double distToStraightLine(const QVector2D &point1, const QVector2D &vec, const QVector2D &point) const;
  QLineF getRectClippedStraightLine(const QVector2D &point1, const QVector2D &vec, const QRect &rect) const;
  QPen mainPen() const;
};


class QCP_LIB_DECL QCPItemLine : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QCPLineEnding head READ head WRITE setHead)
  Q_PROPERTY(QCPLineEnding tail READ tail WRITE setTail)
  /// \endcond
public:
  QCPItemLine(QCustomPlot *parentPlot);
  virtual ~QCPItemLine();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QCPLineEnding head() const { return mHead; }
  QCPLineEnding tail() const { return mTail; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const QCPLineEnding &head);
  void setTail(const QCPLineEnding &tail);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const start;
  QCPItemPosition * const end;

protected:
  // property members:
  QPen mPen, mSelectedPen;
  QCPLineEnding mHead, mTail;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);

  // non-virtual methods:
  QLineF getRectClippedLine(const QVector2D &start, const QVector2D &end, const QRect &rect) const;
  QPen mainPen() const;
};


class QCP_LIB_DECL QCPItemCurve : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QCPLineEnding head READ head WRITE setHead)
  Q_PROPERTY(QCPLineEnding tail READ tail WRITE setTail)
  /// \endcond
public:
  QCPItemCurve(QCustomPlot *parentPlot);
  virtual ~QCPItemCurve();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QCPLineEnding head() const { return mHead; }
  QCPLineEnding tail() const { return mTail; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const QCPLineEnding &head);
  void setTail(const QCPLineEnding &tail);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const start;
  QCPItemPosition * const startDir;
  QCPItemPosition * const endDir;
  QCPItemPosition * const end;

protected:
  // property members:
  QPen mPen, mSelectedPen;
  QCPLineEnding mHead, mTail;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);

  // non-virtual methods:
  QPen mainPen() const;
};


class QCP_LIB_DECL QCPItemRect : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  /// \endcond
public:
  QCPItemRect(QCustomPlot *parentPlot);
  virtual ~QCPItemRect();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const topLeft;
  QCPItemPosition * const bottomRight;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRight;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeft;
  QCPItemAnchor * const left;

protected:
  enum AnchorIndex {aiTop, aiTopRight, aiRight, aiBottom, aiBottomLeft, aiLeft};

  // property members:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;

  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class QCP_LIB_DECL QCPItemText : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QColor color READ color WRITE setColor)
  Q_PROPERTY(QColor selectedColor READ selectedColor WRITE setSelectedColor)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  Q_PROPERTY(QFont font READ font WRITE setFont)
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(Qt::Alignment positionAlignment READ positionAlignment WRITE setPositionAlignment)
  Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment)
  Q_PROPERTY(double rotation READ rotation WRITE setRotation)
  Q_PROPERTY(QMargins padding READ padding WRITE setPadding)
  /// \endcond
public:
  QCPItemText(QCustomPlot *parentPlot);
  virtual ~QCPItemText();

  // getters:
  QColor color() const { return mColor; }
  QColor selectedColor() const { return mSelectedColor; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  QFont font() const { return mFont; }
  QFont selectedFont() const { return mSelectedFont; }
  QString text() const { return mText; }
  Qt::Alignment positionAlignment() const { return mPositionAlignment; }
  Qt::Alignment textAlignment() const { return mTextAlignment; }
  double rotation() const { return mRotation; }
  QMargins padding() const { return mPadding; }

  // setters;
  void setColor(const QColor &color);
  void setSelectedColor(const QColor &color);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setSelectedFont(const QFont &font);
  void setText(const QString &text);
  void setPositionAlignment(Qt::Alignment alignment);
  void setTextAlignment(Qt::Alignment alignment);
  void setRotation(double degrees);
  void setPadding(const QMargins &padding);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const position;
  QCPItemAnchor * const topLeft;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRight;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottomRight;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeft;
  QCPItemAnchor * const left;

protected:
  enum AnchorIndex {aiTopLeft, aiTop, aiTopRight, aiRight, aiBottomRight, aiBottom, aiBottomLeft, aiLeft};

  // property members:
  QColor mColor, mSelectedColor;
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  QFont mFont, mSelectedFont;
  QString mText;
  Qt::Alignment mPositionAlignment;
  Qt::Alignment mTextAlignment;
  double mRotation;
  QMargins mPadding;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;

  // non-virtual methods:
  QPointF getTextDrawPoint(const QPointF &pos, const QRectF &rect, Qt::Alignment positionAlignment) const;
  QFont mainFont() const;
  QColor mainColor() const;
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class QCP_LIB_DECL QCPItemEllipse : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  /// \endcond
public:
  QCPItemEllipse(QCustomPlot *parentPlot);
  virtual ~QCPItemEllipse();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const topLeft;
  QCPItemPosition * const bottomRight;
  QCPItemAnchor * const topLeftRim;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRightRim;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottomRightRim;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeftRim;
  QCPItemAnchor * const left;
  QCPItemAnchor * const center;

protected:
  enum AnchorIndex {aiTopLeftRim, aiTop, aiTopRightRim, aiRight, aiBottomRightRim, aiBottom, aiBottomLeftRim, aiLeft, aiCenter};

  // property members:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;

  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class QCP_LIB_DECL QCPItemPixmap : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
  Q_PROPERTY(bool scaled READ scaled WRITE setScaled)
  Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode)
  Q_PROPERTY(Qt::TransformationMode transformationMode READ transformationMode)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  /// \endcond
public:
  QCPItemPixmap(QCustomPlot *parentPlot);
  virtual ~QCPItemPixmap();

  // getters:
  QPixmap pixmap() const { return mPixmap; }
  bool scaled() const { return mScaled; }
  Qt::AspectRatioMode aspectRatioMode() const { return mAspectRatioMode; }
  Qt::TransformationMode transformationMode() const { return mTransformationMode; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }

  // setters;
  void setPixmap(const QPixmap &pixmap);
  void setScaled(bool scaled, Qt::AspectRatioMode aspectRatioMode=Qt::KeepAspectRatio, Qt::TransformationMode transformationMode=Qt::SmoothTransformation);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const topLeft;
  QCPItemPosition * const bottomRight;
  QCPItemAnchor * const top;
  QCPItemAnchor * const topRight;
  QCPItemAnchor * const right;
  QCPItemAnchor * const bottom;
  QCPItemAnchor * const bottomLeft;
  QCPItemAnchor * const left;

protected:
  enum AnchorIndex {aiTop, aiTopRight, aiRight, aiBottom, aiBottomLeft, aiLeft};

  // property members:
  QPixmap mPixmap;
  QPixmap mScaledPixmap;
  bool mScaled;
  Qt::AspectRatioMode mAspectRatioMode;
  Qt::TransformationMode mTransformationMode;
  QPen mPen, mSelectedPen;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;

  // non-virtual methods:
  void updateScaledPixmap(QRect finalRect=QRect(), bool flipHorz=false, bool flipVert=false);
  QRect getFinalRect(bool *flippedHorz=0, bool *flippedVert=0) const;
  QPen mainPen() const;
};


class QCP_LIB_DECL QCPItemTracer : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  Q_PROPERTY(double size READ size WRITE setSize)
  Q_PROPERTY(TracerStyle style READ style WRITE setStyle)
  Q_PROPERTY(QCPGraph* graph READ graph WRITE setGraph)
  Q_PROPERTY(double graphKey READ graphKey WRITE setGraphKey)
  Q_PROPERTY(bool interpolating READ interpolating WRITE setInterpolating)
  /// \endcond
public:
  /*!
    The different visual appearances a tracer item can have. Some styles size may be controlled with \ref setSize.

    \see setStyle
  */
  enum TracerStyle { tsNone        ///< The tracer is not visible
                     ,tsPlus       ///< A plus shaped crosshair with limited size
                     ,tsCrosshair  ///< A plus shaped crosshair which spans the complete axis rect
                     ,tsCircle     ///< A circle
                     ,tsSquare     ///< A square
                   };
  Q_ENUMS(TracerStyle)

  QCPItemTracer(QCustomPlot *parentPlot);
  virtual ~QCPItemTracer();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  double size() const { return mSize; }
  TracerStyle style() const { return mStyle; }
  QCPGraph *graph() const { return mGraph; }
  double graphKey() const { return mGraphKey; }
  bool interpolating() const { return mInterpolating; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setSize(double size);
  void setStyle(TracerStyle style);
  void setGraph(QCPGraph *graph);
  void setGraphKey(double key);
  void setInterpolating(bool enabled);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  // non-virtual methods:
  void updatePosition();

  QCPItemPosition * const position;

protected:
  // property members:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  double mSize;
  TracerStyle mStyle;
  QCPGraph *mGraph;
  double mGraphKey;
  bool mInterpolating;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);

  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class QCP_LIB_DECL QCPItemBracket : public QCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(double length READ length WRITE setLength)
  Q_PROPERTY(BracketStyle style READ style WRITE setStyle)
  /// \endcond
public:
  enum BracketStyle { bsSquare  ///< A brace with angled edges
                      ,bsRound  ///< A brace with round edges
                      ,bsCurly  ///< A curly brace
                      ,bsCalligraphic ///< A curly brace with varying stroke width giving a calligraphic impression
  };

  QCPItemBracket(QCustomPlot *parentPlot);
  virtual ~QCPItemBracket();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  double length() const { return mLength; }
  BracketStyle style() const { return mStyle; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setLength(double length);
  void setStyle(BracketStyle style);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  QCPItemPosition * const left;
  QCPItemPosition * const right;
  QCPItemAnchor * const center;

protected:
  // property members:
  enum AnchorIndex {aiCenter};
  QPen mPen, mSelectedPen;
  double mLength;
  BracketStyle mStyle;

  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;

  // non-virtual methods:
  QPen mainPen() const;
};

#endif // QCUSTOMPLOT_H

