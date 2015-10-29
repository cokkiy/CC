#ifndef Q2WMAPELEMENTDLG_H
#define Q2WMAPELEMENTDLG_H


class Q2wmapElementDlg : public QDialog
{
    Q_OBJECT
public:
    Q2wmapElementDlg();

    //元素类型
    QString m_strType;

    //元素名称
    QString m_strName;

    //元素线条颜色
    QString m_strColor;

    //元素线条粗细
    qint32 m_iWidth;
};

#endif // Q2WMAPELEMENTDLG_H
