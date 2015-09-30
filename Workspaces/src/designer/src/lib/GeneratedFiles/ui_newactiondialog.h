/*
*********************************************************************
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
*********************************************************************
*/

/********************************************************************************
** Form generated from reading UI file 'newactiondialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWACTIONDIALOG_H
#define UI_NEWACTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include "iconselector_p.h"
#include "textpropertyeditor_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_NewActionDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *textLabel;
    QLineEdit *editActionText;
    QLabel *objectNameLabel;
    QLineEdit *editObjectName;
    QLabel *toolTipLabel;
    QHBoxLayout *toolTipLayout;
    TextPropertyEditor *tooltipEditor;
    QToolButton *toolTipToolButton;
    QLabel *iconThemeLabel;
    qdesigner_internal::IconThemeEditor *iconThemeEditor;
    QLabel *iconLabel;
    QHBoxLayout *horizontalLayout;
    qdesigner_internal::IconSelector *iconSelector;
    QSpacerItem *spacerItem;
    QCheckBox *checkableCheckBox;
    QLabel *checkableLabel;
    QLabel *shortcutLabel;
    QHBoxLayout *keysequenceLayout;
    QKeySequenceEdit *keySequenceEdit;
    QToolButton *keysequenceResetToolButton;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *qdesigner_internal__NewActionDialog)
    {
        if (qdesigner_internal__NewActionDialog->objectName().isEmpty())
            qdesigner_internal__NewActionDialog->setObjectName(QStringLiteral("qdesigner_internal__NewActionDialog"));
        qdesigner_internal__NewActionDialog->resize(366, 270);
        verticalLayout = new QVBoxLayout(qdesigner_internal__NewActionDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        textLabel = new QLabel(qdesigner_internal__NewActionDialog);
        textLabel->setObjectName(QStringLiteral("textLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, textLabel);

        editActionText = new QLineEdit(qdesigner_internal__NewActionDialog);
        editActionText->setObjectName(QStringLiteral("editActionText"));
        editActionText->setMinimumSize(QSize(255, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, editActionText);

        objectNameLabel = new QLabel(qdesigner_internal__NewActionDialog);
        objectNameLabel->setObjectName(QStringLiteral("objectNameLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, objectNameLabel);

        editObjectName = new QLineEdit(qdesigner_internal__NewActionDialog);
        editObjectName->setObjectName(QStringLiteral("editObjectName"));

        formLayout->setWidget(1, QFormLayout::FieldRole, editObjectName);

        toolTipLabel = new QLabel(qdesigner_internal__NewActionDialog);
        toolTipLabel->setObjectName(QStringLiteral("toolTipLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, toolTipLabel);

        toolTipLayout = new QHBoxLayout();
        toolTipLayout->setObjectName(QStringLiteral("toolTipLayout"));
        tooltipEditor = new TextPropertyEditor(qdesigner_internal__NewActionDialog);
        tooltipEditor->setObjectName(QStringLiteral("tooltipEditor"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tooltipEditor->sizePolicy().hasHeightForWidth());
        tooltipEditor->setSizePolicy(sizePolicy);

        toolTipLayout->addWidget(tooltipEditor);

        toolTipToolButton = new QToolButton(qdesigner_internal__NewActionDialog);
        toolTipToolButton->setObjectName(QStringLiteral("toolTipToolButton"));

        toolTipLayout->addWidget(toolTipToolButton);


        formLayout->setLayout(2, QFormLayout::FieldRole, toolTipLayout);

        iconThemeLabel = new QLabel(qdesigner_internal__NewActionDialog);
        iconThemeLabel->setObjectName(QStringLiteral("iconThemeLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, iconThemeLabel);

        iconThemeEditor = new qdesigner_internal::IconThemeEditor(qdesigner_internal__NewActionDialog);
        iconThemeEditor->setObjectName(QStringLiteral("iconThemeEditor"));

        formLayout->setWidget(3, QFormLayout::FieldRole, iconThemeEditor);

        iconLabel = new QLabel(qdesigner_internal__NewActionDialog);
        iconLabel->setObjectName(QStringLiteral("iconLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, iconLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        iconSelector = new qdesigner_internal::IconSelector(qdesigner_internal__NewActionDialog);
        iconSelector->setObjectName(QStringLiteral("iconSelector"));

        horizontalLayout->addWidget(iconSelector);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);


        formLayout->setLayout(4, QFormLayout::FieldRole, horizontalLayout);

        checkableCheckBox = new QCheckBox(qdesigner_internal__NewActionDialog);
        checkableCheckBox->setObjectName(QStringLiteral("checkableCheckBox"));

        formLayout->setWidget(5, QFormLayout::FieldRole, checkableCheckBox);

        checkableLabel = new QLabel(qdesigner_internal__NewActionDialog);
        checkableLabel->setObjectName(QStringLiteral("checkableLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, checkableLabel);

        shortcutLabel = new QLabel(qdesigner_internal__NewActionDialog);
        shortcutLabel->setObjectName(QStringLiteral("shortcutLabel"));

        formLayout->setWidget(6, QFormLayout::LabelRole, shortcutLabel);

        keysequenceLayout = new QHBoxLayout();
        keysequenceLayout->setObjectName(QStringLiteral("keysequenceLayout"));
        keySequenceEdit = new QKeySequenceEdit(qdesigner_internal__NewActionDialog);
        keySequenceEdit->setObjectName(QStringLiteral("keySequenceEdit"));
        sizePolicy.setHeightForWidth(keySequenceEdit->sizePolicy().hasHeightForWidth());
        keySequenceEdit->setSizePolicy(sizePolicy);

        keysequenceLayout->addWidget(keySequenceEdit);

        keysequenceResetToolButton = new QToolButton(qdesigner_internal__NewActionDialog);
        keysequenceResetToolButton->setObjectName(QStringLiteral("keysequenceResetToolButton"));

        keysequenceLayout->addWidget(keysequenceResetToolButton);


        formLayout->setLayout(6, QFormLayout::FieldRole, keysequenceLayout);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        line = new QFrame(qdesigner_internal__NewActionDialog);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        buttonBox = new QDialogButtonBox(qdesigner_internal__NewActionDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        textLabel->setBuddy(editActionText);
        objectNameLabel->setBuddy(editObjectName);
        toolTipLabel->setBuddy(tooltipEditor);
        iconThemeLabel->setBuddy(iconThemeEditor);
        iconLabel->setBuddy(iconSelector);
        checkableLabel->setBuddy(checkableCheckBox);
        shortcutLabel->setBuddy(keySequenceEdit);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(editActionText, editObjectName);

        retranslateUi(qdesigner_internal__NewActionDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), qdesigner_internal__NewActionDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), qdesigner_internal__NewActionDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(qdesigner_internal__NewActionDialog);
    } // setupUi

    void retranslateUi(QDialog *qdesigner_internal__NewActionDialog)
    {
        qdesigner_internal__NewActionDialog->setWindowTitle(QApplication::translate("qdesigner_internal::NewActionDialog", "New Action...", 0));
        textLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Text:", 0));
        objectNameLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "Object &name:", 0));
        toolTipLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "T&oolTip:", 0));
        toolTipToolButton->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "...", 0));
        iconThemeLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "Icon th&eme:", 0));
        iconLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Icon:", 0));
        checkableCheckBox->setText(QString());
        checkableLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Checkable:", 0));
        shortcutLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Shortcut:", 0));
        keysequenceResetToolButton->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "...", 0));
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class NewActionDialog: public Ui_NewActionDialog {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // UI_NEWACTIONDIALOG_H
