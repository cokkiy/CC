#include "tableitemset.h"
#include "ui_tableitemset.h"
#include <QColorDialog>
#include <QFontDialog>

tableitemset::tableitemset(QTableCell TableCell,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableitemset)
{
    m_TextHAlignment = TableCell.GetTextHAlignment();
    m_TextVAlignment = TableCell.GetTextVAlignment();
    m_TextFont = TableCell.GetTextFont();
    m_TextColor = TableCell.GetTextColor();
    m_ItemBGColor = TableCell.GetBackgroundColor();
    ui->setupUi(this);
    if(m_TextHAlignment == Qt::AlignLeft)
        ui->AlignmentHcomboBox->setCurrentIndex(0);
    else if(m_TextHAlignment == Qt::AlignHCenter)
        ui->AlignmentHcomboBox->setCurrentIndex(1);
    else
        ui->AlignmentHcomboBox->setCurrentIndex(2);
    if(m_TextVAlignment == Qt::AlignTop)
        ui->AlignmentVcomboBox->setCurrentIndex(0);
    else if(m_TextVAlignment == Qt::AlignVCenter)
        ui->AlignmentVcomboBox->setCurrentIndex(1);
    else
        ui->AlignmentVcomboBox->setCurrentIndex(2);

    ui->textColorpushButton->setAutoFillBackground(true);
    QPalette Palette = ui->textColorpushButton->palette();
    Palette.setColor(QPalette::Active,QPalette::Button,m_TextColor);
    ui->textColorpushButton->setPalette(Palette);

    QString str = m_TextFont.toString();
    int pos = str.indexOf(',');
    str = str.left(pos-1);
    ui->textFontpushButton->setText(str);
    ui->textFontpushButton->setFont(m_TextFont);

    ui->BGColorpushButton->setAutoFillBackground(true);
    Palette = ui->BGColorpushButton->palette();
    Palette.setColor(QPalette::Active,QPalette::Button,m_ItemBGColor);
    ui->BGColorpushButton->setPalette(Palette);
}

tableitemset::~tableitemset()
{
    delete ui;
}

void tableitemset::on_textFontpushButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,
                                      ui->textFontpushButton->font(),
                                      this,tr("set font"));
    if(ok)
    {
        m_TextFont = font;
        QString str = m_TextFont.toString();
        int pos = str.indexOf(',');
        str = str.left(pos-1);
        ui->textFontpushButton->setText(str);
        ui->textFontpushButton->setFont(font);
    }
}

void tableitemset::on_BGColorpushButton_clicked()
{
    QPalette Palette = ui->BGColorpushButton->palette();
    QColor color = QColorDialog::getColor(Palette.color(QPalette::Base),this);
    if(color.isValid())
    {
        Palette.setColor(QPalette::Active,QPalette::Button,color);
        ui->BGColorpushButton->setPalette(Palette);
        m_ItemBGColor = color;
    }
}

void tableitemset::on_textColorpushButton_clicked()
{
    QPalette Palette = ui->textColorpushButton->palette();
    QColor color = QColorDialog::getColor(Palette.color(QPalette::Base),this);
    if(color.isValid())
    {
        Palette.setColor(QPalette::Active,QPalette::Button,color);
        ui->textColorpushButton->setPalette(Palette);
        m_TextColor = color;
    }
}

void tableitemset::on_buttonBox_accepted()
{
    if(ui->AlignmentHcomboBox->currentIndex() == 0 )
        m_TextHAlignment = Qt::AlignLeft;
    else if(ui->AlignmentHcomboBox->currentIndex() == 1 )
        m_TextHAlignment = Qt::AlignHCenter;
    else
        m_TextHAlignment = Qt::AlignRight;
    if(ui->AlignmentVcomboBox->currentIndex() == 0 )
        m_TextVAlignment = Qt::AlignTop;
    else if(ui->AlignmentVcomboBox->currentIndex() == 1 )
        m_TextVAlignment = Qt::AlignVCenter;
    else
        m_TextVAlignment = Qt::AlignBottom;
}
QTableCell tableitemset::GetTableCell()
{
    m_TableCell.SetTextColor(m_TextColor);
    m_TableCell.SetTextFont(m_TextFont);
    m_TableCell.SetBackgroundColor(m_ItemBGColor);
    m_TableCell.SetTextHAlignment(m_TextHAlignment);
    m_TableCell.SetTextVAlignment(m_TextVAlignment);
    return m_TableCell;
}


