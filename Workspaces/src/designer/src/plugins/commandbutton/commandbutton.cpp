﻿#include "commandbutton.h"

#include <QComboBox>
#include <QVBoxLayout>
#include <QUdpSocket>
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>
#include <QRegExpValidator>

struct struct_FrameDate
{
    unsigned	Day			: 4;//日
    unsigned	Day_10		: 4;//十日
    unsigned	Month		: 4;//月
    unsigned	Month_10	: 4;//十月
    unsigned 	Year		: 4;//年
    unsigned 	Year_10		: 4;//十年
    unsigned 	Year_100	: 4;//百年
    unsigned 	Year_1000	: 4;//千年
};

struct struct_FrameHead
{
    quint8 frameType;		//帧类别
    quint8 dataType;		//信息类别
    quint16 taskCode;		//任务代号
    quint16 dataLength;	//数长
    quint8 dataSource;	//信源
    quint8 dataDest;		//信宿
    quint32 frameCount;	//帧序号
    struct_FrameDate date;	//日期
    quint32 time;			//时间
    quint32 backup;			//备用
};

CommandButton::CommandButton(QWidget *parent) :
    QWidget(parent)
{
    pushButton = new QPushButton(this);
    pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    comboBox = new QComboBox(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(pushButton);
    layout->addWidget(comboBox);  

//    setLayout(layout);
    comboBox->setVisible(false);

    m_normalImage = QUrl::fromUserInput(":/resource/button_normal.png");
    m_hoverImage = QUrl::fromUserInput(":/resource/button_hover.png");
    m_pressedImage = QUrl::fromUserInput(":/resource/button_pressed.png");
    m_disabledImage = QUrl::fromUserInput("://resource/button_disabled.png");
    m_focusImage = QUrl::fromUserInput(":/resource/button_focused.png");

//    ipRegExp = QRegExp("((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}"
//                       "(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)");

    this->setDestAddress("228.1.1.2");
    this->setPort(24704);

    m_command = OneCommand;
    hasReceiveValue = false;
    hasStylesheetUpdate = false;
    m_timer_id = 0;

    commandRegExp = QRegExp("^(\\d+)\\s*(?::(.*))?$");

//    regExp = new QRegExp("((25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)\.){3}"
//                         "(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)");
//    regExp = new QRegExp("\d");

    connect(this, SIGNAL(commandChanged()), this, SLOT(setComboBox()));
    connect(this, SIGNAL(commandListChanged()), this, SLOT(setComboBox()));
    connect(this, SIGNAL(textColorChanged()), this, SLOT(update()));
    connect(pushButton, SIGNAL(clicked()), this, SLOT(sendDatagram()));

    net = NetComponents::getInforCenter();
    datacenter = NetComponents::getDataCenter();

    QUdpSocket udpSocket2;
    pushButton->setText(udpSocket2.localAddress().toString());

}

CommandButton::~CommandButton()
{
    if (m_timer_id != 0) {
        killTimer(m_timer_id);
        m_timer_id = 0;
    }

//    if (net) {
//        delete net;
//        net = NULL;
//    }
    if (datacenter) {
        delete datacenter;
        datacenter = NULL;
    }
}

void CommandButton::setComboBox()
{
    if (m_command == ListCommand) {
        comboBox->setVisible(true);
//        comboBox->clear();
        comboBox->addItems(m_commandList);
        if (m_timer_id != 0) {
            killTimer(m_timer_id);
            m_timer_id = 0;
        }
        hasReceiveValue = false;
    }
    else if (m_command == OneCommand) {
        comboBox->setVisible(false);
        if (m_timer_id != 0) {
            killTimer(m_timer_id);
            m_timer_id = 0;
        }
        hasReceiveValue = false;
    }
    else {
        comboBox->setVisible(false);
        m_timer_id = startTimer(500);
    }

}

void CommandButton::setCommand(const CommandChoice &newCommand)
{
    if (m_command != newCommand) {
        m_command = newCommand;
        emit commandChanged();
    }
}

void CommandButton::setSendData(const QString data)
{
    if (m_data != data) {
        m_data = data;
    }
}

void CommandButton::setReceiveData(QString data)
{
    if (m_secData != data) {
        m_secData = data;
    }
}

//void CommandButton::addItem(const QString &text)
//{
//    comboBox->addItem(text);
//}

void CommandButton::paintEvent(QPaintEvent *event)
{
    if (hasStylesheetUpdate == false) {
        stylesheetUpdate();
        hasStylesheetUpdate = true;
    }
    QWidget::paintEvent(event);
}

void CommandButton::timerEvent(QTimerEvent *event)
{
    if(datacenter->getValue(getReceiveData(),receiveValue) == 1)
    {
        bool haveMeaningInCommandList = false;
        QStringList::const_iterator commandIter;
        for(commandIter = m_commandList.begin(); commandIter != m_commandList.end(); ++commandIter){
            commandRegExp.indexIn(*commandIter);
            if (receiveValue == commandRegExp.cap(1).toDouble()){
                if (commandRegExp.cap(2).trimmed() != NULL)
                    pushButton->setText(commandRegExp.cap(2).trimmed());
                haveMeaningInCommandList = true;
                break;
            }
        }
        if (haveMeaningInCommandList == false)
            pushButton->setText(QString::number(receiveValue));
        hasReceiveValue = true;
    }
}

void CommandButton::mouseDoubleClickEvent(QMouseEvent *event)
{
//    pushButton->mouseDoubleClickEvent(event);
//    QPushButton::mouseDoubleClickEvent(event);
}

void CommandButton::setNormalImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_normalImage != tempUrl) {
        m_normalImage = tempUrl;
        hasStylesheetUpdate = false;
        emit imageChanged();
    }
}

