#include <QtDesigner/QDesignerFormEditorInterface>
#include "tablepropertysetDlg.h"
#include "tableitemset.h"
#include "tableset.h"
#include "selectmultiparamdlg.h"
QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
QTablePropertySetDlg::QTablePropertySetDlg(QDesignerFormEditorInterface *core,QWidget *parent) :
    QDialog(parent),
    m_core(core)
{
    m_Table.clear();
    m_bAddDelMenuVisible = false;
    m_bColumnContextMenuVisible = false;
    m_bItemFormatVisible = false;
    m_bRCHandleMenuVisible = false;
    m_bSetTableMenuVisible = false;
    m_bSetParamMenuVisible = false;
    m_TextType = tp_Default;
    m_TextHAlignment = Qt::AlignHCenter ;
    m_TextVAlignment = Qt::AlignVCenter ;
    m_bEditState = false;
    m_iRowNum = 4;
    m_iColNum = 4;
    m_iCurrentRow = -1;
    m_iCurrentCol = -1;
    m_iCurrentEditRow = -1;
    m_iCurrentEditCol = -1;
    InitTableVector();
    MainMenu = new QMenu(this);
    InitMenu();
    resize(1024,768);
    setWindowTitle(tr("表格属性设置"));
    InitbottomLayout();
    m_TableWidget = new QTableWidget();
    InitTable();
    QGridLayout *mainLayout = new QGridLayout(this);
    m_leftLayout = new QGridLayout();
    m_leftLayout->addWidget(m_TableWidget,1,0);
    mainLayout->addLayout(m_leftLayout,2,0);
    mainLayout->addLayout(m_bottomLayout,4,0,Qt::AlignRight);
}
int QTablePropertySetDlg::showDialog()
{
    return exec();
}
void QTablePropertySetDlg::InitTable()
{
    InitTableContent();
    connect(m_TableWidget,SIGNAL(cellPressed(int,int)),this,SLOT(OnCellClicked(int,int)));
//    connect(m_TableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(OnCellChanged(int,int)));
    //connect(m_TableWidget->horizontalHeader(),SIGNAL(sectionPressed(int)),this,SLOT(OnSectionPressed(int)));

    //connect(m_TableWidget->horizontalHeader(),SIGNAL(sectionResized(int,int,int )),this,SLOT(OnHSectionResized(int,int,int)));
 //   connect(m_TableWidget->verticalHeader(),SIGNAL(sectionResized(int,int,int )),this,SLOT(OnVSectionResized(int,int,int)));
    connect(m_TableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(OnItemEdit(QTableWidgetItem *)));
    connect(m_TableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(OnItemSelectionChanged()));
    //connect(m_TableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(OnItemChanged(QTableWidgetItem *)));
    //QTableWidgetSelectionRange range(0,0,5,5);
    //m_TableWidget->setRangeSelected(range,true);
    m_TableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //m_TableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
}

void QTablePropertySetDlg::InitTableContent()
{
    m_TableWidget->reset();
    m_TableWidget->setRowCount(m_iRowNum);
    m_TableWidget->setColumnCount(m_iColNum);
    for(int i = 0; i < m_iRowNum; i++)
    {
        QTableCell Cell = m_Table.at(i*m_iColNum);
        m_TableWidget->setRowHeight(i,Cell.GetHeight());
        for(int j = 0;j< m_iColNum; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            QTableCell TableCell = m_Table.at(i*m_iColNum + j);
            m_TableWidget->setColumnWidth(j,TableCell.GetWidth());
            item->setFont(TableCell.GetTextFont());
            item->setTextAlignment(TableCell.GetTextHAlignment()|TableCell.GetTextVAlignment());
            item->setTextColor(TableCell.GetTextColor());
            item->setText(TableCell.GetConfigModeContent());
            item->setBackgroundColor(TableCell.GetBackgroundColor());
            m_TableWidget->setItem(i,j,item);
        }
    }
}
void QTablePropertySetDlg::OnItemSelectionChanged()
{
    if(m_bEditState)
    {
       QTableWidgetItem *item = m_TableWidget->item(m_iCurrentEditRow,m_iCurrentEditCol);
       QTableCell TableCell = m_Table.at(m_iCurrentEditRow*m_iColNum + m_iCurrentEditCol);
       QString str = item->text();
       TableCell.SetText(str);
       m_Table.replace(m_iCurrentEditRow*m_iColNum + m_iCurrentEditCol,TableCell);
       InitTableContent();
    }
    m_bEditState = false;

}
void QTablePropertySetDlg::OnSectionPressed(int )
{
    m_bColumnContextMenuVisible = true;
}

