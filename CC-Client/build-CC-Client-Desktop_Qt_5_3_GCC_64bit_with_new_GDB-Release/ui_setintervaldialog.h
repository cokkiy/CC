/********************************************************************************
** Form generated from reading UI file 'setintervaldialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETINTERVALDIALOG_H
#define UI_SETINTERVALDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SetIntervalDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *intervalSpinBox;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *okPushButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelPushButton;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QDialog *SetIntervalDialog)
    {
        if (SetIntervalDialog->objectName().isEmpty())
            SetIntervalDialog->setObjectName(QStringLiteral("SetIntervalDialog"));
        SetIntervalDialog->resize(312, 98);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/52design.com_alth_023.png"), QSize(), QIcon::Normal, QIcon::Off);
        SetIntervalDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(SetIntervalDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(SetIntervalDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        intervalSpinBox = new QSpinBox(frame);
        intervalSpinBox->setObjectName(QStringLiteral("intervalSpinBox"));
        intervalSpinBox->setMinimum(1);
        intervalSpinBox->setMaximum(100);

        horizontalLayout->addWidget(intervalSpinBox);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        verticalLayout->addWidget(frame);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        okPushButton = new QPushButton(SetIntervalDialog);
        okPushButton->setObjectName(QStringLiteral("okPushButton"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(okPushButton->sizePolicy().hasHeightForWidth());
        okPushButton->setSizePolicy(sizePolicy);
        okPushButton->setDefault(true);

        horizontalLayout_2->addWidget(okPushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        cancelPushButton = new QPushButton(SetIntervalDialog);
        cancelPushButton->setObjectName(QStringLiteral("cancelPushButton"));
        sizePolicy.setHeightForWidth(cancelPushButton->sizePolicy().hasHeightForWidth());
        cancelPushButton->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(cancelPushButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(SetIntervalDialog);

        QMetaObject::connectSlotsByName(SetIntervalDialog);
    } // setupUi

    void retranslateUi(QDialog *SetIntervalDialog)
    {
        SetIntervalDialog->setWindowTitle(QApplication::translate("SetIntervalDialog", "\350\256\276\347\275\256\347\233\221\346\216\247\351\227\264\351\232\224", 0));
        label->setText(QApplication::translate("SetIntervalDialog", "\347\233\221\350\247\206\351\227\264\351\232\224\357\274\210\347\247\222)\357\274\232", 0));
        okPushButton->setText(QApplication::translate("SetIntervalDialog", "\347\241\256\345\256\232(&O)", 0));
        cancelPushButton->setText(QApplication::translate("SetIntervalDialog", "\345\217\226\346\266\210(&C)", 0));
    } // retranslateUi

};

namespace Ui {
    class SetIntervalDialog: public Ui_SetIntervalDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETINTERVALDIALOG_H