void CommandButton::setHoverImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_hoverImage != tempUrl) {
        m_hoverImage = tempUrl;
        hasStylesheetUpdate = false;
        emit imageChanged();
    }
}

void CommandButton::setPressedImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_pressedImage != tempUrl) {
        m_pressedImage = tempUrl;
        hasStylesheetUpdate = false;
        emit imageChanged();
    }
}

void CommandButton::setDisabledImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_disabledImage != tempUrl) {
        m_disabledImage = tempUrl;
        hasStylesheetUpdate = false;
        emit imageChanged();
    }
}

void CommandButton::setFocusImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_focusImage != tempUrl) {
        m_focusImage = tempUrl;
        hasStylesheetUpdate = false;
        emit imageChanged();
    }
}

//更新样式表，包括更新字体颜色和背景图片
void CommandButton::stylesheetUpdate()
{
    QString strColor = QString("rgb(%1, %2, %3)").arg(QString::number(textColor().red()),
                                                      QString::number(textColor().green()),
                                                      QString::number(textColor().blue()));
    QString styleSheet = QString("QPushButton{border-image: url(%1);}").arg(m_normalImage.path()) +
                         QString("QPushButton:hover{border-image: url(%1);}").arg(m_hoverImage.path()) +
                         QString("QPushButton:pressed{border-image: url(%1);}").arg(m_pressedImage.path()) +
                         QString("QPushButton:!enabled{border-image: url(%1);}").arg(m_disabledImage.path()) +
                         QString("QPushButton:focus:!hover:!pressed{border-image: url(%1);}").arg(m_focusImage.path()) +
                         QString("QPushButton{color: %1}").arg(strColor);
    this->setStyleSheet(styleSheet);
}

