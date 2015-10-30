#include "statesend.h"

#include <QHeaderView>
#include <QHostAddress>
#include <QUdpSocket>
#include <QDateTime>
#include <QMessageBox>

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

StateSend::StateSend(QWidget *parent) :
    QTableWidget(parent)
{
    setColumnCount(7);
    setBeforeClickedColor(Qt::green);
    setClickedColor(Qt::gray);

    QStringList horizontalHeaderLabel;
    horizontalHeaderLabel <<  "时段" << "动作名称" << "" << ""
                           << "" << "开始时间" << "对应图像或参数";
    setHorizontalHeaderLabels(horizontalHeaderLabel);
    setSelectionMode(QAbstractItemView::NoSelection	);

    this->setDestAddress("228.1.1.2");
    this->setPort(24704);

    net = NetComponents::getInforCenter();
//    param = net->getParam(getData());
//    loadData();

    if (m_data != NULL) {
        loadData();
    }

//    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    hasUpdate = false;

    connect(this, SIGNAL(dataesChanged()), this, SLOT(loadData()));
    connect(this, SIGNAL(cellClicked(int,int)), this, SLOT(sendState(int,int)));
}

void StateSend::setDataes(const QString &data)
{
    if (m_data != data) {
        m_data = data;
        emit dataesChanged();
    }
}

void StateSend::setBeforeClickedColor(const QColor &color)
{
    if (m_beforeClickedColor != color) {
        m_beforeClickedColor = color;
        changeBeforeClickedColor();
    }
}

void StateSend::setClickedColor(const QColor &color)
{
        m_clickedColor = color;
}

void StateSend::setTextColor(const QColor &newColor)
{
    if (m_textColor != newColor) {
        m_textColor = newColor;
        for(int i = 0; i < rowCount(); i++) {
            for(int j = 0; j < columnCount(); j++) {
                item(i, j)->setForeground(QBrush(m_textColor));
            }
        }
    }
}

void StateSend::setBackgroundColor(const QColor &newColor)
{
    if (m_backgroundColor != newColor) {
        m_backgroundColor = newColor;
        for(int i = 0; i < rowCount(); i++) {
            for(int j = 0; j < columnCount(); j++) {
                if(j < 2 || j > 4) {
                    item(i, j)->setBackground(QBrush(m_backgroundColor));
                }
            }
        }
    }
}

void StateSend::loadData()
{
    clearContents();
    m_dataList.clear();
    m_paramTableNoList.clear();
    m_paramNoList.clear();

    QRegExp dataRegExp = QRegExp("^\\[(\\d+),(\\d+)\\]$");
    QString formulaStr = getDataes();
    while (formulaStr.length() > 5)
    {
        int pos = 0;
        formulaStr = formulaStr.right(formulaStr.length() - 1);
        pos = formulaStr.indexOf('}');
        QString str = formulaStr.left(pos);
        int p = dataRegExp.indexIn(str);
        if (p == -1 ) {
            QMessageBox::information(this, tr("Invalid Data"),
                              tr("Data is invalid!"),
                              QMessageBox::Ok);
            return;
        }
        m_dataList << str;
        m_paramTableNoList << dataRegExp.cap(1).toInt();
        m_paramNoList << dataRegExp.cap(2).toInt();
        formulaStr = formulaStr.right(formulaStr.length() - pos - 1);
    }

    setRowCount(m_dataList.count());
//    clickedItem.resize(dataCount);
//    clickedItem[0] = 0;

    for (int i = 0; i < rowCount(); i++) {
        param = net->getParam(m_dataList[i]);

        rangeStr.append(QString::fromStdString(param->GetParamTheoryRange()));
        QTableWidgetItem* rangeItem = new QTableWidgetItem(rangeStr[i]);
        setItem(i, 0, rangeItem);

        titleStr.append(QString::fromStdString(param->GetParamName()));
        QTableWidgetItem* titleItems = new QTableWidgetItem(titleStr[i]);
        setItem(i, 1, titleItems);

        theoryValueStr.append(QString::fromStdString(param->GetParamTheoryValue()));
        QTableWidgetItem* theoryValueItem = new QTableWidgetItem(theoryValueStr[i]);
        setItem(i, 5, theoryValueItem);

        noteStr.append(QString::fromStdString(param->GetParamNote()));
        QTableWidgetItem* noteItem = new QTableWidgetItem(noteStr[i]);
        setItem(i, 6, noteItem);

        QTableWidgetItem* beginItem = new QTableWidgetItem("开始");
        QTableWidgetItem* finishItem = new QTableWidgetItem("完成");
        QTableWidgetItem* notBeginItem = new QTableWidgetItem("未进行");
        beginItem->setBackground(QBrush(normalColor()));
        finishItem->setBackground(QBrush(normalColor()));
        notBeginItem->setBackground(QBrush(normalColor()));
        setItem(i, 2, beginItem);
        setItem(i, 3, finishItem);
        setItem(i, 4, notBeginItem);
    }
}

void StateSend::changeBeforeClickedColor()
{
    for (int i = 0; i < rowCount(); i++) {
        intialColor(i);
    }
}

void StateSend::sendState(int row, int column)
{
    QHostAddress ipAddress;
    if (!ipAddress.setAddress(m_destAddress)) {
        QMessageBox::information(this, tr("Invalid IP address"),
                          tr("IP address is invalid!"),
                          QMessageBox::Ok);
        return;
    }
    if (column >= 2 && column <= 4) {
//        clickedItem[row] = column - 1;
        intialColor(row);
        item(row, column)->setBackground(QBrush(pressedColor()));

        for(int i = 0; i < 2; i++) {
            QUdpSocket udpSocket;
            quint16 paramLenth = param->GetParamDataLen();
            uint size = 40 + paramLenth;
            char* pBuf = new char[size];
            memset(pBuf, 0, size);
            char* pCmd = (char*)(pBuf+34);

            switch(column) {
            case 2:
                *pCmd = 1;
                break;
            case 3:
                *pCmd = 2;
                break;
            case 4:
                *pCmd = 0;
                break;
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
            *pTable = m_paramTableNoList[row];
            quint16* pCode = (quint16*)(pBuf+32);
            *pCode = m_paramNoList[row];

            quint16* wb = (quint16*)(pBuf+34+paramLenth);
            *wb = 0xffff;
            quint32* zwb = (quint32*)(pBuf+36+paramLenth);
            *zwb = 0x89abcdef;

            udpSocket.writeDatagram(pBuf, size, ipAddress, m_port);
        }
    }
}

void StateSend::paintEvent(QPaintEvent *event)
{
    if (hasUpdate == false) {
        for(int i = 0; i < rowCount(); i++) {
            item(i, 0)->setText(rangeStr[i]);
            item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item(i, 1)->setText(titleStr[i]);
            item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item(i, 2)->setText(tr("开始"));
            item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item(i, 3)->setText(tr("完成"));
            item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item(i, 4)->setText(tr("未进行"));
            item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item(i, 5)->setText(theoryValueStr[i]);
            item(i, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item(i, 6)->setText(noteStr[i]);
            item(i, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        hasUpdate = true;
    }
    QTableWidget::paintEvent(event);
}

void StateSend::intialColor(int row)
{
    item(row, 2)->setBackground(QBrush(normalColor()));
    item(row, 3)->setBackground(QBrush(normalColor()));
    item(row, 4)->setBackground(QBrush(normalColor()));
}


