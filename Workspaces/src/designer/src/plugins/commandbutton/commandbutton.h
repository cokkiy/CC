#ifndef COMMANDBUTTON_H
#define COMMANDBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <Net/NetComponents>
#include <QStringList>
#include <QUrl>

class QRegExpValidator;
class QComboBox;

class CommandButton : public QPushButton
{
    Q_OBJECT

    //仅可设置send_data和receive_data, 其中将命令发送至send_data, 从receive_data接收数据
    Q_PROPERTY(QString send_data READ getSendData WRITE setSendData)
    Q_PROPERTY(QString receive_data READ getReceiveData WRITE setReceiveData)

    //设置命令按钮的发送模式，分为“单条指令”、“下一口令”、“列表发令”三种
    Q_PROPERTY(CommandChoice commandtype READ command WRITE setCommand NOTIFY commandChanged)

    //设置发送的命令列表, 在命令类型为“单条指令”时发出CommandList的第一条命令
    //在命令类型为“列表发令”时可选择CommandList中的命令进行发送
    Q_PROPERTY(QStringList commandList READ commandList WRITE setCommandList NOTIFY commandListChanged)

    //设置给指定主机发送数据包的IP地址
    Q_PROPERTY(QString destAddress READ destAddress WRITE setDestAddress)
    //设置UDP数据包的传输端口
    Q_PROPERTY(uint port READ port WRITE setPort)

    //读取图片，用于更换不同状态下显示的背景图片
    Q_PROPERTY(QUrl normalImage READ normalImage WRITE setNormalImage)
    Q_PROPERTY(QUrl hoverImage READ hoverImage WRITE setHoverImage)
    Q_PROPERTY(QUrl pressedImage READ pressedImage WRITE setPressedImage)
    Q_PROPERTY(QUrl disabledImage READ disabledImage WRITE setDisabledImage)
    Q_PROPERTY(QUrl focusImage READ focusImage WRITE setFocusImage)

    //设置按钮显示文本，字体及字体颜色
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)

    //设置按钮的位置信息
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

    Q_ENUMS(CommandChoice)

public:
    enum CommandChoice {OneCommand, NextOne, ListCommand };

    CommandButton(QWidget *parent = 0);

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

private slots:
    void setComboBox();
    void stylesheetUpdate();
    void sendDatagram();

private:
    void addItem(const QString &text);

    QString m_data;
    QString m_secData;
    QString m_file;
    QString m_destAddress;
    QStringList m_commandList;
    uint m_port;
    QColor m_textColor;
    CommandChoice m_command;
    QComboBox *comboBox;
    QUrl m_normalImage;
    QUrl m_hoverImage;
    QUrl m_pressedImage;
    QUrl m_disabledImage;
    QUrl m_focusImage;

    InformationInterface* net;
    AbstractParam* param;
    AbstractParam* param2;
};

#endif

inline void CommandButton::setTextColor(const QColor &color)
{
    m_textColor = color;
    emit textColorChanged();
}


inline void CommandButton::setDestAddress(const QString &address)
{
    m_destAddress = address;
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
