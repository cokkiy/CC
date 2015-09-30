#include "simpleinputdialog.h"
#include <QVBoxLayout>
#include <QApplication>
SimpleInputDialog::SimpleInputDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Input"));
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    name = new QLabel;
    name->setText(tr("Input:"));
    inputName = new QLineEdit;
    vBoxLayout->addWidget(name);
    vBoxLayout->addWidget(inputName);
    QFrame *horizontalLine = new QFrame;
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Sunken);
    vBoxLayout->addWidget(horizontalLine);
    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(QApplication::translate("NewForm", "&Cancel", 0),
                         QDialogButtonBox::RejectRole);
    buttonBox->addButton(QApplication::translate("NewForm", "&Ok", 0),
                         QDialogButtonBox::AcceptRole);
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slotButtonBoxClicked(QAbstractButton*)));
    vBoxLayout->addWidget(buttonBox);
    setLayout(vBoxLayout);
}
void SimpleInputDialog::slotButtonBoxClicked(QAbstractButton *btn)
{
    switch (buttonBox->buttonRole(btn)) {
    case QDialogButtonBox::RejectRole:{
        reject();
        break;
    }
    case QDialogButtonBox::AcceptRole: {
        m_inputString = inputName->text();
        if(!m_inputString.isEmpty())
            accept();
        break;
    }
    default:
        break;
    }
}
QString SimpleInputDialog::inputString()
{
    return m_inputString;
}
void SimpleInputDialog::setInputName(QString inputname)
{
    if(name)
    {
        name->setText(inputname);
    }
}
