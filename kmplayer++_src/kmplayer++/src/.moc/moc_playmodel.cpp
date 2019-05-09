/****************************************************************************
** Meta object code from reading C++ file 'playmodel.h'
**
** Created: Wed May 8 18:45:50 2019
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../playmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KMPlayer__PlayModel[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   46,   59,   59, 0x05,

 // slots: signature, parameters, type, tag, flags
      60,   92,   59,   59, 0x0a,
     107,   59,   59,   59, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_KMPlayer__PlayModel[] = {
    "KMPlayer::PlayModel\0selectionChanged(int,int)\0"
    "index,scroll\0\0updateTree(int,NodePtr,NodePtr)\0"
    "id,root,active\0updateTrees()\0"
};

const QMetaObject KMPlayer::PlayModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_KMPlayer__PlayModel,
      qt_meta_data_KMPlayer__PlayModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KMPlayer::PlayModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KMPlayer::PlayModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KMPlayer::PlayModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMPlayer__PlayModel))
        return static_cast<void*>(const_cast< PlayModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int KMPlayer::PlayModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selectionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: updateTree((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NodePtr(*)>(_a[2])),(*reinterpret_cast< NodePtr(*)>(_a[3]))); break;
        case 2: updateTrees(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void KMPlayer::PlayModel::selectionChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
