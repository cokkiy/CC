#ifndef SIMPLEINPUTDIALOG_H
#define SIMPLEINPUTDIALOG_H
#include <QAbstractButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDialog>

class SimpleInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SimpleInputDialog(QWidget *parent = 0);
    QString inputString();
    void setInputName(QString);
signals:

private slots:
    void slotButtonBoxClicked(QAbstractButton *btn);
private:
    QLabel* name;
    QString m_inputString;
    QLineEdit* inputName;
    QDialogButtonBox *buttonBox;

};

#endif // SIMPLEINPUTDIALOG_H
