#include "filterbuilddialog.h"
#include <QMessageBox>

FilterBuildDialog::FilterBuildDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ip_regx.setPattern("^((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))&");
    ipValidator.setRegExp(ip_regx);
    ui.v2lineEdit->setValidator(&ipValidator);
    ui.v1lineEdit->setValidator(&ipValidator);
    ui.stateComboBox->setVisible(false);
}

FilterBuildDialog::~FilterBuildDialog()
{

}

//选择了属性
void FilterBuildDialog::on_propertyComboBox_currentIndexChanged(QString index)
{
    ui.v1lineEdit->setValidator(0);
    ui.v1lineEdit->setVisible(true);
    ui.stateComboBox->setVisible(false);
    if (index.compare("IP")==0)
    {
        ui.opComboBox->clear();
        QStringList items;
        items << "=" << ">=" << "<=" << tr("Between");
        ui.opComboBox->insertItems(0, items);
        
        ui.v1lineEdit->setValidator(&ipValidator);
    }
    else if (index.compare(tr("State")) == 0)
    {
        ui.opComboBox->clear();
        QStringList items;
        items << "=" << "!=";
        ui.opComboBox->insertItems(0, items);
        ui.v1lineEdit->setVisible(false);
        ui.stateComboBox->setVisible(true);
    }
    else if (index.compare(tr("Name")) == 0)
    {
        ui.opComboBox->clear();
        QStringList items;
        items << "=" << tr("Startwiths") << tr("Endwiths") << tr("Contains");
        ui.opComboBox->insertItems(0, items);
    }
}

//清空按钮按下
void FilterBuildDialog::on_clearButton_clicked()
{
    this->filterOperations.clear();
    ui.opComboBox->setEnabled(false);
}

//选择操作符
void FilterBuildDialog::on_opComboBox_currentIndexChanged(QString index)
{
    if (index.compare(tr("Between")) == 0)
    {
        ui.v2lineEdit->setEnabled(true);
    }
    else
    {
        ui.v2lineEdit->setEnabled(false);
    }
}

//添加一条过滤项
void FilterBuildDialog::on_addToolButton_clicked()
{
    if (ui.v1lineEdit->isVisible() && ui.v1lineEdit->text().isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("必须提供比较的值1"));
        return;
    }
    if (ui.v2lineEdit->isEnabled() && ui.v2lineEdit->text().isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("选择\"介于\"必须提供比较的值1和值1."));
        return;
    }
    auto property = parseProperty(ui.propertyComboBox->currentText());   
    auto op = parseOperator(ui.opComboBox->currentText());
    CompareOperation compare(property, op);
    compare.setValue1(ui.v1lineEdit->isVisible() ? ui.v1lineEdit->text() : ui.stateComboBox->currentText());
    compare.setValue2(ui.v2lineEdit->text());
    if (filterOperations.empty())
    {
        filterOperations.push(compare);
        ui.combineComboBox->setEnabled(true);
    }
    else
    {
        filterOperations.push((FilterOperations::RelationOperator)ui.combineComboBox->currentIndex(), compare);
    }
    ui.v1lineEdit->clear();
    ui.v2lineEdit->clear();
    ui.filterTextEdit->setText(filterOperations.toString());
}

/*!
获取过滤项
@return FilterOperations 构建好的过滤项
作者：cokkiy（张立民)
创建时间：2015/11/05 11:08:31
*/
FilterOperations FilterBuildDialog::getFilter()
{
    return filterOperations;
}

//字符串转成属性
CompareOperation::Property FilterBuildDialog::parseProperty(QString property)
{
    if (property.compare("IP") == 0)
    {
        return CompareOperation::Property::IP;
    }
    else if (property.compare(tr("State")) == 0)
    {
        return CompareOperation::Property::State;
    }
    else if (property.compare(tr("Name")) == 0)
    {
        return CompareOperation::Property::Name;
    }
    return CompareOperation::Property::Name;
}

//字符串转成操作符
CompareOperation::Operator FilterBuildDialog::parseOperator(QString operation)
{
    CompareOperation::Operator op = CompareOperation::Operator::Equals;
    if (operation.compare("=") == 0)
    {
        op = CompareOperation::Operator::Equals;
    }
    else if (operation.compare(">=") == 0)
    {
        op = CompareOperation::Operator::GreaterThan;
    }
    else if (operation.compare("<=") == 0)
    {
        op = CompareOperation::Operator::LessThan;
    }
    else if (operation.compare("!=") == 0)
    {
        op = CompareOperation::Operator::NotEquals;
    }
    else if (operation.compare(tr("Startwiths")) == 0)
    {
        op = CompareOperation::Operator::Startwiths;
    }
    else if (operation.compare(tr("Endwiths")) == 0)
    {
        op = CompareOperation::Operator::Endwiths;
    }
    else if (operation.compare(tr("Same")) == 0)
    {
        op = CompareOperation::Operator::Same;
    }
    else if (operation.compare(tr("Between")) == 0)
    {
        op = CompareOperation::Operator::Between;
    }
    else if (operation.compare(tr("Contains")) == 0)
    {
        op = CompareOperation::Operator::Contains;
    }

    return op;
}
