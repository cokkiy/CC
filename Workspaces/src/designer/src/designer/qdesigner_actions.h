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

#ifndef QDESIGNER_ACTIONS_H
#define QDESIGNER_ACTIONS_H

#include "assistantclient.h"
#include "qdesigner_settings.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#ifndef QT_NO_PRINTER
#include <QtPrintSupport/QPrinter>
#endif

QT_BEGIN_NAMESPACE

class QDesignerWorkbench;

class QDir;
class QTimer;
class QAction;
class QActionGroup;
class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class AppFontDialog;

class QRect;
class QWidget;
class QPixmap;
class QMenu;
//预先定义类
namespace qdesigner_internal {
    class PreviewConfiguration;
    class PreviewManager;
}

class QDesignerActions: public QObject
{
    Q_OBJECT
public:
    explicit QDesignerActions(QDesignerWorkbench *mainWindow);
    virtual ~QDesignerActions();

    QDesignerWorkbench *workbench() const;
    QDesignerFormEditorInterface *core() const;

    bool saveForm(QDesignerFormWindowInterface *fw);
    bool readInForm(const QString &fileName);
    void newForm(const QString &fileName);
    bool writeOutForm(QDesignerFormWindowInterface *formWindow, const QString &fileName, bool check = true);

    QActionGroup *fileActions() const;
    //QActionGroup *recentFilesActions() const;
    QActionGroup *editActions() const;
    //QActionGroup *formActions() const;
    QActionGroup *settingsActions() const;
    QActionGroup *windowActions() const;
    QActionGroup *toolActions() const;
    QActionGroup *helpActions() const;
    QActionGroup *functionActions() const;
    QActionGroup *uiMode() const;
    QActionGroup *styleActions() const;
    // file actions
    QAction *openFormAction() const;
    //QAction *closeFormAction() const;
    // window actions
    //QAction *minimizeAction() const;
    // edit mode actions
    QAction *editWidgets() const;
    // form actions
    QAction *previewFormAction() const;
    QAction *viewCodeAction() const;

    void setBringAllToFrontVisible(bool visible);
    void setWindowListSeparatorVisible(bool visible);

    bool openForm(QWidget *parent);

    QString uiExtension() const;

    void setAppMode(AppMode);

    // Boolean dynamic property set on actions to
    // show them in the default toolbar layout
    static const char *defaultToolbarPropertyName;

public slots:
    void activeFormWindowChanged(QDesignerFormWindowInterface *formWindow);
    void createForm();
    void slotOpenForm();
    void helpRequested(const QString &manual, const QString &document);

signals:
    void useBigIcons(bool);

private slots:
    void saveForm();
    void saveFormAs();
    void saveAllForms();
    void saveFormAsTemplate();
    void viewCode();
    void notImplementedYet();
    void shutdown();
    void editWidgetsSlot();
    void openRecentForm();
    void clearRecentFiles();
    void closeForm();
    void showDesignerHelp();
    void aboutDesigner();
    void outData();
    void showWidgetSpecificHelp();
    void backupForms();
    void showNewFormDialog(const QString &fileName);
    void showPreferencesDialog();
    void showAppFontDialog();
    void setDesingerMode();
    void setRealTimeMode();
    void setFullScreen();
    void savePreviewImage();
    void printPreviewImage();
    void updateCloseAction();
    void formWindowCountChanged();
    void formWindowSettingsChanged(QDesignerFormWindowInterface *fw);
    void slotOpenNextPage();
    void slotOpenPrevPage();
private:
    QAction *createRecentFilesMenu();
    bool saveFormAs(QDesignerFormWindowInterface *fw);
    //设置菜单快捷键的工作模式,andrew,20150326
    void fixActionContext();
    void updateRecentFileActions();
    void addRecentFile(const QString &fileName);
    void showHelp(const QString &help);
    void closePreview();
    QRect fixDialogRect(const QRect &rect) const;
    QString fixResourceFileBackupPath(QDesignerFormWindowInterface *fwi, const QDir& backupDir);
    void showStatusBarMessage(const QString &message) const;
    QActionGroup *createHelpActions();

    //添加功能菜单 rika 20151019
    QActionGroup *createFunctionActions();


    bool ensureBackupDirectories();
    QPixmap createPreviewPixmap(QDesignerFormWindowInterface *fw);
    qdesigner_internal::PreviewConfiguration previewConfiguration();

    enum { MaxRecentFiles = 10 };
    QDesignerWorkbench *m_workbench;
    QDesignerFormEditorInterface *m_core;
    QDesignerSettings m_settings;
    AssistantClient m_assistantClient;
    QString m_openDirectory;
    QString m_saveDirectory;


    QString m_backupPath;
    QString m_backupTmpPath;

    QTimer* m_backupTimer;

    QActionGroup *m_fileActions;
    //QActionGroup *m_recentFilesActions;
    QActionGroup *m_editActions;
    //QActionGroup *m_formActions;
    QActionGroup *m_settingsActions;
    //QActionGroup *m_windowActions;
    QActionGroup *m_toolActions;
    QActionGroup *m_helpActions;
    QActionGroup *m_styleActions;
    QActionGroup *m_functionActions;//Rika add 20151019

    QAction *m_editWidgetsAction;

    QAction *m_newFormAction;
    QAction *m_openFormAction;
    QAction *m_saveFormAction;
    //QAction *m_saveFormAsAction;
    QAction *m_saveAllFormsAction;
    QAction *m_saveFormAsTemplateAction;
    //QAction *m_closeFormAction;
    QAction *m_savePreviewImageAction;
    QAction *m_printPreviewAction;

    QAction *m_quitAction;

    QAction *m_previewFormAction;
    QAction *m_viewCodeAction;

    //QAction *m_minimizeAction;
    QAction *m_bringAllToFrontSeparator;
    QAction *m_bringAllToFrontAction;
    QAction *m_windowListSeparatorAction;

    //QAction *m_preferencesAction;
    //QAction *m_appFontAction;
    //应用程序的工作模式,andrew,20150323
    QAction *actDesigerMode;
    QAction *actRealTimeMode;
    //画面全屏动作,andrew,20150521
    QAction *actFullScreen;
    //打开上个画面,andrew,20150723
    QAction *actOpenPrevPage;
    //打开下个画面,andrew,20150723
    QAction *actOpenNextPage;
    QPointer<AppFontDialog> m_appFontDialog;

#ifndef QT_NO_PRINTER
    QPrinter *m_printer;
#endif

    qdesigner_internal::PreviewManager *m_previewManager;
};

QT_END_NAMESPACE

#endif // QDESIGNER_ACTIONS_H
