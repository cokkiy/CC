#include "tableset.h"
#include "ui_tableset.h"

QTableSet::QTableSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTableSet)
{
    ui->setupUi(this);
    ui->m_RowNumEdit->setText(tr("10"));
    ui->m_ColNumEdit->setText(tr("5"));
}

QTableSet::~QTableSet()
{
    delete ui;
}
void QTableSet::GetNum(int& r,int& c)
{
    r = ui->m_RowNumEdit->text().toInt();
    c = ui->m_ColNumEdit->text().toInt();
}
