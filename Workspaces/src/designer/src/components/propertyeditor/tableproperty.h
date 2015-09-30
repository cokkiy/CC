#ifndef TABLEPROPERTY_H
#define TABLEPROPERTY_H
#include "tablecell.h"
#include <Net/NetComponents>

class QTableProperty
{
public:
    QTableProperty();
private:
	qint16 m_iRowNum;
	qint16 m_iColNum;
    QVector<QTableCell > m_TableVector;
public:
	qint16 GetRowNum() const {return m_iRowNum;}
    void SetRowNum(const qint16 num);
	qint16 GetColNum() const {return m_iColNum;}
    void SetColNum(const qint16 num);
    QVector<QTableCell > GetTableVector() const {return m_TableVector;}
    void SetTableVector(const QVector<QTableCell > Vector);
};

#endif // TABLEPROPERTY_H
