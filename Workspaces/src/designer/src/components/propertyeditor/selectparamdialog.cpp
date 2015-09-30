#include "selectparamdialog.h"
#include <QtDesigner/QDesignerSettingsInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QButtonGroup>

#include <QtWidgets/QGridLayout>
QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
QSelectParamDialog::QSelectParamDialog(QWidget *parent) :
    QDialog(parent)
{
    resize(1024,768);
    QButtonGroup *grp = new QButtonGroup();

    m_iMinTableNo = 1;
    m_iMaxTableNo = 30000;
    m_iCurrentTableNo = 0;

    setWindowTitle(tr("Select Param"));
     //Top Layout
     m_topLayoutLeft = new QHBoxLayout();
     m_topLayoutRight = new QHBoxLayout();
     m_ParamLabel = new QLabel(tr("参数"));
     m_parameditor = new QLineEdit;
     m_topLayoutLeft->addWidget(m_ParamLabel);
     m_topLayoutLeft->addWidget(m_parameditor);

     m_AllSystemRadioButton = new QRadioButton(tr("所有参数表"));
     m_topLayoutRight->addWidget(m_AllSystemRadioButton);

     grp->addButton(m_AllSystemRadioButton);
     connect(m_AllSystemRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectAllSystemRadioButton(bool)));

     m_C3IRadioButton = new QRadioButton(tr("C3I系统[1-10000]"));
     grp->addButton(m_C3IRadioButton);
     m_topLayoutRight->addWidget(m_C3IRadioButton);
     connect(m_C3IRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectC3IRadioButton(bool)));

     m_DFZXRadioButton = new QRadioButton(tr("东风中心[10001-20000]"));
     m_topLayoutRight->addWidget(m_DFZXRadioButton);
     grp->addButton(m_DFZXRadioButton);
     connect(m_DFZXRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectDFZXRadioButton(bool)));

     m_KELRadioButton = new QRadioButton(tr("库尔勒[20001-30000]"));
     m_topLayoutRight->addWidget(m_KELRadioButton);
     grp->addButton(m_KELRadioButton);
     connect(m_KELRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectKELRadioButton(bool)));



     QRadioButton* m_CustomRadioButton = new QRadioButton(tr("自定义参数表范围"));
     m_topLayoutRight->addWidget(m_CustomRadioButton);
     grp->addButton(m_CustomRadioButton);
     connect(m_CustomRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectCustomRadioButton(bool)));

     m_CustomMinParamEdit = new QLineEdit();
     m_CustomMinParamEdit->setFixedWidth(100);
     m_topLayoutRight->addWidget(m_CustomMinParamEdit);
     QLabel* m_LineLable = new QLabel(tr("--"));
     m_LineLable->setFixedWidth(10);
     m_topLayoutRight->addWidget(m_LineLable);
     m_CustomMaxParamEdit = new QLineEdit();
     m_CustomMaxParamEdit->setFixedWidth(100);
     m_topLayoutRight->addWidget(m_CustomMaxParamEdit);

     m_SelectParamButton = new QPushButton(tr("确定"));
     m_SelectParamButton->setFixedWidth(50);
     m_topLayoutRight->addWidget(m_SelectParamButton);
     connect(m_SelectParamButton,SIGNAL(clicked()),this,SLOT(SelectCustom()));

     //Left Layout
     //m_formNameLabel = new QLabel(tr("表列表"));
     m_formTableWidget = new QTableWidget();
     m_formTableWidget->setColumnCount(2);
     QStringList header1;
     header1<<tr("表号")<<tr("表名");
     m_formTableWidget->setHorizontalHeaderLabels(header1);
     m_leftLayout = new QGridLayout();
     //m_leftLayout->addWidget(m_formNameLabel,1,0,Qt::AlignCenter);
     m_leftLayout->addWidget(m_formTableWidget,1,0);
     m_formTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
     m_formTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
     m_formTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
     m_formTableWidget->setColumnWidth(0,80);
     m_formTableWidget->setColumnWidth(1,880);

     connect(m_formTableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(OnClickTableForm(int,int)));

     //Right Layout
     m_paramNameLabel = new QLabel(tr("参数列表"));
     m_paramTableWidget = new QTableWidget();
     m_paramTableWidget->setColumnCount(16);
     QStringList header2;
     header2<<tr("表号")<<tr("参数号")<<tr("名称")<<tr("代号")
            <<tr("传输格式")<<tr("数长")<<tr("转换格式")<<tr("下限")
            <<tr("上限")<<tr("量纲")<<tr("单位")<<tr("显示格式")
            <<tr("理论值")<<tr("范围")<<tr("索引")<<tr("备注");
     m_paramTableWidget->setHorizontalHeaderLabels(header2);
     m_paramTableWidget->resizeColumnsToContents();
     m_rightLayout = new QGridLayout();
     m_rightLayout->addWidget(m_paramNameLabel,1,0,Qt::AlignCenter);
     m_rightLayout->addWidget(m_paramTableWidget,2,0);
     m_paramTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
     m_paramTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
     m_paramTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
     connect(m_paramTableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(OnClickParamForm(int,int)));
     m_paramTableWidget->setColumnWidth(0,80);

     //Bottom Layout
     m_bottomLayout = new QGridLayout();
     buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
     QPushButton *ok_button = buttonBox->button(QDialogButtonBox::Ok);
     ok_button->setDefault(true);
     connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
     m_bottomLayout->addWidget(buttonBox);

     //main Layout
     QGridLayout *mainLayout = new QGridLayout(this);
     mainLayout->setMargin(10);
     mainLayout->setSpacing(10);
     mainLayout->addLayout(m_topLayoutLeft,0,0);
     mainLayout->addLayout(m_topLayoutRight,1,0);
     mainLayout->addLayout(m_leftLayout,2,0);
     mainLayout->addLayout(m_rightLayout,3,0);
     mainLayout->addLayout(m_bottomLayout,4,0,Qt::AlignRight);
