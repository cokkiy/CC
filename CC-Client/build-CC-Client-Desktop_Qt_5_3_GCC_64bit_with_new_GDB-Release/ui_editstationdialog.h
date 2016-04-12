/********************************************************************************
** Form generated from reading UI file 'editstationdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITSTATIONDIALOG_H
#define UI_EDITSTATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
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

class Ui_EditStationDialog
{
public:
    QHBoxLayout *horizontalLayout_11;
    QVBoxLayout *verticalLayout_8;
    QVBoxLayout *niVerticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label;
    QLineEdit *nameLineEdit;
    QSpacerItem *horizontalSpacer_8;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *NIMACLineEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *NIIPLineEdit;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_9;
    QPushButton *addNiPushButton;
    QPushButton *removeNIPushButton;
    QSpacerItem *horizontalSpacer_10;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *OKPushButton;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *CancelPushButton;
    QSpacerItem *horizontalSpacer_7;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *startAppTableWidget;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *appNameLineEdit;
    QLineEdit *argumentLineEdit;
    QLineEdit *appProcNameLineEdit;
    QCheckBox *allowMultiInstanceCheckBox;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_11;
    QPushButton *addStartAppPushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *removeStartAppPushButton;
    QSpacerItem *horizontalSpacer_12;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_5;
    QListWidget *monitorListWidget;
    QHBoxLayout *horizontalLayout_9;
    QLineEdit *procNameLineEdit;
    QPushButton *addMonitorProcPushButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *removeMonitorProcPushButton;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *viewProcPushButton;

    void setupUi(QDialog *EditStationDialog)
    {
        if (EditStationDialog->objectName().isEmpty())
            EditStationDialog->setObjectName(QStringLiteral("EditStationDialog"));
        EditStationDialog->resize(870, 528);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/52design.com_alth_109.png"), QSize(), QIcon::Normal, QIcon::Off);
        EditStationDialog->setWindowIcon(icon);
        horizontalLayout_11 = new QHBoxLayout(EditStationDialog);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        niVerticalLayout = new QVBoxLayout();
        niVerticalLayout->setSpacing(6);
        niVerticalLayout->setObjectName(QStringLiteral("niVerticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer_4 = new QSpacerItem(8, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        label = new QLabel(EditStationDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        nameLineEdit = new QLineEdit(EditStationDialog);
        nameLineEdit->setObjectName(QStringLiteral("nameLineEdit"));

        horizontalLayout->addWidget(nameLineEdit);

        horizontalSpacer_8 = new QSpacerItem(9, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_8);


        niVerticalLayout->addLayout(horizontalLayout);

        groupBox = new QGroupBox(EditStationDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        NIMACLineEdit = new QLineEdit(groupBox);
        NIMACLineEdit->setObjectName(QStringLiteral("NIMACLineEdit"));

        horizontalLayout_2->addWidget(NIMACLineEdit);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        NIIPLineEdit = new QLineEdit(groupBox);
        NIIPLineEdit->setObjectName(QStringLiteral("NIIPLineEdit"));

        horizontalLayout_3->addWidget(NIIPLineEdit);


        verticalLayout->addLayout(horizontalLayout_3);


        niVerticalLayout->addWidget(groupBox);

        frame = new QFrame(EditStationDialog);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer_9 = new QSpacerItem(104, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_9);

        addNiPushButton = new QPushButton(frame);
        addNiPushButton->setObjectName(QStringLiteral("addNiPushButton"));

        horizontalLayout_4->addWidget(addNiPushButton);

        removeNIPushButton = new QPushButton(frame);
        removeNIPushButton->setObjectName(QStringLiteral("removeNIPushButton"));

        horizontalLayout_4->addWidget(removeNIPushButton);

        horizontalSpacer_10 = new QSpacerItem(104, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_10);


        niVerticalLayout->addWidget(frame);


        verticalLayout_8->addLayout(niVerticalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_6);

        OKPushButton = new QPushButton(EditStationDialog);
        OKPushButton->setObjectName(QStringLiteral("OKPushButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(OKPushButton->sizePolicy().hasHeightForWidth());
        OKPushButton->setSizePolicy(sizePolicy1);
        OKPushButton->setDefault(true);

        horizontalLayout_10->addWidget(OKPushButton);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_5);

        CancelPushButton = new QPushButton(EditStationDialog);
        CancelPushButton->setObjectName(QStringLiteral("CancelPushButton"));
        sizePolicy1.setHeightForWidth(CancelPushButton->sizePolicy().hasHeightForWidth());
        CancelPushButton->setSizePolicy(sizePolicy1);

        horizontalLayout_10->addWidget(CancelPushButton);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_7);


        verticalLayout_8->addLayout(horizontalLayout_10);


        horizontalLayout_11->addLayout(verticalLayout_8);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        groupBox_4 = new QGroupBox(EditStationDialog);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        verticalLayout_2 = new QVBoxLayout(groupBox_4);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        startAppTableWidget = new QTableWidget(groupBox_4);
        if (startAppTableWidget->columnCount() < 4)
            startAppTableWidget->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        startAppTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        startAppTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        startAppTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        startAppTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        startAppTableWidget->setObjectName(QStringLiteral("startAppTableWidget"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(startAppTableWidget->sizePolicy().hasHeightForWidth());
        startAppTableWidget->setSizePolicy(sizePolicy2);
        startAppTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        startAppTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        startAppTableWidget->verticalHeader()->setVisible(false);

        verticalLayout_2->addWidget(startAppTableWidget);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        appNameLineEdit = new QLineEdit(groupBox_4);
        appNameLineEdit->setObjectName(QStringLiteral("appNameLineEdit"));

        horizontalLayout_6->addWidget(appNameLineEdit);

        argumentLineEdit = new QLineEdit(groupBox_4);
        argumentLineEdit->setObjectName(QStringLiteral("argumentLineEdit"));

        horizontalLayout_6->addWidget(argumentLineEdit);

        appProcNameLineEdit = new QLineEdit(groupBox_4);
        appProcNameLineEdit->setObjectName(QStringLiteral("appProcNameLineEdit"));
        appProcNameLineEdit->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_6->addWidget(appProcNameLineEdit);

        allowMultiInstanceCheckBox = new QCheckBox(groupBox_4);
        allowMultiInstanceCheckBox->setObjectName(QStringLiteral("allowMultiInstanceCheckBox"));

        horizontalLayout_6->addWidget(allowMultiInstanceCheckBox);


        verticalLayout_2->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_11);

        addStartAppPushButton = new QPushButton(groupBox_4);
        addStartAppPushButton->setObjectName(QStringLiteral("addStartAppPushButton"));

        horizontalLayout_5->addWidget(addStartAppPushButton);

        horizontalSpacer = new QSpacerItem(13, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        removeStartAppPushButton = new QPushButton(groupBox_4);
        removeStartAppPushButton->setObjectName(QStringLiteral("removeStartAppPushButton"));

        horizontalLayout_5->addWidget(removeStartAppPushButton);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_12);


        verticalLayout_2->addLayout(horizontalLayout_5);


        verticalLayout_7->addWidget(groupBox_4);

        groupBox_5 = new QGroupBox(EditStationDialog);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        verticalLayout_5 = new QVBoxLayout(groupBox_5);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        monitorListWidget = new QListWidget(groupBox_5);
        monitorListWidget->setObjectName(QStringLiteral("monitorListWidget"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(monitorListWidget->sizePolicy().hasHeightForWidth());
        monitorListWidget->setSizePolicy(sizePolicy3);

        verticalLayout_5->addWidget(monitorListWidget);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        procNameLineEdit = new QLineEdit(groupBox_5);
        procNameLineEdit->setObjectName(QStringLiteral("procNameLineEdit"));

        horizontalLayout_9->addWidget(procNameLineEdit);

        addMonitorProcPushButton = new QPushButton(groupBox_5);
        addMonitorProcPushButton->setObjectName(QStringLiteral("addMonitorProcPushButton"));

        horizontalLayout_9->addWidget(addMonitorProcPushButton);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_2);

        removeMonitorProcPushButton = new QPushButton(groupBox_5);
        removeMonitorProcPushButton->setObjectName(QStringLiteral("removeMonitorProcPushButton"));

        horizontalLayout_9->addWidget(removeMonitorProcPushButton);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_3);

        viewProcPushButton = new QPushButton(groupBox_5);
        viewProcPushButton->setObjectName(QStringLiteral("viewProcPushButton"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(viewProcPushButton->sizePolicy().hasHeightForWidth());
        viewProcPushButton->setSizePolicy(sizePolicy4);

        horizontalLayout_9->addWidget(viewProcPushButton);


        verticalLayout_5->addLayout(horizontalLayout_9);


        verticalLayout_7->addWidget(groupBox_5);


        horizontalLayout_11->addLayout(verticalLayout_7);

        QWidget::setTabOrder(nameLineEdit, NIMACLineEdit);
        QWidget::setTabOrder(NIMACLineEdit, NIIPLineEdit);
        QWidget::setTabOrder(NIIPLineEdit, addNiPushButton);
        QWidget::setTabOrder(addNiPushButton, removeNIPushButton);
        QWidget::setTabOrder(removeNIPushButton, startAppTableWidget);
        QWidget::setTabOrder(startAppTableWidget, appNameLineEdit);
        QWidget::setTabOrder(appNameLineEdit, argumentLineEdit);
        QWidget::setTabOrder(argumentLineEdit, appProcNameLineEdit);
        QWidget::setTabOrder(appProcNameLineEdit, removeStartAppPushButton);
        QWidget::setTabOrder(removeStartAppPushButton, monitorListWidget);
        QWidget::setTabOrder(monitorListWidget, procNameLineEdit);
        QWidget::setTabOrder(procNameLineEdit, addMonitorProcPushButton);
        QWidget::setTabOrder(addMonitorProcPushButton, removeMonitorProcPushButton);
        QWidget::setTabOrder(removeMonitorProcPushButton, viewProcPushButton);
        QWidget::setTabOrder(viewProcPushButton, OKPushButton);
        QWidget::setTabOrder(OKPushButton, CancelPushButton);

        retranslateUi(EditStationDialog);

        QMetaObject::connectSlotsByName(EditStationDialog);
    } // setupUi

    void retranslateUi(QDialog *EditStationDialog)
    {
        EditStationDialog->setWindowTitle(QApplication::translate("EditStationDialog", "\346\226\260\345\273\272\345\267\245\344\275\234\347\253\231", 0));
        label->setText(QApplication::translate("EditStationDialog", "\345\267\245\344\275\234\347\253\231\345\220\215\347\247\260\357\274\232", 0));
        groupBox->setTitle(QApplication::translate("EditStationDialog", "\347\275\221\345\215\241\344\277\241\346\201\257", 0));
        label_2->setText(QApplication::translate("EditStationDialog", "MAC\345\234\260\345\235\200\357\274\232", 0));
        label_3->setText(QApplication::translate("EditStationDialog", "IP\357\274\210\345\244\232\344\270\252IP\347\224\250\351\200\227\345\217\267\345\210\206\351\232\224)", 0));
        addNiPushButton->setText(QApplication::translate("EditStationDialog", "\345\242\236\345\212\240\347\275\221\345\215\241\351\205\215\347\275\256", 0));
        removeNIPushButton->setText(QApplication::translate("EditStationDialog", "\345\210\240\351\231\244\347\275\221\345\215\241\351\205\215\347\275\256", 0));
        OKPushButton->setText(QApplication::translate("EditStationDialog", "\347\241\256\345\256\232(&O)", 0));
        CancelPushButton->setText(QApplication::translate("EditStationDialog", "\345\217\226\346\266\210(&C)", 0));
        groupBox_4->setTitle(QApplication::translate("EditStationDialog", "\351\234\200\350\246\201\350\277\234\347\250\213\345\220\257\345\212\250\347\250\213\345\272\217\345\210\227\350\241\250(\350\276\223\345\205\245\345\256\214\346\225\264\350\267\257\345\276\204\345\222\214\345\217\202\346\225\260)", 0));
        QTableWidgetItem *___qtablewidgetitem = startAppTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("EditStationDialog", "\347\250\213\345\272\217\350\267\257\345\276\204\345\222\214\345\220\215\347\247\260", 0));
        QTableWidgetItem *___qtablewidgetitem1 = startAppTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("EditStationDialog", "\345\217\202\346\225\260", 0));
        QTableWidgetItem *___qtablewidgetitem2 = startAppTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("EditStationDialog", "\350\277\233\347\250\213\345\220\215", 0));
        QTableWidgetItem *___qtablewidgetitem3 = startAppTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("EditStationDialog", "\345\205\201\350\256\270\345\244\232\345\256\236\344\276\213", 0));
#ifndef QT_NO_TOOLTIP
        appNameLineEdit->setToolTip(QApplication::translate("EditStationDialog", "\350\276\223\345\205\245\351\234\200\350\246\201\350\277\234\347\250\213\345\220\257\345\212\250\347\232\204\345\272\224\347\224\250\347\250\213\345\272\217\345\256\214\346\225\264\350\267\257\345\276\204\345\222\214\345\220\215\347\247\260", 0));
#endif // QT_NO_TOOLTIP
        appNameLineEdit->setPlaceholderText(QApplication::translate("EditStationDialog", "\345\256\214\346\225\264\350\267\257\345\276\204\345\222\214\345\220\215\347\247\260", 0));
#ifndef QT_NO_TOOLTIP
        argumentLineEdit->setToolTip(QApplication::translate("EditStationDialog", "\350\276\223\345\205\245\347\250\213\345\272\217\351\234\200\350\246\201\347\232\204\345\217\202\346\225\260", 0));
#endif // QT_NO_TOOLTIP
        argumentLineEdit->setPlaceholderText(QApplication::translate("EditStationDialog", "\345\217\202\346\225\260", 0));
#ifndef QT_NO_TOOLTIP
        appProcNameLineEdit->setToolTip(QApplication::translate("EditStationDialog", "\350\276\223\345\205\245\345\257\271\345\272\224\347\232\204\350\277\233\347\250\213\345\220\215\347\247\260,\344\273\245\344\276\277\347\233\221\346\216\247\345\205\266\347\212\266\346\200\201", 0));
#endif // QT_NO_TOOLTIP
        appProcNameLineEdit->setPlaceholderText(QApplication::translate("EditStationDialog", "\350\277\233\347\250\213\345\220\215", 0));
#ifndef QT_NO_TOOLTIP
        allowMultiInstanceCheckBox->setToolTip(QApplication::translate("EditStationDialog", "\346\230\257\345\220\246\345\205\201\350\256\270\350\277\220\350\241\214\347\250\213\345\272\217\347\232\204\345\244\232\344\270\252\345\256\236\344\276\213", 0));
#endif // QT_NO_TOOLTIP
        allowMultiInstanceCheckBox->setText(QApplication::translate("EditStationDialog", "\345\205\201\350\256\270\345\244\232\345\256\236\344\276\213", 0));
        addStartAppPushButton->setText(QApplication::translate("EditStationDialog", "\346\267\273\345\212\240(&A)", 0));
        removeStartAppPushButton->setText(QApplication::translate("EditStationDialog", "\345\210\240\351\231\244", 0));
        groupBox_5->setTitle(QApplication::translate("EditStationDialog", "\351\234\200\350\246\201\347\233\221\350\247\206\350\277\233\347\250\213\345\210\227\350\241\250(\351\234\200\350\246\201\345\207\206\347\241\256\350\277\233\347\250\213\345\220\215\347\247\260)", 0));
        procNameLineEdit->setPlaceholderText(QApplication::translate("EditStationDialog", "\350\276\223\345\205\245\351\234\200\350\246\201\347\233\221\350\247\206\347\232\204\350\277\233\347\250\213\345\220\215\347\247\260", 0));
        addMonitorProcPushButton->setText(QApplication::translate("EditStationDialog", "\346\267\273\345\212\240(&M)", 0));
        removeMonitorProcPushButton->setText(QApplication::translate("EditStationDialog", "\345\210\240\351\231\244", 0));
        viewProcPushButton->setText(QApplication::translate("EditStationDialog", "\346\237\245\347\234\213\350\277\233\347\250\213(&V)", 0));
    } // retranslateUi

};

namespace Ui {
    class EditStationDialog: public Ui_EditStationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITSTATIONDIALOG_H
