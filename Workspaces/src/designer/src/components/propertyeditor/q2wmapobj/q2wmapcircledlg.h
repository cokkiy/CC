#ifndef Q2WMAPCIRCLEDLG_H
#define Q2WMAPCIRCLEDLG_H

#include <QDialog>
#include "q2wmapelementdlg.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace Ui {
class Q2wmapCircleDlg;
}

class Q2wmapCircleDlg : public Q2wmapElementDlg
{
    Q_OBJECT

public:
    explicit Q2wmapCircleDlg(QJsonObject jobj);
    ~Q2wmapCircleDlg();

    //圆心经度
    double m_dbPosL;

    //圆心纬度
    double m_dbPosB;

    //圆心半径
    double m_dbRadius;

    //设置控件
    void setControls();

private slots:
    void on_lineEditAreaName_textChanged(const QString &arg1);

    void on_lineEditPosL_textChanged(const QString &arg1);

    void on_lineEditPosB_textChanged(const QString &arg1);

    void on_lineEditRadius_textChanged(const QString &arg1);

    void on_lineEditWidth_textChanged(const QString &arg1);

    void on_pushButton_SelectColor_clicked();

private:
    Ui::Q2wmapCircleDlg *ui;
};

#endif // Q2WMAPCIRCLEDLG_H
