#include "tableitemset.h"
#include "ui_tableitemset.h"
#include <QColorDialog>
#include <QFontDialog>

tableitemset::tableitemset(QTableCell TableCell,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableitemset)
{
    m_TextHAlignment = TableCell.getTextHAlignment();
    m_TextVAlignment = TableCell.getTextVAlignment();
    m_TextFont = TableCell.getTextFont();
    m_TextColor = TableCell.getTextColor();
    m_ItemBGColor = TableCell.getBackgroundColor();
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

    QString str = m_TextFont.family();
    str = str + tr("  ") + QString::number(m_TextFont.pointSize());
    ui->textFontpushButton->setText(str);
    ui->textFontpushButton->setFont(m_TextFont);
    ui->BGColorpushButton->setAutoFillBackground(true);
    Palette = ui->BGColorpushButton->palette();
    Palette.setColor(QPalette::Active,QPalette::Button,m_ItemBGColor);
    ui->BGColorpushButton->setPalette(Palette);

    ui->LeftEdgeStateCheckBox->setChecked(TableCell.getLeftEdgeVisible());
    ui->TopEdgeStateCheckBox->setChecked(TableCell.getTopEdgeVisible());

    ui->LeftMarginEdit->setText(QString::number(TableCell.getLeftMargin()));
    ui->RightMarginEdit->setText(QString::number(TableCell.getRightMargin()));
    ui->TopMarginEdit->setText(QString::number(TableCell.getTopMargin()));
    ui->BottomMarginEdit->setText(QString::number(TableCell.getBottomMargin()));

    ui->ShowTypeEdit->setText(TableCell.getShowType());
    QDesktopWidget* pDesk = QApplication::desktop();

    uiRect = geometry();
    bMoreOperate = false;
    setGeometry((pDesk->width() - uiRect.width())/2,(pDesk->height() - uiRect.height())/2,uiRect.width()/2,uiRect.height());
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
        QString str = m_TextFont.family();
        str = str + tr("  ") + QString::number(m_TextFont.pointSize());
        ui->textFontpushButton->setText(str);
        ui->textFontpushButton->setFont(font);
    }
}

void tableitemset::on_BGColorpushButton_clicked()
{
    QColor color = QColorDialog::getColor(m_ItemBGColor,this,QString(), QColorDialog::ShowAlphaChannel);
    if(color.isValid())
    {
        QPalette Palette;
        Palette.setColor(QPalette::Button,color);
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


QTableCell tableitemset::getTableCell()
{
    m_TableCell.setTextColor(m_TextColor);
    m_TableCell.setTextFont(m_TextFont);
    m_TableCell.setBackgroundColor(m_ItemBGColor);
    m_TableCell.setTextHAlignment(m_TextHAlignment);
    m_TableCell.setTextVAlignment(m_TextVAlignment);
    m_TableCell.setLeftEdgeVisible((bool)ui->LeftEdgeStateCheckBox->checkState());
    m_TableCell.setTopEdgeVisible((bool)ui->TopEdgeStateCheckBox->checkState());
    m_TableCell.setShowType(ui->ShowTypeEdit->text());
    m_TableCell.setLeftMargin(ui->LeftMarginEdit->text().toInt());
    m_TableCell.setRightMargin(ui->RightMarginEdit->text().toInt());
    m_TableCell.setTopMargin(ui->TopMarginEdit->text().toInt());
    m_TableCell.setBottomMargin(ui->BottomMarginEdit->text().toInt());
    return m_TableCell;
}

void tableitemset::on_moreButton_clicked()
{
    bMoreOperate = !bMoreOperate;
    QRect rect = geometry();
    if(bMoreOperate)
    {
        ui->moreButton->setText(tr("<<"));
        setGeometry(rect.x(),rect.y(),uiRect.width(),uiRect.height());
    }
    else
    {
        ui->moreButton->setText(tr(">>"));
        setGeometry(rect.x(),rect.y(),uiRect.width()/2,uiRect.height());
    }
}
