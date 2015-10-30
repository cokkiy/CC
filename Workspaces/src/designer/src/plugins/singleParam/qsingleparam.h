#ifndef QSINGLEPARAM_H
#define QSINGLEPARAM_H
#include <Net/NetComponents>
#include <QWidget>
#include <QtGui>
class QSingleParam : public QWidget
{
    Q_OBJECT
    //文本属性
    Q_PROPERTY(QString data READ textString WRITE setTextString RESET resetData)
    Q_PROPERTY(QString showType READ getShowType WRITE setShowType)
    Q_PROPERTY(QFont font READ getFont WRITE setFont)
    Q_PROPERTY(Qt::Alignment alignment READ getAlignment WRITE setAlignment)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor )
    //插件框架属性
    Q_PROPERTY(QBrush backgroundColor READ getBackgroundBrush WRITE setBackgroundBrush )

    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)
    Q_PROPERTY(qint32 borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(Qt::PenStyle borderStyle READ getBorderStyle WRITE setBorderStyle)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)

protected:
     void paintEvent(QPaintEvent *event);
public:
    //文本属性
     QString textString() const {return m_textString;}
     void setTextString(const QString string);
     void resetData();

     QString getShowType() const {return showType;}
     void setShowType(const QString string);

     QFont getFont() const {return font;}
     void setFont(const QFont Font);

     Qt::Alignment getAlignment() const {return alignment;}
     void setAlignment(const Qt::Alignment flag);

     QColor getTextColor() const {return textColor;}
     void setTextColor(const QColor Color);

     QBrush getBackgroundBrush() const {return backgroundColor;}
     void setBackgroundBrush(const QBrush Brush);

    //插件框架属性
     QColor getBorderColor() const {return borderColor;}
     void setBorderColor(const QColor Color);

     qint32 getBorderWidth() const {return borderWidth;}
     void setBorderWidth(const qint32 w);

     Qt::PenStyle getBorderStyle() const {return borderStyle;}
     void setBorderStyle(const Qt::PenStyle style);

private:
    //文本属性
    QString m_textString;//输入参数
    QString showType;
    QString dispString;//显示参数
    QString resultString;//显示参数
    QFont font;//文本字体
    Qt::Alignment alignment;//文本对齐方式
    QColor textColor;//文本颜色
    //插件框架属性
    QBrush backgroundColor;//插件背景颜色
    QColor borderBrush;//边框画笔
    QColor borderColor;//插件边框颜色
    qint32 borderWidth;//插件边框宽度
    Qt::PenStyle borderStyle;//插件边框画笔样式
private:
    DataCenterInterface* dc;
    QRect pluginRect;//插件大小区域
    QPen textPen;//文本画笔
    int m_timer_id;
protected slots:
    void timerEvent(QTimerEvent *);
private:
    void showPluginFrame();//显示插件矩形区域
    void showPluginText();//显示插件文本
    void showPlugin();//显示插件
    QPen setCustomPen(Qt::PenStyle style,QColor color,uint width);//设置自定义画笔
    void setPluginRect();//设置插件矩形区域和背景区域
	//解析显示字符串，字符串中包括字体、颜色、文本
    void analyzeDisplayStr(QFont &,QColor &);
public:
    QSingleParam(QWidget *parent = 0);
    ~QSingleParam();
};

#endif
