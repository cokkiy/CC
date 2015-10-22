﻿#ifndef TABLEPROPERTY_H
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
	//�������
    qint32 m_iRowNum;
	//�������
    qint32 m_iColNum;
	//������е�Ԫ����������
    QVector<QTableCell > m_TableVector;
public:
	//��ȡ��������
    qint32 getRowNum() const {return m_iRowNum;}
	//���ñ�������
    void setRowNum(const qint32 num);
	//��ȡ��������
    qint32 getColNum() const {return m_iColNum;}
	//���ñ�������
    void setColNum(const qint32 num);
	//��ȡ������������
    QVector<QTableCell >* getTableVector();
	//��ȡ���ĸ߶ȺͿ��
    QSize size();
	//���������ת��ΪJson�ַ�������Json�ַ����������������������е�Ԫ�������
    QString toJsonStr(float wr,float hr);
	//��������ַ���ת��Ϊ�������
    void setTableVector(const QString textStr);
	//׷����
    void addRow();
	//������,���λ�������е���һ��
    void insertRow(const int currentRow);
	//ɾ�����λ��������
    void delRow(const int currentCol);
	//׷����
    void addColumn();
	//������,���λ�������е����һ��
    void insertColumn(const int currentCol);
	//ɾ�����λ��������
    void delColumn(const int currentCol);
	//����ָ���еĿ��
    void setColumnWidth(int column,int width);
	//����ָ���еĸ߶�
    void setRowHeight(int row,int height);
	//���ñ��ĸ߶ȺͿ��
    void setSize(const int width,const int height);
private:
	//����������ַ��������ַ����������������������е�Ԫ�������
    bool analyzeTextStr(const QString textStr);
	//��Json�ַ���ת��Ϊ���Ԫ����
    bool initTableVector(const QString jsonStr);
};

#endif // TABLEPROPERTY_H
