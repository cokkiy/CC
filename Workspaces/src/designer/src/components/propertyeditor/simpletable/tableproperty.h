#ifndef TABLEPROPERTY_H
#define TABLEPROPERTY_H
#include "tablecell.h"
#include <Net/NetComponents>

typedef enum TableOperateType
{
    tp_addRow,
    tp_insertRow,
    tp_delRow,
    tp_addColumn,
    tp_insertColumn,
    tp_delColumn,
}TableOperateType;

class QTableProperty
{
public:
    QTableProperty();
private:
    //表格行数
    qint32 m_iRowNum;
    //表格列数
    qint32 m_iColNum;
    //表格所有单元格属性数组
    QVector<QTableCell > m_TableVector;
public:
    //获取表格的行数
    qint32 getRowNum() const {return m_iRowNum;}
    //设置表格的行数
    void setRowNum(const qint32 num);
    //获取表格的列数
    qint32 getColNum() const {return m_iColNum;}
    //设置表格的列数
    void setColNum(const qint32 num);
    //获取表格的属性数组
    QVector<QTableCell >* getTableVector();
    //获取表格的高度和宽度
    QSize size();
    //将表格属性转换为Json字符串，该Json字符串包括表格的行列数和所有单元格的属性
    QString toJsonStr(float wr,float hr);
    //将传入的字符串转换为表格属性
    void setTableVector(const QString textStr);
    //追加行
    void addRow();
    //插入行,鼠标位置所在行的上一行
    void insertRow(const int currentRow);
    //删除鼠标位置所在行
    void delRow(const int currentCol);
    //追加列
    void addColumn();
    //插入列,鼠标位置所在列的左侧一列
    void insertColumn(const int currentCol);
    //删除鼠标位置所在列
    void delColumn(const int currentCol);
    //设置指定列的宽度
    void setColumnWidth(int column,int width);
    //设置指定行的高度
    void setRowHeight(int row,int height);
    //设置表格的高度和宽度
    void setSize(const int width,const int height);
private:
    //解析传入的字符串，该字符串包括表格的行列数和所有单元格的属性
    bool analyzeTextStr(const QString textStr);
    //将Json字符串转换为表格单元属性
    bool initTableVector(const QString jsonStr);
};

#endif // TABLEPROPERTY_H
