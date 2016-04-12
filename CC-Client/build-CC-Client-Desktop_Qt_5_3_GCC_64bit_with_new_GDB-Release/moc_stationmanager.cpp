/****************************************************************************
** Meta object code from reading C++ file 'stationmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CC-Client/stationmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stationmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_StationManager_t {
    QByteArrayData data[17];
    char stringdata[172];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StationManager_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StationManager_t qt_meta_stringdata_StationManager = {
    {
QT_MOC_LITERAL(0, 0, 14),
QT_MOC_LITERAL(1, 15, 10),
QT_MOC_LITERAL(2, 26, 0),
QT_MOC_LITERAL(3, 27, 7),
QT_MOC_LITERAL(4, 35, 6),
QT_MOC_LITERAL(5, 42, 8),
QT_MOC_LITERAL(6, 51, 9),
QT_MOC_LITERAL(7, 61, 11),
QT_MOC_LITERAL(8, 73, 10),
QT_MOC_LITERAL(9, 84, 13),
QT_MOC_LITERAL(10, 98, 7),
QT_MOC_LITERAL(11, 106, 10),
QT_MOC_LITERAL(12, 117, 8),
QT_MOC_LITERAL(13, 126, 11),
QT_MOC_LITERAL(14, 138, 12),
QT_MOC_LITERAL(15, 151, 11),
QT_MOC_LITERAL(16, 163, 8)
    },
    "StationManager\0powerOnAll\0\0powerOn\0"
    "reboot\0shutdown\0rebootAll\0shutdownAll\0"
    "restartApp\0restartAllApp\0exitApp\0"
    "exitAllApp\0startApp\0startAllApp\0"
    "forceExitApp\0setInterval\0interval"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StationManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x0a /* Public */,
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    0,   87,    2, 0x0a /* Public */,
       6,    0,   88,    2, 0x0a /* Public */,
       7,    0,   89,    2, 0x0a /* Public */,
       8,    0,   90,    2, 0x0a /* Public */,
       9,    0,   91,    2, 0x0a /* Public */,
      10,    0,   92,    2, 0x0a /* Public */,
      11,    0,   93,    2, 0x0a /* Public */,
      12,    0,   94,    2, 0x0a /* Public */,
      13,    0,   95,    2, 0x0a /* Public */,
      14,    0,   96,    2, 0x0a /* Public */,
      15,    1,   97,    2, 0x0a /* Public */,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   16,

       0        // eod
};

void StationManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StationManager *_t = static_cast<StationManager *>(_o);
        switch (_id) {
        case 0: _t->powerOnAll(); break;
        case 1: _t->powerOn(); break;
        case 2: _t->reboot(); break;
        case 3: _t->shutdown(); break;
        case 4: _t->rebootAll(); break;
        case 5: _t->shutdownAll(); break;
        case 6: _t->restartApp(); break;
        case 7: _t->restartAllApp(); break;
        case 8: _t->exitApp(); break;
        case 9: _t->exitAllApp(); break;
        case 10: _t->startApp(); break;
        case 11: _t->startAllApp(); break;
        case 12: _t->forceExitApp(); break;
        case 13: _t->setInterval((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject StationManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_StationManager.data,
      qt_meta_data_StationManager,  qt_static_metacall, 0, 0}
};


const QMetaObject *StationManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StationManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_StationManager.stringdata))
        return static_cast<void*>(const_cast< StationManager*>(this));
    return QObject::qt_metacast(_clname);
}

int StationManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
