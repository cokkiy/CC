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

    //元素名称
    QString m_strName;

private:
    Ui::Q2wmapLabelDlg *ui;
};

#endif // Q2WMAPLABELDLG_H
