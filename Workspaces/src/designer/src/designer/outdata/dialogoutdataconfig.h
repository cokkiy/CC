#ifndef DIALOGOUTDATACONFIG_H
#define DIALOGOUTDATACONFIG_H

#include <QtWidgets/QDialog>

//图元数据输出配置界面类
//将图元收到的参数输出成文本文件，在此界面中配置输出文本的选项设置

namespace Ui {
class DialogOutDataConfig_UI;
}

//带时间参数类
class ParamWithTime
{
public:
    ParamWithTime()
    {
        m_strParam = "";
        m_time =  0;
        m_value = 0;
    }

    ~ParamWithTime()
    {
    };

    //参数名称字符串
    QString m_strParam;

    //参数时间
    unsigned int m_time;

    //参数值
    double m_value;
};

bool cmp(const ParamWithTime p1, const ParamWithTime p2 );

class DialogOutDataConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOutDataConfig(QWidget *parent = 0);
    ~DialogOutDataConfig();

private slots:
    void on_pushButtonSelectDir_clicked();

    void on_pushButtonSelectParams_clicked();

    void on_pushButtonOutData_clicked();

    void on_pushButtonCancel_clicked();

    void on_radioButtonMutiFile_clicked();

    void on_radioButtonSingleFile_clicked();

private:
    Ui::DialogOutDataConfig_UI *ui;

    //选择的参数(以字符串显示)
    QString m_strSelectedParams;

    //保存的文件路径
    QString m_strFilePath;

    //分隔符
    QString m_strSeperator;

    //参数vector
    //v1 : [100,1]电压1  [100,2]电压2
    //v2 : [100,1]  [100,2]
    QVector<QString> m_v1, m_v2;

    //导成一个文件还是多个文件
    bool m_bMultiOutFile;

    //时间转成可视格式
    QString CvTime(unsigned int time);

};

#endif // DIALOGOUTDATACONFIG_H
