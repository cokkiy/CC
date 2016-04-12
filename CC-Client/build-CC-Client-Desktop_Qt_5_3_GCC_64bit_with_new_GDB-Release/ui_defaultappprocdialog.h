/********************************************************************************
** Form generated from reading UI file 'defaultappprocdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEFAULTAPPPROCDIALOG_H
#define UI_DEFAULTAPPPROCDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DefaultAppProcDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QTableWidget *appTableWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *appLineEdit;
    QLineEdit *argumentLineEdit;
    QLineEdit *appProcNameLineEdit;
    QCheckBox *allowMultiInstanceCheckBox;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *addAppButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *removeAppPushButton;
    QSpacerItem *horizontalSpacer_6;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QListWidget *procListWidget;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QLineEdit *procLineEdit;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *addProcButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *removeProcPushButton;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *OKPushButton;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *canclePushButton;
    QSpacerItem *horizontalSpacer_9;

    void setupUi(QDialog *DefaultAppProcDialog)
    {
        if (DefaultAppProcDialog->objectName().isEmpty())
            DefaultAppProcDialog->setObjectName(QStringLiteral("DefaultAppProcDialog"));
        DefaultAppProcDialog->resize(732, 426);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/MAGNIFY.ico"), QSize(), QIcon::Normal, QIcon::Off);
        DefaultAppProcDialog->setWindowIcon(icon);
        verticalLayout_3 = new QVBoxLayout(DefaultAppProcDialog);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        groupBox = new QGroupBox(DefaultAppProcDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        appTableWidget = new QTableWidget(groupBox);
        if (appTableWidget->columnCount() < 4)
            appTableWidget->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        appTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        appTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        appTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        appTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        appTableWidget->setObjectName(QStringLiteral("appTableWidget"));
        appTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        appTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        appTableWidget->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(appTableWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        appLineEdit = new QLineEdit(groupBox);
        appLineEdit->setObjectName(QStringLiteral("appLineEdit"));

        horizontalLayout->addWidget(appLineEdit);

        argumentLineEdit = new QLineEdit(groupBox);
        argumentLineEdit->setObjectName(QStringLiteral("argumentLineEdit"));

        horizontalLayout->addWidget(argumentLineEdit);

        appProcNameLineEdit = new QLineEdit(groupBox);
        appProcNameLineEdit->setObjectName(QStringLiteral("appProcNameLineEdit"));
        appProcNameLineEdit->setMaximumSize(QSize(180, 16777215));

        horizontalLayout->addWidget(appProcNameLineEdit);

        allowMultiInstanceCheckBox = new QCheckBox(groupBox);
        allowMultiInstanceCheckBox->setObjectName(QStringLiteral("allowMultiInstanceCheckBox"));

        horizontalLayout->addWidget(allowMultiInstanceCheckBox);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_5);

        addAppButton = new QPushButton(groupBox);
        addAppButton->setObjectName(QStringLiteral("addAppButton"));

        horizontalLayout_2->addWidget(addAppButton);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        removeAppPushButton = new QPushButton(groupBox);
        removeAppPushButton->setObjectName(QStringLiteral("removeAppPushButton"));

        horizontalLayout_2->addWidget(removeAppPushButton);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_6);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_6->addWidget(groupBox);

        groupBox_2 = new QGroupBox(DefaultAppProcDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMaximumSize(QSize(260, 16777215));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        procListWidget = new QListWidget(groupBox_2);
        procListWidget->setObjectName(QStringLiteral("procListWidget"));

        verticalLayout_2->addWidget(procListWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_4->addWidget(label_2);

        procLineEdit = new QLineEdit(groupBox_2);
        procLineEdit->setObjectName(QStringLiteral("procLineEdit"));

        horizontalLayout_4->addWidget(procLineEdit);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        addProcButton = new QPushButton(groupBox_2);
        addProcButton->setObjectName(QStringLiteral("addProcButton"));

        horizontalLayout_3->addWidget(addProcButton);

        horizontalSpacer_2 = new QSpacerItem(17, 17, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        removeProcPushButton = new QPushButton(groupBox_2);
        removeProcPushButton->setObjectName(QStringLiteral("removeProcPushButton"));

        horizontalLayout_3->addWidget(removeProcPushButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout_6->addWidget(groupBox_2);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_8);

        OKPushButton = new QPushButton(DefaultAppProcDialog);
        OKPushButton->setObjectName(QStringLiteral("OKPushButton"));
        OKPushButton->setDefault(true);

        horizontalLayout_5->addWidget(OKPushButton);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_7);

        canclePushButton = new QPushButton(DefaultAppProcDialog);
        canclePushButton->setObjectName(QStringLiteral("canclePushButton"));

        horizontalLayout_5->addWidget(canclePushButton);

        horizontalSpacer_9 = new QSpacerItem(150, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_9);


        verticalLayout_3->addLayout(horizontalLayout_5);

        QWidget::setTabOrder(appTableWidget, appLineEdit);
        QWidget::setTabOrder(appLineEdit, argumentLineEdit);
        QWidget::setTabOrder(argumentLineEdit, appProcNameLineEdit);
        QWidget::setTabOrder(appProcNameLineEdit, allowMultiInstanceCheckBox);
        QWidget::setTabOrder(allowMultiInstanceCheckBox, addAppButton);
        QWidget::setTabOrder(addAppButton, removeAppPushButton);
        QWidget::setTabOrder(removeAppPushButton, procListWidget);
        QWidget::setTabOrder(procListWidget, procLineEdit);
        QWidget::setTabOrder(procLineEdit, addProcButton);
        QWidget::setTabOrder(addProcButton, removeProcPushButton);
        QWidget::setTabOrder(removeProcPushButton, OKPushButton);
        QWidget::setTabOrder(OKPushButton, canclePushButton);

        retranslateUi(DefaultAppProcDialog);

        QMetaObject::connectSlotsByName(DefaultAppProcDialog);
    } // setupUi

    void retranslateUi(QDialog *DefaultAppProcDialog)
    {
        DefaultAppProcDialog->setWindowTitle(QApplication::translate("DefaultAppProcDialog", "\350\256\276\347\275\256\346\226\260\345\273\272\345\267\245\344\275\234\347\253\231\351\273\230\350\256\244\350\277\234\347\250\213\345\220\257\345\212\250\345\272\224\347\224\250\345\222\214\347\233\221\350\247\206\350\277\233\347\250\213", 0));
        groupBox->setTitle(QApplication::translate("DefaultAppProcDialog", "\351\234\200\350\246\201\350\277\234\347\250\213\345\220\257\345\212\250\347\250\213\345\272\217", 0));
        QTableWidgetItem *___qtablewidgetitem = appTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("DefaultAppProcDialog", "\347\250\213\345\272\217\350\267\257\345\276\204\345\222\214\345\220\215\347\247\260", 0));
        QTableWidgetItem *___qtablewidgetitem1 = appTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("DefaultAppProcDialog", "\345\217\202\346\225\260", 0));
        QTableWidgetItem *___qtablewidgetitem2 = appTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("DefaultAppProcDialog", "\350\277\233\347\250\213\345\220\215", 0));
        QTableWidgetItem *___qtablewidgetitem3 = appTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("DefaultAppProcDialog", "\345\205\201\350\256\270\345\244\232\345\256\236\344\276\213", 0));
        label->setText(QApplication::translate("DefaultAppProcDialog", "\347\250\213\345\272\217\357\274\232", 0));
#ifndef QT_NO_TOOLTIP
        appLineEdit->setToolTip(QApplication::translate("DefaultAppProcDialog", "\350\276\223\345\205\245\351\234\200\350\246\201\350\277\234\347\250\213\345\220\257\345\212\250\347\232\204\345\272\224\347\224\250\347\250\213\345\272\217\345\256\214\346\225\264\350\267\257\345\276\204\345\222\214\345\220\215\347\247\260", 0));
#endif // QT_NO_TOOLTIP
        appLineEdit->setPlaceholderText(QApplication::translate("DefaultAppProcDialog", "\345\256\214\346\225\264\350\267\257\345\276\204\345\222\214\345\220\215\347\247\260", 0));
#ifndef QT_NO_TOOLTIP
        argumentLineEdit->setToolTip(QApplication::translate("DefaultAppProcDialog", "\350\276\223\345\205\245\347\250\213\345\272\217\351\234\200\350\246\201\347\232\204\351\242\235\345\244\226\345\217\202\346\225\260", 0));
#endif // QT_NO_TOOLTIP
        argumentLineEdit->setPlaceholderText(QApplication::translate("DefaultAppProcDialog", "\345\217\202\346\225\260", 0));
#ifndef QT_NO_TOOLTIP
        appProcNameLineEdit->setToolTip(QApplication::translate("DefaultAppProcDialog", "\350\276\223\345\205\245\345\257\271\345\272\224\347\232\204\350\277\233\347\250\213\345\220\215\347\247\260,\344\273\245\344\276\277\347\233\221\346\216\247\345\205\266\347\212\266\346\200\201", 0));
#endif // QT_NO_TOOLTIP
        appProcNameLineEdit->setPlaceholderText(QApplication::translate("DefaultAppProcDialog", "\350\277\233\347\250\213\345\220\215,\344\273\245\344\276\277\347\233\221\346\216\247\345\205\266\347\212\266\346\200\201", 0));
#ifndef QT_NO_TOOLTIP
        allowMultiInstanceCheckBox->setToolTip(QApplication::translate("DefaultAppProcDialog", "\346\230\257\345\220\246\345\205\201\350\256\270\350\277\220\350\241\214\347\250\213\345\272\217\347\232\204\345\244\232\344\270\252\345\256\236\344\276\213", 0));
#endif // QT_NO_TOOLTIP
        allowMultiInstanceCheckBox->setText(QApplication::translate("DefaultAppProcDialog", "\345\205\201\350\256\270\345\244\232\345\256\236\344\276\213", 0));
        addAppButton->setText(QApplication::translate("DefaultAppProcDialog", "\346\267\273\345\212\240", 0));
        removeAppPushButton->setText(QApplication::translate("DefaultAppProcDialog", "\345\210\240\351\231\244", 0));
        groupBox_2->setTitle(QApplication::translate("DefaultAppProcDialog", "\351\234\200\350\246\201\347\233\221\350\247\206\350\277\233\347\250\213", 0));
        label_2->setText(QApplication::translate("DefaultAppProcDialog", "\350\277\233\347\250\213\357\274\232", 0));
        procLineEdit->setPlaceholderText(QApplication::translate("DefaultAppProcDialog", "\350\276\223\345\205\245\351\234\200\350\246\201\347\233\221\350\247\206\350\277\233\347\250\213\345\220\215\347\247\260", 0));
        addProcButton->setText(QApplication::translate("DefaultAppProcDialog", "\346\267\273\345\212\240", 0));
        removeProcPushButton->setText(QApplication::translate("DefaultAppProcDialog", "\345\210\240\351\231\244", 0));
        OKPushButton->setText(QApplication::translate("DefaultAppProcDialog", "\347\241\256\345\256\232(&O)", 0));
        canclePushButton->setText(QApplication::translate("DefaultAppProcDialog", "\345\217\226\346\266\210(&C)", 0));
    } // retranslateUi

};

namespace Ui {
    class DefaultAppProcDialog: public Ui_DefaultAppProcDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEFAULTAPPPROCDIALOG_H
