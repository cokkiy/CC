#include "ellipseangle.h"
#include "ellipseangleplugin.h"

#include <QtPlugin>

EllipseanglePlugin::EllipseanglePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void EllipseanglePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool EllipseanglePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *EllipseanglePlugin::createWidget(QWidget *parent)
{
    return new Ellipseangle(parent);
}

QString EllipseanglePlugin::name() const
{
    return QLatin1String("Ellipseangle");
}

QString EllipseanglePlugin::group() const
{
    return tr("static");
}

QIcon EllipseanglePlugin::icon() const
{
   return QIcon(QStringLiteral("://images/ellipse.png"));

}

QString EllipseanglePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("Ellipseangle");
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

QString EllipseanglePlugin::toolTip() const
{
    return "ellipse";
}

QString EllipseanglePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool EllipseanglePlugin::isContainer() const
{
    return false;
}

QString EllipseanglePlugin::includeFile() const
{
    return QLatin1String("ellipseangle.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ellipseangleplugin, EllipseanglePlugin)
#endif // QT_VERSION < 0x050000
