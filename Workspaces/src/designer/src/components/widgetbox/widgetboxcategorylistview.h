﻿/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef WIDGETBOXCATEGORYLISTVIEW_H
#define WIDGETBOXCATEGORYLISTVIEW_H

#include <QtDesigner/QDesignerWidgetBoxInterface>

#include <QtWidgets/QListView>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerDnDItemInterface;

class QSortFilterProxyModel;
class QRegExp;

namespace qdesigner_internal {

class WidgetBoxCategoryModel;

// List view of a category, switchable between icon and list mode.
// Provides a filtered view.
class WidgetBoxCategoryListView : public QListView
{
    Q_OBJECT
public:
    // Whether to access the filtered or unfiltered view
    enum AccessMode { FilteredAccess, UnfilteredAccess };

    explicit WidgetBoxCategoryListView(QDesignerFormEditorInterface *core, QWidget *parent = 0);
    void setViewMode(ViewMode vm);
    // No Use, Andrew
    void dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list);

    using QListView::contentsSize;

    // These methods operate on the filtered/unfiltered model according to accessmode
    int count(AccessMode am) const;
    QDesignerWidgetBoxInterface::Widget widgetAt(AccessMode am, const QModelIndex &index) const;
    QDesignerWidgetBoxInterface::Widget widgetAt(AccessMode am, int row) const;
    void removeRow(AccessMode am, int row);
    void setCurrentItem(AccessMode am, int row);

    // These methods operate on the unfiltered model and are used for serialization
    void addWidget(const QDesignerWidgetBoxInterface::Widget &widget, const QIcon &icon, bool editable);
    bool containsWidget(const QString &name);
    QDesignerWidgetBoxInterface::Category category() const;
    bool removeCustomWidgets();

    // Helper: Ensure a <ui> tag in the case of empty XML
    static QString widgetDomXml(const QDesignerWidgetBoxInterface::Widget &widget);

signals:
    void scratchPadChanged();
    void pressed(const QString &name, const QString &xml, const QPoint &globalPos);
    void itemRemoved();
    void lastItemRemoved();

public slots:
    void filter(const QRegExp &re);

private slots:
    void slotPressed(const QModelIndex &index);
    void removeCurrentItem();
    void editCurrentItem();

private:
    int mapRowToSource(int filterRow) const;
    // 查找模型
    QSortFilterProxyModel *m_proxyModel;
    // 全部分类模型
    WidgetBoxCategoryModel *m_model;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // WIDGETBOXCATEGORYLISTVIEW_H
