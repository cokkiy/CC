#ifndef Q2WMAPOBJDLG_H
#define Q2WMAPOBJDLG_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace Ui {
class Q2wmapObjDlg;
}

class Q2wmapObjDlg : public QDialog
{
    Q_OBJECT

public:
    explicit Q2wmapObjDlg(QJsonObject jobj, QWidget *parent = 0);
    ~Q2wmapObjDlg();

    //目标名称
    QString m_strName;

    //是否主目标
    bool m_bMainObj;

    //目标曲线颜色
    QString m_strCColor;

    //理论曲线颜色
    QString m_strLColor;

    //曲线粗细
    qint32 m_iCWidth;

    //理论曲线粗细
    qint32 m_iLWidth;

    //X轴参数
    QString m_strXParam;

    //Y轴参数
    QString m_strYParam;

    //理论曲线文件
    QString m_strLFile;

    //设置控件
    void setControls();

private slots:

    void on_pushButton_SelectCColor_clicked();

    void on_pushButton_SelectLColor_clicked();

    void on_pushButton_SelectXParam_clicked();

    void on_pushButton_SelectYParam_clicked();

    void on_checkBox_MainObj_toggled(bool checked);

    void on_textEdit_targetName_textChanged();

    void on_textEdit_LWidth_textChanged();

    void on_textEdit_CWidth_textChanged();

    void on_textEdit_LFile_textChanged();

    void on_pushButton_SelectLFile_clicked();

private:
    Ui::Q2wmapObjDlg *ui;
};

#endif // Q2WMAPOBJDLG_H
