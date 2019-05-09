/****************************************************************************
** Meta object code from reading C++ file 'control.h'
**
** Created: Wed May 8 18:45:42 2019
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../control.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'control.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KMPlayer__Control[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      38,   14, // methods
       4,  204, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   38,   38,   38, 0x05,
      39,   38,   38,   38, 0x05,
      59,   38,   38,   38, 0x05,
      84,  105,   38,   38, 0x05,
     114,  145,   38,   38, 0x05,
     151,  191,   38,   38, 0x05,
     211,  237,   38,   38, 0x05,
     248,  270,   38,   38, 0x05,
     275,   38,   38,   38, 0x05,
     290,  322,   38,   38, 0x05,

 // slots: signature, parameters, type, tag, flags
     333,   38,   38,   38, 0x0a,
     351,   38,   38,   38, 0x0a,
     361,   38,   38,   38, 0x0a,
     368,   38,   38,   38, 0x0a,
     382,   38,   38,   38, 0x0a,
     390,   38,   38,   38, 0x0a,
     415,   38,   38,   38, 0x0a,
     422,   38,   38,   38, 0x0a,
     451,   38,   38,   38, 0x0a,
     477,   38,   38,   38, 0x0a,
     517,   38,   38,   38, 0x0a,
     544,  594,   38,   38, 0x08,
     604,  661,   38,   38, 0x08,

 // methods: signature, parameters, type, tag, flags
     670,  688,   38,   38, 0x02,
     697,  718,   38,   38, 0x02,
     728,  745,   38,   38, 0x02,
     747,  769,   38,   38, 0x02,
     776,  745,   38,   38, 0x02,
     797,  745,   38,   38, 0x02,
     818,  844,   38,   38, 0x02,
     854,   38,   38,   38, 0x02,
     868,  901,   38,   38, 0x02,
     919,  934,   38,   38, 0x02,
     949,  963,   38,   38, 0x02,
     965,   38,   38,   38, 0x02,
     990,   38,   38,   38, 0x02,
    1003,   38,   38,   38, 0x02,
    1014, 1040,   38,   38, 0x02,

 // properties: name, type, flags
    1051, 1062, 0x02495003,
     105, 1062, 0x02495103,
    1066, 1062, 0x02495103,
    1077, 1062, 0x02495001,

 // properties: notify_signal_id
       0,
       3,
       1,
       2,

       0        // eod
};

static const char qt_meta_stringdata_KMPlayer__Control[] = {
    "KMPlayer::Control\0loadStatusChanged()\0"
    "\0playStatusChanged()\0videoVisibilityChanged()\0"
    "positionChanged(int)\0position\0"
    "selectContextItem(QStringList)\0items\0"
    "selectItem(QString,QString,QStringList)\0"
    "context,title,items\0showEdit(QString,QString)\0"
    "title,text\0showErrorMsg(QString)\0text\0"
    "showFileOpen()\0showSaveFileAs(QString,QString)\0"
    "title,file\0openInitialUrls()\0panZoom()\0"
    "play()\0playCurrent()\0scale()\0"
    "videoWidgetVisible(bool)\0Stop()\0"
    "transferCancelled(Transfer*)\0"
    "transferPaused(Transfer*)\0"
    "transferErrorRepairRequested(Transfer*)\0"
    "transferResumed(Transfer*)\0"
    "keyEvent(MeeGo::QmKeys::Key,MeeGo::QmKeys::State)\0"
    "key,state\0"
    "locksChanged(MeeGo::QmLocks::Lock,MeeGo::QmLocks::State)\0"
    "what,how\0openFile(QString)\0filename\0"
    "itemClicked(int,int)\0i,current\0"
    "itemContext(int)\0i\0itemEdit(int,QString)\0"
    "i,text\0downloadClicked(int)\0"
    "contextSelected(int)\0itemSelected(QString,int)\0"
    "context,i\0menuClicked()\0"
    "saveFileAs(QString,QString,bool)\0"
    "title,file,remove\0Seek(int,bool)\0"
    "value,promille\0log(QVariant)\0s\0"
    "directoryChosen(QString)\0raiseVideo()\0"
    "uiLoaded()\0openFile(QString,QString)\0"
    "filename,a\0loadStatus\0int\0playStatus\0"
    "videoVisible\0"
};

const QMetaObject KMPlayer::Control::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KMPlayer__Control,
      qt_meta_data_KMPlayer__Control, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KMPlayer::Control::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KMPlayer::Control::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KMPlayer::Control::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KMPlayer__Control))
        return static_cast<void*>(const_cast< Control*>(this));
    if (!strcmp(_clname, "PlayListNotify"))
        return static_cast< PlayListNotify*>(const_cast< Control*>(this));
    if (!strcmp(_clname, "ProcessNotify"))
        return static_cast< ProcessNotify*>(const_cast< Control*>(this));
    return QObject::qt_metacast(_clname);
}

int KMPlayer::Control::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadStatusChanged(); break;
        case 1: playStatusChanged(); break;
        case 2: videoVisibilityChanged(); break;
        case 3: positionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: selectContextItem((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 5: selectItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3]))); break;
        case 6: showEdit((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: showErrorMsg((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: showFileOpen(); break;
        case 9: showSaveFileAs((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 10: openInitialUrls(); break;
        case 11: panZoom(); break;
        case 12: play(); break;
        case 13: playCurrent(); break;
        case 14: scale(); break;
        case 15: videoWidgetVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: Stop(); break;
        case 17: transferCancelled((*reinterpret_cast< Transfer*(*)>(_a[1]))); break;
        case 18: transferPaused((*reinterpret_cast< Transfer*(*)>(_a[1]))); break;
        case 19: transferErrorRepairRequested((*reinterpret_cast< Transfer*(*)>(_a[1]))); break;
        case 20: transferResumed((*reinterpret_cast< Transfer*(*)>(_a[1]))); break;
        case 21: keyEvent((*reinterpret_cast< MeeGo::QmKeys::Key(*)>(_a[1])),(*reinterpret_cast< MeeGo::QmKeys::State(*)>(_a[2]))); break;
        case 22: locksChanged((*reinterpret_cast< MeeGo::QmLocks::Lock(*)>(_a[1])),(*reinterpret_cast< MeeGo::QmLocks::State(*)>(_a[2]))); break;
        case 23: openFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 24: itemClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 25: itemContext((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: itemEdit((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 27: downloadClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: contextSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: itemSelected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 30: menuClicked(); break;
        case 31: saveFileAs((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 32: Seek((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 33: log((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 34: directoryChosen((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 35: raiseVideo(); break;
        case 36: uiLoaded(); break;
        case 37: openFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 38;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = loadStatus(); break;
        case 1: *reinterpret_cast< int*>(_v) = position(); break;
        case 2: *reinterpret_cast< int*>(_v) = playStatus(); break;
        case 3: *reinterpret_cast< int*>(_v) = videoVisible(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setLoading(*reinterpret_cast< int*>(_v)); break;
        case 1: setPosition(*reinterpret_cast< int*>(_v)); break;
        case 2: setPlayStatus(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void KMPlayer::Control::loadStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void KMPlayer::Control::playStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void KMPlayer::Control::videoVisibilityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void KMPlayer::Control::positionChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KMPlayer::Control::selectContextItem(const QStringList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KMPlayer::Control::selectItem(const QString & _t1, const QString & _t2, const QStringList & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void KMPlayer::Control::showEdit(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void KMPlayer::Control::showErrorMsg(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void KMPlayer::Control::showFileOpen()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void KMPlayer::Control::showSaveFileAs(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
