#ifndef TABLESET_H
#define TABLESET_H

#include <QDialog>

namespace Ui {
class QTableSet;
}

class QTableSet : public QDialog
{
    Q_OBJECT

public:
    explicit QTableSet(QWidget *parent = 0);
    ~QTableSet();
    void GetNum(int& r,int& c);

private:
    Ui::QTableSet *ui;
};

#endif // TABLESET_H
