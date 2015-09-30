#ifndef SINGLESTATELINEEDITDELEGATE_H
#define SINGLESTATELINEEDITDELEGATE_H

#include <QItemDelegate>

class SingleStateLineEditDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit SingleStateLineEditDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex	&index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const  QModelIndex &index) const;

};

#endif // SINGLESTATELINEEDITDELEGATE_H
