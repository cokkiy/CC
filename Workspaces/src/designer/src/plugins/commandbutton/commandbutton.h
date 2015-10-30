#ifndef COMMANDBUTTON_H
#define COMMANDBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <Net/NetComponents>
#include <QStringList>
#include <QUrl>
#include <QUdpSocket>

class QRegExpValidator;
class QComboBox;

class CommandButton : public QWidget
{
    Q_OBJECT

    //仅可设置send_data和receive_data, 其中将命令发送至send_data, 从receive_data接收数据
    Q_PROPERTY(QString sendData READ getSendData WRITE setSendData)
    Q_PROPERTY(QString receiveData READ getReceiveData WRITE setReceiveData)
    //设置命令按钮的发送模式，分为“单条指令”、“下一口令”、“列表发令”三种
    Q_PROPERTY(CommandChoice type READ command WRITE setCommand NOTIFY commandChanged)
    //设置发送的命令列表, 在命令类型为“单条指令”时发出CommandList的第一条命令
    //在命令类型为“列表发令”时可选择CommandList中的命令进行发送
    Q_PROPERTY(QStringList dataList READ commandList WRITE setCommandList NOTIFY commandListChanged)
    //设置给指定主机发送数据包的IP地址
    Q_PROPERTY(QString destAddress READ destAddress WRITE setDestAddress)
    //设置本机IP地址
    Q_PROPERTY(QString localAddress READ localAddress WRITE setLocalAddress)
    //设置UDP数据包的传输端口
    Q_PROPERTY(uint port READ port WRITE setPort)
    //设置按钮显示文本，字体及字体颜色
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    //读取图片，用于更换不同状态下显示的背景图片
    Q_PROPERTY(QUrl normalImage READ normalImage WRITE setNormalImage)
    Q_PROPERTY(QUrl hoverImage READ hoverImage WRITE setHoverImage)
    Q_PROPERTY(QUrl pressedImage READ pressedImage WRITE setPressedImage)
    Q_PROPERTY(QUrl disabledImage READ disabledImage WRITE setDisabledImage)
    Q_PROPERTY(QUrl focusImage READ focusImage WRITE setFocusImage)
    //设置按钮的位置信息
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)

    Q_ENUMS(CommandChoice)

public:
    enum CommandChoice {OneCommand, NextOne, ListCommand };

    CommandButton(QWidget *parent = 0);
    virtual ~CommandButton();

    QString text() const { return pushButton->text(); }
    void setText(const QString &newText);

    QFont font() const { return pushButton->font(); }
    void setFont(const QFont&);

    QColor textColor() const { return m_textColor; }
    void setTextColor(const QColor &color);

    CommandChoice command() const{ return m_command; }
    void setCommand(const CommandChoice &newCommand);

    QString getSendData() const {return m_data;}
    void setSendData(const QString data);

    QString getReceiveData() const {return m_secData;}
    void setReceiveData(QString data);

    QString destAddress() const { return m_destAddress; }
    void setDestAddress(const QString &address);

    QString localAddress() const { return m_localAddress; }
    void setLocalAddress(const QString &address);

    uint port() const { return m_port; }
    void setPort(const uint &newPort);

    QStringList commandList() const { return m_commandList; }
    void setCommandList(const QStringList &newCommandList);

    QUrl normalImage() const { return m_normalImage; }
    QUrl hoverImage() const { return m_hoverImage; }
    QUrl pressedImage() const { return m_pressedImage; }
    QUrl disabledImage() const { return m_disabledImage; }
    QUrl focusImage() const { return m_focusImage; }
    void setNormalImage(const QUrl &newUrl);
    void setHoverImage(const QUrl &newUrl);
    void setPressedImage(const QUrl &newUrl);
    void setDisabledImage(const QUrl &newUrl);
    void setFocusImage(const QUrl &newUrl);

signals:
    void commandChanged();
    void textColorChanged();
    void fileChanged();
    void commandListChanged();
    void imageChanged();
//    void localAddressChanged(QString address);

protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void setComboBox();
    void stylesheetUpdate();
    void sendDatagram();

private:
//    void addItem(const QString &text);
    QPushButton *pushButton;
    QComboBox *comboBox;

    QString m_data;
    QString m_secData;
    QString m_file;
    QString m_destAddress;
    QString m_localAddress;
    QStringList m_commandList;
    uint m_port;
    QColor m_textColor;
    CommandChoice m_command;

    QUrl m_normalImage;
    QUrl m_hoverImage;
    QUrl m_pressedImage;
    QUrl m_disabledImage;
    QUrl m_focusImage;

    QRegExp commandRegExp;

    InformationInterface* net;
    DataCenterInterface* datacenter;
    AbstractParam* param;
    AbstractParam* param2;
    int m_timer_id;
    double receiveValue;
    bool hasReceiveValue;
    bool hasStylesheetUpdate;

    QUdpSocket udpSocket;
};

#endif

inline void CommandButton::setText(const QString &newText)
{
    pushButton->setText(newText);
}

inline void CommandButton::setFont(const QFont &newFont)
{
    pushButton->setFont(newFont);
}

inline void CommandButton::setTextColor(const QColor &color)
{
    if (m_textColor != color) {
        m_textColor = color;
        hasStylesheetUpdate = false;
        emit textColorChanged();
    }
}

inline void CommandButton::setDestAddress(const QString &address)
{
    m_destAddress = address;
}

inline void CommandButton::setLocalAddress(const QString &address)
{
    if (m_localAddress != address) {
        m_localAddress = address;
        QHostAddress ipAddress;
        if (ipAddress.setAddress(m_localAddress)) {
            udpSocket.bind(ipAddress);
        }
    }
}

inline void CommandButton::setPort(const uint &newPort)
{
    m_port = newPort;
}

inline void CommandButton::setCommandList(const QStringList &newCommandList)
{
    if (m_commandList != newCommandList) {
        m_commandList = newCommandList;
        emit commandListChanged();
    }
}
