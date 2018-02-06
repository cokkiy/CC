#ifndef FLOATINGMENU_H
#define FLOATINGMENU_H

#include <QDialog>
#include <QTimer>
#include <QPen>
#include <QToolButton>
#include <QPropertyAnimation>

namespace Ui { class FloatingMenu; }

class FloatingMenu : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int size READ size WRITE setSize) /*size property*/
public:
    FloatingMenu(QWidget *parent = 0);
    ~FloatingMenu();

    //鼠标进入
    virtual void enterEvent(QEvent *) override;
    //鼠标退出
    virtual void leaveEvent(QEvent *) override;

    /*!
    显示悬浮式菜单
    @param int posX 显示位置（中心)
    @param int posY 显示位置（中心)
    @return void
    @remarks 设置好所有选项,按钮后调用show显示
    作者：cokkiy（张立民)
    创建时间：2015/11/09 21:07:00
    */
    void show(int posX, int posY);    

    /*绘制圆形底图*/
    virtual void paintEvent(QPaintEvent *) override;
    /*!
    设置大小
    @param int size 大小
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/09 16:37:20
    */
    void setSize(int size = 250);

    /*!
    获取菜单大小
    @return int 悬浮菜单大小
    作者：cokkiy（张立民)
    创建时间：2015/11/09 16:37:20
    */
    int size();

    /*!
    设置边框
    @param const QPen & borderPen
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/09 16:48:01
    */
    void setBorder(const QPen& borderPen);
    /*!
    设置背景
    @param const QBrush & backBrush
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/09 16:48:17
    */
    void setBackground(const QBrush& backBrush);

    /*!
    设置按钮大小
    @param int buttonSize 按钮大小，默认54
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/09 21:50:00
    */
    void setButtonSize(int buttonSize = 54);

    /*!
    添加一个按钮
    @param const QString& iconName 按钮图标文件
    @param const QString& text 按钮文字
    @param const QObject * receiver 处理函数所在对象
    @param const char * member 处理函数名称
    @param int iconSize 图标大小，默认为32
    @param bool enabled 是否使能
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/09 17:16:58
    */
    void addButton(const QString& iconName, const QString& text,
        const QObject* receiver, const char* member, int iconSize = 32, bool enabled = true);

    /*!
    添加一个禁用的按钮
    @param const QString & iconName 按钮图标名称
    @param const QString & text 按钮文字
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/10 10:17:44
    */
    void addButton(const QString& iconName, const QString& text);

private:
    Ui::FloatingMenu *ui;
    //退出计时器
    QTimer* exitTimer;
    //实例,保证只有一个实例
    static FloatingMenu* instance;
    //默认大小
    int m_size = 250;
    //边框（默认为QColor(255, 170, 0),土豪金)
    QPen borderPen;
    //背景
    QBrush background;
    //按钮列表
    QList<QToolButton*> buttons;
    //按钮大小
    int buttonSize = 54;

    //修正位置
    void AmendPosition(int &posX, int &posY);    

#ifdef NeoLinux
    void AmendPositionForNeoLinx(int &posX, int &posY);
#endif

private slots:
    //关闭菜单
    void closeMe();
};

#endif // FLOATINGMENU_H
