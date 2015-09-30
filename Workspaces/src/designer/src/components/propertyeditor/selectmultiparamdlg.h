#ifndef SELECTMULTIPARAMDLG_H
#define SELECTMULTIPARAMDLG_H
#include "propertyeditor_global.h"
#include <Net/NetComponents>
#include <QDialog>

namespace Ui {
class QSelectMultiParamDlg;
}

class QSelectMultiParamDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QSelectMultiParamDlg(QWidget *parent = 0);
    ~QSelectMultiParamDlg();
    QVector<QString> GetParamList();
private slots:
    void on_m_AddButton_clicked();

    void on_m_DelButton_clicked();

    void on_m_DFZXRadio_clicked();

    void on_m_C3IRadio_clicked();

    void on_m_KELRadio_clicked();

    void on_m_CustomRadio_clicked();

    void on_m_SetCustom_clicked();

    void on_m_FormTableWidget_cellClicked(int row, int );

    void on_m_ParamTableWidget_cellClicked(int , int );

    void on_m_TopButton_clicked();

    void on_m_BottomButton_clicked();

    void on_m_UpButton_clicked();

    void on_m_DownButton_clicked();

    void on_m_SelectedtableWidget_cellClicked(int , int );

private:
    InformationInterface* net;
    void InitSelectedWidget();
    void SetCustomControl();
    bool InitParamFormMap();
    void InitParamFormWidget();
    void InitParamWidget();
    void SetParamFormWidgetItem();
    void SetSelectedWidgetItem();
    QMap<unsigned short, QString > tablemap;
private:
    Ui::QSelectMultiParamDlg *ui;
private:
    QVector<QString> SelectingVector;
    QVector<QString> SelectedVector;
    ushort m_iMaxTableNo;
    ushort m_iMinTableNo;
    int m_iCurrentItemNo;
    int m_iSystemType;
};

#endif // SELECTMULTIPARAMDLG_H
