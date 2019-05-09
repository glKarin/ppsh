/****************************************************************************
** Meta object code from reading C++ file 'dirmodel.h'
**
** Created: Wed May 8 18:45:38 2019
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../dirmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dirmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KMPlayer__DirModel[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       2,   29, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   38,   38,   38, 0x05,

 // methods: signature, parameters, type, tag, flags
      39,   38,   58,   38, 0x02,
      66,   38,   38,   38, 0x02,

 // properties: name, type, flags
      73,   58, 0x0a495003,
      83,   91, 0x01495001,

 // properties: notify_signal_id
       0,
       0,

       0        // eod
};

static const char qt_meta_stringdata_KMPlayer__DirModel[] = {
    "KMPlayer::DirModel\0directoryChanged()\0"
    "\0currentDirectory()\0QString\0goUp()\0"
    "directory\0canGoUp\0bool\0"
};

const QMetaObject KMPlayer::DirModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_KMPlayer__DirModel,
      qt_meta_data_KMPlayer__DirModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KMPlayer::DirModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KMPlayer::DirModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KMPlayer::DirModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMPlayer__DirModel))
        return static_cast<void*>(const_cast< DirModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int KMPlayer::DirModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: directoryChanged(); break;
        case 1: { QString _r = currentDirectory();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 2: goUp(); break;
        default: ;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = directory(); break;
        case 1: *reinterpret_cast< bool*>(_v) = canGoUp(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: enterDirectory(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void KMPlayer::DirModel::directoryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_KMPlayer__FileModel[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   39,   39,   39, 0x08,

 // methods: signature, parameters, type, tag, flags
      40,   39,   39,   39, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_KMPlayer__FileModel[] = {
    "KMPlayer::FileModel\0directoryChanged()\0"
    "\0updateFiles()\0"
};

const QMetaObject KMPlayer::FileModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_KMPlayer__FileModel,
      qt_meta_data_KMPlayer__FileModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KMPlayer::FileModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KMPlayer::FileModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KMPlayer::FileModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMPlayer__FileModel))
        return static_cast<void*>(const_cast< FileModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int KMPlayer::FileModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: directoryChanged(); break;
        case 1: updateFiles(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
