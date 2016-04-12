/****************************************************************************
** Meta object code from reading C++ file 'filterbuilddialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CC-Client/filterbuilddialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filterbuilddialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FilterBuildDialog_t {
    QByteArrayData data[9];
    char stringdata[174];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FilterBuildDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FilterBuildDialog_t qt_meta_stringdata_FilterBuildDialog = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 39),
QT_MOC_LITERAL(2, 58, 0),
QT_MOC_LITERAL(3, 59, 5),
QT_MOC_LITERAL(4, 65, 33),
QT_MOC_LITERAL(5, 99, 24),
QT_MOC_LITERAL(6, 124, 22),
QT_MOC_LITERAL(7, 147, 9),
QT_MOC_LITERAL(8, 157, 16)
    },
    "FilterBuildDialog\0"
    "on_propertyComboBox_currentIndexChanged\0"
    "\0index\0on_opComboBox_currentIndexChanged\0"
    "on_addToolButton_clicked\0"
    "on_clearButton_clicked\0getFilter\0"
    "FilterOperations"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FilterBuildDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       4,    1,   42,    2, 0x0a /* Public */,
       5,    0,   45,    2, 0x0a /* Public */,
       6,    0,   46,    2, 0x0a /* Public */,
       7,    0,   47,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 8,

       0        // eod
};

void FilterBuildDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FilterBuildDialog *_t = static_cast<FilterBuildDialog *>(_o);
        switch (_id) {
        case 0: _t->on_propertyComboBox_currentIndexChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->on_opComboBox_currentIndexChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->on_addToolButton_clicked(); break;
        case 3: _t->on_clearButton_clicked(); break;
        case 4: { FilterOperations _r = _t->getFilter();
            if (_a[0]) *reinterpret_cast< FilterOperations*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject FilterBuildDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FilterBuildDialog.data,
      qt_meta_data_FilterBuildDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *FilterBuildDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FilterBuildDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FilterBuildDialog.stringdata))
        return static_cast<void*>(const_cast< FilterBuildDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int FilterBuildDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
