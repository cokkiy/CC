#ifndef TABLEPROPERTYSETDLG_H
#define TABLEPROPERTYSETDLG_H
#include <QtWidgets/QTableWidget>
#include <QDialog>
#include <QMenu>
#include "propertyeditor_global.h"
#include <Net/NetComponents>
#include "tablecell.h"
#include "tableproperty.h"

class QDesignerFormEditorInterface;

QT_BEGIN_NAMESPACE
namespace qdesigner_internal {

class QT_PROPERTYEDITOR_EXPORT QTablePropertySetDlg : public QDialog
{
    Q_OBJECT
public:
    explicit QTablePropertySetDlg(QDesignerFormEditorInterface *core,QWidget *parent = 0);
    int showDialog();
signals:

public slots:
private:
    QDesignerFormEditorInterface *m_core;
    void SetTableItemTextType();
    void InitTable();
    void SetVectorTableItemSize();
    void InitTableContent();
    void InitbottomLayout();
    QGridLayout *m_leftLayout;
    QLabel *m_formNameLabel;
    QTableWidget *m_TableWidget;

    QGridLayout *m_bottomLayout;
    QDialogButtonBox *buttonBox;

private:
    bool m_bEditState;
    int m_iCurrentRow;
    int m_iCurrentCol;
    int m_iCurrentEditRow;
    int m_iCurrentEditCol;
    int m_iRowNum;
    int m_iColNum;
    QVector<QTableCell > m_Table;
	//文本内容
    qint8 m_TextType;
	//水平对齐方式
    Qt::Alignment m_TextHAlignment;
	//垂直对齐方式
    Qt::Alignment m_TextVAlignment;
public:
    void InitTableVector();
private slots:
    void OnItemSelectionChanged();
    void OnSectionPressed(int );
    void OnHSectionResized(int logicalIndex, int oldSize, int newSize);
    void OnVSectionResized(int logicalIndex, int oldSize, int newSize);
    void OnCellClicked(int ,int );
    void OnItemEdit(QTableWidgetItem *);
	//表格初始设置，行数和列数
	void SetTable();
	//关联参数
    void SetParam();
    void AddRow();
    void InsertRow();
    void DelRow();
    void AddColumn();
    void InsertColumn();
    void DelColumn();

	//显示默认内容
    void SetShowDefault();
	//显示参数名称
    void SetShowParaName();
	 //显示参数代号
    void SetShowParaCode();
	//参数编号
	void SetShowParaNumber();
	//显示参数传输类型
    void SetShowParaTransType();
	//显示参数转换类型
    void SetShowParaConvType();
	//显示参数下限
    void SetShowParaLowerLimite();
	//显示参数上限
    void SetShowParaUpperLimite();
	//显示参数量纲
    void SetShowParaDimension();
	//显示参数单位
    void SetShowParaUnit();
	//显示参数显示格式
    void SetShowParaDisplayType();
	//显示参数理论值
    void SetShowParaTheoryValue();
	//显示参数理论值范围
    void SetShowParaTheoryValueRange();
	//显示参数备注
    void SetShowParaRemark();
    //单元格格式
    void SetItemFormat();

private:
    //右键菜单事件
    void contextMenuEvent(QContextMenuEvent* );
	//处理右键菜单
    bool HandleMenuVisible(QCursor );
	//右键菜单
    QMenu* MainMenu;
	//初始化右键菜单
    void InitMenu();
	//初始化右键菜单可见状态
	void InitMenuVisible();

	//关联参数
    QAction* act_SetParam;
    bool m_bSetParamMenuVisible;

	//表格设置
    QAction* act_SetTable;
	//表格设置菜单是否可见
    bool m_bSetTableMenuVisible;

    //增加行
    QAction* act_AddRow;
    //增加列
    QAction* act_AddColumn;
    //增加行或列菜单是否可见
    bool m_bAddDelMenuVisible;

    //插入行
    QAction* act_InsertRow;
    //删除行
    QAction* act_DelRow;
    //插入列
    QAction* act_InsertColumn;
    //删除列
    QAction* act_DelColumn;
    //行和列操作菜单是否可见
    bool m_bRCHandleMenuVisible;

	//单元格格式
    QAction* act_ItemFormat;
    //单元格格式菜单是否可见
    bool m_bItemFormatVisible;

	//列内容
    QMenu* ColumnContextMenu;
    //默认显示
    QAction* act_ShowDefault;
    //显示参数名称
    QAction* act_ShowParaName;
	 //显示参数代号
    QAction* act_ShowParaCode;
	//参数编号
    QAction* act_ShowParaNumber;
	//显示参数传输类型
    QAction* act_ShowParaTransType;
	//显示参数转换类型
    QAction* act_ShowParaConvType;
	//显示参数下限
    QAction* act_ShowParaLowerLimite;
	//显示参数上限
    QAction* act_ShowParaUpperLimite;
	//显示参数量纲
    QAction* act_ShowParaDimension;
	//显示参数单位
    QAction* act_ShowParaUnit;
	//显示参数显示格式
    QAction* act_ShowParaDisplayType;
	//显示参数理论值
    QAction* act_ShowParaTheoryValue;
	//显示参数理论值范围
    QAction* act_ShowParaTheoryValueRange;
	//显示参数备注
    QAction* act_ShowParaRemark;

    //列内容菜单是否可见
    bool m_bColumnContextMenuVisible;
};
}
QT_END_NAMESPACE

#endif // TABLEPROPERTYSET_H
