#include "q2wmappolygondialog.h"
#include "ui_q2wmappolygondialog.h"

Q2wmapPolygonDialog::Q2wmapPolygonDialog(QJsonObject jobj) : ui(new Ui::Q2wmapPolygonDialog)
{
    ui->setupUi(this);

    m_strType = "Polygon";
}

Q2wmapPolygonDialog::~Q2wmapPolygonDialog()
{
    delete ui;
}
