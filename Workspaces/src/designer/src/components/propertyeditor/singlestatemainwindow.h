#ifndef SINGLESTATEMAINWINDOW_H
#define SINGLESTATEMAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include<QGridLayout>

class SingleStateMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    SingleStateMainWindow(QWidget *parent = 0);
//    ~SingleStateMainWindow();
public:
    int rowValue;//输入的行数

private:
    QVBoxLayout *mainLayout;
    QStandardItemModel *model;
    QTableView *tableView;
    QPushButton *addRowButton,*delRowButton,*okButton,*cancelButton,*delCurrentRowButton;

    QLineEdit *lineedit;
    QWidget  *widget;
    QHBoxLayout *TopAddDelLayout,*TopDelButtonLayout,*TopLayout,*BottomLayout;


private slots:
    void showTableAddRow();
    void showTableDelRow();
    void showTableDelCurrentRow();
    void showTableOk();
    void quitWindow();

};

#endif // SINGLESTATEMAINWINDOW_H
