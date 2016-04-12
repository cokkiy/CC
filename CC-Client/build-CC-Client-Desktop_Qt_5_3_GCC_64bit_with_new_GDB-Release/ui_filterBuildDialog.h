/********************************************************************************
** Form generated from reading UI file 'filterBuildDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILTERBUILDDIALOG_H
#define UI_FILTERBUILDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FilterBuildDialog
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QComboBox *propertyComboBox;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_3;
    QComboBox *opComboBox;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_4;
    QLineEdit *v1lineEdit;
    QComboBox *stateComboBox;
    QSpacerItem *horizontalSpacer_4;
    QLineEdit *v2lineEdit;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QComboBox *combineComboBox;
    QSpacerItem *horizontalSpacer_7;
    QToolButton *addToolButton;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *clearButton;
    QTextEdit *filterTextEdit;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *FilterBuildDialog)
    {
        if (FilterBuildDialog->objectName().isEmpty())
            FilterBuildDialog->setObjectName(QStringLiteral("FilterBuildDialog"));
        FilterBuildDialog->resize(617, 250);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        FilterBuildDialog->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/SCI_MATHfa_3.ico"), QSize(), QIcon::Normal, QIcon::Off);
        FilterBuildDialog->setWindowIcon(icon);
        horizontalLayout_2 = new QHBoxLayout(FilterBuildDialog);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(2, 2, 2, 9);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(FilterBuildDialog);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(label_2);

        propertyComboBox = new QComboBox(frame);
        propertyComboBox->setObjectName(QStringLiteral("propertyComboBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(propertyComboBox->sizePolicy().hasHeightForWidth());
        propertyComboBox->setSizePolicy(sizePolicy2);

        horizontalLayout_4->addWidget(propertyComboBox);

        horizontalSpacer_3 = new QSpacerItem(13, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(label_3);

        opComboBox = new QComboBox(frame);
        opComboBox->setObjectName(QStringLiteral("opComboBox"));
        sizePolicy2.setHeightForWidth(opComboBox->sizePolicy().hasHeightForWidth());
        opComboBox->setSizePolicy(sizePolicy2);
        opComboBox->setMinimumSize(QSize(120, 0));

        horizontalLayout_4->addWidget(opComboBox);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_5->addWidget(label_4);

        v1lineEdit = new QLineEdit(frame);
        v1lineEdit->setObjectName(QStringLiteral("v1lineEdit"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(v1lineEdit->sizePolicy().hasHeightForWidth());
        v1lineEdit->setSizePolicy(sizePolicy3);
        v1lineEdit->setClearButtonEnabled(true);

        horizontalLayout_5->addWidget(v1lineEdit);

        stateComboBox = new QComboBox(frame);
        stateComboBox->setObjectName(QStringLiteral("stateComboBox"));

        horizontalLayout_5->addWidget(stateComboBox);

        horizontalSpacer_4 = new QSpacerItem(13, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        v2lineEdit = new QLineEdit(frame);
        v2lineEdit->setObjectName(QStringLiteral("v2lineEdit"));
        v2lineEdit->setEnabled(false);
        sizePolicy3.setHeightForWidth(v2lineEdit->sizePolicy().hasHeightForWidth());
        v2lineEdit->setSizePolicy(sizePolicy3);
        v2lineEdit->setReadOnly(false);
        v2lineEdit->setClearButtonEnabled(true);

        horizontalLayout_5->addWidget(v2lineEdit);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_3->addWidget(label_5);

        combineComboBox = new QComboBox(frame);
        combineComboBox->setObjectName(QStringLiteral("combineComboBox"));
        combineComboBox->setEnabled(false);

        horizontalLayout_3->addWidget(combineComboBox);

        horizontalSpacer_7 = new QSpacerItem(13, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_7);

        addToolButton = new QToolButton(frame);
        addToolButton->setObjectName(QStringLiteral("addToolButton"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(addToolButton->sizePolicy().hasHeightForWidth());
        addToolButton->setSizePolicy(sizePolicy4);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Icons/people_officee_o.ico"), QSize(), QIcon::Normal, QIcon::Off);
        addToolButton->setIcon(icon1);

        horizontalLayout_3->addWidget(addToolButton);

        horizontalSpacer_8 = new QSpacerItem(208, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_8);

        clearButton = new QPushButton(frame);
        clearButton->setObjectName(QStringLiteral("clearButton"));
        sizePolicy3.setHeightForWidth(clearButton->sizePolicy().hasHeightForWidth());
        clearButton->setSizePolicy(sizePolicy3);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Icons/sign046.ico"), QSize(), QIcon::Normal, QIcon::Off);
        clearButton->setIcon(icon2);

        horizontalLayout_3->addWidget(clearButton);


        verticalLayout_2->addLayout(horizontalLayout_3);

        filterTextEdit = new QTextEdit(frame);
        filterTextEdit->setObjectName(QStringLiteral("filterTextEdit"));
        filterTextEdit->setReadOnly(false);

        verticalLayout_2->addWidget(filterTextEdit);


        verticalLayout->addWidget(frame);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);

        okButton = new QPushButton(FilterBuildDialog);
        okButton->setObjectName(QStringLiteral("okButton"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Icons/png-0044.png"), QSize(), QIcon::Normal, QIcon::Off);
        okButton->setIcon(icon3);

        horizontalLayout->addWidget(okButton);

        horizontalSpacer_2 = new QSpacerItem(18, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        cancelButton = new QPushButton(FilterBuildDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        horizontalSpacer = new QSpacerItem(78, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(FilterBuildDialog);
        QObject::connect(okButton, SIGNAL(clicked()), FilterBuildDialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), FilterBuildDialog, SLOT(reject()));
        QObject::connect(clearButton, SIGNAL(clicked()), filterTextEdit, SLOT(clear()));

        QMetaObject::connectSlotsByName(FilterBuildDialog);
    } // setupUi

    void retranslateUi(QDialog *FilterBuildDialog)
    {
        FilterBuildDialog->setWindowTitle(QApplication::translate("FilterBuildDialog", "\345\210\233\345\273\272\350\277\207\346\273\244\346\235\241\344\273\266", 0));
        label_2->setText(QApplication::translate("FilterBuildDialog", "\351\200\211\346\213\251\345\261\236\346\200\247\357\274\232", 0));
        propertyComboBox->clear();
        propertyComboBox->insertItems(0, QStringList()
         << QApplication::translate("FilterBuildDialog", "IP", 0)
         << QApplication::translate("FilterBuildDialog", "State", 0)
         << QApplication::translate("FilterBuildDialog", "Name", 0)
        );
        label_3->setText(QApplication::translate("FilterBuildDialog", "\351\200\211\346\213\251\346\235\241\344\273\266\357\274\232", 0));
        opComboBox->clear();
        opComboBox->insertItems(0, QStringList()
         << QApplication::translate("FilterBuildDialog", "=", 0)
         << QApplication::translate("FilterBuildDialog", ">=", 0)
         << QApplication::translate("FilterBuildDialog", "<=", 0)
         << QApplication::translate("FilterBuildDialog", "Between", 0)
        );
        label_4->setText(QApplication::translate("FilterBuildDialog", "\345\200\274\357\274\232", 0));
        v1lineEdit->setPlaceholderText(QApplication::translate("FilterBuildDialog", "\346\235\241\344\273\2661", 0));
        stateComboBox->clear();
        stateComboBox->insertItems(0, QStringList()
         << QApplication::translate("FilterBuildDialog", "Unknown", 0)
         << QApplication::translate("FilterBuildDialog", "Normal", 0)
         << QApplication::translate("FilterBuildDialog", "Warning", 0)
         << QApplication::translate("FilterBuildDialog", "Error", 0)
         << QApplication::translate("FilterBuildDialog", "Powering", 0)
         << QApplication::translate("FilterBuildDialog", "AppStarting", 0)
         << QApplication::translate("FilterBuildDialog", "Shutdowning", 0)
         << QApplication::translate("FilterBuildDialog", "Rebooting", 0)
         << QApplication::translate("FilterBuildDialog", "PowerOnFailure", 0)
         << QApplication::translate("FilterBuildDialog", "RebootFailure", 0)
         << QApplication::translate("FilterBuildDialog", "AppStartFailure", 0)
         << QApplication::translate("FilterBuildDialog", "ShutdownFailure", 0)
        );
        v2lineEdit->setPlaceholderText(QApplication::translate("FilterBuildDialog", "\346\235\241\344\273\2662,\351\200\211\346\213\251\344\273\213\344\272\216\346\227\266\346\234\211\346\225\210", 0));
        label_5->setText(QApplication::translate("FilterBuildDialog", "\345\205\263\347\263\273\357\274\232", 0));
        combineComboBox->clear();
        combineComboBox->insertItems(0, QStringList()
         << QApplication::translate("FilterBuildDialog", "and", 0)
         << QApplication::translate("FilterBuildDialog", "or", 0)
        );
#ifndef QT_NO_TOOLTIP
        addToolButton->setToolTip(QApplication::translate("FilterBuildDialog", "\346\267\273\345\212\240\346\235\241\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        addToolButton->setText(QApplication::translate("FilterBuildDialog", "...", 0));
#ifndef QT_NO_TOOLTIP
        clearButton->setToolTip(QApplication::translate("FilterBuildDialog", "\346\270\205\347\251\272\345\275\223\345\211\215\346\235\241\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        clearButton->setText(QApplication::translate("FilterBuildDialog", "\346\270\205\347\251\272", 0));
        filterTextEdit->setPlaceholderText(QApplication::translate("FilterBuildDialog", "\350\277\207\346\273\244\346\235\241\344\273\266\346\230\276\347\244\272\345\234\250\350\277\231\345\204\277", 0));
        okButton->setText(QApplication::translate("FilterBuildDialog", "OK", 0));
        cancelButton->setText(QApplication::translate("FilterBuildDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class FilterBuildDialog: public Ui_FilterBuildDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILTERBUILDDIALOG_H