void QTablePropertySetDlg::OnHSectionResized(int logicalIndex, int , int newSize)
{
    for(int i = 0;i<m_iRowNum;i++)
    {
        QTableCell TableCell = m_Table.at(i*m_iColNum + logicalIndex);
        TableCell.SetWidth(newSize);
        m_Table.replace(i*m_iColNum + logicalIndex,TableCell);
    }
}

void QTablePropertySetDlg::OnVSectionResized(int logicalIndex, int , int newSize)
{
    for(int i = 0;i<m_iColNum;i++)
    {
        QTableCell TableCell = m_Table.at(logicalIndex*m_iRowNum + i);
        TableCell.SetHeight(newSize);
        m_Table.replace(i*m_iColNum + logicalIndex,TableCell);
    }
}

void QTablePropertySetDlg::OnItemEdit(QTableWidgetItem * item)
{
    m_iCurrentEditRow = item->row();
    m_iCurrentEditCol  = item->column();
    QTableCell TableCell = m_Table.at(m_iCurrentEditRow*m_iColNum + m_iCurrentEditCol);
    item->setText(TableCell.GetText());
    m_TableWidget->setItem(m_iCurrentEditRow,m_iCurrentEditCol,item);
    m_bEditState = true;
}

void QTablePropertySetDlg::OnCellClicked(int row,int col)
{
    m_iCurrentRow = row;
    m_iCurrentCol = col;
	m_bRCHandleMenuVisible = true;
}

void QTablePropertySetDlg::InitbottomLayout()
{
    m_bottomLayout = new QGridLayout();
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    QPushButton *ok_button = buttonBox->button(QDialogButtonBox::Ok);
    ok_button->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    m_bottomLayout->addWidget(buttonBox);
}

bool QTablePropertySetDlg::HandleMenuVisible(QCursor cur)
{
    QPoint point = mapFromGlobal(cur.pos());
    QRect rect = m_TableWidget->geometry();
    if(!((point.y()<rect.bottom())&&
        (point.y()>rect.top())&&
        (point.x()<rect.right())&&
        (point.x()>rect.left())))
        return false;
    if(m_bRCHandleMenuVisible)
	{
		m_bAddDelMenuVisible = true;
		if(m_iRowNum == 0 && m_iColNum == 0)
			m_bSetTableMenuVisible = true;
		QList<QTableWidgetItem* > items = m_TableWidget->selectedItems();
		if(items.count() > 0)
			m_bItemFormatVisible = true;
	}

    QList<QTableWidgetItem* > items = m_TableWidget->selectedItems();
    for(int i = 0;i < items.count();i++)
    {
        if(items.at(i)->column() != 0)
        {
            m_bSetParamMenuVisible = false;
            break;
        }
        m_bSetParamMenuVisible = true;
    }
    if(items.count() > 0)
        m_bColumnContextMenuVisible = true;

	if(m_bAddDelMenuVisible)
	{
		if(m_iRowNum == 0 && m_iColNum == 0)
			m_bSetTableMenuVisible = true;
	}
    if(m_bColumnContextMenuVisible)
    {
        m_bItemFormatVisible = true;
        if(m_iRowNum == 0 && m_iColNum == 0)
            m_bSetTableMenuVisible = true;
        m_bAddDelMenuVisible = true;
    }

    m_bAddDelMenuVisible = true;
    if(m_iRowNum == 0 && m_iColNum == 0)
        m_bSetTableMenuVisible = true;

    if(m_iRowNum > 0 && m_iColNum > 0)
        m_bSetTableMenuVisible = false;
	if(m_bSetTableMenuVisible||m_bAddDelMenuVisible||
        m_bRCHandleMenuVisible||m_bItemFormatVisible||
        m_bColumnContextMenuVisible||m_bSetParamMenuVisible)
		return true;
    else
        return false;
}

