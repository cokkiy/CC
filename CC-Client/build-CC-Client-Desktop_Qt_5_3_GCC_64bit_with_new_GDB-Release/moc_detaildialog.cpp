/****************************************************************************
** Meta object code from reading C++ file 'detaildialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CC-Client/detaildialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'detaildialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DetailDialog_t {
    QByteArrayData data[9];
    char stringdata[175];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DetailDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DetailDialog_t qt_meta_stringdata_DetailDialog = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 28),
QT_MOC_LITERAL(2, 42, 0),
QT_MOC_LITERAL(3, 43, 29),
QT_MOC_LITERAL(4, 73, 27),
QT_MOC_LITERAL(5, 101, 34),
QT_MOC_LITERAL(6, 136, 19),
QT_MOC_LITERAL(7, 156, 14),
QT_MOC_LITERAL(8, 171, 3)
    },
    "DetailDialog\0on_powerOnPushButton_clicked\0"
    "\0on_powerOffPushButton_clicked\0"
    "on_rebootPushButton_clicked\0"
    "on_screenCapturePushButton_clicked\0"
    "stationStateChanged\0const QObject*\0"
    "obj"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DetailDialog[] = {

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
       1,    0,   39,    2, 0x09 /* Protected */,
       3,    0,   40,    2, 0x09 /* Protected */,
       4,    0,   41,    2, 0x09 /* Protected */,
       5,    0,   42,    2, 0x09 /* Protected */,
       6,    1,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void DetailDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DetailDialog *_t = static_cast<DetailDialog *>(_o);
        switch (_id) {
        case 0: _t->on_powerOnPushButton_clicked(); break;
        case 1: _t->on_powerOffPushButton_clicked(); break;
        case 2: _t->on_rebootPushButton_clicked(); break;
        case 3: _t->on_screenCapturePushButton_clicked(); break;
        case 4: _t->stationStateChanged((*reinterpret_cast< const QObject*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject DetailDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DetailDialog.data,
      qt_meta_data_DetailDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *DetailDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DetailDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DetailDialog.stringdata))
        return static_cast<void*>(const_cast< DetailDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DetailDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
