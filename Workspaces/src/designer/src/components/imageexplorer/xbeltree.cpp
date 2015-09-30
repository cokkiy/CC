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

#include <QtWidgets>

#include "xbeltree.h"
#include<QMimeData>
#include<QDebug>
// sdk
#include <QtDesigner/QDesignerFormEditorInterface>
//#include <QtDesigner/QDesignerTaskMenuExtension>
//#include <QtDesigner/QExtensionManager>
//#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
//#include <QtDesigner/QDesignerContainerExtension>
//#include <QtDesigner/QDesignerMetaDataBaseInterface>
//#include <QtDesigner/QDesignerPropertyEditorInterface>
#include "../../designer/qdesigner_actions.h"
#include "../../designer/qdesigner_workbench.h"
#include "textpropertyeditor_p.h"
#include "simpleinputdialog.h"
#include <ConstVar>
#include <Net/NetComponents>
const QString conRootTag = "xbel";
const QString conVersionTag = "version";
const QString conVersionValue = "1.0";
const QString conFolderTag = "folder";
const QString conFolderFoldTag = "folded";
const QString conTitleTag = "title";
const QString conPageTag = "bookmark";
const QString conPageUrlTag = "href";
const QString conSeparatorTag = "separator";

// --------------- ObjectNameDialog
class ObjectNameDialog : public QDialog
{
     public:
         ObjectNameDialog(QWidget *parent, const QString &oldName);
         QString newObjectName() const;

     private:
         qdesigner_internal::TextPropertyEditor *m_editor;
};

ObjectNameDialog::ObjectNameDialog(QWidget *parent, const QString &oldName)
    : QDialog(parent),
      m_editor( new qdesigner_internal::TextPropertyEditor(this, qdesigner_internal::TextPropertyEditor::EmbeddingNone,
                                                           qdesigner_internal::ValidationObjectName))
{
    setWindowTitle(QCoreApplication::translate("ObjectNameDialog", "Change Object Name"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->addWidget(new QLabel(QCoreApplication::translate("ObjectNameDialog", "Object Name")));

    m_editor->setText(oldName);
    m_editor->selectAll();
    m_editor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    vboxLayout->addWidget(m_editor);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    vboxLayout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ObjectNameDialog::newObjectName() const
{
    return m_editor->text();
}


XbelTree::XbelTree(QWidget *parent)
    : QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Title") << tr("Location");

    header()->setSectionResizeMode(QHeaderView::Stretch);
    setHeaderLabels(labels);

    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    createActions();
    popMenu = new QMenu(this);
}

XbelTree::XbelTree(QDesignerFormEditorInterface *core, QString fileName, QWidget *parent) :
    m_core(core),m_fileName(fileName),QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Title") << tr("Location");

    header()->setSectionResizeMode(QHeaderView::Stretch);
    setHeaderLabels(labels);

    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    createActions();
    popMenu = new QMenu(this);
    // Build connect to open Command and Display's Page,andrew
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(openPage(QModelIndex)));
    // connect the signal and slot of create page, andrew,20150304
    connect(m_core, SIGNAL(callbackCreatePage(QString,QString)),this, SLOT(callbackCreatePage(QString,QString)));
    // 连接打开下个画面的信号和槽,andrew,20150723
    connect(m_core, SIGNAL(openNextPage()),this, SLOT(openNextPage()));
    connect(m_core, SIGNAL(openPrevPage()),this, SLOT(openPrevPage()));
}

XbelTree::~XbelTree()
{
    write();
    if(popMenu)
    {
        delete popMenu;
        popMenu = NULL;
    }
}

bool XbelTree::read(QIODevice *device)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::information(window(), tr("DOM Bookmarks"),
                                 tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return false;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != conRootTag) {
        QMessageBox::information(window(), tr("DOM Bookmarks"),
                                 tr("The file is not an XBEL file."));
        return false;
    } else if (root.hasAttribute(conVersionTag)
               && root.attribute(conVersionTag) != conVersionValue) {
        QMessageBox::information(window(), tr("DOM Bookmarks"),
                                 tr("The file is not an XBEL version 1.0 "
                                    "file."));
        return false;
    }

    clear();

    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
               this, SLOT(updateDomElement(QTreeWidgetItem*,int)));

    QDomElement child = root.firstChildElement(conFolderTag);
    while (!child.isNull()) {
        parseFolderElement(child);
        child = child.nextSiblingElement(conFolderTag);
    }

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updateDomElement(QTreeWidgetItem*,int)));

    return true;
}

