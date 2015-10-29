#ifndef Q2WMAPLABELDLG_H
#define Q2WMAPLABELDLG_H

#include <QDialog>
#include "q2wmapelementdlg.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace Ui {
class Q2wmapLabelDlg;
}

class Q2wmapLabelDlg : public Q2wmapElementDlg
{
    Q_OBJECT

public:
    explicit Q2wmapLabelDlg(QJsonObject jobj);
    ~Q2wmapLabelDlg();

    //位置经度
    double m_dbPosL;

    //位置纬度
    double m_dbPosB;

    //标签字体
    QString m_strFont;

    //设置控件
    void setControls();

private slots:
    void on_lineEditLabelName_textChanged(const QString &arg1);

    void on_lineEditPosL_textChanged(const QString &arg1);

    void on_lineEditPosB_textChanged(const QString &arg1);

    void on_pushButton_SelectColor_clicked();

    void on_pushButton_Font_clicked();

private:
    Ui::Q2wmapLabelDlg *ui;
};

#endif // Q2WMAPLABELDLG_H
