#ifndef ABSTRACTIMAGEEXPLORER_H
#define ABSTRACTIMAGEEXPLORER_H

#include <QtDesigner/sdk_global.h>

#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;

class QDESIGNER_SDK_EXPORT QDesignerImageExplorerInterface: public QWidget
{
    Q_OBJECT
public:
    QDesignerImageExplorerInterface(QWidget *parent, Qt::WindowFlags flags = 0);
    virtual ~QDesignerImageExplorerInterface();

    virtual void setFileName(const QString &file_name) = 0;
    virtual QString fileName() const = 0;
    virtual bool load() = 0;
    virtual bool save() = 0;
    virtual QDesignerFormEditorInterface *core() const;
};

QT_END_NAMESPACE

#endif // ABSTRACTIMAGEEXPLORER_H
