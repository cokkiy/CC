#include "controlprogram.h"
#include "controlprogramplugin.h"

#include <QtPlugin>

controlProgramPlugin::controlProgramPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void controlProgramPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool controlProgramPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *controlProgramPlugin::createWidget(QWidget *parent)
{
    return new controlProgram(parent);
}

QString controlProgramPlugin::name() const
{
    return QLatin1String("controlProgram");
}

QString controlProgramPlugin::group() const
{
    return tr("dynamic");
}

QIcon controlProgramPlugin::icon() const
{
    return QIcon(QStringLiteral("://images/controlProgram.png"));
//    return QIcon();
}

QString controlProgramPlugin::toolTip() const
{
    return QLatin1String("");
}

QString controlProgramPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool controlProgramPlugin::isContainer() const
{
    return false;
}

QString controlProgramPlugin::domXml() const
{
//    return QLatin1String("<widget class=\"controlProgram\" name=\"controlProgram\">\n</widget>\n");
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("controlProgram");
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

QString controlProgramPlugin::includeFile() const
{
    return QLatin1String("controlprogram.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(controlprogramplugin, controlProgramPlugin)
#endif // QT_VERSION < 0x050000
