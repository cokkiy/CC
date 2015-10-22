﻿#ifndef TABLEITEMSET_H
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
	//��ȡ���õĵ�Ԫ������
    QTableCell getTableCell();
private slots:

    void on_textFontpushButton_clicked();

    void on_BGColorpushButton_clicked();

    void on_textColorpushButton_clicked();

    void on_buttonBox_accepted();

    void on_moreButton_clicked();

private:
	//ui����
    Ui::tableitemset *ui;
	//���������ť״̬
    bool bMoreOperate;
	//ui �Ի���geometry
    QRect uiRect;
	//ˮƽ���뷽ʽ
    Qt::Alignment m_TextHAlignment;
	//���¸�����ʽ
    Qt::Alignment m_TextVAlignment;
	//�ı�����
    QFont m_TextFont;
	//�ı���ɫ
	QColor m_TextColor;
	//����ɫ
	QColor m_ItemBGColor;
	//��Ԫ������
    QTableCell m_TableCell;
};

#endif // TABLEITEMSET_H
