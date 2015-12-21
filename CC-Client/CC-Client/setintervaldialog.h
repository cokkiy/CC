#ifndef SETINTERVALDIALOG_H
#define SETINTERVALDIALOG_H

#include <QDialog>
namespace Ui { class SetIntervalDialog; };

class SetIntervalDialog : public QDialog
{
    Q_OBJECT
        Q_PROPERTY(int Interval READ Interval WRITE setInterval)
private:
    int m_Interval;
    void setInterval(int value)
    {
        if (m_Interval != value)
        {
            m_Interval = value;
        }
    }
public:
    /**
     *获取监视间隔（秒)
     ***/
    int Interval()
    {
        return m_Interval;
    }


public:
    SetIntervalDialog(int interval, QWidget *parent = 0);
    ~SetIntervalDialog();

    protected slots:
    //确定按钮
    void on_okPushButton_clicked();
    //取消按钮
    void on_cancelPushButton_clicked();

private:
    Ui::SetIntervalDialog *ui;
};

#endif // SETINTERVALDIALOG_H
