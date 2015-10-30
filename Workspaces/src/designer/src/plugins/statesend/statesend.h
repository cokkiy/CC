#ifndef STATESEND_H
#define STATESEND_H

#include <QTableWidget>
#include <Net/NetComponents>

class StateSend : public QTableWidget
{
    Q_OBJECT
    Q_PROPERTY(QString dataes READ getDataes WRITE setDataes NOTIFY dataesChanged)
    Q_PROPERTY(QColor normalColor READ normalColor WRITE setBeforeClickedColor)
    Q_PROPERTY(QColor pressedColor READ pressedColor WRITE setClickedColor)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    //设置给指定主机发送数据包的IP地址
    Q_PROPERTY(QString destAddress READ destAddress WRITE setDestAddress)
    //设置UDP数据包的传输端口
    Q_PROPERTY(uint port READ port WRITE setPort)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)

public:
    StateSend(QWidget *parent = 0);

    QString getDataes() const { return m_data; }
    void setDataes(const QString &data);

    QColor normalColor() const { return m_beforeClickedColor; }
    void setBeforeClickedColor(const QColor &color);

    QColor pressedColor() const { return m_clickedColor; }
    void setClickedColor(const QColor &color);

    QColor textColor() const { return m_textColor; }
    void setTextColor(const QColor &newColor);

    QColor backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const QColor &newColor);

    QString destAddress() const { return m_destAddress; }
    void setDestAddress(const QString &address);

    uint port() const { return m_port; }
    void setPort(const uint &newPort);

signals:
    void dataesChanged();

public slots:
    void loadData();
    void changeBeforeClickedColor();
    void sendState(int row, int column);

protected:
    void paintEvent(QPaintEvent *event);

private:
    //
    void intialColor(int row);

    QString m_data;
    QColor m_beforeClickedColor;
    QColor m_clickedColor;

    QColor m_textColor;
    QColor m_backgroundColor;
    //clickedItem用来表示被点击的动作, 0表示没有任何动作被点击,
    //1表示开始动作被点击, 2表示完成动作被点击, 3表示未进行动作被点击
//    QVector<quint8> clickedItem;

    QStringList rangeStr;
    QStringList titleStr;
    QStringList theoryValueStr;
    QStringList noteStr;

    QStringList m_dataList;
    QList<int> m_paramTableNoList;
    QList<int> m_paramNoList;

    QString m_destAddress;
    uint m_port;

    InformationInterface* net;
    AbstractParam* param;

    bool hasUpdate;
};

inline void StateSend::setDestAddress(const QString &address)
{
    m_destAddress = address;
}

inline void StateSend::setPort(const uint &newPort)
{
    m_port = newPort;
}

#endif
