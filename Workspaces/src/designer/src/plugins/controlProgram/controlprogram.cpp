#include "controlprogram.h"
#include<QMessageBox>

controlProgram::controlProgram(QWidget *parent) :
    QTableWidget(parent)
{
    //init param
    setRowCount(1);
    setColumnCount(2);
    setPercent(70);
    setWordWrap(true);//自动换行

    ////no边框
    setFrameShape(QFrame::NoFrame);
    setLineWidth(0);

    setBackgroundColor(Qt::gray);//背景色
    setTextColor(Qt::black);//文本颜色
    setFont(QFont("Times", 15, QFont::Normal));//字体

    //grid
    setShowGrid(true);//显示网格
    setGridStyle(Qt::SolidLine);
    m_gridlineColor.setRgb(0,0,255);//网格颜色

    //设置三种状态与三种字体和颜色
    stateList<<"未开始"<<"进行中"<<"已完成";
    m_undoColor.setRgb(255,0,0);
    m_doingColor.setRgb(0,255,0);
    m_doneColor.setRgb(0,0,255);
    setUndoFont(QFont("Times", 15, QFont::Normal));
    setDoingFont(QFont("Times", 15, QFont::Bold));
    setDoneFont(QFont("Times", 15, QFont::Black));

    //行平均高度
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //隐藏表头
    this->horizontalHeader()->setVisible(false);
    this->verticalHeader()->setVisible(false);//隐藏纵向表头

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);//no edit
    this->setSelectionMode(QAbstractItemView::NoSelection);//no chose

    //load data
    net = NetComponents::getInforCenter();
    if (m_dataes != NULL) {
        loadData();
    }

    haspercent = false;
    hashandlename = false;
    hasStyleSheetUpdate = false;

    stylesheetUpdate();
    connect(this, SIGNAL(gridlineColorChanged(QColor)), this, SLOT(stylesheetUpdate()));

    //定时器类 //设置一个500ms定时器
    m_ti = NetComponents::getTimer();
    m_dc = NetComponents::getDataCenter();
    //关联定时器500ms的周期信号，这样可以使用全局的周期信号
    connect(m_ti, SIGNAL(timeout_500()), this, SLOT(timeEvent_500ms()));
}

controlProgram::~controlProgram()
{
    //    killTimer(m_timer_id);
    if(m_dc)
    {
        delete m_dc;
        m_dc = NULL;
    }
}

//每次load data将参数名自动添加到titleList后面。
void controlProgram::loadData()
{
    //解释多参数
    clearContents();
    m_FormulaVector.clear();
    QString formulaStr = getDataes();
    while (formulaStr.length() > 5)
    {
        int pos = 0;
        formulaStr = formulaStr.right(formulaStr.length() - 1);
        pos = formulaStr.indexOf('}');
        QString str = formulaStr.left(pos);
        formulaStr = formulaStr.right(formulaStr.length() - pos - 1);
        m_FormulaVector.append(str);
    }
    setRowCount(m_FormulaVector.size());//设置行数

    //根据每个参数获取参数名称
    titleList.clear();
    for (int pos=0;pos<m_FormulaVector.size();pos++)
    {
        param = net->getParam(m_FormulaVector.at(pos));
        string str = param->GetParamName();
        titleList.append(QString::fromStdString(str));
    }

    //清楚第二列数据，初始化均为：未开始
    datalist2.clear();
    for(int pos=0;pos<m_FormulaVector.size();pos++)
    {
        QString str = "0";
        datalist2.append(str);
    }
}

//定时器
void controlProgram::timeEvent_500ms()
{
    ////使用数据处理中心的接口方法
    double ret;
    bool ok;
    for(int pos=0;pos<m_FormulaVector.size();pos++)
    {
        if(m_dc->getValue(m_FormulaVector[pos],ret)==1)
        {
            float fdata = ret;
            intdata = (int)fdata;
            datalist1.append(QString::number(intdata,10));//实时获取的数据
        }
        else
        {
            datalist1.append(datalist2.at(pos));//历史数据
        }

        intdata = datalist1.at(pos).toInt(&ok,10);//字符串转整型
        handleData(pos,1);//根据intdata判断对应参数所属状态
    }
    datalist2 = datalist1;//本帧数据替换上一帧数据
    datalist1.clear();//本帧数据清零
}

