#include "singlestatespindelegate.h"
//#include <QSpinBox>
#include <QDoubleSpinBox>
SingleStateSpinDelegate::SingleStateSpinDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

//创建控件，创建由参数中QModelIndex对象指定的表项数据的编辑控件，并对控件的内容进行限定
QWidget *SingleStateSpinDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);                         //新建一个QSpinBox对象作为编辑时的输入控件
   editor->setMinimum(-1000000000000);
    editor->setMaximum(1000000000000);
    editor->installEventFilter(const_cast<SingleStateSpinDelegate*>(this)); //调用QObeject类的installEventFilter()函数安装事件过滤器，
                                                                                                // 使SpinDelegate能够捕获QSpinBox对象的事件
    return editor;
}

//设置控件显示的数据，将Model中的数据更新至Delegate中，相当于初始化工作
void SingleStateSpinDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    double value =index.model()->data(index).toDouble();       //获取指定index数据项的数据。调用QMdelIndex的model()函数可获得提供index的
                                                                               //Model对象，data()函数返回的是QVariant对象，toInt将它转换为一个int数据
    QDoubleSpinBox *box = static_cast<QDoubleSpinBox*>(editor);//将box转换为QSpinBox对象，以获得编辑控件的对象指针
    box->setValue(value);                                           //设置控件的显示数据
}

//将Delegate中对数据的改变更新至Model中
void SingleStateSpinDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    QDoubleSpinBox *box = static_cast<QDoubleSpinBox*>(editor);
    double value = box->value();         //获得编辑控件中的数据更新
    model->setData(index,value); //调用setData()将数据修改更新到Model中
}

//更新控件去的显示
void SingleStateSpinDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

