#ifndef TABLEITEMSET_H
#define TABLEITEMSET_H
#include "tablecell.h"
#include <QDialog>

namespace Ui {
class tableitemset;
}

class tableitemset : public QDialog
{
    Q_OBJECT

public:
    explicit tableitemset(QTableCell ,QWidget *parent = 0);
    ~tableitemset();
	//获取设置的单元格属性
    QTableCell getTableCell();
private slots:

    void on_textFontpushButton_clicked();

    void on_BGColorpushButton_clicked();

    void on_textColorpushButton_clicked();

    void on_buttonBox_accepted();

    void on_moreButton_clicked();

private:
	//ui界面
    Ui::tableitemset *ui;
	//更多操作按钮状态
    bool bMoreOperate;
	//ui 对话框geometry
    QRect uiRect;
	//水平对齐方式
    Qt::Alignment m_TextHAlignment;
	//上下浮动方式
    Qt::Alignment m_TextVAlignment;
	//文本字体
    QFont m_TextFont;
	//文本颜色
	QColor m_TextColor;
	//背景色
	QColor m_ItemBGColor;
	//单元格属性
    QTableCell m_TableCell;
};

#endif // TABLEITEMSET_H
