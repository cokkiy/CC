#include "singleparam.h"
#include "qsingleparam.h"
#include "singleparamplugin.h"

#include <QtPlugin>

SingleParamPlugin::SingleParamPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void SingleParamPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool SingleParamPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *SingleParamPlugin::createWidget(QWidget *parent)
{
    return new QSingleParam(parent);
}

QString SingleParamPlugin::name() const
{
    return QLatin1String("QSingleParam");
}

QString SingleParamPlugin::group() const
{
    return tr("dynamic");
}

QIcon SingleParamPlugin::icon() const
{
    return QIcon();
}

QString SingleParamPlugin::toolTip() const
{
    return QLatin1String("");
}

QString SingleParamPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool SingleParamPlugin::isContainer() const
{
    return false;
}

QString SingleParamPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("QSingleParam");
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

QString SingleParamPlugin::includeFile() const
{
    return QLatin1String("qsingleparam.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(singleparamplugin, SingleParamPlugin)
#endif // QT_VERSION < 0x050000
