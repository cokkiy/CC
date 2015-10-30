#ifndef CODESOURCE_H
#define CODESOURCE_H
#include <QWidget>
#include <QTextEdit>
#include <QByteArray>
#include<QTimerEvent>
#include <Net/NetComponents>

class codeSource : public QTextEdit
{
    Q_OBJECT
    //1.参数
    Q_PROPERTY(QString data READ getData WRITE setData)

    //2.矩形中的文本属性
    Q_PROPERTY(QFont font READ font WRITE setFont)//1、设置文本字体    
    Q_PROPERTY(Qt::Alignment alignment READ txtalignment
               WRITE settxtalignment )//3.文本对齐方式
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)//2.文本颜色
    //3.背景颜色
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE
               setBackgroundColor NOTIFY backgroundColorChanged)
    //4.源码间空格个数
    Q_PROPERTY(int blankNum READ getBlankNum WRITE setBlankNum)//blank number

    //5.图元背景矩形坐标(x,y)width and height
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)

public:
    codeSource(QWidget *parent = 0);
    virtual~codeSource();

    Qt::Alignment txtalignment() const{return m_alignMent;}
    void settxtalignment(const Qt::Alignment newtxt);

    QColor backgroundColor() const{return m_bgcolor;}
    void setBackgroundColor(const QColor newbgcolor);

    QString getData() const{return m_data;}
    void setData(const QString newdata);

    int getBlankNum() const{return blankNum;}
    void setBlankNum(const int blanknum);

signals:
    void textChanged(QString);
    void backgroundColorChanged(const QColor &);
    void txtalignmentChanged();

private:
    QString m_data;
    QString disData;
    int blankNum;
    QColor m_bgcolor;
    Qt::Alignment m_alignMent;
    // //定时器 用于更新图形绘制
    TimerInterface* m_timer;
    DataCenterInterface* m_dc;//
    bool hasStyleSheetUpdate;

protected:
    void handleData();//写入数据到text

public slots:
    //样式表
    void stylesheetUpdate();//用于更新背景颜色    
    virtual void paintEvent(QPaintEvent *e);
private slots:
    //事件-定时器触发
    void getDataEvent();
};

#endif
