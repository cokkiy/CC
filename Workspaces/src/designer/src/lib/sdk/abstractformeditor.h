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

#ifndef ABSTRACTFORMEDITOR_H
#define ABSTRACTFORMEDITOR_H

#include <QtDesigner/sdk_global.h>

#include <QtCore/QObject>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QDesignerWidgetBoxInterface;
class QDesignerPropertyEditorInterface;
class QDesignerFormWindowManagerInterface;
class QDesignerWidgetDataBaseInterface;
class QDesignerMetaDataBaseInterface;
class QDesignerWidgetFactoryInterface;
class QDesignerObjectInspectorInterface;
class QDesignerImageExplorerInterface;
class QDesignerPromotionInterface;
class QDesignerActionEditorInterface;
class QDesignerIntegrationInterface;
class QDesignerPluginManager;
class QDesignerIntrospectionInterface;
class QDesignerDialogGuiInterface;
class QDesignerSettingsInterface;
class QDesignerOptionsPageInterface;
class QtResourceModel;
class QtGradientManager;

class QWidget;
class QIcon;

class QExtensionManager;

class  QDesignerFormEditorInterfacePrivate;

class QDESIGNER_SDK_EXPORT QDesignerFormEditorInterface : public QObject
{
    Q_OBJECT
public:
    QDesignerFormEditorInterface(QObject *parent = 0);
    virtual ~QDesignerFormEditorInterface();

    QExtensionManager *extensionManager() const;

    QWidget *topLevel() const;
    QDesignerWidgetBoxInterface *widgetBox() const;
    QDesignerPropertyEditorInterface *propertyEditor() const;
    QDesignerObjectInspectorInterface *objectInspector() const;
    QDesignerImageExplorerInterface *imageExplorer() const;
    QDesignerFormWindowManagerInterface *formWindowManager() const;
    QDesignerWidgetDataBaseInterface *widgetDataBase() const;
    QDesignerMetaDataBaseInterface *metaDataBase() const;
    QDesignerPromotionInterface *promotion() const;
    QDesignerWidgetFactoryInterface *widgetFactory() const;
    QDesignerActionEditorInterface *actionEditor() const;
    QDesignerIntegrationInterface *integration() const;
    QDesignerPluginManager *pluginManager() const;
    QDesignerIntrospectionInterface *introspection() const;
    QDesignerDialogGuiInterface *dialogGui() const;
    QDesignerSettingsInterface *settingsManager() const;
    QString resourceLocation() const;
    QtResourceModel *resourceModel() const;
    QtGradientManager *gradientManager() const;
    QList<QDesignerOptionsPageInterface*> optionsPages() const;

    void setTopLevel(QWidget *topLevel);
    void setWidgetBox(QDesignerWidgetBoxInterface *widgetBox);
    void setPropertyEditor(QDesignerPropertyEditorInterface *propertyEditor);
    void setObjectInspector(QDesignerObjectInspectorInterface *objectInspector);
    void setImageExplorer(QDesignerImageExplorerInterface *imageexplorer);
    void setPluginManager(QDesignerPluginManager *pluginManager);
    void setActionEditor(QDesignerActionEditorInterface *actionEditor);
    void setIntegration(QDesignerIntegrationInterface *integration);
    void setIntrospection(QDesignerIntrospectionInterface *introspection);
    void setDialogGui(QDesignerDialogGuiInterface *dialogGui);
    void setSettingsManager(QDesignerSettingsInterface *settingsManager);
    void setResourceModel(QtResourceModel *model);
    void setGradientManager(QtGradientManager *manager);
    void setOptionsPages(const QList<QDesignerOptionsPageInterface*> &optionsPages);

    QObjectList pluginInstances() const;

    static QIcon createIcon(const QString &name);
signals:
    // 打开某画面
    void openPage(QString imageName);
    // 编辑某画面
    void editPage(QString imageName);
    // 创建某画面
    void createPage();
    // 创建标签及地址
    void callbackCreatePage(QString markName,QString url);
    // 打开上个画面
    void openPrevPage();
    // 打开下个画面
    void openNextPage();
protected:
    void setFormManager(QDesignerFormWindowManagerInterface *formWindowManager);
    void setMetaDataBase(QDesignerMetaDataBaseInterface *metaDataBase);
    void setWidgetDataBase(QDesignerWidgetDataBaseInterface *widgetDataBase);
    void setPromotion(QDesignerPromotionInterface *promotion);
    void setWidgetFactory(QDesignerWidgetFactoryInterface *widgetFactory);
    void setExtensionManager(QExtensionManager *extensionManager);

private:
    //QDesignerFormEditorInterface的具体实现类
    QScopedPointer<QDesignerFormEditorInterfacePrivate> d;

private:
    QDesignerFormEditorInterface(const QDesignerFormEditorInterface &other);
    void operator = (const QDesignerFormEditorInterface &other);
};

QT_END_NAMESPACE

#endif // ABSTRACTFORMEDITOR_H
