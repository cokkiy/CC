/****************************************************************************
** Meta object code from reading C++ file 'StationInfo.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CC-Client/StationInfo.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StationInfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_StationInfo_t {
    QByteArrayData data[44];
    char stringdata[513];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StationInfo_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StationInfo_t qt_meta_stringdata_StationInfo = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 15),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 12),
QT_MOC_LITERAL(4, 42, 14),
QT_MOC_LITERAL(5, 57, 5),
QT_MOC_LITERAL(6, 63, 12),
QT_MOC_LITERAL(7, 76, 14),
QT_MOC_LITERAL(8, 91, 3),
QT_MOC_LITERAL(9, 95, 6),
QT_MOC_LITERAL(10, 102, 6),
QT_MOC_LITERAL(11, 109, 9),
QT_MOC_LITERAL(12, 119, 14),
QT_MOC_LITERAL(13, 134, 11),
QT_MOC_LITERAL(14, 146, 6),
QT_MOC_LITERAL(15, 153, 9),
QT_MOC_LITERAL(16, 163, 15),
QT_MOC_LITERAL(17, 179, 4),
QT_MOC_LITERAL(18, 184, 12),
QT_MOC_LITERAL(19, 197, 7),
QT_MOC_LITERAL(20, 205, 6),
QT_MOC_LITERAL(21, 212, 7),
QT_MOC_LITERAL(22, 220, 5),
QT_MOC_LITERAL(23, 226, 15),
QT_MOC_LITERAL(24, 242, 24),
QT_MOC_LITERAL(25, 267, 11),
QT_MOC_LITERAL(26, 279, 8),
QT_MOC_LITERAL(27, 288, 11),
QT_MOC_LITERAL(28, 300, 11),
QT_MOC_LITERAL(29, 312, 9),
QT_MOC_LITERAL(30, 322, 14),
QT_MOC_LITERAL(31, 337, 15),
QT_MOC_LITERAL(32, 353, 15),
QT_MOC_LITERAL(33, 369, 15),
QT_MOC_LITERAL(34, 385, 13),
QT_MOC_LITERAL(35, 399, 10),
QT_MOC_LITERAL(36, 410, 13),
QT_MOC_LITERAL(37, 424, 17),
QT_MOC_LITERAL(38, 442, 13),
QT_MOC_LITERAL(39, 456, 10),
QT_MOC_LITERAL(40, 467, 8),
QT_MOC_LITERAL(41, 476, 7),
QT_MOC_LITERAL(42, 484, 12),
QT_MOC_LITERAL(43, 497, 15)
    },
    "StationInfo\0propertyChanged\0\0propertyName\0"
    "const QObject*\0owner\0stateChanged\0"
    "stationChanged\0CPU\0Memory\0AppCPU\0"
    "ProcCount\0AppThreadCount\0TotalMemory\0"
    "size_t\0AppMemory\0ExecuteCounting\0Name\0"
    "RunningState\0Unknown\0Normal\0Warning\0"
    "Error\0OperatingStatus\0PowerOffOrNetworkFailure\0"
    "NoHeartbeat\0Powering\0AppStarting\0"
    "Shutdowning\0Rebooting\0PowerOnFailure\0"
    "AppStartFailure\0AppCloseFailure\0"
    "ShutdownFailure\0RebootFailure\0AppStarted\0"
    "AppNotRunning\0SomeAppNotRunning\0"
    "MemoryTooHigh\0CPUTooHigh\0DiskFull\0"
    "Running\0GeneralError\0CanNotOperating"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StationInfo[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       9,   40, // properties
       2,   67, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x06 /* Public */,
       6,    1,   34,    2, 0x06 /* Public */,
       7,    1,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,

 // properties: name, type, flags
       8, QMetaType::Float, 0x00095103,
       9, QMetaType::Float, 0x00095103,
      10, QMetaType::Float, 0x00095103,
      11, QMetaType::Int, 0x00095103,
      12, QMetaType::Int, 0x00095103,
      13, 0x80000000 | 14, 0x0009510b,
      15, QMetaType::Float, 0x00095103,
      16, QMetaType::Int, 0x00095103,
      17, QMetaType::QString, 0x00095103,

 // enums: name, flags, count, data
      18, 0x0,    4,   75,
      23, 0x0,   20,   83,

 // enum data: key, value
      19, uint(StationInfo::Unknown),
      20, uint(StationInfo::Normal),
      21, uint(StationInfo::Warning),
      22, uint(StationInfo::Error),
      24, uint(StationInfo::PowerOffOrNetworkFailure),
      25, uint(StationInfo::NoHeartbeat),
      26, uint(StationInfo::Powering),
      27, uint(StationInfo::AppStarting),
      28, uint(StationInfo::Shutdowning),
      29, uint(StationInfo::Rebooting),
      30, uint(StationInfo::PowerOnFailure),
      31, uint(StationInfo::AppStartFailure),
      32, uint(StationInfo::AppCloseFailure),
      33, uint(StationInfo::ShutdownFailure),
      34, uint(StationInfo::RebootFailure),
      35, uint(StationInfo::AppStarted),
      36, uint(StationInfo::AppNotRunning),
      37, uint(StationInfo::SomeAppNotRunning),
      38, uint(StationInfo::MemoryTooHigh),
      39, uint(StationInfo::CPUTooHigh),
      40, uint(StationInfo::DiskFull),
      41, uint(StationInfo::Running),
      42, uint(StationInfo::GeneralError),
      43, uint(StationInfo::CanNotOperating),

       0        // eod
};

void StationInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StationInfo *_t = static_cast<StationInfo *>(_o);
        switch (_id) {
        case 0: _t->propertyChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QObject*(*)>(_a[2]))); break;
        case 1: _t->stateChanged((*reinterpret_cast< const QObject*(*)>(_a[1]))); break;
        case 2: _t->stationChanged((*reinterpret_cast< const QObject*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (StationInfo::*_t)(const QString & , const QObject * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StationInfo::propertyChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (StationInfo::*_t)(const QObject * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StationInfo::stateChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (StationInfo::*_t)(const QObject * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StationInfo::stationChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject StationInfo::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_StationInfo.data,
      qt_meta_data_StationInfo,  qt_static_metacall, 0, 0}
};


const QMetaObject *StationInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StationInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_StationInfo.stringdata))
        return static_cast<void*>(const_cast< StationInfo*>(this));
    return QObject::qt_metacast(_clname);
}

int StationInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< float*>(_v) = CPU(); break;
        case 1: *reinterpret_cast< float*>(_v) = Memory(); break;
        case 2: *reinterpret_cast< float*>(_v) = AppCPU(); break;
        case 3: *reinterpret_cast< int*>(_v) = ProcCount(); break;
        case 4: *reinterpret_cast< int*>(_v) = AppThreadCount(); break;
        case 5: *reinterpret_cast< size_t*>(_v) = TotalMemory(); break;
        case 6: *reinterpret_cast< float*>(_v) = AppMemory(); break;
        case 7: *reinterpret_cast< int*>(_v) = ExecuteCounting(); break;
        case 8: *reinterpret_cast< QString*>(_v) = Name(); break;
        default: break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCPU(*reinterpret_cast< float*>(_v)); break;
        case 1: setMemory(*reinterpret_cast< float*>(_v)); break;
        case 2: setAppCPU(*reinterpret_cast< float*>(_v)); break;
        case 3: setProcCount(*reinterpret_cast< int*>(_v)); break;
        case 4: setAppThreadCount(*reinterpret_cast< int*>(_v)); break;
        case 5: setTotalMemory(*reinterpret_cast< size_t*>(_v)); break;
        case 6: setAppMemory(*reinterpret_cast< float*>(_v)); break;
        case 7: setExecuteCounting(*reinterpret_cast< int*>(_v)); break;
        case 8: setName(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void StationInfo::propertyChanged(const QString & _t1, const QObject * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void StationInfo::stateChanged(const QObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void StationInfo::stationChanged(const QObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
