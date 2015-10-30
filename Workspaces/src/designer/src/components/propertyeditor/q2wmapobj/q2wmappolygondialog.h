#ifndef Q2WMAPPOLYGONDIALOG_H
#define Q2WMAPPOLYGONDIALOG_H

#include <QDialog>
#include "q2wmapelementdlg.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace Ui {
class Q2wmapPolygonDialog;
}

class Q2wmapPolygonDialog : public Q2wmapElementDlg
{
    Q_OBJECT

public:
    explicit Q2wmapPolygonDialog(QJsonObject jobj);
    ~Q2wmapPolygonDialog();    

    //设置控件
    void setControls();

    //组成定制图形的点
    QString m_strPoly;

private slots:
    void on_lineEditName_textChanged(const QString &arg1);

    void on_lineEditWidth_textChanged(const QString &arg1);

    void on_pushButton_SelectColor_clicked();

    void on_textEdit_textChanged();

private:
    Ui::Q2wmapPolygonDialog *ui;
};

#endif // Q2WMAPPOLYGONDIALOG_H