bool XbelTree::read()
{
    QFile file(m_fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        read(&file);
        file.close();
        return true;
    }
    return false;
}

bool XbelTree::write(QIODevice *device)
{
    const int IndentSize = 6;

    QTextStream out(device);
    domDocument.save(out, IndentSize);
    return true;
}

bool XbelTree::write()
{
    QFile file(m_fileName);
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        return write(&file);
    }
    return false;
}

void XbelTree::updateDomElement(QTreeWidgetItem *item, int column)
{
    QDomElement element = domElementForItem.value(item);
    if (!element.isNull()) {
        if (column == 0) {
            QDomElement oldTitleElement = element.firstChildElement(conTitleTag);
            QDomElement newTitleElement = domDocument.createElement(conTitleTag);

            QDomText newTitleText = domDocument.createTextNode(item->text(0));
            newTitleElement.appendChild(newTitleText);

            element.replaceChild(newTitleElement, oldTitleElement);
        } else {
            if (element.tagName() == conPageTag)
                element.setAttribute(conPageUrlTag, item->text(1));
        }
    }
}

void XbelTree::parseFolderElement(const QDomElement &element,
                                  QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item = createItem(element, parentItem,Folder);

    QString title = element.firstChildElement(conTitleTag).text();
    if (title.isEmpty())
        title = QObject::tr("Folder Name is Null");

    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setIcon(0, folderIcon);
    item->setText(0, title);

    bool folded = (element.attribute(conFolderFoldTag) != "no");
    setItemExpanded(item, !folded);

    QDomElement child = element.firstChildElement();
    while (!child.isNull()) {
        if (child.tagName() == conFolderTag) {
            parseFolderElement(child, item);
        } else if (child.tagName() == conPageTag) {
            QTreeWidgetItem *childItem = createItem(child, item,Page);

            QString title = child.firstChildElement(conTitleTag).text();
            if (title.isEmpty())
                title = QObject::tr("Page Name is Null");

            childItem->setFlags(item->flags() | Qt::ItemIsEditable);
            childItem->setIcon(0, bookmarkIcon);
            childItem->setText(0, title);
            childItem->setText(1, child.attribute(conPageUrlTag));
        } else if (child.tagName() == conSeparatorTag) {
            QTreeWidgetItem *childItem = createItem(child, item,Separator);
            childItem->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            childItem->setText(0, QString(30, 0xB7));
        }
        child = child.nextSiblingElement();
    }
}

