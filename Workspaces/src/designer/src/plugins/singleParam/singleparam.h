#ifndef SINGLEPARAM_H
#define SINGLEPARAM_H

#include <QLabel>

class SingleParam : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QColor front_color READ frontColor WRITE frontColor)
public:
    SingleParam(QWidget *parent = 0);
    ~SingleParam();
public:
    inline QColor frontColor()
    {
        return m_front_color;
    };
    inline void frontColor(QColor front_color)
    {
        m_front_color = front_color;
    };
protected slots:
    void timerEvent(QTimerEvent *event);
private :
    int m_timer_id;
    QColor m_front_color;
};

#endif
