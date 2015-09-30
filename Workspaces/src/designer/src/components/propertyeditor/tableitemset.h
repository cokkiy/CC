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
    QTableCell GetTableCell();
private slots:

    void on_textFontpushButton_clicked();

    void on_BGColorpushButton_clicked();

    void on_textColorpushButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::tableitemset *ui;
    Qt::Alignment m_TextHAlignment;
    Qt::Alignment m_TextVAlignment;
    QFont m_TextFont;
	QColor m_TextColor;
	QColor m_ItemBGColor;
    QTableCell m_TableCell;
};

#endif // TABLEITEMSET_H
