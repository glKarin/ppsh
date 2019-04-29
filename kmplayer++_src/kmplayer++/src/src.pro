TEMPLATE = app
QT += opengl declarative
TARGET = "kmplayer++"
DEPENDPATH += .
INCLUDEPATH += .

DEFINES += _HARMATTAN
contains(DEFINES, _HARMATTAN) {
	LIBS += -lqmsystem2
	INCLUDEPATH += /usr/include/qmsystem2

	PPSH = ppsh
	DEFINES += _HARMATTAN_KMPLAYER=\\\"$${TARGET}\\\"

	MOC_DIR = .moc
	OBJECTS_DIR = .obj
}
else {
  PKGCONFIG += qmsystem2
}

# Input
HEADERS += io.h log.h dirmodel.h \
    control.h playlist.h playmodel.h player.h lists.h mediaobject.h \
    surface.h viewarea.h triestring.h expression.h \
    kmplayer_asx.h kmplayer_atom.h kmplayer_rss.h kmplayer_xspf.h \
    kmplayer_rp.h kmplayer_smil.h kmplayer_opml.h
SOURCES += main.cpp io.cpp log.cpp dirmodel.cpp \
    control.cpp playlist.cpp playmodel.cpp player.cpp lists.cpp mediaobject.cpp \
    surface.cpp viewarea.cpp triestring.cpp expression.cpp \
    kmplayer_asx.cpp kmplayer_atom.cpp kmplayer_rss.cpp kmplayer_xspf.cpp \
    kmplayer_rp.cpp kmplayer_smil.cpp kmplayer_opml.cpp
#FORMS#

unix {
  #VARIABLES
  OPTDIR = $$PREFIX/$${PPSH}
  isEmpty(PREFIX) {
        PREFIX = /usr
        OPTDIR = /opt/$${PPSH}
  }
  CONFIG += link_pkgconfig
  PKGCONFIG += gio-2.0
  PKGCONFIG += cairo-xlib-xrender
  PKGCONFIG += libcurl
  PKGCONFIG += conic
  PKGCONFIG += gconf-2.0
  #PKGCONFIG += qmsystem2
  PKGCONFIG += libtuiclient
BINDIR = $$OPTDIR/$${TARGET}/bin
DATADIR =$$PREFIX/share
I18NDIR =$$OPTDIR/$${TARGET}/i18n
QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti -fno-check-new -fvisibility=hidden -fvisibility-inlines-hidden
DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
DEFINES += OPTDIR=\\\"$$OPTDIR\\\"
qmfiles.files = kmplayer_nl.qm kmplayer_zh_CN.qm
qmfiles.path = $$I18NDIR

#MAKE INSTALL

#INSTALLS += target qmlgui listdata desktop service iconxpm icon26 icon48 icon64 icon80 syspart
INSTALLS += target qmlgui listdata desktop service icon80 syspart qmfiles \
mplayer

  target.path =$$BINDIR

  qmlgui.path = $$OPTDIR/$${TARGET}
  qmlgui.files += kmplayer.qml
  qmlgui.files += splash.qml
  qmlgui.files += MainPage.qml
  qmlgui.files += DirectoryDialog.qml
  qmlgui.files += FileDialog.qml
  qmlgui.files += QueryInput.qml
  qmlgui.files += SaveAsDialog.qml
  qmlgui.files += SingleSelectionDialog.qml

  listdata.path = $$OPTDIR/$${TARGET}/share
  listdata.files += ../data/playlist.xml

  desktop.path = $$DATADIR/applications
  desktop.files += $${TARGET}.desktop

  service.path = $$DATADIR/dbus-1/services/
  service.files += com.meego.$${TARGET}.service

  syspart.path = $$DATADIR/policy/etc/syspart.conf.d
  syspart.files += $${TARGET}.conf

  iconxpm.path = $$DATADIR/pixmap
  iconxpm.files += ../data/maemo/$${TARGET}.xpm

  icon26.path = $$DATADIR/icons/hicolor/26x26/apps
  icon26.files += ../data/26x26/$${TARGET}.png

  icon48.path = $$DATADIR/icons/hicolor/48x48/apps
  icon48.files += ../data/48x48/$${TARGET}.png

  icon64.path = $$DATADIR/icons/hicolor/64x64/apps
  icon64.files += ../data/64x64/$${TARGET}.png

  icon80.path = $$OPTDIR/$${TARGET}/share
  icon80.files += ../data/80x80/$${TARGET}.svg


# Add mplayer binary 2012
  mplayer.path = $$OPTDIR/$${TARGET}/bin
  mplayer.files += ./mplayer
}
