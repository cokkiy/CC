/****************************************************************************
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

#include "imageexplorer.h"
#include "imageexplorermodel_p.h"
#include "formwindow.h"
#include "xbeltree.h"
#include "../../designer/newform.h"
#include "../../designer/qdesigner_workbench.h"
// sdk
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerTaskMenuExtension>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>

// shared
#include <qdesigner_utils_p.h>
#include <formwindowbase_p.h>
#include <qdesigner_dnditem_p.h>
#include <textpropertyeditor_p.h>
#include <qdesigner_command_p.h>
#include <grid_p.h>

// Qt
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QItemSelectionModel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QItemDelegate>
#include <QtGui/qevent.h>

#include <QtCore/QVector>
#include <QtCore/QDebug>
#include <ConstVar>
QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
// ------------ ImageExplorerPrivate

class ImageExplorer::ImageExplorerPrivate {
public:
    explicit ImageExplorerPrivate(QDesignerFormEditorInterface *core);
    ~ImageExplorerPrivate();
    QLineEdit *findWidget() const { return m_findWidget; }
    QTreeView *pageTreeView() const { return m_pageTreeView; }
    QTreeView *markTreeView() const { return m_markTreeView; }
    QString findPageKey() const { return m_findPageKey; }
    QString setFindPageKey(QString key) { return m_findPageKey = key; }
    QString findMarkKey() const { return m_findMarkKey; }
    QString setFindMarkKey(QString key) { return m_findMarkKey = key; }
    QDesignerFormEditorInterface *core() const { return m_core; }

    void setFileName(const QString &file_name);
    QString fileName() const;
    bool load();
    bool save();
private:
    QDesignerFormEditorInterface *m_core;
    QTreeView * m_pageTreeView;
    QTreeView * m_markTreeView;
    QLineEdit *m_findWidget;
    QString m_findPageKey;
    QString m_findMarkKey;
};

ImageExplorer::ImageExplorerPrivate::ImageExplorerPrivate(QDesignerFormEditorInterface *core) :
    m_core(core),
    m_pageTreeView(new XbelTree(core)),
    m_markTreeView(new XbelTree(core))
{
    m_findWidget = new QLineEdit();
    m_findWidget->setPlaceholderText(tr("Key Word"));
    m_findWidget->setClearButtonEnabled(true);

    ((XbelTree*)m_pageTreeView)->setColumnCount(2);
    ((XbelTree*)m_pageTreeView)->setDragEnabled(true);
    ((XbelTree*)m_pageTreeView)->viewport()->setAcceptDrops(true);
    ((XbelTree*)m_pageTreeView)->setDropIndicatorShown(true);
    ((XbelTree*)m_pageTreeView)->setDragDropMode(QAbstractItemView::InternalMove);
    ((XbelTree*)m_pageTreeView)->setAlternatingRowColors(true);

    ((XbelTree*)m_markTreeView)->setColumnCount(1);
    ((XbelTree*)m_markTreeView)->setDragEnabled(true);
    ((XbelTree*)m_markTreeView)->viewport()->setAcceptDrops(true);
    ((XbelTree*)m_markTreeView)->setDropIndicatorShown(true);
    ((XbelTree*)m_markTreeView)->setDragDropMode(QAbstractItemView::InternalMove);
    ((XbelTree*)m_markTreeView)->setAlternatingRowColors(true);
}

ImageExplorer::ImageExplorerPrivate::~ImageExplorerPrivate()
{
    delete m_pageTreeView;
    delete m_markTreeView;
    delete m_findWidget;
}

void ImageExplorer::ImageExplorerPrivate::setFileName(const QString &file_name)
{
    ((XbelTree *)m_pageTreeView)->setFileName(file_name);
    ((XbelTree *)m_markTreeView)->setFileName(file_name);
}

QString ImageExplorer::ImageExplorerPrivate::fileName() const
{
    return ((XbelTree *)m_pageTreeView)->fileName();
}

bool ImageExplorer::ImageExplorerPrivate::load()
{
    ((XbelTree *)m_markTreeView)->load();
    return ((XbelTree *)m_pageTreeView)->load();
}

bool ImageExplorer::ImageExplorerPrivate::save()
{
    return ((XbelTree *)m_pageTreeView)->save();
}


// ------------ ImageExplorer
ImageExplorer::ImageExplorer(QDesignerFormEditorInterface *core, QWidget *parent) :
    QDesignerImageExplorer(parent),
    m_impl(new ImageExplorerPrivate(core))
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->setSpacing(0);
    QToolBar *toolBar = new QToolBar(this);    
    connect(m_impl->findWidget(), SIGNAL(textChanged(QString)), m_impl->pageTreeView(), SLOT(filter(QString)));
    toolBar->addWidget(m_impl->findWidget());
    vbox->addWidget(toolBar);
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(m_impl->pageTreeView(),tr("PageTree"));
    tabWidget->addTab(m_impl->markTreeView(),tr("BookMark"));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeFilter(int)));
    vbox->addWidget(tabWidget);
}

ImageExplorer::~ImageExplorer()
{
    delete m_impl;
}

QDesignerFormEditorInterface *ImageExplorer::core() const
{
    return m_impl->core();
}

void ImageExplorer::setFileName(const QString &file_name)
{
    return m_impl->setFileName(file_name);
}

QString ImageExplorer::fileName() const
{
    return m_impl->fileName();
}
bool ImageExplorer::load()
{
    return m_impl->load();
}

bool ImageExplorer::save()
{
    return m_impl->save();
}
void ImageExplorer::changeFilter(int pos)
{
    switch(pos)
    {
    case 0:
        connect(m_impl->findWidget(), SIGNAL(textChanged(QString)), m_impl->pageTreeView(), SLOT(filter(QString)));
        disconnect(m_impl->findWidget(), SIGNAL(textChanged(QString)), m_impl->markTreeView(), SLOT(filter(QString)));
        m_impl->setFindMarkKey(m_impl->findWidget()->text());
        m_impl->findWidget()->setText(m_impl->findPageKey());
        break;
    case 1:
        connect(m_impl->findWidget(), SIGNAL(textChanged(QString)), m_impl->markTreeView(), SLOT(filter(QString)));
        disconnect(m_impl->findWidget(), SIGNAL(textChanged(QString)), m_impl->pageTreeView(), SLOT(filter(QString)));
        m_impl->setFindPageKey(m_impl->findWidget()->text());
        m_impl->findWidget()->setText(m_impl->findMarkKey());
        break;
    }
}

}
QT_END_NAMESPACE
