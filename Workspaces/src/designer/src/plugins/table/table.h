#ifndef TABLE_H
#define TABLE_H
#include <Net/NetComponents>
#include "../../components/propertyeditor/simpletable/tablecell.h"
#include "../../components/propertyeditor/simpletable/tableproperty.h"
#include <QWidget>
#include <QtGui>

class QTable : public QWidget
{
    Q_OBJECT
    //文本属性
    Q_PROPERTY(QString table READ textString WRITE setTextString RESET resetTable)
	//斑马线显示
	Q_PROPERTY(QColor zebraLineColor READ getZebraLineColor WRITE setZebraLineColor )

    //网格线属性
    Q_PROPERTY(QColor gridLineColor READ getGridLineColor WRITE setGridLineColor)
    Q_PROPERTY(qint32 gridLineWidth READ getGridLineWidth WRITE setGridLineWidth)
    Q_PROPERTY(Qt::PenStyle gridLineStyle READ getGridLineStyle WRITE setGridLineStyle)

    //插件框架属性
    Q_PROPERTY(QBrush backgroundBrush READ getBackgroundBrush WRITE setBackgroundBrush )
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)
    Q_PROPERTY(qint32 borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(Qt::PenStyle borderStyle READ getBorderStyle WRITE setBorderStyle)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
protected:
	//绘制事件
	void paintEvent(QPaintEvent *event);
	//改变图元大小事件
	void resizeEvent(QResizeEvent *event);
public:
    //文本属性
	QString textString() const {return m_textString;}
	void setTextString(const QString string);
    void resetTable();

	//斑马线属性
	QColor getZebraLineColor() const {return zebraLineColor;}
	void setZebraLineColor(const QColor Color);

	//网格线属性
	//网格线颜色
	QColor getGridLineColor() const {return gridLineColor;}
	void setGridLineColor(const QColor Color);
	//网格线宽度
	qint32 getGridLineWidth() const {return gridLineWidth;}
	void setGridLineWidth(const qint32 w);
	//网格线样式
    Qt::PenStyle getGridLineStyle() const {return gridLineStyle;}
	void setGridLineStyle(const Qt::PenStyle style);
    //插件框架属性
	//插件背景色
    QBrush getBackgroundBrush() const {return backgroundBrush;}
    void setBackgroundBrush(const QBrush Brush);
	//插件边框颜色
	QColor getBorderColor() const {return borderColor;}
	void setBorderColor(const QColor Color);
	//插件边框线宽
	qint32 getBorderWidth() const {return borderWidth;}
	void setBorderWidth(const qint32 w);
	//插件边框线样式
	Qt::PenStyle getBorderStyle() const {return borderStyle;}
	void setBorderStyle(const Qt::PenStyle style);
private:
    //文本属性
	QString m_textString;//输入参数
    QString m_lastTextString;//输入参数
    QString dispString;//显示参数

	//斑马线显示
	QColor zebraLineColor;//斑马线颜色

    //网格线属性
    QColor gridLineBrush;//网格线画笔
    QColor gridLineColor;//网格线颜色
    qint32 gridLineWidth;//网格线宽度
	Qt::PenStyle gridLineStyle;//网格线样式

    //表格框架属性
    QBrush backgroundBrush;//插件背景颜色
    QColor borderColor;//插件边框颜色
    qint32 borderWidth;//插件边框宽度
    Qt::PenStyle borderStyle;//插件边框样式
private:
    QRect pluginRect;//插件大小区域
    QPen textPen;//文本画笔
    int m_timer_id;
	//表格宽度和高度的放大倍数
    float m_fWidthRatio;
    float m_fHeightRatio;
protected slots:
    void timerEvent(QTimerEvent *event);
private:
    DataCenterInterface* dc;
    InformationInterface* paramInformation;
	//获取单元格显示字符串
    QString getCellShowStr(QTableCell TableCell);
	//将表格属性转换为Json字符串
    void setJsonStr(float wr,float hr);
    //显示插件边框
    void showPluginBord();
	//显示插件背景
    void showPluginBackgroud();
	//显示表格
    void showTable(float wr,float hr);
	//显示表格文本
    void showTableText(float wr,float hr);
	//显示插件
    void showPlugin();
	//显示斑马线
    void showZebraLine(float wr,float hr);
	//设置自定义画笔
    QPen setCustomPen(Qt::PenStyle style,QColor color,uint width);
	//设置插件矩形区域和背景区域
    void setPluginRect();
	//设置插件大小和位置
    void setPluginGeometry();
	//初始化表格属性
    void initTableProperty();
    //解析显示字符串，字符串中包括字体、颜色、文本
    void analyzeDisplayStr(QFont &,QColor &);
private:
	//表格属性
    QTableProperty m_TableProperty;
public:
    QTable(QWidget *parent = 0);
    ~QTable();
};

#endif // TABLE_H
