#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QUrl>

class Button : public QPushButton
{
    Q_OBJECT

    //设置按钮显示文本，字体及字体颜色
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)

    //读取图片，用于更换不同状态下显示的背景图片
    Q_PROPERTY(QUrl normalImage READ normalImage WRITE setNormalImage)
    Q_PROPERTY(QUrl hoverImage READ hoverImage WRITE setHoverImage)
    Q_PROPERTY(QUrl pressedImage READ pressedImage WRITE setPressedImage)
    Q_PROPERTY(QUrl disabledImage READ disabledImage WRITE setDisabledImage)
    Q_PROPERTY(QUrl focusImage READ focusImage WRITE setFocusImage)

    //设置按钮的位置信息
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

public:

    Button(QWidget *parent = 0);

    QColor textColor() const { return m_textColor; }
    void setTextColor(const QColor &color);

    QUrl normalImage() const { return m_normalImage; }
    QUrl hoverImage() const { return m_hoverImage; }
    QUrl pressedImage() const { return m_pressedImage; }
    QUrl disabledImage() const { return m_disabledImage; }
    QUrl focusImage() const { return m_focusImage; }
    void setNormalImage(const QUrl &newUrl);
    void setHoverImage(const QUrl &newUrl);
    void setPressedImage(const QUrl &newUrl);
    void setDisabledImage(const QUrl &newUrl);
    void setFocusImage(const QUrl &newUrl);

signals:
    void textColorChanged();
    void imageChanged();
//    void localAddressChanged(QString address);

private slots:
    void stylesheetUpdate();
//    void imageUpdate();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QColor m_textColor;    
    QUrl m_normalImage;
    QUrl m_hoverImage;
    QUrl m_pressedImage;
    QUrl m_disabledImage;
    QUrl m_focusImage;
};

#endif
