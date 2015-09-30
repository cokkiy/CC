#include "tableproperty.h"

QTableProperty::QTableProperty()
{
}
void QTableProperty::SetRowNum(const qint16 num)
{
	m_iRowNum = num;
}
void QTableProperty::SetColNum(const qint16 num)
{
	m_iColNum = num;
}

void QTableProperty::SetTableVector(const QVector<QTableCell > Vector)
{
    m_TableVector = Vector;
}
