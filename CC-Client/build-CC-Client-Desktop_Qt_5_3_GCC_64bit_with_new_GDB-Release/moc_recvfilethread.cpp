/****************************************************************************
** Meta object code from reading C++ file 'recvfilethread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CC-Client/recvfilethread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'recvfilethread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RecvFileThread_t {
    QByteArrayData data[13];
    char stringdata[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RecvFileThread_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RecvFileThread_t qt_meta_stringdata_RecvFileThread = {
    {
QT_MOC_LITERAL(0, 0, 14),
QT_MOC_LITERAL(1, 15, 14),
QT_MOC_LITERAL(2, 30, 0),
QT_MOC_LITERAL(3, 31, 12),
QT_MOC_LITERAL(4, 44, 7),
QT_MOC_LITERAL(5, 52, 4),
QT_MOC_LITERAL(6, 57, 12),
QT_MOC_LITERAL(7, 70, 7),
QT_MOC_LITERAL(8, 78, 15),
QT_MOC_LITERAL(9, 94, 8),
QT_MOC_LITERAL(10, 103, 17),
QT_MOC_LITERAL(11, 121, 9),
QT_MOC_LITERAL(12, 131, 6)
    },
    "RecvFileThread\0notifyFileSize\0\0"
    "StationInfo*\0station\0size\0getDataError\0"
    "message\0createFileError\0fileName\0"
    "transFileComplete\0fileGeted\0length"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RecvFileThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       6,    2,   44,    2, 0x06 /* Public */,
       8,    3,   49,    2, 0x06 /* Public */,
      10,    1,   56,    2, 0x06 /* Public */,
      11,    2,   59,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::LongLong,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    7,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::QString,    4,    9,    7,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::LongLong,    4,   12,

       0        // eod
};

void RecvFileThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RecvFileThread *_t = static_cast<RecvFileThread *>(_o);
        switch (_id) {
        case 0: _t->notifyFileSize((*reinterpret_cast< StationInfo*(*)>(_a[1])),(*reinterpret_cast< long long(*)>(_a[2]))); break;
        case 1: _t->getDataError((*reinterpret_cast< StationInfo*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->createFileError((*reinterpret_cast< StationInfo*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 3: _t->transFileComplete((*reinterpret_cast< StationInfo*(*)>(_a[1]))); break;
        case 4: _t->fileGeted((*reinterpret_cast< StationInfo*(*)>(_a[1])),(*reinterpret_cast< long long(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< StationInfo* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< StationInfo* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< StationInfo* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< StationInfo* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< StationInfo* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RecvFileThread::*_t)(StationInfo * , long long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecvFileThread::notifyFileSize)) {
                *result = 0;
            }
        }
        {
            typedef void (RecvFileThread::*_t)(StationInfo * , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecvFileThread::getDataError)) {
                *result = 1;
            }
        }
        {
            typedef void (RecvFileThread::*_t)(StationInfo * , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecvFileThread::createFileError)) {
                *result = 2;
            }
        }
        {
            typedef void (RecvFileThread::*_t)(StationInfo * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecvFileThread::transFileComplete)) {
                *result = 3;
            }
        }
        {
            typedef void (RecvFileThread::*_t)(StationInfo * , long long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RecvFileThread::fileGeted)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject RecvFileThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_RecvFileThread.data,
      qt_meta_data_RecvFileThread,  qt_static_metacall, 0, 0}
};


const QMetaObject *RecvFileThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RecvFileThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RecvFileThread.stringdata))
        return static_cast<void*>(const_cast< RecvFileThread*>(this));
    return QThread::qt_metacast(_clname);
}

int RecvFileThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RecvFileThread::notifyFileSize(StationInfo * _t1, long long _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RecvFileThread::getDataError(StationInfo * _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RecvFileThread::createFileError(StationInfo * _t1, QString _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RecvFileThread::transFileComplete(StationInfo * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void RecvFileThread::fileGeted(StationInfo * _t1, long long _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
