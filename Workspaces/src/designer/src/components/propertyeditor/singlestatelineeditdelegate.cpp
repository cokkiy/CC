#include "singlestatelineeditdelegate.h"
#include<QApplication>
#include <QLineEdit>
#include <QFileDialog>

SingleStateLineEditDelegate::SingleStateLineEditDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

//创建控件，创建由参数中QModelIndex对象指定的表项数据的编辑控件，并对控件的内容进行限定。
QWidget *SingleStateLineEditDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &/*option*/,const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);                  //新建一个QLineEdit对象作为编辑时的输入控件
    editor->installEventFilter(const_cast<SingleStateLineEditDelegate*>(this)); //调用QObeject类的installEventFilter()函数安装事件过滤器，
                                                                                                     // 使LineEditDelegate能够捕获QLineEdit对象的事件
    QString strpath=index.model()->data(index).toString();     //获取指定index数据项的数据。调用QMdelIndex的model()函数可获得提供index的
                                                                                            //Model对象，data()函数返回的是QVariant对象，toString将它转换为一个String数据

    strpath=QFileDialog::getOpenFileName(parent, tr("打开"),"/",tr("Images (*.png *.bmp *.jpg .*gif .*psd)"));       //打开文件并将选择的文件路径返回给strpath
    QLineEdit *lineedit = static_cast<QLineEdit*>(editor);        //将lineedit转换为QLineEdit对象，以获得编辑控件的对象指针
    lineedit->setText(strpath);                                            //在LineEdit显示打开的文件路径
    return editor;
}

//设置控件显示的数据，将Model中的数据更新至Delegate中，相当于初始化工作
void SingleStateLineEditDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{

}

//将Delegate中对数据的改变更新至Model中
void SingleStateLineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineeditpath = static_cast<QLineEdit*>(editor); //通过紧缩转换获得编辑控件的对象指针
    QString strpath=lineeditpath->text();                                    //获得编辑控件中的数据更新
    model->setData(index,strpath);                                         //调用setData函数将数据修改更新到Model中
}
//更新控件区的显示
void SingleStateLineEditDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
