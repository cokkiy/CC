#include "configshowtypedialog.h"
#include "ui_configshowtypedialog.h"
#include "../../../net/C3I/paramstyle/paramstyleconfig.h"
QConfigShowTypeDialog::QConfigShowTypeDialog(QWidget *parent,QString showTypeStr) :
    QDialog(parent),
    ui(new Ui::QConfigShowTypeDialog)
{
    m_styleVector.clear();
    m_ShowTypeStr = showTypeStr;
    ui->setupUi(this);
    paramStyleInfor = NetComponents::getInforCenter();
    getParamStyleData();
    ui->ShowTypelineEdit->setText(m_ShowTypeStr);
    initTreeWidget();
    this->setFixedSize(geometry().width(),geometry().height());
}
QString QConfigShowTypeDialog::getShowTypeStr()
{
    return m_ShowTypeStr;
}
void QConfigShowTypeDialog::getParamStyleData()
{
    list<QParamStyle>* paramStyleList = paramStyleInfor->getParamStyleList();
    list<QParamStyle>::iterator paramStyleIt = paramStyleList->begin();
    //遍历所有样式
    for(;paramStyleIt != paramStyleList->end();paramStyleIt++)
    {
        QString styleNameStr = paramStyleIt->getStyleName();
        int pos = styleNameStr.indexOf(tr("."));
        QString domainStr = styleNameStr.left(pos);
        styleNameStr = styleNameStr.right(styleNameStr.length() - pos -1);
        bool bIsDomainStr = true;
        for(int i = 0;i < m_styleVector.size();i++ )
        {
            if(m_styleVector.at(i).m_DomainStr == domainStr)
            {
                bIsDomainStr = false;
                bool bIsIndexName = true;
                for(int j = 0;j < m_styleVector.at(i).m_IndexNameVector.size();j++ )
                {
                    if(m_styleVector.at(i).m_IndexNameVector.at(j) == styleNameStr)
                        bIsIndexName = false;
                }
                if(bIsIndexName)
                {
                    stru_ParamStyleIndex paramStyleIndex = m_styleVector.at(i);
                    paramStyleIndex.m_IndexNameVector.push_back(styleNameStr);
                    m_styleVector.replace(i,paramStyleIndex);
                }
            }
        }
        if(bIsDomainStr)
        {
            stru_ParamStyleIndex paramStyleIndex;
            paramStyleIndex.m_DomainStr = domainStr;
            paramStyleIndex.m_IndexNameVector.push_back(styleNameStr);
            m_styleVector.push_back(paramStyleIndex);
        }
    }
}

void QConfigShowTypeDialog::initTreeWidget()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->StyletreeWidget,QStringList(tr("普通格式")));
    QTreeWidgetItem *childItem = new QTreeWidgetItem(item,QStringList(tr("数值格式")));
    item->addChild(childItem);
    QTreeWidgetItem * grandsonItem = new QTreeWidgetItem(childItem,QStringList(".3f(.xfunit格式,x可配,unit为单位可配)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("?远%.1f公里:近%.0f公里:正中靶心(?后配置可配)"));
    childItem->addChild(grandsonItem);

    childItem = new QTreeWidgetItem(item,QStringList(tr("时间格式")));
    item->addChild(childItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("T1(%02d时%02d分%02d秒)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("t1(%02d:%02d:%02d)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("T2(%d秒)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("t2(%ds)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("T3(%02d时%02d分%02d秒%03d 毫秒)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("t3(%02d:%02d:%02d.%03d)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("T4(%.3f秒)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("t4(%.3fs)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("t5(%.3f)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("t6(%d)"));
    childItem->addChild(grandsonItem);

    childItem = new QTreeWidgetItem(item,QStringList(tr("日期格式")));
    item->addChild(childItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("LED_DT(XXXX年XX月XX日XX时XX分XX秒)"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("D2000(积日2000年1月1日)"));
    childItem->addChild(grandsonItem);
    childItem = new QTreeWidgetItem(item,QStringList(tr("源码格式")));
    item->addChild(childItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("H(十六进制(大写))"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("h(十六进制(小写))"));
    childItem->addChild(grandsonItem);
    grandsonItem = new QTreeWidgetItem(childItem,QStringList("b(二进制)"));
    childItem->addChild(grandsonItem);

    item = new QTreeWidgetItem(ui->StyletreeWidget,QStringList(tr("参数样式")));
    for(int i = 0;i < m_styleVector.size();i++ )
    {
        childItem = new QTreeWidgetItem(item,QStringList(m_styleVector.at(i).m_DomainStr));
        item->addChild(childItem);
        for(int j = 0;j < m_styleVector.at(i).m_IndexNameVector.size();j++ )
        {
            grandsonItem = new QTreeWidgetItem(childItem,QStringList(m_styleVector.at(i).m_IndexNameVector.at(j)));
            item->addChild(grandsonItem);
        }
    }

}
QConfigShowTypeDialog::~QConfigShowTypeDialog()
{
    delete ui;
}

void QConfigShowTypeDialog::on_StyletreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{

    int count = item->childCount();
    if(count == 0)
    {
        QTreeWidgetItem *parentItem = item->parent();
        QTreeWidgetItem *grandItem = parentItem->parent();
        if(grandItem->text(0) == tr("参数样式"))
        {
            m_ShowTypeStr =  parentItem->text(0);
            m_ShowTypeStr =  m_ShowTypeStr + tr(".") + item->text(column);
            //ui->ShowTypelineEdit->setEnabled(false);
        }
        if(grandItem->text(0) == tr("普通格式"))
        {
            m_ShowTypeStr =  item->text(column);
            int pos = m_ShowTypeStr.indexOf("(");
            m_ShowTypeStr = m_ShowTypeStr.left(pos);

           //m_ShowTypeStr =  m_ShowTypeStr + tr(".") + item->text(column);
        }
        ui->ShowTypelineEdit->setText(m_ShowTypeStr);
        //
    }
}
