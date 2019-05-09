/****************************************************************************
** Meta object code from reading C++ file 'viewarea.h'
**
** Created: Wed May 8 18:45:54 2019
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../viewarea.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewarea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KMPlayer__Button[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   27,   27,   27, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_KMPlayer__Button[] = {
    "KMPlayer::Button\0clicked()\0\0"
};

const QMetaObject KMPlayer::Button::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KMPlayer__Button,
      qt_meta_data_KMPlayer__Button, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KMPlayer::Button::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KMPlayer::Button::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KMPlayer::Button::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMPlayer__Button))
        return static_cast<void*>(const_cast< Button*>(this));
    return QWidget::qt_metacast(_clname);
}

int KMPlayer::Button::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clicked(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void KMPlayer::Button::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_KMPlayer__ViewArea[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   44,   44,   44, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_KMPlayer__ViewArea[] = {
    "KMPlayer::ViewArea\0videoWidgetVisible(bool)\0"
    "\0"
};

const QMetaObject KMPlayer::ViewArea::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KMPlayer__ViewArea,
      qt_meta_data_KMPlayer__ViewArea, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KMPlayer::ViewArea::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KMPlayer::ViewArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KMPlayer::ViewArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMPlayer__ViewArea))
        return static_cast<void*>(const_cast< ViewArea*>(this));
    return QWidget::qt_metacast(_clname);
}

int KMPlayer::ViewArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: videoWidgetVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void KMPlayer::ViewArea::videoWidgetVisible(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_KMPlayer__VideoOutput[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_KMPlayer__VideoOutput[] = {
    "KMPlayer::VideoOutput\0"
};

const QMetaObject KMPlayer::VideoOutput::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KMPlayer__VideoOutput,
      qt_meta_data_KMPlayer__VideoOutput, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KMPlayer::VideoOutput::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KMPlayer::VideoOutput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KMPlayer::VideoOutput::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMPlayer__VideoOutput))
        return static_cast<void*>(const_cast< VideoOutput*>(this));
    return QObject::qt_metacast(_clname);
}

int KMPlayer::VideoOutput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
