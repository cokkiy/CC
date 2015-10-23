
#include "codesource.h"
#include "codeSourceplugin.h"


#include <QtPlugin>

codeSourcePlugin::codeSourcePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void codeSourcePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool codeSourcePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *codeSourcePlugin::createWidget(QWidget *parent)
{
    return new codeSource(parent);
}

QString codeSourcePlugin::name() const
{
    return QLatin1String("codeSource");
}

QString codeSourcePlugin::group() const
{
    return tr("dynamic");
}

QIcon codeSourcePlugin::icon() const
{
    return QIcon(QStringLiteral("://images/sourceCode.png"));
}

QString codeSourcePlugin::toolTip() const
{
    return QLatin1String("");
}

QString codeSourcePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool codeSourcePlugin::isContainer() const
{
    return false;
}

QString codeSourcePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("codeSource");
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
QString codeSourcePlugin::includeFile() const
{
    return QLatin1String("codesource.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(codesourceplugin, codeSourcePlugin)
#endif // QT_VERSION < 0x050000
