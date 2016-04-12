/****************************************************************************
** Meta object code from reading C++ file 'defaultappprocdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CC-Client/defaultappprocdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'defaultappprocdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DefaultAppProcDialog_t {
    QByteArrayData data[8];
    char stringdata[186];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DefaultAppProcDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DefaultAppProcDialog_t qt_meta_stringdata_DefaultAppProcDialog = {
    {
QT_MOC_LITERAL(0, 0, 20),
QT_MOC_LITERAL(1, 21, 23),
QT_MOC_LITERAL(2, 45, 0),
QT_MOC_LITERAL(3, 46, 30),
QT_MOC_LITERAL(4, 77, 24),
QT_MOC_LITERAL(5, 102, 31),
QT_MOC_LITERAL(6, 134, 23),
QT_MOC_LITERAL(7, 158, 27)
    },
    "DefaultAppProcDialog\0on_addAppButton_clicked\0"
    "\0on_removeAppPushButton_clicked\0"
    "on_addProcButton_clicked\0"
    "on_removeProcPushButton_clicked\0"
    "on_OKPushButton_clicked\0"
    "on_canclePushButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DefaultAppProcDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x08 /* Private */,
       7,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DefaultAppProcDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DefaultAppProcDialog *_t = static_cast<DefaultAppProcDialog *>(_o);
        switch (_id) {
        case 0: _t->on_addAppButton_clicked(); break;
        case 1: _t->on_removeAppPushButton_clicked(); break;
        case 2: _t->on_addProcButton_clicked(); break;
        case 3: _t->on_removeProcPushButton_clicked(); break;
        case 4: _t->on_OKPushButton_clicked(); break;
        case 5: _t->on_canclePushButton_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject DefaultAppProcDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DefaultAppProcDialog.data,
      qt_meta_data_DefaultAppProcDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *DefaultAppProcDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DefaultAppProcDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DefaultAppProcDialog.stringdata))
        return static_cast<void*>(const_cast< DefaultAppProcDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DefaultAppProcDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