//绘图事件
void controlProgram::paintEvent(QPaintEvent *e)
{
    //name 处理参数名以及文本 文本颜色和背景色
    if(hashandlename == false){
        handleName();
        hashandlename = true;
    }

    //百分比，确定两列的宽度
    if(m_percent>=0&&m_percent<=100)
    {
        int width0 = geometry().width()*m_percent/100;
        setColumnWidth(0,width0);
        int width1 = geometry().width()-width0-2*lineWidth();
        setColumnWidth(1,width1);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("不在其数值范围，可选范围为[0,100]!");
        msgBox.exec();
        setPercent(80);
    }

    //用于更新网格颜色
    stylesheetUpdate();
    QTableWidget::paintEvent(e);
}

void controlProgram::handleName()
{
    //name 处理参数名以及文本 文本颜色和背景色
    for(int pos=0;pos<rowCount();pos++)
    {
        QString str = QString::number(pos+1,10)+": ";//项目编号

        QTableWidgetItem *item;
        item = new QTableWidgetItem;

        if(pos<m_nameList.size())
            item->setText(str+m_nameList.at(pos));//字符串列表
        else if(pos<titleList.size())
            item->setText(str+titleList.at(pos));//参数名
        else
            item->setText(" ");
        item->setForeground(m_textColor);
        item->setBackgroundColor(m_backgroundColor);
        this->setItem(pos,0,item);
    }
}

//重写第二列内容，背景色，字体和颜色
void controlProgram::handleData(int rows,int columns)
{
    QTableWidgetItem *item;
    item = new QTableWidgetItem;

    switch (intdata) {
    case 0:
        item->setText(stateList.at(0));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setForeground(m_undoColor);
        item->setFont(m_undofont);
        break;
    case 1:
        item->setText(stateList.at(1));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setForeground(m_doingColor);
        item->setFont(m_doingfont);
        break;
    case 2:
        item->setText(stateList.at(2));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setForeground(m_doneColor);
        item->setFont(m_donefont);
        break;
    }
    item->setBackgroundColor(m_backgroundColor);
    this->setItem(rows,columns,item);
}

//percent
void controlProgram::setPercent(const int &newp)
{
    if(m_percent != newp){
        m_percent = newp;
        update();
    }
}

//gridlinecolor
void controlProgram::setGridlineColor(const QColor &newcolor)
{
    if(m_gridlineColor != newcolor)
    {
        hasStyleSheetUpdate = false;

        m_gridlineColor = newcolor;
        emit gridlineColorChanged(m_gridlineColor);
    }
}

//样式表更改网格颜色
void controlProgram::stylesheetUpdate()
{
    if(hasStyleSheetUpdate == false)//用于限制样式表的绘制，在颜色确定改变时工作
    {
        QString m_gridlineColor = QString("rgb(%1, %2, %3)").arg(QString::number(gridlineColor().red()),
                                                                 QString::number(gridlineColor().green()),
                                                                 QString::number(gridlineColor().blue()));
        this->setStyleSheet( QString("QTableWidget{gridline-color: %1}").arg(m_gridlineColor));

        hasStyleSheetUpdate =true;
    }
}

//bgcolor
void controlProgram::setBackgroundColor(const QColor &newcolor)
{
    if(m_backgroundColor != newcolor){
        m_backgroundColor = newcolor;
        QPalette pll = this->palette();
        pll.setBrush(QPalette::Base,QBrush(m_backgroundColor));
        this->setPalette(pll);
        hashandlename = false;
        update();
    }
}

//txtcolor
void controlProgram::setTextColor(const QColor &newcolor)
{
    if(m_textColor != newcolor){
        m_textColor = newcolor;
        hashandlename = false;
        update();
    }
}

////data
void controlProgram::setDataes(const QString &newdata)
{
    if(m_dataes != newdata){
        m_dataes = newdata;
        loadData();
        hashandlename = false;
        update();
    }
}

////QStringList
void controlProgram::setNameList(const QStringList &newnameList)
{
    if (m_nameList != newnameList)
    {
        m_nameList = newnameList;
        hashandlename = false;
        update();
    }
}

//
void controlProgram::setUndoColor(const QColor &newcolor)
{
    m_undoColor = newcolor;
    update();
}
void controlProgram::setDoingColor(const QColor &newcolor)
{
    m_doingColor = newcolor;
    update();
}
void controlProgram::setDoneColor(const QColor &newcolor)
{
    m_doneColor = newcolor;
    update();
}
void controlProgram::setUndoFont(const QFont &newfont)
{
    m_undofont = newfont;
    update();
}
void controlProgram::setDoingFont(const QFont &newfont)
{
    m_doingfont = newfont;
    update();
}
void controlProgram::setDoneFont(const QFont &newfont)
{
    m_donefont = newfont;
    update();
}
