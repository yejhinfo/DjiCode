/****************************************************************************
** Meta object code from reading C++ file 'bridge.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bridge.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bridge.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_bridge_t {
    QByteArrayData data[13];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_bridge_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_bridge_t qt_meta_stringdata_bridge = {
    {
QT_MOC_LITERAL(0, 0, 6), // "bridge"
QT_MOC_LITERAL(1, 7, 11), // "sigYourName"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 4), // "name"
QT_MOC_LITERAL(4, 25, 11), // "AddMapPoint"
QT_MOC_LITERAL(5, 37, 3), // "lat"
QT_MOC_LITERAL(6, 41, 3), // "lon"
QT_MOC_LITERAL(7, 45, 15), // "AddMissionPoint"
QT_MOC_LITERAL(8, 61, 9), // "FinshFlag"
QT_MOC_LITERAL(9, 71, 10), // "showMsgBox"
QT_MOC_LITERAL(10, 82, 11), // "getYourName"
QT_MOC_LITERAL(11, 94, 13), // "getCoordinate"
QT_MOC_LITERAL(12, 108, 15) // "getMissionPoint"

    },
    "bridge\0sigYourName\0\0name\0AddMapPoint\0"
    "lat\0lon\0AddMissionPoint\0FinshFlag\0"
    "showMsgBox\0getYourName\0getCoordinate\0"
    "getMissionPoint"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_bridge[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    2,   52,    2, 0x06 /* Public */,
       7,    3,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   64,    2, 0x0a /* Public */,
      10,    0,   65,    2, 0x0a /* Public */,
      11,    2,   66,    2, 0x0a /* Public */,
      12,    3,   71,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    5,    6,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    6,    5,    8,

       0        // eod
};

void bridge::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        bridge *_t = static_cast<bridge *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigYourName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->AddMapPoint((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->AddMissionPoint((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 3: _t->showMsgBox(); break;
        case 4: _t->getYourName(); break;
        case 5: _t->getCoordinate((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->getMissionPoint((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (bridge::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&bridge::sigYourName)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (bridge::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&bridge::AddMapPoint)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (bridge::*_t)(QString , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&bridge::AddMissionPoint)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject bridge::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_bridge.data,
      qt_meta_data_bridge,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *bridge::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *bridge::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_bridge.stringdata0))
        return static_cast<void*>(const_cast< bridge*>(this));
    return QObject::qt_metacast(_clname);
}

int bridge::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void bridge::sigYourName(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void bridge::AddMapPoint(QString _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void bridge::AddMissionPoint(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
