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

    //元素名称
    QString m_strName;

private:
    Ui::Q2wmapCircleDlg *ui;
};

#endif // Q2WMAPCIRCLEDLG_H
