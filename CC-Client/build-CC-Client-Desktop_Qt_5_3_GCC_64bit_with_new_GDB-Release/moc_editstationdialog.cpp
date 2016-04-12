/****************************************************************************
** Meta object code from reading C++ file 'editstationdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CC-Client/editstationdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editstationdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EditStationDialog_t {
    QByteArrayData data[11];
    char stringdata[302];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditStationDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditStationDialog_t qt_meta_stringdata_EditStationDialog = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 26),
QT_MOC_LITERAL(2, 45, 0),
QT_MOC_LITERAL(3, 46, 29),
QT_MOC_LITERAL(4, 76, 23),
QT_MOC_LITERAL(5, 100, 27),
QT_MOC_LITERAL(6, 128, 32),
QT_MOC_LITERAL(7, 161, 35),
QT_MOC_LITERAL(8, 197, 35),
QT_MOC_LITERAL(9, 233, 38),
QT_MOC_LITERAL(10, 272, 29)
    },
    "EditStationDialog\0on_addNiPushButton_clicked\0"
    "\0on_removeNIPushButton_clicked\0"
    "on_OKPushButton_clicked\0"
    "on_CancelPushButton_clicked\0"
    "on_addStartAppPushButton_clicked\0"
    "on_removeStartAppPushButton_clicked\0"
    "on_addMonitorProcPushButton_clicked\0"
    "on_removeMonitorProcPushButton_clicked\0"
    "on_viewProcPushButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditStationDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    0,   65,    2, 0x0a /* Public */,
       9,    0,   66,    2, 0x0a /* Public */,
      10,    0,   67,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void EditStationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditStationDialog *_t = static_cast<EditStationDialog *>(_o);
        switch (_id) {
        case 0: _t->on_addNiPushButton_clicked(); break;
        case 1: _t->on_removeNIPushButton_clicked(); break;
        case 2: _t->on_OKPushButton_clicked(); break;
        case 3: _t->on_CancelPushButton_clicked(); break;
        case 4: _t->on_addStartAppPushButton_clicked(); break;
        case 5: _t->on_removeStartAppPushButton_clicked(); break;
        case 6: _t->on_addMonitorProcPushButton_clicked(); break;
        case 7: _t->on_removeMonitorProcPushButton_clicked(); break;
        case 8: _t->on_viewProcPushButton_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject EditStationDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EditStationDialog.data,
      qt_meta_data_EditStationDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *EditStationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditStationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditStationDialog.stringdata))
        return static_cast<void*>(const_cast< EditStationDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int EditStationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
