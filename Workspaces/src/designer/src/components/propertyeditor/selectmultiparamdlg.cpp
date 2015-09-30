#include "selectmultiparamdlg.h"
#include "ui_selectmultiparamdlg.h"

QSelectMultiParamDlg::QSelectMultiParamDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSelectMultiParamDlg)
{
    m_iMinTableNo = 1;
    m_iMaxTableNo = 30000;
    m_iCurrentItemNo = -1;
    m_iSystemType = 4;
    SelectingVector.clear();
    SelectedVector.clear();
    ui->setupUi(this);
    InitParamFormMap();
    InitParamFormWidget();
    SetParamFormWidgetItem();
    SetCustomControl();
    InitParamWidget();
    InitSelectedWidget();
    ui->m_AddButton->setEnabled(false);
    ui->m_DelButton->setEnabled(false);
    ui->m_UpButton->setEnabled(false);
    ui->m_DownButton->setEnabled(false);
    ui->m_TopButton->setEnabled(false);
    ui->m_BottomButton->setEnabled(false);
}
void QSelectMultiParamDlg::SetCustomControl()
{
    ui->m_MinlineEdit->setText(QString::number(m_iMinTableNo));
    ui->m_MaxlineEdit->setText(QString::number(m_iMaxTableNo));
    ui->m_MinlineEdit->setEnabled(m_iSystemType == 4);
    ui->m_MaxlineEdit->setEnabled(m_iSystemType == 4);
    ui->label->setEnabled(m_iSystemType == 4);
    ui->label_2->setEnabled(m_iSystemType == 4);
    ui->m_SetCustom->setEnabled(m_iSystemType == 4);
}
QSelectMultiParamDlg::~QSelectMultiParamDlg()
{
    delete ui;
}
void QSelectMultiParamDlg::InitParamFormWidget()
{
    QStringList header1;
    header1<<tr("表号")<<tr("表名"); 
    ui->m_FormTableWidget->setColumnCount(2);
    ui->m_FormTableWidget->setHorizontalHeaderLabels(header1);
    ui->m_FormTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_FormTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_FormTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_FormTableWidget->setColumnWidth(0,100);
    ui->m_FormTableWidget->setColumnWidth(1,950);
}

void QSelectMultiParamDlg::InitParamWidget()
{
    ui->m_ParamTableWidget->setColumnCount(16);
    QStringList header2;
    header2<<tr("表号")<<tr("参数号")<<tr("名称")<<tr("代号")
           <<tr("传输格式")<<tr("数长")<<tr("转换格式")<<tr("下限")
           <<tr("上限")<<tr("量纲")<<tr("单位")<<tr("显示格式")
           <<tr("理论值")<<tr("范围")<<tr("索引")<<tr("备注");
    ui->m_ParamTableWidget->setHorizontalHeaderLabels(header2);
    //ui->m_ParamTableWidget->resizeColumnsToContents();
    ui->m_ParamTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_ParamTableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->m_ParamTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void QSelectMultiParamDlg::InitSelectedWidget()
{
    ui->m_SelectedtableWidget->setColumnCount(3);
    QStringList header2;
    header2<<tr("表号")<<tr("参数号")<<tr("名称");
    ui->m_SelectedtableWidget->setHorizontalHeaderLabels(header2);
    //ui->m_SelectedtableWidget->resizeColumnsToContents();
    ui->m_SelectedtableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_SelectedtableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_SelectedtableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void QSelectMultiParamDlg::SetParamFormWidgetItem()
{
    ui->m_FormTableWidget->setRowCount(0);
    QMap<unsigned short, QString >::iterator it = tablemap.begin();
    for(;it != tablemap.end();it++)
    {
        ushort tableno = it.key();
        if(tableno >= m_iMinTableNo && tableno <= m_iMaxTableNo)
        {
            int row = ui->m_FormTableWidget->rowCount();
            ui->m_FormTableWidget->setRowCount(row+1);
            ui->m_FormTableWidget->setRowHeight(row,25);
            ui->m_FormTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(it.key())));
            ui->m_FormTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
            ui->m_FormTableWidget->setItem(row,1,new QTableWidgetItem(it.value()));
        }
    }
}

