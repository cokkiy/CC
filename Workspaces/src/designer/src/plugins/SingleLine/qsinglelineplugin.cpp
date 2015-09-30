/*******************************************************************
 ** 文件名：rectangleplugin.h
 ** Rectangle插件设计器支持类定义，实现在设计器上设计时显示Rectangle外观，属性
 ** 版本：v1.0
 ** 作者：张立民
 ** 2015-08-13
 *****************************************************************/
#include "qsingleline.h"
#include "qsinglelineplugin.h"

#include <QtPlugin>

QSingleLinePlugin::QSingleLinePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QSingleLinePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QSingleLinePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QSingleLinePlugin::createWidget(QWidget *parent)
{
    return new QSingleLine(parent);
}

QString QSingleLinePlugin::name() const
{
    return QLatin1String("QSingleLine");
}

QString QSingleLinePlugin::group() const
{
    return tr("static");
}

QIcon QSingleLinePlugin::icon() const
{
    return QIcon(QStringLiteral("://images/singleLine.png"));
}

QString QSingleLinePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("QSingleLine");
    xmls += "\">\n";
    xmls += "<widget class=\"";
    xmls += name();
    xmls += "\" name=\"";
    xmls += name();
    xmls += "\">\n";
    xmls += "   <property name=\"geometry\">\n";
    xmls += "       <rect>\n";
    xmls += "       <x>0</x>\n";
    xmls += "       <y>0</y>\n";
    xmls += "       <width>150</width>\n";
    xmls += "       <height>100</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString QSingleLinePlugin::toolTip() const
{
    return QLatin1String("");
}

QString QSingleLinePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QSingleLinePlugin::isContainer() const
{
    return false;
}

QString QSingleLinePlugin::includeFile() const
{
    return QLatin1String("qsingleline.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qsinglelineplugin, QSingleLinePlugin)
#endif // QT_VERSION < 0x050000
