#ifndef QSINGLEPARAM_H
#define QSINGLEPARAM_H
#include <Net/NetComponents>
#include <QWidget>
#include <QtGui>
class QSingleParam : public QWidget
{
    Q_OBJECT
    //文本属性
    Q_PROPERTY(QString data READ textString WRITE setTextString RESET rstobj)
    Q_PROPERTY(QString showType READ getShowType WRITE setShowType)
    Q_PROPERTY(QFont font READ getFont WRITE setFont)
    Q_PROPERTY(Qt::Alignment alignment READ getAlignment WRITE setAlignment)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor )
    //插件框架属性
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor )

    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)
    Q_PROPERTY(qint32 borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(Qt::PenStyle borderStyle READ getBorderStyle WRITE setBorderStyle)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
protected:
     void paintEvent(QPaintEvent *event);
public:
    //文本属性
     QString textString() const {return m_textString;}
     void setTextString(const QString string);
     void rstobj(){};

     QString getShowType() const {return showType;}
     void setShowType(const QString string);

     QFont getFont() const {return font;}
     void setFont(const QFont Font);

     Qt::Alignment getAlignment() const {return alignment;}
     void setAlignment(const Qt::Alignment flag);

     QColor getTextColor() const {return textColor;}
     void setTextColor(const QColor Color);

     QColor getBackgroundColor() const {return backgroundColor;}
     void setBackgroundColor(const QColor Color);

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

    QFont font;//文本字体
    Qt::Alignment alignment;//文本对齐方式
    QColor textColor;//文本颜色

    //插件框架属性
    QColor backgroundColor;//插件背景颜色
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
    void timerEvent(QTimerEvent *event);
private:
    void ShowPluginFrame();//显示插件矩形区域
    void ShowPluginText();//显示插件文本
    void ShowPlugin();//显示插件
    QPen SetCustomPen(Qt::PenStyle style,QColor color,uint width);//设置自定义画笔
	void SetPluginRect();//设置插件矩形区域和背景区域
public:
    QSingleParam(QWidget *parent = 0);
    ~QSingleParam();
};

#endif
