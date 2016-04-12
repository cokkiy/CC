/********************************************************************************
** Form generated from reading UI file 'floatingmenu.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FLOATINGMENU_H
#define UI_FLOATINGMENU_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FloatingMenu
{
public:

    void setupUi(QWidget *FloatingMenu)
    {
        if (FloatingMenu->objectName().isEmpty())
            FloatingMenu->setObjectName(QStringLiteral("FloatingMenu"));
        FloatingMenu->setWindowModality(Qt::NonModal);
        FloatingMenu->resize(300, 300);
        QFont font;
        font.setPointSize(10);
        FloatingMenu->setFont(font);
        FloatingMenu->setContextMenuPolicy(Qt::NoContextMenu);

        retranslateUi(FloatingMenu);

        QMetaObject::connectSlotsByName(FloatingMenu);
    } // setupUi

    void retranslateUi(QWidget *FloatingMenu)
    {
        FloatingMenu->setWindowTitle(QApplication::translate("FloatingMenu", "FloatingMenu", 0));
    } // retranslateUi

};

namespace Ui {
    class FloatingMenu: public Ui_FloatingMenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FLOATINGMENU_H
