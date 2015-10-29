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

private:
    Ui::Q2wmapPolygonDialog *ui;
};

#endif // Q2WMAPPOLYGONDIALOG_H