bool QSelectMultiParamDlg::InitParamFormMap()
{
    net = NetComponents::getInforCenter();
    tablemap.clear();
    tablemap = net->getTabList();
    if(tablemap.isEmpty())
        return false;
    else
        return true;
}

void QSelectMultiParamDlg::SetSelectedWidgetItem()
{
    ui->m_SelectedtableWidget->setRowCount(0);
    for(int j = 0;j < SelectedVector.count();j++)
    {
        QString str = SelectedVector.at(j);
        ui->m_SelectedtableWidget->setRowHeight(j,25);
        int r = ui->m_SelectedtableWidget->rowCount();
        ui->m_SelectedtableWidget->setRowCount(r+1);
        int pos = str.indexOf(',');
        QString text = str.left(pos);
        ui->m_SelectedtableWidget->setItem(j,0,new QTableWidgetItem(text));
        ui->m_SelectedtableWidget->item(j,0)->setTextAlignment(Qt::AlignCenter);
        str = str.right(str.length()-pos-1);
        pos = str.indexOf(',');
        text = str.left(pos);
        ui->m_SelectedtableWidget->setItem(j,1,new QTableWidgetItem(text));
        //ui->m_SelectedtableWidget->item(j,1)->setTextAlignment(Qt::AlignCenter);
        str = str.right(str.length()-pos-1);
        ui->m_SelectedtableWidget->setItem(j,2,new QTableWidgetItem(str));
        //ui->m_SelectedtableWidget->item(j,2)->setTextAlignment(Qt::AlignCenter);
    }
    ui->m_SelectedtableWidget->resizeColumnsToContents();
    ui->m_DelButton->setEnabled(false);
    ui->m_UpButton->setEnabled(false);
    ui->m_DownButton->setEnabled(false);
    ui->m_TopButton->setEnabled(false);
    ui->m_BottomButton->setEnabled(false);
}

void QSelectMultiParamDlg::on_m_AddButton_clicked()
{
    bool ok = false;
    for(int i = 0;i < SelectingVector.count();i++)
    {
        for(int j = 0;j < SelectedVector.count();j++)
        {
            if(SelectedVector.at(j) == SelectingVector.at(i))
                ok = true;
        }
        if(!ok)
            SelectedVector.push_back(SelectingVector.at(i));
        ok = false;
    }
    SetSelectedWidgetItem();
    QList<QTableWidgetItem* > items = ui->m_ParamTableWidget->selectedItems();
    for(int i = 0;i<items.count();i++)
        ui->m_ParamTableWidget->setItemSelected(items.at(i),false);
    ui->m_AddButton->setEnabled(false);
}

