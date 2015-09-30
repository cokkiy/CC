#include "reserves.h"
#include "reservesplugin.h"

#include <QtPlugin>

reservesPlugin::reservesPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void reservesPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool reservesPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *reservesPlugin::createWidget(QWidget *parent)
{
    return new reserves(parent);
}

QString reservesPlugin::name() const
{
    return QLatin1String("reserves");
}

QString reservesPlugin::group() const
{
    return tr("dynamic");
}

QIcon reservesPlugin::icon() const
{
//    return QIcon();
    return QIcon(QStringLiteral("://images/reserves.png"));
}

QString reservesPlugin::toolTip() const
{
    return QLatin1String("");
}

QString reservesPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool reservesPlugin::isContainer() const
{
    return false;
}

//QString reservesPlugin::domXml() const
//{
//    return QLatin1String("<widget class=\"reserves\" name=\"reserves\">\n</widget>\n");
//}
QString reservesPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("reserves");
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

QString reservesPlugin::includeFile() const
{
    return QLatin1String("reserves.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(reservesplugin, reservesPlugin)
#endif // QT_VERSION < 0x050000
