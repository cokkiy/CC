#include "commandbutton.h"
#include "commandbuttonplugin.h"

#include <QtPlugin>

CommandButtonPlugin::CommandButtonPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void CommandButtonPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool CommandButtonPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *CommandButtonPlugin::createWidget(QWidget *parent)
{
    return new CommandButton(parent);
}

QString CommandButtonPlugin::name() const
{
    return QLatin1String("CommandButton");
}

QString CommandButtonPlugin::group() const
{
    return tr("command");
}

QIcon CommandButtonPlugin::icon() const
{
    return QIcon("://resource/commmandbutton.png");
}

QString CommandButtonPlugin::toolTip() const
{
    return QLatin1String("");
}

QString CommandButtonPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool CommandButtonPlugin::isContainer() const
{
    return false;
}

QString CommandButtonPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("CommandButton");
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
    xmls += "       <width>100</width>\n";
    xmls += "       <height>50</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString CommandButtonPlugin::includeFile() const
{
    return QLatin1String("commandbutton.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(commandbuttonplugin, CommandButtonPlugin)
#endif // QT_VERSION < 0x050000