void QTablePropertySetDlg::contextMenuEvent(QContextMenuEvent*)
{
    QCursor cur=this->cursor();
    if(HandleMenuVisible(cur))
    {
        InitMenuVisible();
		QCursor cur=this->cursor();
        MainMenu->exec(cur.pos());
        m_iCurrentRow = -1;
        m_iCurrentCol = -1;
    }
    m_bAddDelMenuVisible = false;
    m_bColumnContextMenuVisible = false;
    m_bItemFormatVisible = false;
    m_bRCHandleMenuVisible = false;
    m_bSetTableMenuVisible = false;
    m_bSetParamMenuVisible = false;
}

void QTablePropertySetDlg::SetTableItemTextType()
{
    QList<QTableWidgetItem* > items = m_TableWidget->selectedItems();
    for(int i = 0;i<items.count();i++)
    {
        int col = items.at(i)->column();
        int row = items.at(i)->row();
        QTableCell TableCell = m_Table.at(row*m_iColNum + col);
        TableCell.SetTextType(m_TextType);
        m_Table.replace(row*m_iColNum + col,TableCell);
    }
    InitTableContent();
}

void QTablePropertySetDlg::SetTable()
{
    QTableSet Set;
    if(Set.exec() == QDialog::Accepted)
    {
        Set.GetNum(m_iRowNum,m_iColNum);
        InitTableVector();
        InitTable();
    }
}

//关联参数
void QTablePropertySetDlg::SetParam()
{
    QSelectMultiParamDlg Dlg;
    if(Dlg.exec() == QDialog::Accepted)
    {
        QVector<QString> vector = Dlg.GetParamList();
        QList<QTableWidgetItem* > items = m_TableWidget->selectedItems();
        int num = 0;
        if(vector.count() > items.count())
            num = items.count();
        else
            num = vector.count();
        for(int i = 0;i < num; i++)
        {
            for(int j = 0;j < m_iColNum;j++)
            {
                QTableCell TableCell = m_Table.at(items.at(i)->row()*m_iColNum + j);
                TableCell.SetParamStr(vector.at(0));
                m_Table.replace(items.at(i)->row()*m_iColNum + j,TableCell);
            }
            vector.pop_front();
        }
        InitTable();
//        for(int i = 0;i<items.count();i++)
//        {
//            int r = items.at(i)->row();
//            int c = items.at(i)->column();
//            m_TableWidget->setItemSelected(items.at(i),true);
//        }
    }
}

//初始化右键菜单可见状态
void QTablePropertySetDlg::InitMenuVisible()
{
    act_SetParam->setVisible(m_bSetParamMenuVisible);
    act_SetTable->setVisible(m_bSetTableMenuVisible);

    act_AddRow->setVisible(m_bAddDelMenuVisible);
    act_AddColumn->setVisible(m_bAddDelMenuVisible);

    act_DelRow->setVisible(m_bRCHandleMenuVisible);
    act_InsertRow->setVisible(m_bRCHandleMenuVisible);
    act_DelColumn->setVisible(m_bRCHandleMenuVisible);
    act_InsertColumn->setVisible(m_bRCHandleMenuVisible);

    act_ItemFormat->setVisible(m_bItemFormatVisible);

    if(!m_bColumnContextMenuVisible)
        ColumnContextMenu->hide();
    act_ShowDefault->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaName->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaCode->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaNumber->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaTransType->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaConvType->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaLowerLimite->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaUpperLimite->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaDimension->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaUnit->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaDisplayType->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaTheoryValue->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaTheoryValueRange->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaRemark->setVisible(m_bColumnContextMenuVisible);
}