//     mainLayout->setColumnStretch(0,1);
 //    mainLayout->setColumnStretch(1,1);

     net = NetComponents::getInforCenter();
}
void QSelectParamDialog::OnClickTableForm(int row,int)
{
    QString tableNoStr = m_formTableWidget->item(row,0)->text();
    ushort itableNo = tableNoStr.toUShort();
    SingleTableParamVector ParamVector = net->getParamVector(itableNo);
    m_paramTableWidget->setRowCount(0);
    for(int i = 0;i < ParamVector.size();i++ )
    {
        int row = m_paramTableWidget->rowCount();
        m_paramTableWidget->setRowCount(row+1);
        AbstractParam Param = ParamVector.at(i);
        m_paramTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(Param.GetZXParamTableNo())));
        m_paramTableWidget->setItem(row,1,new QTableWidgetItem(QString::number(Param.GetZXParamNo())));
        m_paramTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
        m_paramTableWidget->setItem(row,2,new QTableWidgetItem(tr(Param.GetZXParamTitle().c_str())));
        m_paramTableWidget->setItem(row,3,new QTableWidgetItem(tr(Param.GetZXParamCode().c_str())));
        m_paramTableWidget->setItem(row,4,new QTableWidgetItem(QString::number(Param.GetZXParamTranType())));
        m_paramTableWidget->setItem(row,5,new QTableWidgetItem(QString::number(Param.GetZXParamDataLength())));
        m_paramTableWidget->setItem(row,6,new QTableWidgetItem(tr(Param.GetZXParamDataType().c_str())));
        m_paramTableWidget->setItem(row,7,new QTableWidgetItem(QString::number(Param.GetZXParamLorlmt())));
        m_paramTableWidget->setItem(row,8,new QTableWidgetItem(QString::number(Param.GetZXParamUprlmt())));
        m_paramTableWidget->setItem(row,9,new QTableWidgetItem(QString::number(Param.GetZXParamQuotiety())));
        m_paramTableWidget->setItem(row,10,new QTableWidgetItem(tr(Param.GetZXParamUnit().c_str())));
        m_paramTableWidget->setItem(row,11,new QTableWidgetItem(tr(Param.GetZXParamShowType().c_str())));
        m_paramTableWidget->setItem(row,12,new QTableWidgetItem(tr(Param.GetZXParamTheoryValue().c_str())));
        m_paramTableWidget->setItem(row,13,new QTableWidgetItem(tr(Param.GetZXParamTheoryRange().c_str())));
        m_paramTableWidget->setItem(row,14,new QTableWidgetItem(QString::number(Param.GetZXParamGroupIndex())));
        m_paramTableWidget->setItem(row,15,new QTableWidgetItem(tr(Param.GetZXParamNote().c_str())));
    }
    m_paramTableWidget->resizeColumnsToContents();
}
void QSelectParamDialog::OnClickParamForm(int row,int)
{
    QString tableNoStr = m_paramTableWidget->item(row,0)->text();
    QString paramNoStr = m_paramTableWidget->item(row,1)->text();
    m_parameditor->setText(tr("[") + tableNoStr + tr(",") + paramNoStr + tr("]") );
}

