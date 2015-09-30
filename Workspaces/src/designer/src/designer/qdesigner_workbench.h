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

#ifndef QDESIGNER_WORKBENCH_H
#define QDESIGNER_WORKBENCH_H

#include "designer_enums.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QList>
#include <QtCore/QRect>

QT_BEGIN_NAMESPACE

class QDesignerActions;
class QDesignerToolWindow;
class QDesignerFormWindow;
class DockedMainWindow;
class QDesignerSettings;

class QAction;
class QActionGroup;
class QDockWidget;
class QMenu;
class QMenuBar;
class QMainWindow;
class QToolBar;
class QMdiArea;
class QMdiSubWindow;
class QCloseEvent;
class QFont;
class QtToolBarManager;
class ToolBarManager;

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QDesignerFormWindowManagerInterface;
class QDesignerIntegration;

class QDesignerWorkbench: public QObject
{
    Q_OBJECT

public:
    QDesignerWorkbench();
    virtual ~QDesignerWorkbench();

    UIMode mode() const;
    AppMode appmode() const;
    void setAppMode(const AppMode m);
    void reopenPage();
    void update();
    QDesignerFormEditorInterface *core() const;
    QDesignerFormWindow *findFormWindow(QWidget *widget) const;
    // 打开画面ui文件
    QDesignerFormWindow *openForm(const QString &fileName, QString *errorMessage);    
    // 打开画面ui模板文件
    QDesignerFormWindow *openTemplate(const QString &templateFileName,
                                      const QString &editorFileName,
                                      QString *errorMessage);

    int toolWindowCount() const;
    QDesignerToolWindow *toolWindow(int index) const;

    int formWindowCount() const;
    QDesignerFormWindow *formWindow(int index) const;

    QDesignerActions *actionManager() const;

    QActionGroup *modeActionGroup() const;

    QRect availableGeometry() const;
    QRect desktopGeometry() const;

    int marginHint() const;

    bool readInForm(const QString &fileName) const;
    bool writeOutForm(QDesignerFormWindowInterface *formWindow, const QString &fileName) const;
    bool saveForm(QDesignerFormWindowInterface *fw);
    bool handleClose();
    bool readInBackup();
    void updateBackup(QDesignerFormWindowInterface* fwi);
    void applyUiSettings();

signals:
    void modeChanged(UIMode mode);
    void initialized();

public slots:
    void addFormWindow(QDesignerFormWindow *formWindow);
    void removeFormWindow(QDesignerFormWindow *formWindow);
    void bringAllToFront();
    void toggleFormMinimizationState();
    // 打开某画面
    void openPage(QString imageName);
    // 编辑某画面
    void editPage(QString imageName);
    // 创建某画面
    void createPage();
    //切换全屏显示或还原显示画面窗口
    void fullScreen();

private:
    QString m_currentFileName;

private slots:
    void switchToNeutralMode();
    void switchToDockedMode();
    void switchToTopLevelMode();
    void initializeCorePlugins();
    void handleCloseEvent(QCloseEvent *);
    void slotFormWindowActivated(QDesignerFormWindow* fw);
    void updateWindowMenu(QDesignerFormWindowInterface *fw);
    void formWindowActionTriggered(QAction *a);
    void adjustMDIFormPositions();
    void minimizationStateChanged(QDesignerFormWindowInterface *formWindow, bool minimized);

    void restoreUISettings();
    void notifyUISettingsChanged();
    void slotFileDropped(const QString &f);

private:
    QWidget *magicalParent(const QWidget *w) const;
    Qt::WindowFlags magicalWindowFlags(const QWidget *widgetForFlags) const;
    QDesignerFormWindowManagerInterface *formWindowManager() const;
    void closeAllToolWindows();
    QDesignerToolWindow *widgetBoxToolWindow() const;
    QDesignerFormWindow *loadForm(const QString &fileName, bool detectLineTermiantorMode, QString *errorMessage);
    void resizeForm(QDesignerFormWindow *fw,  const QWidget *mainContainer) const;
    void saveGeometriesForModeChange();
    void saveGeometries(QDesignerSettings &settings) const;

    bool isFormWindowMinimized(const QDesignerFormWindow *fw);
    //设置画面窗口的最大化和最小化,andrew,20150717
    void setFormWindowMinimized(QDesignerFormWindow *fw, bool minimized);
    void saveSettings() const;
    // 获取画面窗口的指针,实现只有一个画面窗口的功能,andrew,20150316
    QDesignerFormWindow *getFormWindow(QDesignerFormWindowInterface *formWindow, QDesignerWorkbench *workbench);
    /**
     * @brief getFormWindow 通过画面名称获取该画面的窗口指针,andrew,20150520
     * @param pageName 画面名称
     * @return 画面窗口的指针
     */
    QDesignerFormWindow *getFormWindow(QString pageName);
    QDesignerFormEditorInterface *m_core;
    QDesignerIntegration *m_integration;

    QDesignerActions *m_actionManager;
    //QActionGroup *m_windowActions;

    //QMenu *m_windowMenu;

    QMenuBar *m_globalMenuBar;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;

    struct TopLevelData {
        ToolBarManager *toolbarManager;
        QList<QToolBar *> toolbars;
    };
    TopLevelData m_topLevelData;
    //ui窗口的显示样式,停靠、置顶等
    UIMode m_mode;
    //应用程序的使用模式,andrew,20150319
    AppMode m_appMode;
    DockedMainWindow *m_dockedMainWindow;
    //指显画面的容器窗口指针,20150917
    QWidget * m_currentPageContainer;
    //指显画面的原始大小,andrew,20150917
    QSize m_currentPageSize;
    //工具窗口的列表，刘裕贵，２０１４０８２５
    QList<QDesignerToolWindow*> m_toolWindows;
    //画面窗口的列表,刘裕贵,20150313
    QList<QDesignerFormWindow*> m_formWindows;
    //缓存画面列表的长度
    int m_formWindowListLength;
    //画面名称，窗口指针的映射,刘裕贵,20150520
    QMap<QString,QDesignerFormWindow*> m_pageName_formWindow;
    //窗口指针，画面名称的映射,刘裕贵,20150521
    QMap<QDesignerFormWindow*,QString> m_formWindow_pageName;
    //全屏控制开关,true为全屏显示,false为还原显示
    bool m_bFullScreen;
    QMenu *m_toolbarMenu;    
    // Helper class to remember the position of a window while switching user
    // interface modes.
    class Position {
    public:
        Position(const QDockWidget *dockWidget);
        Position(const QMdiSubWindow *mdiSubWindow, const QPoint &mdiAreaOffset);
        Position(const QWidget *topLevelWindow, const QPoint &desktopTopLeft);

        void applyTo(QMdiSubWindow *mdiSubWindow, const QPoint &mdiAreaOffset) const;
        void applyTo(QWidget *topLevelWindow, const QPoint &desktopTopLeft) const;
        void applyTo(QDockWidget *dockWidget) const;

        QPoint position() const { return m_position; }
    private:
        bool m_minimized;
        // Position referring to top-left corner (desktop in top-level mode or
        // main window in MDI mode)
        QPoint m_position;
    };
    typedef  QHash<QWidget*, Position> PositionMap;
    PositionMap m_Positions;

    enum State { StateInitializing, StateUp, StateClosing };
    State m_state;
    bool m_uiSettingsChanged; // UI mode changed in preference dialog, trigger delayed slot.
};

QT_END_NAMESPACE

#endif // QDESIGNER_WORKBENCH_H
