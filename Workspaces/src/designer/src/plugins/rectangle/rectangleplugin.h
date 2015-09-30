/*******************************************************************
 ** 文件名：rectangleplugin.h
 ** Rectangle插件设计器支持类定义，实现在设计器上设计时显示Rectangle外观，属性
 ** 版本：v1.0
 ** 作者：张立民
 ** 2015-08-13
 *****************************************************************/
#ifndef RECTANGLEPLUGIN_H
#define RECTANGLEPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class RectanglePlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
#endif // QT_VERSION >= 0x050000

public:
    RectanglePlugin(QObject *parent = 0);

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool m_initialized;
};

#endif