QTreeWidgetItem *XbelTree::createItem(const QDomElement &element,
                                      QTreeWidgetItem *parentItem,int type)
{
    QTreeWidgetItem *item;
    if (parentItem) {
        item = new QTreeWidgetItem(parentItem,type);
    } else {
        item = new QTreeWidgetItem(this,type);
    }
    domElementForItem.insert(item, element);
    return item;
}
void XbelTree::contextMenuEvent(QContextMenuEvent *event)
{
    //Clear Already add Actions,andie,20140902
    popMenu->clear();
    QTreeWidgetItem * item = currentItem();
    if(item!=NULL)
    {
        qDebug()<< "right click tree item: "<< item->text(0);
        // add right menu action
        switch(item->type())
        {
        case Folder:
        {
            popMenu->addAction(createFolderAction);
            popMenu->addAction(createPageAction);
            popMenu->addAction(removeFolderAction);
            break;
        }
        case Page:
        {
            popMenu->addAction(removePageAction);
            break;
        }
        default:
        {
            break;
        }
        }
        popMenu->addAction(moveTopAction);
        popMenu->addAction(moveUpAction);
        popMenu->addAction(moveDownAction);
        popMenu->addAction(moveBottomAction);
    }
    else
    {
        popMenu->addAction(createFolderAction);
    }
    popMenu->exec(QCursor::pos());
    event->accept();
}
void XbelTree::dropEvent(QDropEvent *event)
{
    //QTreeWidgetPrivate * const d = d_func();
//    Q_D(XbelTree);
//    if (event->source() == this && (event->dropAction() == Qt::MoveAction ||
//                                    dragDropMode() == QAbstractItemView::InternalMove)) {
//        QModelIndex topIndex;
//        int col = -1;
//        int row = -1;
//        if (d->dropOn(event, &row, &col, &topIndex)) {
//            QList<QModelIndex> idxs = selectedIndexes();
//            QList<QPersistentModelIndex> indexes;
//            for (int i = 0; i < idxs.count(); i++)
//                indexes.append(idxs.at(i));

//            if (indexes.contains(topIndex))
//                return;

//            // When removing items the drop location could shift
//            QPersistentModelIndex dropRow = model()->index(row, col, topIndex);

//            // Remove the items
//            QList<QTreeWidgetItem *> taken;
//            for (int i = indexes.count() - 1; i >= 0; --i) {
//                QTreeWidgetItem *parent = itemFromIndex(indexes.at(i));
//                if (!parent || !parent->parent()) {
//                    taken.append(takeTopLevelItem(indexes.at(i).row()));
//                } else {
//                    taken.append(parent->parent()->takeChild(indexes.at(i).row()));
//                }
//            }

//            // insert them back in at their new positions
//            for (int i = 0; i < indexes.count(); ++i) {
//                // Either at a specific point or appended
//                if (row == -1) {
//                    if (topIndex.isValid()) {
//                        QTreeWidgetItem *parent = itemFromIndex(topIndex);
//                        parent->insertChild(parent->childCount(), taken.takeFirst());
//                    } else {
//                        insertTopLevelItem(topLevelItemCount(), taken.takeFirst());
//                    }
//                } else {
//                    int r = dropRow.row() >= 0 ? dropRow.row() : row;
//                    if (topIndex.isValid()) {
//                        QTreeWidgetItem *parent = itemFromIndex(topIndex);
//                        parent->insertChild(qMin(r, parent->childCount()), taken.takeFirst());
//                    } else {
//                        insertTopLevelItem(qMin(r, topLevelItemCount()), taken.takeFirst());
//                    }
//                }
//            }

//            event->accept();
//            // Don't want QAbstractItemView to delete it because it was "moved" we already did it
//            event->setDropAction(Qt::CopyAction);
//        }
//    }
//    QTreeView::dropEvent(event);
}

void XbelTree::createActions()
{
    createFolderAction = new QAction(tr("createFolder"),this);
    connect(createFolderAction,SIGNAL(triggered()),this,SLOT(createFolder()));
    removeFolderAction = new QAction(tr("removeFolder"),this);
    connect(removeFolderAction,SIGNAL(triggered()),this,SLOT(removeFolder()));
    createPageAction = new QAction(tr("createPage"),this);
    connect(createPageAction,SIGNAL(triggered()),this,SLOT(createPage()));
    createPageAction->setStatusTip("createPage");
    removePageAction = new QAction(tr("removePage"),this);
    connect(removePageAction,SIGNAL(triggered()),this,SLOT(removePage()));
    moveUpAction = new QAction("moveUp",this);
    connect(moveUpAction,SIGNAL(triggered()),this,SLOT(moveUp()));
    moveDownAction = new QAction("moveDown",this);
    connect(moveDownAction,SIGNAL(triggered()),this,SLOT(moveDown()));
    moveTopAction = new QAction("moveTop",this);
    connect(moveTopAction,SIGNAL(triggered()),this,SLOT(moveTop()));
    moveBottomAction = new QAction("moveBottom",this);
    connect(moveBottomAction,SIGNAL(triggered()),this,SLOT(moveBottom()));
}

void XbelTree::setFileName(const QString &file_name)
{
    m_fileName = file_name;
}

