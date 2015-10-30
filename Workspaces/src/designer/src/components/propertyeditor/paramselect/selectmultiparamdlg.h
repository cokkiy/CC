#ifndef SELECTMULTIPARAMDLG_H
#define SELECTMULTIPARAMDLG_H
#include <Net/NetComponents>
#include <QDialog>
#include <QtCore/qglobal.h>
namespace Ui {
class QSelectMultiParamDlg_UI;
}

#if defined(WIN32)
#if defined(QDESIGNER_COMPONENTS_LIBRARY)
#define Q_SelectMultiParamDlg_EXPORT Q_DECL_EXPORT
#else
#define  Q_SelectMultiParamDlg_EXPORT
#endif
#else
#define Q_SelectMultiParamDlg_EXPORT Q_DECL_EXPORT
#endif

class Q_SelectMultiParamDlg_EXPORT QSelectMultiParamDlg : public QDialog
{
    Q_OBJECT
public:
    explicit QSelectMultiParamDlg(QWidget *parent = 0);
    ~QSelectMultiParamDlg();
    void getParamList(QVector<QString>& ,QVector<QString>& );
    QString getMultiParamFormulaStr();
    void setMultiParamFormulaStr(const QString );
    void setParamList(QVector<QString> );
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

    void on_m_ReplaceButton_clicked();

    void on_selectAllParamButton_clicked();

private:
    InformationInterface* net;
    void getLineEditText();
    void initSelectedWidget();
    void setRadioState();
    bool initParamFormMap();
    void initParamFormWidget();
    void initParamWidget();
    void setParamFormWidgetItem();
    void setSelectedWidgetItem();
    void setButtonState(bool );
    QMap<unsigned short, QString > tablemap;
private:
    Ui::QSelectMultiParamDlg_UI *ui;
private:
    QVector<QString> m_SelectingVector;
    QVector<QString> m_SelectedVector;
    QVector<QString> m_FormulaVector;
    ushort m_iMaxTableNo;
    ushort m_iMinTableNo;
    int m_iCurrentItemNo;
    int m_iSystemType;
};

#endif // SELECTMULTIPARAMDLG_H