void QSelectMultiParamDlg::on_m_DFZXRadio_clicked()
{
    m_iSystemType = 2;
    m_iMinTableNo = 10001;
    m_iMaxTableNo = 20000;
    SetCustomControl();
    SetParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_C3IRadio_clicked()
{
    m_iMinTableNo = 1;
    m_iMaxTableNo = 10000;
    m_iSystemType = 1;
    SetCustomControl();
    SetParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_KELRadio_clicked()
{
    m_iSystemType = 3;
    m_iMinTableNo = 20001;
    m_iMaxTableNo = 30000;
    SetCustomControl();
    SetParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_CustomRadio_clicked()
{
    m_iSystemType = 4;
    SetCustomControl();
}

void QSelectMultiParamDlg::on_m_SetCustom_clicked()
{
    m_iMinTableNo = ui->m_MinlineEdit->text().toInt();
    m_iMaxTableNo = ui->m_MaxlineEdit->text().toInt();
    SetParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_FormTableWidget_cellClicked(int row, int )
{
    QString tableNoStr = ui->m_FormTableWidget->item(row,0)->text();
    ushort itableNo = tableNoStr.toUShort();
    SingleTableParamVector ParamVector = net->getParamVector(itableNo);
    ui->m_ParamTableWidget->setRowCount(0);
    for(int i = 0;i < ParamVector.size();i++ )
    {
        int row = ui->m_ParamTableWidget->rowCount();
        ui->m_ParamTableWidget->setRowHeight(row,25);
        ui->m_ParamTableWidget->setRowCount(row+1);
        AbstractParam Param = ParamVector.at(i);
        ui->m_ParamTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(Param.GetZXParamTableNo())));
        ui->m_ParamTableWidget->setItem(row,1,new QTableWidgetItem(QString::number(Param.GetZXParamNo())));
        ui->m_ParamTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
        ui->m_ParamTableWidget->setItem(row,2,new QTableWidgetItem(tr(Param.GetZXParamTitle().c_str())));
        ui->m_ParamTableWidget->setItem(row,3,new QTableWidgetItem(tr(Param.GetZXParamCode().c_str())));
        ui->m_ParamTableWidget->setItem(row,4,new QTableWidgetItem(QString::number(Param.GetZXParamTranType())));
        ui->m_ParamTableWidget->setItem(row,5,new QTableWidgetItem(QString::number(Param.GetZXParamDataLength())));
        ui->m_ParamTableWidget->setItem(row,6,new QTableWidgetItem(tr(Param.GetZXParamDataType().c_str())));
        ui->m_ParamTableWidget->setItem(row,7,new QTableWidgetItem(QString::number(Param.GetZXParamLorlmt())));
        ui->m_ParamTableWidget->setItem(row,8,new QTableWidgetItem(QString::number(Param.GetZXParamUprlmt())));
        ui->m_ParamTableWidget->setItem(row,9,new QTableWidgetItem(QString::number(Param.GetZXParamQuotiety())));
        ui->m_ParamTableWidget->setItem(row,10,new QTableWidgetItem(tr(Param.GetZXParamUnit().c_str())));
        ui->m_ParamTableWidget->setItem(row,11,new QTableWidgetItem(tr(Param.GetZXParamShowType().c_str())));
        ui->m_ParamTableWidget->setItem(row,12,new QTableWidgetItem(tr(Param.GetZXParamTheoryValue().c_str())));
        ui->m_ParamTableWidget->setItem(row,13,new QTableWidgetItem(tr(Param.GetZXParamTheoryRange().c_str())));
        ui->m_ParamTableWidget->setItem(row,14,new QTableWidgetItem(QString::number(Param.GetZXParamGroupIndex())));
        ui->m_ParamTableWidget->setItem(row,15,new QTableWidgetItem(tr(Param.GetZXParamNote().c_str())));
    }
    ui->m_ParamTableWidget->resizeColumnsToContents();
}

void QSelectMultiParamDlg::on_m_ParamTableWidget_cellClicked(int , int )
{
    ui->m_AddButton->setEnabled(false);
    SelectingVector.clear();
    QList<QTableWidgetItem* > items = ui->m_ParamTableWidget->selectedItems();
    for(int i = 0;i<items.count();i=i+16)
    {
        int row = items.at(i)->row();
        QString str = ui->m_ParamTableWidget->item(row,0)->text();
        str = str + ",";
        str =  str + ui->m_ParamTableWidget->item(row,1)->text();
        str = str + ",";
        str =  str + ui->m_ParamTableWidget->item(row,2)->text();
        SelectingVector.push_back(str);
        ui->m_AddButton->setEnabled(true);
    }
}

void QSelectMultiParamDlg::on_m_DelButton_clicked()
{
    if(m_iCurrentItemNo != -1)
        SelectedVector.removeAt(m_iCurrentItemNo);
    SetSelectedWidgetItem();
    m_iCurrentItemNo = -1;
}

void QSelectMultiParamDlg::on_m_TopButton_clicked()
{
    if(m_iCurrentItemNo != -1)
    {
        QString str = SelectedVector.at(m_iCurrentItemNo);
        SelectedVector.removeAt(m_iCurrentItemNo);
        SelectedVector.insert(0,str);
        SetSelectedWidgetItem();
        m_iCurrentItemNo = -1;
    }

}

void QSelectMultiParamDlg::on_m_BottomButton_clicked()
{
    if(m_iCurrentItemNo != -1)
    {
        QString str = SelectedVector.at(m_iCurrentItemNo);
        SelectedVector.removeAt(m_iCurrentItemNo);
        SelectedVector.push_back(str);
        SetSelectedWidgetItem();
        m_iCurrentItemNo = -1;
    }
}

void QSelectMultiParamDlg::on_m_UpButton_clicked()
{
    if(m_iCurrentItemNo != -1)
    {
        QString str1 = SelectedVector.at(m_iCurrentItemNo);
        if(m_iCurrentItemNo != 0)
        {

            SelectedVector.replace(m_iCurrentItemNo,SelectedVector.at(m_iCurrentItemNo-1));
            SelectedVector.replace(m_iCurrentItemNo-1,str1);
            m_iCurrentItemNo = m_iCurrentItemNo - 1;
        }
        else
        {
            SelectedVector.pop_front();
            SelectedVector.push_back(str1);
            m_iCurrentItemNo = SelectedVector.count() - 1;
        }
    }
    SetSelectedWidgetItem();
    ui->m_SelectedtableWidget->selectRow(m_iCurrentItemNo);
    ui->m_DelButton->setEnabled(true);
    ui->m_UpButton->setEnabled(true);
    ui->m_DownButton->setEnabled(true);
    ui->m_TopButton->setEnabled(true);
    ui->m_BottomButton->setEnabled(true);
}

void QSelectMultiParamDlg::on_m_DownButton_clicked()
{
    if(m_iCurrentItemNo != -1)
    {
        QString str1 = SelectedVector.at(m_iCurrentItemNo);
        if(m_iCurrentItemNo != SelectedVector.count() - 1)
        {

            SelectedVector.replace(m_iCurrentItemNo,SelectedVector.at(m_iCurrentItemNo+1));
            SelectedVector.replace(m_iCurrentItemNo+1,str1);
            m_iCurrentItemNo = m_iCurrentItemNo + 1;
        }
        else
        {
            SelectedVector.pop_back();
            SelectedVector.push_front(str1);
            m_iCurrentItemNo = 0;
        }
    }
    SetSelectedWidgetItem();
    ui->m_SelectedtableWidget->selectRow(m_iCurrentItemNo);
    ui->m_DelButton->setEnabled(true);
    ui->m_UpButton->setEnabled(true);
    ui->m_DownButton->setEnabled(true);
    ui->m_TopButton->setEnabled(true);
    ui->m_BottomButton->setEnabled(true);
}

void QSelectMultiParamDlg::on_m_SelectedtableWidget_cellClicked(int row, int )
{
    ui->m_DelButton->setEnabled(false);
    ui->m_UpButton->setEnabled(false);
    ui->m_DownButton->setEnabled(false);
    ui->m_TopButton->setEnabled(false);
    ui->m_BottomButton->setEnabled(false);
    QList<QTableWidgetItem* > items = ui->m_SelectedtableWidget->selectedItems();
    m_iCurrentItemNo = row;
    if(items.count() > 0)
    {
        ui->m_DelButton->setEnabled(true);
        ui->m_UpButton->setEnabled(true);
        ui->m_DownButton->setEnabled(true);
        ui->m_TopButton->setEnabled(true);
        ui->m_BottomButton->setEnabled(true);
    }
}
QVector<QString> QSelectMultiParamDlg::GetParamList()
{
    QVector<QString> Vector;
    Vector.clear();
    for(int j = 0;j < SelectedVector.count();j++)
    {
        QString paramstr = "[";
        QString str = SelectedVector.at(j);
        int pos = str.indexOf(',');
        paramstr = paramstr + str.left(pos);
        str = str.right(str.length()-pos-1);
        pos = str.indexOf(',');
        paramstr += ",";
        paramstr += str.left(pos);
        paramstr += "]";
        Vector.push_back(paramstr);
    }
    return Vector;
}