QString XbelTree::fileName() const
{
    return m_fileName;
}
bool XbelTree::load()
{
    QFile file(m_fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        read(&file);
        file.close();
        return true;
    }
    return false;
}

bool XbelTree::save()
{
    QFile file(m_fileName);
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        return write(&file);
    }
    return false;
}

void XbelTree::createFolder()
{
    SimpleInputDialog dlg(this);
    dlg.setInputName(tr("Folder Name:"));
    switch(dlg.exec())
    {
    case QDialog::DialogCode::Rejected:
        return;
    case QDialog::DialogCode::Accepted:
        break;
    }
    QString folderName = dlg.inputString();
    qDebug()<<"start create folder " << folderName;
    // create xml node
    QDomElement element = domDocument.createElement(conFolderTag);
    element.setAttribute(conFolderFoldTag,"no");
    QDomElement element_title = domDocument.createElement(conTitleTag);
    element_title.appendChild(domDocument.createTextNode(folderName));
    element.appendChild(element_title);
    QList<QTreeWidgetItem *> selItems = selectedItems();
    QTreeWidgetItem * item = NULL;
    QDomElement currentElement;
    if(selItems.size()>0) {
        // create a tree item
        QTreeWidgetItem* current_Item = currentItem();
        item = new QTreeWidgetItem(current_Item,Folder);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setIcon(0, folderIcon);
        item->setText(0, folderName);
        setItemExpanded(current_Item, true);
        current_Item->addChild(item);
        currentElement = domElementForItem.value(current_Item);
    } else {
        // create a tree item
        item = new QTreeWidgetItem(this,Folder);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setIcon(0, folderIcon);
        item->setText(0, folderName);
        //
        insertTopLevelItem(0,item);
        currentElement = domDocument.documentElement();
    }
    currentElement.appendChild(element);
    domElementForItem.insert(item, element);
    write();
}

void XbelTree::removeFolder()
{
    qDebug() << tr("start remove Folder!");
    removeItem(currentItem());
    write();
//    if (current_Item!=NULL) {
//        qDebug() << tr("start delete folder!");
//        QTreeWidgetItem * tParent = current_Item->parent();
//        if(tParent)
//        {
//            tParent->removeChild(current_Item);
//            QDomElement currentElement = domElementForItem.value(current_Item);
//            currentElement.parentNode().removeChild(currentElement);
//        }
//        else
//        {
//            QWidget * tParent1=parentWidget();
//            //tParent1->removeChild(current_Item);
//        }
//    }
}

void XbelTree::callbackCreatePage(QString bookMarkName,QString bookMarkHref)
{
    qDebug()<<"createBookMark";
    QTreeWidgetItem * current_Item = currentItem();
    QTreeWidgetItem * item = NULL;
    if (current_Item!=NULL) {
        item = new QTreeWidgetItem(current_Item,Page);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setIcon(0, bookmarkIcon);
        item->setText(0, bookMarkName);
        item->setText(1, bookMarkHref);
        current_Item->addChild(item);
        QDomElement currentElement = domElementForItem.value(current_Item);
        QDomElement element = domDocument.createElement(conPageTag);
        element.setAttribute(conPageUrlTag,bookMarkHref);
        QDomElement element_title = domDocument.createElement(conTitleTag);
        element_title.appendChild(domDocument.createTextNode(bookMarkName));
        element.appendChild(element_title);
        currentElement.appendChild(element);
        domElementForItem.insert(item, element);
    } else {
        item = new QTreeWidgetItem(this,Page);
    }
    write();
}

void XbelTree::createPage()
{
    if(m_core)
    {
        qDebug() << "start createPage!";
        emit m_core->createPage();
    }
}

