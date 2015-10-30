#include "table.h"
#include "tableplugin.h"

#include <QtPlugin>

TablePlugin::TablePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void TablePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool TablePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *TablePlugin::createWidget(QWidget *parent)
{
    return new QTable(parent);
}

QString TablePlugin::name() const
{
    return QLatin1String("QTable");
}

QString TablePlugin::group() const
{
    return tr("dynamic");
}

QIcon TablePlugin::icon() const
{
    return QIcon();
}

QString TablePlugin::toolTip() const
{
    return QLatin1String("");
}

QString TablePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool TablePlugin::isContainer() const
{
    return false;
}

QString TablePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("QTable");
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
    xmls += "       <width>300</width>\n";
    xmls += "       <height>200</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString TablePlugin::includeFile() const
{
    return QLatin1String("table.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qtableplugin, TablePlugin)
#endif // QT_VERSION < 0x050000
