#include "image.h"
#include "imageplugin.h"

#include <QtPlugin>

imagePlugin::imagePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void imagePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool imagePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *imagePlugin::createWidget(QWidget *parent)
{
    return new image(parent);
}

QString imagePlugin::name() const
{
    return QLatin1String("image");
}

QString imagePlugin::group() const
{
    return tr("static");
}

QIcon imagePlugin::icon() const
{
    return QIcon(QStringLiteral("://images/image.png"));

}

QString imagePlugin::toolTip() const
{
    return QLatin1String("");
}

QString imagePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool imagePlugin::isContainer() const
{
    return false;
}

QString imagePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("image");
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

QString imagePlugin::includeFile() const
{
    return QLatin1String("image.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(imageplugin, imagePlugin)
#endif // QT_VERSION < 0x050000
