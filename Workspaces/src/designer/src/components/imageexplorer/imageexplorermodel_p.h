#ifndef IMAGEEXPLORERMODEL_H
#define IMAGEEXPLORERMODEL_H

#include <layoutinfo_p.h>

#include <QtGui/QStandardItemModel>
#include <QtGui/QIcon>
#include <QtCore/QModelIndex>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMultiMap>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

namespace qdesigner_internal {

    // Data structure containing the fixed item type icons
    struct ImageExplorerIcons {
        QIcon layoutIcons[LayoutInfo::UnknownLayout + 1];
    };

    struct ModelRecursionContext_Image;

    // Data structure representing one item of the object inspector.
    class ImageData {
    public:
        enum Type {
            Object,
            Action,
            SeparatorAction,
            ChildWidget,         // A child widget
            LayoutableContainer, // A container that can be laid out
            LayoutWidget,        // A QLayoutWidget
            ExtensionContainer   // QTabWidget and the like, container extension
        };

        typedef QList<QStandardItem *> StandardItemList;

        explicit ImageData(QObject *parent, QObject *object, const ModelRecursionContext_Image &ctx);
        ImageData();

        inline Type     type()       const { return m_type; }
        inline QObject *object()     const { return m_object; }
        inline QObject *parent()     const { return m_parent; }
        inline QString  objectName() const { return m_objectName; }

        bool equals(const ImageData & me) const;

        enum ChangedMask { ClassNameChanged = 1, ObjectNameChanged = 2,
                           ClassIconChanged = 4, TypeChanged = 8,
                           LayoutTypeChanged = 16};

        unsigned compare(const ImageData & me) const;

        // Initially set up a row
        void setItems(const StandardItemList &row, const ImageExplorerIcons &icons) const;
        // Update row data according to change mask
        void setItemsDisplayData(const StandardItemList &row, const ImageExplorerIcons &icons, unsigned mask) const;

    private:
        void initObject(const ModelRecursionContext_Image &ctx);
        void initWidget(QWidget *w, const ModelRecursionContext_Image &ctx);

        QObject *m_parent;
        QObject *m_object;
        Type m_type;
        QString m_className;
        QString m_objectName;
        QIcon m_classIcon;
        LayoutInfo::Type m_managedLayoutType;
    };

    inline bool operator==(const ImageData &e1, const ImageData &e2) { return e1.equals(e2); }
    inline bool operator!=(const ImageData &e1, const ImageData &e2) { return !e1.equals(e2); }

    typedef QList<ImageData> ImageModel;

    // QStandardItemModel for ObjectInspector. Uses ObjectData/ObjectModel
    // internally for its updates.
    class ImageExplorerModel : public QStandardItemModel {
    public:
        typedef QList<QStandardItem *> StandardItemList;
        enum { ObjectNameColumn, ClassNameColumn, NumColumns };

        explicit ImageExplorerModel(QObject *parent);

        enum UpdateResult { NoForm, Rebuilt, Updated };
        UpdateResult update(QDesignerFormWindowInterface *fw);

        const QModelIndexList indexesOf(QObject *o) const { return m_objectIndexMultiMap.values(o); }
        QObject *objectAt(const QModelIndex &index) const;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    private:
        typedef QMultiMap<QObject *,QModelIndex> ImageIndexMultiMap;

        void rebuild(const ImageModel &newModel);
        void updateItemContents(ImageModel &oldModel, const ImageModel &newModel);
        void clearItems();
        StandardItemList rowAt(QModelIndex index) const;

        ImageExplorerIcons m_icons;
        ImageIndexMultiMap m_objectIndexMultiMap;
        ImageModel m_model;
        QPointer<QDesignerFormWindowInterface> m_formWindow;
    };
}  // namespace qdesigner_internal

#endif // IMAGEEXPLORERMODEL_H

QT_END_NAMESPACE