void CommandButton::sendDatagram()
{
    //该正则表达式用来验证data格式是否规范
    QRegExp dataRegExp = QRegExp("\\[[1-9]\\d{0,4},[1-9]\\d*\\]");
    //验证data是否不为空且格式符合规范
    if (m_data == NULL) {
        QMessageBox::information(this, tr("Error"),
                          tr("Send data is empty!"),
                          QMessageBox::Ok);
        return;
    }
    else if (!dataRegExp.exactMatch(m_data)) {
        QMessageBox::information(this, tr("Error"),
                          tr("Send data is invalid!"),
                          QMessageBox::Ok);
        return;
    }
    //验证ip地址是否正确
    QHostAddress ipAddress;
    if (!ipAddress.setAddress(m_destAddress)) {
        QMessageBox::information(this, tr("Error"),
                          tr("IP address is invalid!"),
                          QMessageBox::Ok);
        return;
    }

    for(int i = 0; i < 2; i++) {
        param = net->getParam(getSendData());
        quint16 paramLenth = param->GetParamDataLen();
        uint size = 40 + paramLenth;
        char* pBuf = new char[size];
        memset(pBuf, 0, size);

        switch(m_command) {
        case OneCommand:
        {
            if (m_commandList.isEmpty()) {
                QMessageBox::information(this, tr("Error"),
                                  tr("Command is invalid!"),
                                  QMessageBox::Ok);
                return;
            }
            int pos = commandRegExp.indexIn(m_commandList.at(0));
            if (pos == -1) {
                QMessageBox::information(this, tr("Error"),
                                  tr("Command is invalid!"),
                                  QMessageBox::Ok);
                return;
            }
            char* pCmd = (char*)(pBuf+34);
            int t_i = commandRegExp.cap(1).toInt();
            *pCmd = t_i;
            break;
        }
        case NextOne:
        {
            if (m_secData == NULL) {
                QMessageBox::information(this, tr("Error"),
                                  tr("Receive data is empty!"),
                                  QMessageBox::Ok);
                return;
            }
            else if (!dataRegExp.exactMatch(m_secData)) {
                QMessageBox::information(this, tr("Error"),
                                  tr("Receive data is invalid!"),
                                  QMessageBox::Ok);
                return;
            }
            if (!hasReceiveValue) {
                QMessageBox::information(this, tr("No Data"),
                                         tr("Receive no data from receive parameter!"),
                                         QMessageBox::Ok);
                return;
            }
//            param2 = net->getParam(getReceiveData());

//            param2->getValue(value);
//            if (param->GetZXParamTranType() != param2->GetZXParamTranType()) {
//                QMessageBox::critical(this, tr("Error"),
//                                     tr("Send data has the different translation type with receive data!"),
//                                     QMessageBox::Ok);
//                return;
//            }

//            char* pCmd = (char*)(pBuf+34);
//            *pCmd = value;
//            break;
            switch(param->GetParamTranType())
            {
            case tp_char://char，1字节
            case tp_BYTE://Byte，1字节
            {
                char* pCmd = (char*)(pBuf+34);
                *pCmd = receiveValue;
                break;
            }
            case tp_short://short，2字节
            case tp_WORD://WORD, 2字节
            {
                quint16* pCmd = (quint16*)(pBuf+34);
                *pCmd = receiveValue;
                break;
            }
            case tp_long://long，4字节
            case tp_DWORD://DWORD，4字节
            case tp_PMTime://PMTime，4字节
            case tp_Date://Date，4字节
            case tp_BCDTime://BCDTime，4字节
            case tp_UTCTime://UTCTime，4字节
            {
                quint32* pCmd = (quint32*)(pBuf+34);
                *pCmd = receiveValue;
                break;
            }
            case tp_float://float，4字节
            {
                float* pCmd = (float*)(pBuf+34);
                *pCmd = receiveValue;
                break;
            }
            case tp_double://double, 8字节
            {
                double* pCmd = (double*)(pBuf+34);
                *pCmd = receiveValue;
                break;
            }
            }
            break;
        }
        case ListCommand:
        {
            //该正则表达式用来判断命令格式是否规范, 并提取命令码
            int pos = commandRegExp.indexIn(comboBox->currentText());
            if (pos == -1) {
                QMessageBox::information(this, tr("Error"),
                                  tr("Command is invalid!"),
                                  QMessageBox::Ok);
                return;
            }

            char* pCmd = (char*)(pBuf+34);
            *pCmd = commandRegExp.cap(1).toInt();
            break;
        }
        }

        struct_FrameHead* pHead = (struct_FrameHead*)pBuf;
        pHead->frameType = 0x01;
        pHead->dataType = 0x00;
        pHead->taskCode = 0x0108;
        pHead->dataLength = 12 + paramLenth;
        pHead->dataSource = 202;
        pHead->dataDest = 201;

        pHead->frameCount = i+1;
        pHead->backup = 0;

        QDateTime datetime = QDateTime::currentDateTime();
        pHead->date.Year_1000 = datetime.date().year() / 1000;
        pHead->date.Year_100 = (datetime.date().year() % 1000) / 100;
        pHead->date.Year_10 = (datetime.date().year() % 100) / 10;
        pHead->date.Year = datetime.date().year() % 10;
        pHead->date.Month_10 = datetime.date().month() / 10;
        pHead->date.Month = datetime.date().month() % 10;
        pHead->date.Day_10 = datetime.date().day() / 10;
        pHead->date.Day = datetime.date().day() % 10;

        pHead->time = (datetime.time().hour() * 3600 + datetime.time().minute() * 60 +
                datetime.time().second()) * 10000 + datetime.time().msec() * 10;

        quint16* zzlen = (quint16*)(pBuf+24);
        *zzlen = paramLenth + 2;
        quint32* zztime = (quint32*)(pBuf+26);
        *zztime = pHead->time;
        quint16* pTable = (quint16*)(pBuf+30);
        *pTable = param->GetTableNo();
        quint16* pCode = (quint16*)(pBuf+32);
        *pCode = param->GetParamNo();

        quint16* wb = (quint16*)(pBuf+34+paramLenth);
        *wb = 0xffff;
        quint32* zwb = (quint32*)(pBuf+36+paramLenth);
        *zwb = 0x89abcdef;
        udpSocket.writeDatagram(pBuf, size, ipAddress, m_port);
    }
}

