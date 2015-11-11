#include "staticgraph.h"
#include "staticgraphplugin.h"

#include <QtPlugin>

staticgraphPlugin::staticgraphPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void staticgraphPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool staticgraphPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *staticgraphPlugin::createWidget(QWidget *parent)
{
    return new staticgraph(parent);
}
/*
QString staticgraph1Plugin::name() const
{
    return QLatin1String("staticgraph");
}

QString staticgraph1Plugin::group() const
{
    return QLatin1String("");
}
*/
QString staticgraphPlugin::name() const
{
    return QLatin1String("staticgraph");
}


QString staticgraphPlugin::group() const
{
    return tr("graph");
}


QIcon staticgraphPlugin::icon() const
{
    return QIcon(QStringLiteral("://images/staticgraph.png"));
}

QString staticgraphPlugin::toolTip() const
{
    return tr("静态曲线图元");
}

QString staticgraphPlugin::whatsThis() const
{
    return tr("静态曲线图元");
}

bool staticgraphPlugin::isContainer() const
{
    return false;
}
/*
QString staticgraphPlugin::domXml() const
{
    return QLatin1String("<widget class=\"staticgraph\" name=\"staticgraph\">\n</widget>\n");
}
*/
QString staticgraphPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("staticgraph");
    xmls += "\">\n";
    xmls += "<widget class=\"";
    xmls += name();
    xmls += "\" name=\"";
    xmls += name();
    xmls += "\">\n";
    xmls += "   <property name=\"geometry\">\n";
    xmls += "       <rect>\n";
    xmls += "       <x>150</x>\n";
    xmls += "       <y>130</y>\n";
    xmls += "       <width>600</width>\n";
    xmls += "       <height>600</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString staticgraphPlugin::includeFile() const
{
    return QLatin1String("staticgraph.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(staticgraphplugin, staticgraphPlugin)
#endif // QT_VERSION < 0x050000
