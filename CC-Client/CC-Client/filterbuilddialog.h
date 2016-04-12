#ifndef FILTERBUILDDIALOG_H
#define FILTERBUILDDIALOG_H

#include <QDialog>
#include "ui_filterBuildDialog.h"
#include "FilterOperations.h"
#include <QRegExp>

class FilterBuildDialog : public QDialog
{
    Q_OBJECT

public:
    FilterBuildDialog(QWidget *parent = 0);
    ~FilterBuildDialog();
    public slots:
    //选择了属性
    void on_propertyComboBox_currentIndexChanged(QString index);
    //选择操作符
    void on_opComboBox_currentIndexChanged(QString index);
    //添加新的过滤项
    void on_addToolButton_clicked();
    //清空按钮按下
    void on_clearButton_clicked();

    /*!
    获取过滤项
    @return FilterOperations 构建好的过滤项
    作者：cokkiy（张立民)
    创建时间：2015/11/05 11:08:31
    */
    FilterOperations getFilter();

private:
    Ui::FilterBuildDialog ui;
    //过滤项
    FilterOperations filterOperations;
    //Ip 正则表达式
    QRegExp ip_regx;
    //IP 验证
    QRegExpValidator ipValidator;

    //字符串转成属性
    CompareOperation::Property parseProperty(QString property);
    //字符串转成操作符
    CompareOperation::Operator parseOperator(QString operation);
};

#endif // FILTERBUILDDIALOG_H
