#ifndef CONFIGSHOWTYPEDIALOG_H
#define CONFIGSHOWTYPEDIALOG_H
#include <Net/NetComponents>
#include <QDialog>
#include <QVector>

struct stru_ParamStyleIndex
{
    //索引名称
    QString m_DomainStr;
    //索引名称
    QVector<QString> m_IndexNameVector;
    stru_ParamStyleIndex()
    {
        m_DomainStr = QObject::tr("");
        m_IndexNameVector.clear();
    }
};
namespace Ui {
class QConfigShowTypeDialog;
}

class QConfigShowTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QConfigShowTypeDialog(QWidget *parent = 0, QString showTypeStr = tr(""));
    ~QConfigShowTypeDialog();
private slots:
    void on_StyletreeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    void initTreeWidget();
    void getParamStyleData();
private:
    QVector <stru_ParamStyleIndex> m_styleVector;
    Ui::QConfigShowTypeDialog *ui;
    //信息约定表参数接口
    InformationInterface* paramStyleInfor;
    QString m_ShowTypeStr;
public:
    QString getShowTypeStr();
};

#endif // CONFIGSHOWTYPEDIALOG_H
