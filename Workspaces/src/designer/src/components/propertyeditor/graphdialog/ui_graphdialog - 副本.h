/********************************************************************************
** Form generated from reading UI file 'graphdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHDIALOG_H
#define UI_GRAPHDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_TargetAbout_target;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_insertnewGraph_target;
    QPushButton *pushButton_delchooseGraph_target;
    QPushButton *pushButton_leftmovegraph_target;
    QPushButton *pushButton_rightmovegraph_target;
    QSpacerItem *horizontalSpacer_1_target;
    QDialogButtonBox *buttonBox_OKorCancel_target;
    QSpacerItem *horizontalSpacer_2_target;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_AxisConfig_tab;
    QHBoxLayout *horizontalLayout_AxisConfig;
    QGroupBox *groupBox_xAxisConfig_tab;
    QGridLayout *gridLayout_xAxisConfig;
    QLabel *label_kedufanwei_x_tab;
    QLabel *label_Xmax_tab;
    QLineEdit *lineEdit_Xmax_tab;
    QSpacerItem *horizontalSpacer_1_tab;
    QLabel *label_Xmin_tab;
    QLineEdit *lineEdit_Xmin_tab;
    QLabel *label_wenbenpianyi_x_tab;
    QSpacerItem *horizontalSpacer__3_tab;
    QLineEdit *lineEdit_wenbenpianyi_x;
    QLabel *label_AxisLabel_x;
    QLineEdit *lineEdit_inputAxisLabel_x;
    QLabel *label_keduweizhi_x;
    QSpacerItem *horizontalSpacer_5_tab;
    QComboBox *comboBox_keduweizhi_x;
    QLabel *label_Axisdisplay_x;
    QCheckBox *checkBox_chooseAxisdisplay_x;
    QSpacerItem *horizontalSpacer_7_tab;
    QGroupBox *groupBox_yAxisConfig_tab;
    QGridLayout *gridLayout_yAxisconfig;
    QLabel *label_kedufanwei_y;
    QLabel *label_Ymax;
    QLineEdit *lineEdit_Ymax;
    QSpacerItem *horizontalSpacer_2_tab;
    QLabel *label_Ymin;
    QLineEdit *lineEdit_Ymin;
    QLabel *label_13_wenbenpianyi_y;
    QSpacerItem *horizontalSpacer_4_tab;
    QLineEdit *lineEdit_wenbenpianyi_y;
    QLabel *label_AxisLabel_y;
    QLineEdit *lineEdit_inputAxisLabel_y;
    QLabel *label_keduweizhi_y;
    QSpacerItem *horizontalSpacer_6_tab;
    QComboBox *comboBox_keduweizhi_y;
    QLabel *label_Axisdisplay_y;
    QCheckBox *checkBox_chooseAxisdisplay_y;
    QSpacerItem *horizontalSpacer_8_tab;
    QSpacerItem *verticalSpacer_9_tab;
    QGroupBox *groupBox_GraphConfig_tab;
    QHBoxLayout *horizontalLayout_Graphconfig;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_BaseCharacter_tab;
    QGridLayout *gridLayout;
    QLabel *label_GraphName_tab;
    QLineEdit *lineEdit_inputGraphName_tab;
    QLabel *label_GraphColor_tab;
    QLabel *label_GraphWidth_tab;
    QLineEdit *lineEdit_GraphWidth_tab;
    QLabel *label_GraphBuffer_tab;
    QLineEdit *lineEdit_GraphBuffer_tab;
    QLabel *label_chooseGraphColor;
    QPushButton *pushButton_chooseGraphColor_tab;
    QGroupBox *groupBox_Data_tab;
    QGridLayout *gridLayout_Data;
    QPushButton *pushButton_chooseXparam_tab;
    QLineEdit *lineEdit_inputXparam_tab;
    QLabel *label_Xparam_tab;
    QPushButton *pushButton_chooseT0_tab;
    QLabel *label_inputT0_tab;
    QPushButton *pushButton_chooseGraphbasemap_tab;
    QLineEdit *lineEdit_inputYparam_tab;
    QLabel *label_Yparam_tab;
    QPushButton *pushButton_chooseYparam_tab;

    void setupUi(QDialog *GraphDialog)
    {
        if (GraphDialog->objectName().isEmpty())
            GraphDialog->setObjectName(QStringLiteral("GraphDialog"));
        GraphDialog->resize(926, 531);
        GraphDialog->setStyleSheet(QStringLiteral("border-color: rgb(255, 170, 0);"));
        verticalLayout_2 = new QVBoxLayout(GraphDialog);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        groupBox_TargetAbout_target = new QGroupBox(GraphDialog);
        groupBox_TargetAbout_target->setObjectName(QStringLiteral("groupBox_TargetAbout_target"));
        horizontalLayout = new QHBoxLayout(groupBox_TargetAbout_target);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_insertnewGraph_target = new QPushButton(groupBox_TargetAbout_target);
        pushButton_insertnewGraph_target->setObjectName(QStringLiteral("pushButton_insertnewGraph_target"));

        horizontalLayout->addWidget(pushButton_insertnewGraph_target);

        pushButton_delchooseGraph_target = new QPushButton(groupBox_TargetAbout_target);
        pushButton_delchooseGraph_target->setObjectName(QStringLiteral("pushButton_delchooseGraph_target"));

        horizontalLayout->addWidget(pushButton_delchooseGraph_target);

        pushButton_leftmovegraph_target = new QPushButton(groupBox_TargetAbout_target);
        pushButton_leftmovegraph_target->setObjectName(QStringLiteral("pushButton_leftmovegraph_target"));

        horizontalLayout->addWidget(pushButton_leftmovegraph_target);

        pushButton_rightmovegraph_target = new QPushButton(groupBox_TargetAbout_target);
        pushButton_rightmovegraph_target->setObjectName(QStringLiteral("pushButton_rightmovegraph_target"));

        horizontalLayout->addWidget(pushButton_rightmovegraph_target);

        horizontalSpacer_1_target = new QSpacerItem(567, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_1_target);

        buttonBox_OKorCancel_target = new QDialogButtonBox(groupBox_TargetAbout_target);
        buttonBox_OKorCancel_target->setObjectName(QStringLiteral("buttonBox_OKorCancel_target"));
        buttonBox_OKorCancel_target->setLayoutDirection(Qt::LeftToRight);
        buttonBox_OKorCancel_target->setStyleSheet(QStringLiteral(""));
        buttonBox_OKorCancel_target->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox_OKorCancel_target);

        horizontalSpacer_2_target = new QSpacerItem(13, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2_target);


        verticalLayout_2->addWidget(groupBox_TargetAbout_target);

        tabWidget = new QTabWidget(GraphDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setMinimumSize(QSize(0, 21));
        tabWidget->setMaximumSize(QSize(881, 16777215));
        tabWidget->setStyleSheet(QStringLiteral("selection-color: rgb(255, 85, 0);"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tabWidget->addTab(tab, QString());

        verticalLayout_2->addWidget(tabWidget);

        widget = new QWidget(GraphDialog);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox_AxisConfig_tab = new QGroupBox(widget);
        groupBox_AxisConfig_tab->setObjectName(QStringLiteral("groupBox_AxisConfig_tab"));
        horizontalLayout_AxisConfig = new QHBoxLayout(groupBox_AxisConfig_tab);
        horizontalLayout_AxisConfig->setSpacing(6);
        horizontalLayout_AxisConfig->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_AxisConfig->setObjectName(QStringLiteral("horizontalLayout_AxisConfig"));
        groupBox_xAxisConfig_tab = new QGroupBox(groupBox_AxisConfig_tab);
        groupBox_xAxisConfig_tab->setObjectName(QStringLiteral("groupBox_xAxisConfig_tab"));
        gridLayout_xAxisConfig = new QGridLayout(groupBox_xAxisConfig_tab);
        gridLayout_xAxisConfig->setSpacing(6);
        gridLayout_xAxisConfig->setContentsMargins(11, 11, 11, 11);
        gridLayout_xAxisConfig->setObjectName(QStringLiteral("gridLayout_xAxisConfig"));
        label_kedufanwei_x_tab = new QLabel(groupBox_xAxisConfig_tab);
        label_kedufanwei_x_tab->setObjectName(QStringLiteral("label_kedufanwei_x_tab"));

        gridLayout_xAxisConfig->addWidget(label_kedufanwei_x_tab, 0, 0, 1, 1);

        label_Xmax_tab = new QLabel(groupBox_xAxisConfig_tab);
        label_Xmax_tab->setObjectName(QStringLiteral("label_Xmax_tab"));

        gridLayout_xAxisConfig->addWidget(label_Xmax_tab, 0, 1, 1, 1);

        lineEdit_Xmax_tab = new QLineEdit(groupBox_xAxisConfig_tab);
        lineEdit_Xmax_tab->setObjectName(QStringLiteral("lineEdit_Xmax_tab"));

        gridLayout_xAxisConfig->addWidget(lineEdit_Xmax_tab, 0, 2, 1, 2);

        horizontalSpacer_1_tab = new QSpacerItem(48, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_xAxisConfig->addItem(horizontalSpacer_1_tab, 1, 0, 1, 1);

        label_Xmin_tab = new QLabel(groupBox_xAxisConfig_tab);
        label_Xmin_tab->setObjectName(QStringLiteral("label_Xmin_tab"));

        gridLayout_xAxisConfig->addWidget(label_Xmin_tab, 1, 1, 1, 1);

        lineEdit_Xmin_tab = new QLineEdit(groupBox_xAxisConfig_tab);
        lineEdit_Xmin_tab->setObjectName(QStringLiteral("lineEdit_Xmin_tab"));

        gridLayout_xAxisConfig->addWidget(lineEdit_Xmin_tab, 1, 2, 1, 2);

        label_wenbenpianyi_x_tab = new QLabel(groupBox_xAxisConfig_tab);
        label_wenbenpianyi_x_tab->setObjectName(QStringLiteral("label_wenbenpianyi_x_tab"));

        gridLayout_xAxisConfig->addWidget(label_wenbenpianyi_x_tab, 2, 0, 1, 1);

        horizontalSpacer__3_tab = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_xAxisConfig->addItem(horizontalSpacer__3_tab, 2, 1, 1, 1);

        lineEdit_wenbenpianyi_x = new QLineEdit(groupBox_xAxisConfig_tab);
        lineEdit_wenbenpianyi_x->setObjectName(QStringLiteral("lineEdit_wenbenpianyi_x"));

        gridLayout_xAxisConfig->addWidget(lineEdit_wenbenpianyi_x, 2, 2, 1, 2);

        label_AxisLabel_x = new QLabel(groupBox_xAxisConfig_tab);
        label_AxisLabel_x->setObjectName(QStringLiteral("label_AxisLabel_x"));

        gridLayout_xAxisConfig->addWidget(label_AxisLabel_x, 3, 0, 1, 2);

        lineEdit_inputAxisLabel_x = new QLineEdit(groupBox_xAxisConfig_tab);
        lineEdit_inputAxisLabel_x->setObjectName(QStringLiteral("lineEdit_inputAxisLabel_x"));

        gridLayout_xAxisConfig->addWidget(lineEdit_inputAxisLabel_x, 3, 2, 1, 2);

        label_keduweizhi_x = new QLabel(groupBox_xAxisConfig_tab);
        label_keduweizhi_x->setObjectName(QStringLiteral("label_keduweizhi_x"));

        gridLayout_xAxisConfig->addWidget(label_keduweizhi_x, 4, 0, 1, 1);

        horizontalSpacer_5_tab = new QSpacerItem(48, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_xAxisConfig->addItem(horizontalSpacer_5_tab, 4, 1, 1, 1);

        comboBox_keduweizhi_x = new QComboBox(groupBox_xAxisConfig_tab);
        comboBox_keduweizhi_x->setObjectName(QStringLiteral("comboBox_keduweizhi_x"));

        gridLayout_xAxisConfig->addWidget(comboBox_keduweizhi_x, 4, 2, 1, 1);

        label_Axisdisplay_x = new QLabel(groupBox_xAxisConfig_tab);
        label_Axisdisplay_x->setObjectName(QStringLiteral("label_Axisdisplay_x"));

        gridLayout_xAxisConfig->addWidget(label_Axisdisplay_x, 5, 0, 1, 2);

        checkBox_chooseAxisdisplay_x = new QCheckBox(groupBox_xAxisConfig_tab);
        checkBox_chooseAxisdisplay_x->setObjectName(QStringLiteral("checkBox_chooseAxisdisplay_x"));

        gridLayout_xAxisConfig->addWidget(checkBox_chooseAxisdisplay_x, 5, 2, 1, 1);

        horizontalSpacer_7_tab = new QSpacerItem(31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_xAxisConfig->addItem(horizontalSpacer_7_tab, 5, 3, 1, 1);


        horizontalLayout_AxisConfig->addWidget(groupBox_xAxisConfig_tab);

        groupBox_yAxisConfig_tab = new QGroupBox(groupBox_AxisConfig_tab);
        groupBox_yAxisConfig_tab->setObjectName(QStringLiteral("groupBox_yAxisConfig_tab"));
        gridLayout_yAxisconfig = new QGridLayout(groupBox_yAxisConfig_tab);
        gridLayout_yAxisconfig->setSpacing(6);
        gridLayout_yAxisconfig->setContentsMargins(11, 11, 11, 11);
        gridLayout_yAxisconfig->setObjectName(QStringLiteral("gridLayout_yAxisconfig"));
        label_kedufanwei_y = new QLabel(groupBox_yAxisConfig_tab);
        label_kedufanwei_y->setObjectName(QStringLiteral("label_kedufanwei_y"));

        gridLayout_yAxisconfig->addWidget(label_kedufanwei_y, 0, 0, 1, 1);

        label_Ymax = new QLabel(groupBox_yAxisConfig_tab);
        label_Ymax->setObjectName(QStringLiteral("label_Ymax"));

        gridLayout_yAxisconfig->addWidget(label_Ymax, 0, 1, 1, 1);

        lineEdit_Ymax = new QLineEdit(groupBox_yAxisConfig_tab);
        lineEdit_Ymax->setObjectName(QStringLiteral("lineEdit_Ymax"));

        gridLayout_yAxisconfig->addWidget(lineEdit_Ymax, 0, 2, 1, 2);

        horizontalSpacer_2_tab = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_yAxisconfig->addItem(horizontalSpacer_2_tab, 1, 0, 1, 1);

        label_Ymin = new QLabel(groupBox_yAxisConfig_tab);
        label_Ymin->setObjectName(QStringLiteral("label_Ymin"));

        gridLayout_yAxisconfig->addWidget(label_Ymin, 1, 1, 1, 1);

        lineEdit_Ymin = new QLineEdit(groupBox_yAxisConfig_tab);
        lineEdit_Ymin->setObjectName(QStringLiteral("lineEdit_Ymin"));

        gridLayout_yAxisconfig->addWidget(lineEdit_Ymin, 1, 2, 1, 2);

        label_13_wenbenpianyi_y = new QLabel(groupBox_yAxisConfig_tab);
        label_13_wenbenpianyi_y->setObjectName(QStringLiteral("label_13_wenbenpianyi_y"));

        gridLayout_yAxisconfig->addWidget(label_13_wenbenpianyi_y, 2, 0, 1, 1);

        horizontalSpacer_4_tab = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_yAxisconfig->addItem(horizontalSpacer_4_tab, 2, 1, 1, 1);

        lineEdit_wenbenpianyi_y = new QLineEdit(groupBox_yAxisConfig_tab);
        lineEdit_wenbenpianyi_y->setObjectName(QStringLiteral("lineEdit_wenbenpianyi_y"));

        gridLayout_yAxisconfig->addWidget(lineEdit_wenbenpianyi_y, 2, 2, 1, 2);

        label_AxisLabel_y = new QLabel(groupBox_yAxisConfig_tab);
        label_AxisLabel_y->setObjectName(QStringLiteral("label_AxisLabel_y"));

        gridLayout_yAxisconfig->addWidget(label_AxisLabel_y, 3, 0, 1, 2);

        lineEdit_inputAxisLabel_y = new QLineEdit(groupBox_yAxisConfig_tab);
        lineEdit_inputAxisLabel_y->setObjectName(QStringLiteral("lineEdit_inputAxisLabel_y"));

        gridLayout_yAxisconfig->addWidget(lineEdit_inputAxisLabel_y, 3, 2, 1, 2);

        label_keduweizhi_y = new QLabel(groupBox_yAxisConfig_tab);
        label_keduweizhi_y->setObjectName(QStringLiteral("label_keduweizhi_y"));

        gridLayout_yAxisconfig->addWidget(label_keduweizhi_y, 4, 0, 1, 1);

        horizontalSpacer_6_tab = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_yAxisconfig->addItem(horizontalSpacer_6_tab, 4, 1, 1, 1);

        comboBox_keduweizhi_y = new QComboBox(groupBox_yAxisConfig_tab);
        comboBox_keduweizhi_y->setObjectName(QStringLiteral("comboBox_keduweizhi_y"));

        gridLayout_yAxisconfig->addWidget(comboBox_keduweizhi_y, 4, 2, 1, 2);

        label_Axisdisplay_y = new QLabel(groupBox_yAxisConfig_tab);
        label_Axisdisplay_y->setObjectName(QStringLiteral("label_Axisdisplay_y"));

        gridLayout_yAxisconfig->addWidget(label_Axisdisplay_y, 5, 0, 1, 2);

        checkBox_chooseAxisdisplay_y = new QCheckBox(groupBox_yAxisConfig_tab);
        checkBox_chooseAxisdisplay_y->setObjectName(QStringLiteral("checkBox_chooseAxisdisplay_y"));

        gridLayout_yAxisconfig->addWidget(checkBox_chooseAxisdisplay_y, 5, 2, 1, 1);

        horizontalSpacer_8_tab = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_yAxisconfig->addItem(horizontalSpacer_8_tab, 5, 3, 1, 1);

        label_13_wenbenpianyi_y->raise();
        label_Ymax->raise();
        label_Axisdisplay_y->raise();
        label_AxisLabel_y->raise();
        label_Ymin->raise();
        label_kedufanwei_y->raise();
        label_keduweizhi_y->raise();
        lineEdit_Ymin->raise();
        lineEdit_wenbenpianyi_y->raise();
        lineEdit_inputAxisLabel_y->raise();
        comboBox_keduweizhi_y->raise();
        checkBox_chooseAxisdisplay_y->raise();
        lineEdit_Ymax->raise();

        horizontalLayout_AxisConfig->addWidget(groupBox_yAxisConfig_tab);


        verticalLayout->addWidget(groupBox_AxisConfig_tab);

        verticalSpacer_9_tab = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_9_tab);

        groupBox_GraphConfig_tab = new QGroupBox(widget);
        groupBox_GraphConfig_tab->setObjectName(QStringLiteral("groupBox_GraphConfig_tab"));
        groupBox_GraphConfig_tab->setMinimumSize(QSize(0, 181));
        groupBox_GraphConfig_tab->setMaximumSize(QSize(16777215, 181));
        horizontalLayout_Graphconfig = new QHBoxLayout(groupBox_GraphConfig_tab);
        horizontalLayout_Graphconfig->setSpacing(6);
        horizontalLayout_Graphconfig->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_Graphconfig->setObjectName(QStringLiteral("horizontalLayout_Graphconfig"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        groupBox_BaseCharacter_tab = new QGroupBox(groupBox_GraphConfig_tab);
        groupBox_BaseCharacter_tab->setObjectName(QStringLiteral("groupBox_BaseCharacter_tab"));
        gridLayout = new QGridLayout(groupBox_BaseCharacter_tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_GraphName_tab = new QLabel(groupBox_BaseCharacter_tab);
        label_GraphName_tab->setObjectName(QStringLiteral("label_GraphName_tab"));

        gridLayout->addWidget(label_GraphName_tab, 0, 0, 1, 1);

        lineEdit_inputGraphName_tab = new QLineEdit(groupBox_BaseCharacter_tab);
        lineEdit_inputGraphName_tab->setObjectName(QStringLiteral("lineEdit_inputGraphName_tab"));

        gridLayout->addWidget(lineEdit_inputGraphName_tab, 0, 1, 1, 2);

        label_GraphColor_tab = new QLabel(groupBox_BaseCharacter_tab);
        label_GraphColor_tab->setObjectName(QStringLiteral("label_GraphColor_tab"));

        gridLayout->addWidget(label_GraphColor_tab, 2, 0, 1, 1);

        label_GraphWidth_tab = new QLabel(groupBox_BaseCharacter_tab);
        label_GraphWidth_tab->setObjectName(QStringLiteral("label_GraphWidth_tab"));

        gridLayout->addWidget(label_GraphWidth_tab, 3, 0, 1, 1);

        lineEdit_GraphWidth_tab = new QLineEdit(groupBox_BaseCharacter_tab);
        lineEdit_GraphWidth_tab->setObjectName(QStringLiteral("lineEdit_GraphWidth_tab"));

        gridLayout->addWidget(lineEdit_GraphWidth_tab, 3, 1, 1, 2);

        label_GraphBuffer_tab = new QLabel(groupBox_BaseCharacter_tab);
        label_GraphBuffer_tab->setObjectName(QStringLiteral("label_GraphBuffer_tab"));

        gridLayout->addWidget(label_GraphBuffer_tab, 4, 0, 1, 1);

        lineEdit_GraphBuffer_tab = new QLineEdit(groupBox_BaseCharacter_tab);
        lineEdit_GraphBuffer_tab->setObjectName(QStringLiteral("lineEdit_GraphBuffer_tab"));

        gridLayout->addWidget(lineEdit_GraphBuffer_tab, 4, 1, 1, 2);

        label_chooseGraphColor = new QLabel(groupBox_BaseCharacter_tab);
        label_chooseGraphColor->setObjectName(QStringLiteral("label_chooseGraphColor"));
        label_chooseGraphColor->setStyleSheet(QStringLiteral(""));
        label_chooseGraphColor->setFrameShape(QFrame::Box);

        gridLayout->addWidget(label_chooseGraphColor, 2, 1, 1, 1);

        pushButton_chooseGraphColor_tab = new QPushButton(groupBox_BaseCharacter_tab);
        pushButton_chooseGraphColor_tab->setObjectName(QStringLiteral("pushButton_chooseGraphColor_tab"));

        gridLayout->addWidget(pushButton_chooseGraphColor_tab, 2, 2, 1, 1);


        horizontalLayout_2->addWidget(groupBox_BaseCharacter_tab);

        groupBox_Data_tab = new QGroupBox(groupBox_GraphConfig_tab);
        groupBox_Data_tab->setObjectName(QStringLiteral("groupBox_Data_tab"));
        gridLayout_Data = new QGridLayout(groupBox_Data_tab);
        gridLayout_Data->setSpacing(6);
        gridLayout_Data->setContentsMargins(11, 11, 11, 11);
        gridLayout_Data->setObjectName(QStringLiteral("gridLayout_Data"));
        pushButton_chooseXparam_tab = new QPushButton(groupBox_Data_tab);
        pushButton_chooseXparam_tab->setObjectName(QStringLiteral("pushButton_chooseXparam_tab"));

        gridLayout_Data->addWidget(pushButton_chooseXparam_tab, 1, 4, 1, 1);

        lineEdit_inputXparam_tab = new QLineEdit(groupBox_Data_tab);
        lineEdit_inputXparam_tab->setObjectName(QStringLiteral("lineEdit_inputXparam_tab"));

        gridLayout_Data->addWidget(lineEdit_inputXparam_tab, 1, 1, 1, 3);

        label_Xparam_tab = new QLabel(groupBox_Data_tab);
        label_Xparam_tab->setObjectName(QStringLiteral("label_Xparam_tab"));

        gridLayout_Data->addWidget(label_Xparam_tab, 1, 0, 1, 1);

        pushButton_chooseT0_tab = new QPushButton(groupBox_Data_tab);
        pushButton_chooseT0_tab->setObjectName(QStringLiteral("pushButton_chooseT0_tab"));

        gridLayout_Data->addWidget(pushButton_chooseT0_tab, 0, 4, 1, 2);

        label_inputT0_tab = new QLabel(groupBox_Data_tab);
        label_inputT0_tab->setObjectName(QStringLiteral("label_inputT0_tab"));

        gridLayout_Data->addWidget(label_inputT0_tab, 0, 3, 1, 1);

        pushButton_chooseGraphbasemap_tab = new QPushButton(groupBox_Data_tab);
        pushButton_chooseGraphbasemap_tab->setObjectName(QStringLiteral("pushButton_chooseGraphbasemap_tab"));

        gridLayout_Data->addWidget(pushButton_chooseGraphbasemap_tab, 0, 0, 1, 2);

        lineEdit_inputYparam_tab = new QLineEdit(groupBox_Data_tab);
        lineEdit_inputYparam_tab->setObjectName(QStringLiteral("lineEdit_inputYparam_tab"));

        gridLayout_Data->addWidget(lineEdit_inputYparam_tab, 2, 1, 1, 3);

        label_Yparam_tab = new QLabel(groupBox_Data_tab);
        label_Yparam_tab->setObjectName(QStringLiteral("label_Yparam_tab"));

        gridLayout_Data->addWidget(label_Yparam_tab, 2, 0, 1, 1);

        pushButton_chooseYparam_tab = new QPushButton(groupBox_Data_tab);
        pushButton_chooseYparam_tab->setObjectName(QStringLiteral("pushButton_chooseYparam_tab"));

        gridLayout_Data->addWidget(pushButton_chooseYparam_tab, 2, 4, 1, 1);


        horizontalLayout_2->addWidget(groupBox_Data_tab);


        horizontalLayout_Graphconfig->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(groupBox_GraphConfig_tab);


        verticalLayout_2->addWidget(widget);


        retranslateUi(GraphDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(GraphDialog);
    } // setupUi

    void retranslateUi(QDialog *GraphDialog)
    {
        GraphDialog->setWindowTitle(QApplication::translate("GraphDialog", "GraphDialog", 0));
        groupBox_TargetAbout_target->setTitle(QApplication::translate("GraphDialog", "\347\233\256\346\240\207\347\233\270\345\205\263", 0));
        pushButton_insertnewGraph_target->setText(QApplication::translate("GraphDialog", "\345\242\236\345\212\240\346\226\260\347\232\204\346\233\262\347\272\277", 0));
        pushButton_delchooseGraph_target->setText(QApplication::translate("GraphDialog", "\345\210\240\351\231\244\351\200\211\344\270\255\346\233\262\347\272\277", 0));
        pushButton_leftmovegraph_target->setText(QApplication::translate("GraphDialog", "\345\267\246\347\247\273\346\233\262\347\272\277", 0));
        pushButton_rightmovegraph_target->setText(QApplication::translate("GraphDialog", "\345\217\263\347\247\273\346\233\262\347\272\277", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("GraphDialog", "\346\233\262\347\272\2770", 0));
        groupBox_AxisConfig_tab->setTitle(QApplication::translate("GraphDialog", "\345\235\220\346\240\207\351\241\271\351\205\215\347\275\256", 0));
        groupBox_xAxisConfig_tab->setTitle(QApplication::translate("GraphDialog", "X\350\275\264\351\205\215\347\275\256", 0));
        label_kedufanwei_x_tab->setText(QApplication::translate("GraphDialog", "\345\210\273\345\272\246\350\214\203\345\233\264\357\274\232", 0));
        label_Xmax_tab->setText(QApplication::translate("GraphDialog", "\346\234\200\345\244\247\345\200\274", 0));
        label_Xmin_tab->setText(QApplication::translate("GraphDialog", "\346\234\200\345\260\217\345\200\274", 0));
        label_wenbenpianyi_x_tab->setText(QApplication::translate("GraphDialog", "\346\226\207\346\234\254\345\201\217\347\247\273\357\274\232", 0));
        label_AxisLabel_x->setText(QApplication::translate("GraphDialog", "\350\275\264\346\240\207\347\255\276\357\274\210\345\220\215\347\247\260\345\217\212\345\215\225\344\275\215\357\274\211\357\274\232", 0));
        label_keduweizhi_x->setText(QApplication::translate("GraphDialog", "\345\210\273\345\272\246\344\275\215\347\275\256\357\274\232", 0));
        comboBox_keduweizhi_x->clear();
        comboBox_keduweizhi_x->insertItems(0, QStringList()
         << QApplication::translate("GraphDialog", "\344\270\212", 0)
         << QApplication::translate("GraphDialog", "\344\270\255", 0)
         << QApplication::translate("GraphDialog", "\344\270\213", 0)
        );
        label_Axisdisplay_x->setText(QApplication::translate("GraphDialog", "\346\230\276\347\244\272\345\235\220\346\240\207\350\275\264\357\274\232", 0));
        checkBox_chooseAxisdisplay_x->setText(QApplication::translate("GraphDialog", "\351\273\230\350\256\244\351\200\211\344\270\255\346\230\276\347\244\272X\350\275\264", 0));
        groupBox_yAxisConfig_tab->setTitle(QApplication::translate("GraphDialog", "Y\350\275\264\351\205\215\347\275\256", 0));
        label_kedufanwei_y->setText(QApplication::translate("GraphDialog", "\345\210\273\345\272\246\350\214\203\345\233\264\357\274\232", 0));
        label_Ymax->setText(QApplication::translate("GraphDialog", "\346\234\200\345\244\247\345\200\274", 0));
        label_Ymin->setText(QApplication::translate("GraphDialog", "\346\234\200\345\260\217\345\200\274", 0));
        label_13_wenbenpianyi_y->setText(QApplication::translate("GraphDialog", "\346\226\207\346\234\254\345\201\217\347\247\273\357\274\232", 0));
        label_AxisLabel_y->setText(QApplication::translate("GraphDialog", "\350\275\264\346\240\207\347\255\276\357\274\210\345\220\215\347\247\260\345\217\212\345\215\225\344\275\215\357\274\211\357\274\232", 0));
        label_keduweizhi_y->setText(QApplication::translate("GraphDialog", "\345\210\273\345\272\246\344\275\215\347\275\256\357\274\232", 0));
        comboBox_keduweizhi_y->clear();
        comboBox_keduweizhi_y->insertItems(0, QStringList()
         << QApplication::translate("GraphDialog", "\345\267\246", 0)
         << QApplication::translate("GraphDialog", "\344\270\255", 0)
         << QApplication::translate("GraphDialog", "\345\217\263", 0)
        );
        label_Axisdisplay_y->setText(QApplication::translate("GraphDialog", "\346\230\276\347\244\272\345\235\220\346\240\207\350\275\264\357\274\232", 0));
        checkBox_chooseAxisdisplay_y->setText(QApplication::translate("GraphDialog", "\351\273\230\350\256\244\351\200\211\344\270\255\346\230\276\347\244\272Y\350\275\264", 0));
        groupBox_GraphConfig_tab->setTitle(QApplication::translate("GraphDialog", "\346\233\262\347\272\277\350\207\252\350\272\253\351\205\215\347\275\256", 0));
        groupBox_BaseCharacter_tab->setTitle(QApplication::translate("GraphDialog", "\345\237\272\346\234\254\347\211\271\345\276\201", 0));
        label_GraphName_tab->setText(QApplication::translate("GraphDialog", "\346\233\262\347\272\277\345\220\215\347\247\260\357\274\210\345\233\276\344\276\213\345\220\215\347\247\260\357\274\211\357\274\232", 0));
        label_GraphColor_tab->setText(QApplication::translate("GraphDialog", "\346\233\262\347\272\277\351\242\234\350\211\262\357\274\232", 0));
        label_GraphWidth_tab->setText(QApplication::translate("GraphDialog", "\346\233\262\347\272\277\345\256\275\345\272\246\357\274\232", 0));
        label_GraphBuffer_tab->setText(QApplication::translate("GraphDialog", "\346\233\262\347\272\277\347\274\223\345\206\262\345\214\272\357\274\232", 0));
        label_chooseGraphColor->setText(QString());
        pushButton_chooseGraphColor_tab->setText(QApplication::translate("GraphDialog", "\351\200\211\346\213\251\346\233\262\347\272\277\351\242\234\350\211\262", 0));
        groupBox_Data_tab->setTitle(QApplication::translate("GraphDialog", "\346\225\260\346\215\256\346\272\220\347\233\270\345\205\263", 0));
        pushButton_chooseXparam_tab->setText(QApplication::translate("GraphDialog", "\351\200\211\346\213\251X\345\217\202\346\225\260", 0));
        label_Xparam_tab->setText(QApplication::translate("GraphDialog", "X\345\217\202\346\225\260", 0));
        pushButton_chooseT0_tab->setText(QApplication::translate("GraphDialog", "\351\200\211\346\213\251T0", 0));
        label_inputT0_tab->setText(QApplication::translate("GraphDialog", "\350\276\223\345\205\245T0", 0));
        pushButton_chooseGraphbasemap_tab->setText(QApplication::translate("GraphDialog", "\351\200\211\346\213\251\345\274\271\351\201\223\345\272\225\345\233\276\357\274\210\347\220\206\350\256\272\345\274\271\351\201\223\357\274\211", 0));
        label_Yparam_tab->setText(QApplication::translate("GraphDialog", "Y\345\217\202\346\225\260", 0));
        pushButton_chooseYparam_tab->setText(QApplication::translate("GraphDialog", "\351\200\211\346\213\251Y\345\217\202\346\225\260", 0));
    } // retranslateUi

};

namespace qdesigner_internal {
 class GraphDialog: public Ui_GraphDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHDIALOG_H
