#include "q2wmaplabeldlg.h"
#include "ui_q2wmaplabeldlg.h"

Q2wmapLabelDlg::Q2wmapLabelDlg(QJsonObject jobj) :  ui(new Ui::Q2wmapLabelDlg)
{
    ui->setupUi(this);

    m_strType = "Label";
}

Q2wmapLabelDlg::~Q2wmapLabelDlg()
{
    delete ui;
}
