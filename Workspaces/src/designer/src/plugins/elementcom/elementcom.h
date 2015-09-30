#ifndef SINGLEPARAM_H
#define SINGLEPARAM_H

#include <QLabel>
class Elementcom : public QLabel//
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString styleSheet READ styleSheet WRITE setStyleSheet)
public:
    Elementcom(QWidget *parent = 0);
    ~Elementcom();
public slots:
    void visible();
private :
    bool m_bVisible;
};

#endif