void QTablePropertySetDlg::InitMenu()
{
    //关联参数
	act_SetParam = new QAction(tr("关联参数"),this);
    connect(act_SetParam,SIGNAL(triggered(bool)),this,SLOT(SetParam()));
    MainMenu->addAction(act_SetParam);
	
	//表格设置
	act_SetTable = new QAction(tr("表格设置"),this);
    connect(act_SetTable,SIGNAL(triggered(bool)),this,SLOT(SetTable()));
    MainMenu->addAction(act_SetTable);

    act_AddRow = new QAction(tr("增加行"),this);
    connect(act_AddRow,SIGNAL(triggered(bool)),this,SLOT(AddRow()));
    MainMenu->addAction(act_AddRow);

    act_InsertRow = new QAction(tr("插入行"),this);
    connect(act_InsertRow,SIGNAL(triggered(bool)),this,SLOT(InsertRow()));
    MainMenu->addAction(act_InsertRow);

    act_DelRow = new QAction(tr("删除行"),this);
    connect(act_DelRow,SIGNAL(triggered(bool)),this,SLOT(DelRow()));
    MainMenu->addAction(act_DelRow);

    MainMenu->addSeparator();
    
    act_AddColumn = new QAction(tr("增加列"),this);
    connect(act_AddColumn,SIGNAL(triggered(bool)),this,SLOT(AddColumn()));
    MainMenu->addAction(act_AddColumn);

    act_InsertColumn = new QAction(tr("插入列"),this);
    connect(act_InsertColumn,SIGNAL(triggered(bool)),this,SLOT(InsertColumn()));
    MainMenu->addAction(act_InsertColumn);

    act_DelColumn = new QAction(tr("删除列"),this);
    connect(act_DelColumn,SIGNAL(triggered(bool)),this,SLOT(DelColumn()));
    MainMenu->addAction(act_DelColumn);

    MainMenu->addSeparator();

    act_ItemFormat = new QAction(tr("格式"),this);
    connect(act_ItemFormat,SIGNAL(triggered(bool)),this,SLOT(SetItemFormat()));
    MainMenu->addAction(act_ItemFormat);
    act_ItemFormat->setVisible(false);

    MainMenu->addSeparator();
    ColumnContextMenu = MainMenu->addMenu(tr("列内容"));
	act_ShowDefault = new QAction(tr("默认"),this);
    connect(act_ShowDefault,SIGNAL(triggered(bool)),this,SLOT(SetShowDefault()));
    ColumnContextMenu->addAction(act_ShowDefault);

	act_ShowParaName = new QAction(tr("名称"),this);
    connect(act_ShowParaName,SIGNAL(triggered(bool)),this,SLOT(SetShowParaName()));
    ColumnContextMenu->addAction(act_ShowParaName);

	act_ShowParaNumber = new QAction(tr("编号"),this);
    connect(act_ShowParaNumber,SIGNAL(triggered(bool)),this,SLOT(SetShowParaNumber()));
    ColumnContextMenu->addAction(act_ShowParaNumber);

	act_ShowParaCode = new QAction(tr("代号"),this);
    connect(act_ShowParaCode,SIGNAL(triggered(bool)),this,SLOT(SetShowParaCode()));
    ColumnContextMenu->addAction(act_ShowParaCode);
	
	act_ShowParaDimension = new QAction(tr("量纲"),this);
    connect(act_ShowParaDimension,SIGNAL(triggered(bool)),this,SLOT(SetShowParaDimension()));
    ColumnContextMenu->addAction(act_ShowParaDimension);
	
	act_ShowParaUnit = new QAction(tr("单位"),this);
    connect(act_ShowParaUnit,SIGNAL(triggered(bool)),this,SLOT(SetShowParaUnit()));
    ColumnContextMenu->addAction(act_ShowParaUnit);

    act_ShowParaUpperLimite = new QAction(tr("上限"),this);
    connect(act_ShowParaUpperLimite,SIGNAL(triggered(bool)),this,SLOT(SetShowParaUpperLimite()));
    ColumnContextMenu->addAction(act_ShowParaUpperLimite);

    act_ShowParaLowerLimite = new QAction(tr("下限"),this);
    connect(act_ShowParaLowerLimite,SIGNAL(triggered(bool)),this,SLOT(SetShowParaLowerLimite()));
    ColumnContextMenu->addAction(act_ShowParaLowerLimite);
	
	act_ShowParaDisplayType = new QAction(tr("显示格式"),this);
    connect(act_ShowParaDisplayType,SIGNAL(triggered(bool)),this,SLOT(SetShowParaDisplayType()));
    ColumnContextMenu->addAction(act_ShowParaDisplayType);

	act_ShowParaTheoryValue = new QAction(tr("理论值"),this);
    connect(act_ShowParaTheoryValue,SIGNAL(triggered(bool)),this,SLOT(SetShowParaTheoryValue()));
    ColumnContextMenu->addAction(act_ShowParaTheoryValue);
	
	act_ShowParaTheoryValueRange = new QAction(tr("范围"),this);
    connect(act_ShowParaTheoryValueRange,SIGNAL(triggered(bool)),this,SLOT(SetShowParaTheoryValueRange()));
    ColumnContextMenu->addAction(act_ShowParaTheoryValueRange);
		
	act_ShowParaTransType = new QAction(tr("传输类型"),this);
    connect(act_ShowParaTransType,SIGNAL(triggered(bool)),this,SLOT(SetShowParaTransType()));
    ColumnContextMenu->addAction(act_ShowParaTransType);

	act_ShowParaConvType = new QAction(tr("转换类型"),this);
    connect(act_ShowParaConvType,SIGNAL(triggered(bool)),this,SLOT(SetShowParaConvType()));
    ColumnContextMenu->addAction(act_ShowParaConvType);


    act_ShowParaRemark = new QAction(tr("备注"),this);
    connect(act_ShowParaRemark,SIGNAL(triggered(bool)),this,SLOT(SetShowParaRemark()));
    ColumnContextMenu->addAction(act_ShowParaRemark);

}