void QSelectParamDialog::SelectAllSystemRadioButton(bool check)
{
    if(check)
    {
        m_iMaxTableNo = 30000;
        m_iMinTableNo = 1;
        m_CustomMinParamEdit->setText(tr("1"));
        m_CustomMinParamEdit->setEnabled(false);
        m_CustomMaxParamEdit->setText(tr("30000"));
        m_CustomMaxParamEdit->setEnabled(false);
        m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectC3IRadioButton(bool check)
{
    if(check)
    {
        m_iMinTableNo = 1;
        m_iMaxTableNo = 10000;
        m_CustomMinParamEdit->setText(tr("1"));
        m_CustomMinParamEdit->setEnabled(false);
        m_CustomMaxParamEdit->setText(tr("10000"));
        m_CustomMaxParamEdit->setEnabled(false);
        m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectKELRadioButton(bool check)
{
    if(check)
    {
        m_iMaxTableNo = 30000;
        m_iMinTableNo = 20001;
        m_CustomMinParamEdit->setText(tr("20001"));
        m_CustomMinParamEdit->setEnabled(false);
        m_CustomMaxParamEdit->setText(tr("30000"));
        m_CustomMaxParamEdit->setEnabled(false);
        m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectDFZXRadioButton(bool check)
{
    if(check)
    {
        m_iMaxTableNo = 20000;
        m_iMinTableNo = 10001;
        m_CustomMinParamEdit->setText(tr("10001"));
        m_CustomMinParamEdit->setEnabled(false);
        m_CustomMaxParamEdit->setText(tr("20000"));
        m_CustomMaxParamEdit->setEnabled(false);
        m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectCustomRadioButton(bool check)
{
    if(check)
    {
        m_CustomMinParamEdit->setText(QString::number(m_iMinTableNo));
        m_CustomMinParamEdit->setEnabled(true);
        m_CustomMaxParamEdit->setText(QString::number(m_iMaxTableNo));
        m_CustomMaxParamEdit->setEnabled(true);
        m_SelectParamButton->setEnabled(true);
    }
}

void QSelectParamDialog::SelectCustom()
{
    m_iMinTableNo = m_CustomMinParamEdit->text().toUShort();
    m_iMaxTableNo = m_CustomMaxParamEdit->text().toUShort();
    initFormTable();
}

bool QSelectParamDialog::initFormTable()
{
    //开始装载网络配置
    //net->load("../zx_config/task01/public");
    QMap<unsigned short, QString > tablemap;
    tablemap = net->getTabList();
    m_formTableWidget->setRowCount(0);
    QMap<unsigned short, QString >::iterator it = tablemap.begin();
    for(;it != tablemap.end();it++)
    {
        ushort tableno = it.key();
        if(tableno >= m_iMinTableNo && tableno <= m_iMaxTableNo)
        {
            int row = m_formTableWidget->rowCount();
            m_formTableWidget->setRowCount(row+1);
            m_formTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(it.key())));
            m_formTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
            m_formTableWidget->setItem(row,1,new QTableWidgetItem(it.value()));
        }
    }
    
    return true;
}


int QSelectParamDialog::showDialog()
{

    m_parameditor->setFocus();
    return exec();
}

QString QSelectParamDialog::text() const
{
    return m_parameditor->text();
}

void QSelectParamDialog::SetText(QString str)
{
    m_parameditor->setText(str);
    if(str.length() > 4)
    {
        int pos = str.indexOf(tr(","));
        QString str1 = str.left(pos);
        str1 = str1.right(pos-1);
        m_iCurrentTableNo = str1.toUShort();
    }
    if(m_iCurrentTableNo >= 1  && m_iCurrentTableNo <= 10000)
    {
        m_iMinTableNo = 1;
        m_iMaxTableNo = 10000;
        m_C3IRadioButton->setChecked(true);
    }
    else if(m_iCurrentTableNo >= 10001  && m_iCurrentTableNo <= 20000)
    {
        m_iMinTableNo = 10001;
        m_iMaxTableNo = 20000;
        m_DFZXRadioButton->setChecked(true);
    }
    else if(m_iCurrentTableNo >= 20001  && m_iCurrentTableNo <= 30000)
    {
        m_iMinTableNo = 20001;
        m_iMaxTableNo = 30000;
        m_KELRadioButton->setChecked(true);
    }
    else
    {
        m_iMinTableNo = 1;
        m_iMaxTableNo = 30000;
        m_AllSystemRadioButton->setChecked(true);
    }
    initFormTable();
}
QSelectParamDialog::~QSelectParamDialog()
{

}
} // namespace qdesigner_internal

QT_END_NAMESPACE