void XbelTree::removePage()
{
    qDebug() << tr("start remove page!");
    removeItem(currentItem());
    write();
}
void XbelTree::removeItem(QTreeWidgetItem * Item)
{
    if (Item)
    {
        for(int pos=0;pos<Item->childCount();pos++)
        {
            QTreeWidgetItem * childItem = Item->child(pos);
            removeItem(childItem);
        }
        switch(Item->type())
        {
        case Page:
        {
            //remove page file
            QFile::remove(pageFileName(Item));
            break;
        }
        case Folder:
        case Separator:
        default:
            break;
        }
        //remove xml node
        QDomElement currentElement = domElementForItem.value(Item);
        currentElement.parentNode().removeChild(currentElement);
        //remove tree node
        domElementForItem.remove(Item);
        QTreeWidgetItem * parent = Item->parent();
        if(parent)
            parent->removeChild(Item);
        else
            takeTopLevelItem(indexOfTopLevelItem(Item));
        qDebug() << tr("remove item name:")<<Item->text(0);
    }
}

QString XbelTree::pageFileName(QTreeWidgetItem * Item)
{
    if(Item)
    {
        QString bookMarkHref = Item->text(1);
        QString pageFileName = qApp->applicationDirPath();
        pageFileName += "/";
        pageFileName += bookMarkHref;
        return pageFileName;
    }
    return "";
}
QString XbelTree::pageName()
{


    //QDesignerSettings settings(m_workbench->core());
    return "dlg.inputString";
}

void XbelTree::moveUp()
{
    qDebug()<<"moveUpBookMark";
    QTreeWidgetItem * current_Item = currentItem();
    if(current_Item!=NULL)
    {
        QTreeWidgetItem * parent_Item = current_Item->parent();
        int current_Index = parent_Item->indexOfChild(current_Item);
        if(current_Index==0)
            return;
        //上移树节点
        QTreeWidgetItem * clone_Item = current_Item->clone();
        parent_Item->insertChild(current_Index-1,clone_Item);
        parent_Item->removeChild(current_Item);
        //上移XML节点
        QDomElement current_Element = domElementForItem.value(current_Item);
        QDomNode parent_Element = current_Element.parentNode();
        QDomNode before_Element = current_Element.previousSibling();
        parent_Element.insertBefore(current_Element,before_Element);
        //更改映射
        domElementForItem.remove(current_Item);
        domElementForItem.insert(clone_Item,current_Element);
        write();
    }
}
void XbelTree::moveDown()
{
    qDebug()<<"moveDownBookMark";
    QTreeWidgetItem * current_Item = currentItem();
    if(current_Item!=NULL)
    {
        QTreeWidgetItem * parent_Item = current_Item->parent();
        int current_Index = parent_Item->indexOfChild(current_Item);
        if(current_Index==parent_Item->childCount())
            return;
        //下移树节点
        QTreeWidgetItem * clone_Item = current_Item->clone();
        parent_Item->insertChild(current_Index+2,clone_Item);
        parent_Item->removeChild(current_Item);
        //下移XML节点
        QDomElement current_Element = domElementForItem.value(current_Item);
        QDomNode parent_Element = current_Element.parentNode();
        QDomNode after_Element = current_Element.nextSibling();
        parent_Element.insertAfter(current_Element,after_Element);
        //更改映射
        domElementForItem.remove(current_Item);
        domElementForItem.insert(clone_Item,current_Element);
        write();
    }
}
void XbelTree::moveTop()
{
    if(m_core)
        m_core->formWindowManager()->showPreview();
    qDebug()<<"moveTopBookMark";
    QTreeWidgetItem * current_Item = currentItem();
    if(current_Item!=NULL)
    {
        QTreeWidgetItem * parent_Item = current_Item->parent();
        if(parent_Item)
        {
            int current_Index = parent_Item->indexOfChild(current_Item);
            if(current_Index==0)
                return;
            //置顶树节点
            QTreeWidgetItem * clone_Item = current_Item->clone();
            parent_Item->insertChild(0,clone_Item);
            parent_Item->removeChild(current_Item);
            //置顶XML节点
            QDomElement current_Element = domElementForItem.value(current_Item);
            QDomNode parent_Element = current_Element.parentNode();
            QDomNode first_Element = parent_Element.firstChild();
            parent_Element.insertBefore(current_Element,first_Element);
            //更改映射
            domElementForItem.remove(current_Item);
            domElementForItem.insert(clone_Item,current_Element);
            write();
        }
    }    
}
void XbelTree::moveBottom()
{
    qDebug()<<"moveBottomBookMark";
    QTreeWidgetItem * current_Item = currentItem();
    if(current_Item!=NULL)
    {
        QTreeWidgetItem * parent_Item = current_Item->parent();
        int current_Index = parent_Item->indexOfChild(current_Item);
        int count = parent_Item->childCount();
        if(current_Index==count)
            return;
        //置底树节点
        QTreeWidgetItem * clone_Item = current_Item->clone();
        parent_Item->insertChild(count,clone_Item);
        parent_Item->removeChild(current_Item);
        //置底XML节点
        QDomElement current_Element = domElementForItem.value(current_Item);
        QDomNode parent_Element = current_Element.parentNode();
        QDomNode last_Element = parent_Element.lastChild();
        parent_Element.insertAfter(current_Element,last_Element);
        //更改映射
        domElementForItem.remove(current_Item);
        domElementForItem.insert(clone_Item,current_Element);
        write();
    }
}
void XbelTree::openPage(QModelIndex index)
{
    QTreeWidgetItem * pickItem = itemFromIndex(index);
    if(pickItem)
    {
        if(pickItem->columnCount()==2)
        {
            TaskInterface * task = NetComponents::getTastCenter();
            QString pageName = task->currentTaskPath() + g_pageRelativeFilePath + pickItem->text(1);
            //QString pageName = ".\/page\/sample1.ui";//pickItem->text(1);
            if(m_core)
            {
                //m_core->formWindowManager()->closeAllPreviews();
                //m_core->formWindowManager()->showPreview();
                emit m_core->openPage(pageName);
            }
            qDebug() << "openZXPage: " << pageName;
        }
    }
}

