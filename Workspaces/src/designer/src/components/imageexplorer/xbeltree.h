/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef XBELTREE_H
#define XBELTREE_H

#include <QDomDocument>
#include <QHash>
#include <QIcon>
#include <QTreeWidget>
#include <ConstVar>
class QDesignerFormEditorInterface;
class QDesignerWorkbench;
class XbelTreePrivate;
class XbelTree : public QTreeWidget
{
    Q_OBJECT
public:
    enum ItemType { Type = 0, UserType = 1000,Folder,Page ,Separator};
public:
    XbelTree(QWidget *parent = 0);
    //construct function
    //
    explicit XbelTree(QDesignerFormEditorInterface *core, QString fileName = g_bookmarkRelativeFilePath, QWidget *parent = 0);
    virtual ~XbelTree();
    void createActions();
public:
    void setFileName(const QString &file_name);
    QString fileName() const;
    bool load();
    bool save();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    //drag and drop event, andie, 20140902
    void dropEvent(QDropEvent *event);
private:
    QAction *createFolderAction;
    QAction *removeFolderAction;
    QAction *createPageAction;
    QAction *removePageAction;
    QAction *moveUpAction;
    QAction *moveDownAction;
    QAction *moveTopAction;
    QAction *moveBottomAction;
    QMenu *popMenu;

private slots:
    void updateDomElement(QTreeWidgetItem *item, int column);
    void createFolder();
    void removeFolder();
    void createPage();
    void callbackCreatePage(QString,QString);
    void removePage();
    void moveUp();
    void moveDown();
    void moveTop();
    void moveBottom();
    void openPage(QModelIndex index);
    void openNextPage();
    void openPrevPage();
    // 使用关键字过滤画面目录树,andrew,20150923
    void filter(const QString &key);

private:
    Q_DECLARE_PRIVATE(XbelTree)

    void parseFolderElement(const QDomElement &element,QTreeWidgetItem *parentItem = 0);
    bool filterFolderElement(const QRegExp &key,QTreeWidgetItem *parentItem = 0);
    bool itemContainsKey(const QRegExp &key,QTreeWidgetItem *item);
    QTreeWidgetItem *createItem(const QDomElement &element,
                                QTreeWidgetItem *parentItem = 0,int type=Type);
    // remove an tree item
    void removeItem(QTreeWidgetItem * Item);
    // get page file name from an tree item
    QString pageFileName(QTreeWidgetItem * Item);
    // get a distant page name
    QString pageName();

    bool read(QIODevice *device);

    bool write(QIODevice *device);

    bool read();

    bool write();

    QDomDocument domDocument;
    QHash<QTreeWidgetItem *, QDomElement> domElementForItem;
    QIcon folderIcon;
    QIcon bookmarkIcon;

    QDesignerFormEditorInterface *m_core;

    QString m_fileName;
    //
    //QTreeWidgetItem * currentItem;
};

#endif