void QTablePropertySetDlg::InitTableVector()
{
    m_Table.clear();
    for(int i = 0;i<m_iRowNum;i++)
        for(int j = 0;j<m_iColNum;j++)
        {
            QTableCell TableCell;
            m_Table.push_back(TableCell);
        }
}
void QTablePropertySetDlg::AddRow()
{
    SetVectorTableItemSize();
    for(int i = 0;i<m_iColNum;i++)
    {
        QTableCell TableCell;
        m_Table.push_back(TableCell);
    }
    m_iRowNum++;
    InitTableContent();
}

void QTablePropertySetDlg::InsertRow()
{
    SetVectorTableItemSize();
    for(int i = 0;i<m_iColNum;i++)
    {
        QTableCell TableCell;
        m_Table.insert(m_Table.begin() + m_iCurrentRow*m_iColNum + i,TableCell);
    }
    m_iRowNum++;
    InitTableContent();
}
void QTablePropertySetDlg::DelRow()
{
    SetVectorTableItemSize();
    for(int i = 0;i<m_iColNum;i++)
        m_Table.erase(m_Table.begin() + m_iCurrentRow*m_iColNum);
    m_iRowNum--;
    if(m_iRowNum == 0)
        m_iColNum = 0;
    InitTableContent();
}
void QTablePropertySetDlg::AddColumn()
{
    SetVectorTableItemSize();
    for(int i = 0;i<m_iRowNum;i++)
    {
        QTableCell TableCell;
        m_Table.insert(m_Table.begin() + (i+1)*m_iColNum + i,TableCell);
    }
    m_iColNum++;
    InitTableContent();
}

void QTablePropertySetDlg::InsertColumn()
{
    SetVectorTableItemSize();
    for(int i = 0;i<m_iRowNum;i++)
    {
        QTableCell TableCell;
        m_Table.insert(m_Table.begin() + i*m_iColNum + m_iCurrentCol + i,TableCell);
    }
    m_iColNum++;
    InitTableContent();
}
void QTablePropertySetDlg::DelColumn()
{
    SetVectorTableItemSize();
    for(int i = 0;i<m_iRowNum;i++)
        m_Table.erase(m_Table.begin() + i*m_iColNum + m_iCurrentCol - i);
    m_iColNum--;
    if(m_iColNum == 0)
        m_iRowNum = 0;
    InitTableContent();
}