void XbelTree::openNextPage()
{
    QTreeWidgetItem * curItem = currentItem();
    if(curItem!=NULL)
    {
        QTreeWidgetItem * belowItem = itemBelow(curItem);
        if(belowItem!=NULL)
        {
            setCurrentItem(belowItem);
            openPage(indexFromItem(belowItem));
        }
    }
}
void XbelTree::openPrevPage()
{
    QTreeWidgetItem * curItem = currentItem();
    if(curItem!=NULL)
    {
        QTreeWidgetItem * aboveItem = itemAbove(curItem);
        if(aboveItem!=NULL)
        {
            setCurrentItem(aboveItem);
            openPage(indexFromItem(aboveItem));
        }
    }
}

void XbelTree::filter(const QString &key)
{
    const bool empty = key.isEmpty();
    QRegExp re = empty ? QRegExp() : QRegExp(key, Qt::CaseInsensitive, QRegExp::FixedString);
    if (filterFolderElement(re))
        updateGeometries();
}

bool XbelTree::filterFolderElement(const QRegExp &key,QTreeWidgetItem *parentItem)
{
    bool changed = false;
    if(parentItem==0)
    {
        const int numTopLevels = topLevelItemCount();
        for (int i = 0; i < numTopLevels; i++)
        {
            QTreeWidgetItem *tl = topLevelItem(i);
            filterFolderElement(key,tl);
        }
    }
    else if(parentItem->type() == Folder)
    {
        int childCount = parentItem->childCount();
        bool t_Find = false;
        for(int pos = 0;pos<childCount;pos++)
        {
            QTreeWidgetItem *child = parentItem->child(pos);
            if(filterFolderElement(key,child))
                t_Find = true;
        }
        bool t_contains = itemContainsKey(key,parentItem);;
        parentItem->setHidden(!t_contains&&!t_Find);
        return (t_contains||t_Find);
    }
    else if(parentItem->type() == Page)
    {
        changed = itemContainsKey(key,parentItem);
        parentItem->setHidden(!changed);
        return changed;
    }
    else if(parentItem->type() == Separator)
    {
        parentItem->setHidden(true);
    }
}
bool XbelTree::itemContainsKey(const QRegExp &key,QTreeWidgetItem *item)
{
    QString data = item->text(0);
    return data.contains(key);
}
