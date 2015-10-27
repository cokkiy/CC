#include "q2wmapcircledlg.h"
#include "ui_q2wmapcircledlg.h"

Q2wmapCircleDlg::Q2wmapCircleDlg(QJsonObject jobj) : ui(new Ui::Q2wmapCircleDlg)
{
    ui->setupUi(this);

    m_strType = "Circle";
}

Q2wmapCircleDlg::~Q2wmapCircleDlg()
{
    delete ui;
}