void QTablePropertySetDlg::SetVectorTableItemSize()
{
    for(int i = 0; i < m_iRowNum; i++)
    {
        int h = m_TableWidget->rowHeight(i);
        for(int j = 0;j< m_iColNum; j++)
        {
            int w = m_TableWidget->columnWidth(j);
            QTableCell TableCell = m_Table.at(i*m_iColNum+j);
            TableCell.SetWidth(w);
            TableCell.SetHeight(h);
            m_Table.replace(i*m_iColNum+j,TableCell);
        }
     }
}
//显示默认内容
void QTablePropertySetDlg::SetShowDefault()
{
    m_TextType = tp_Default;
    SetTableItemTextType();
}
//显示参数名称
void QTablePropertySetDlg::SetShowParaName()
{
    m_TextType = tp_Name;
    SetTableItemTextType();
}
void QTablePropertySetDlg::SetShowParaNumber()
{
    m_TextType = tp_No;
    SetTableItemTextType();
}
//显示参数代号
void QTablePropertySetDlg::SetShowParaCode()
{
    m_TextType = tp_Mark;
    SetTableItemTextType();
}
//显示参数传输类型
void QTablePropertySetDlg::SetShowParaTransType()
{
    m_TextType = tp_TransType;
    SetTableItemTextType();
}
//显示参数转换类型
void QTablePropertySetDlg::SetShowParaConvType()
{
    m_TextType = tp_ConvType;
    SetTableItemTextType();
}
//显示参数下限
void QTablePropertySetDlg::SetShowParaLowerLimite()
{
    m_TextType = tp_LowerLimite;
    SetTableItemTextType();
}
//显示参数上限
void QTablePropertySetDlg::SetShowParaUpperLimite()
{
    m_TextType = tp_UpperLimite;
    SetTableItemTextType();
}
//显示参数量纲
void QTablePropertySetDlg::SetShowParaDimension()
{
    m_TextType = tp_Dimension;
    SetTableItemTextType();
}
//显示参数单位
void QTablePropertySetDlg::SetShowParaUnit()
{
    m_TextType = tp_Unit;
    SetTableItemTextType();
}
//显示参数显示格式
void QTablePropertySetDlg::SetShowParaDisplayType()
{
    m_TextType = tp_DisplayType;
    SetTableItemTextType();
}
//显示参数理论值
void QTablePropertySetDlg::SetShowParaTheoryValue()
{
    m_TextType = tp_TheoryValue;
    SetTableItemTextType();
}
//显示参数理论值范围
void QTablePropertySetDlg::SetShowParaTheoryValueRange()
{
    m_TextType = tp_TheoryValueRange;
    SetTableItemTextType();
}
//显示参数备注
void QTablePropertySetDlg::SetShowParaRemark()
{
    m_TextType = tp_Remark;
    SetTableItemTextType();
}

//单元格文本颜色
void QTablePropertySetDlg::SetItemFormat()
{
    QList<QTableWidgetItem* > items = m_TableWidget->selectedItems();
    int col = items.at(0)->column();
    int row = items.at(0)->row();
    QTableCell TableCell = m_Table.at(row*m_iColNum + col);
    tableitemset set(TableCell);
    if(set.exec() == QDialog::Accepted)
    {
        TableCell = set.GetTableCell();
        for(int i = 0;i<items.count();i++)
        {
            int col = items.at(i)->column();
            int row = items.at(i)->row();
            QTableCell TableCell1 = m_Table.at(row*m_iColNum + col);
            TableCell1.SetTextColor(TableCell.GetTextColor());
            TableCell1.SetTextFont(TableCell.GetTextFont());
            TableCell1.SetTextHAlignment(TableCell.GetTextHAlignment());
            TableCell1.SetTextVAlignment(TableCell.GetTextVAlignment());
            TableCell1.SetBackgroundColor(TableCell.GetBackgroundColor());
            m_Table.replace(row*m_iColNum + col,TableCell1);
        }
        InitTableContent();
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
