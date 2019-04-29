TEMPLATE = app
TARGET = "kgstplayer"
DEPENDPATH += .
INCLUDEPATH += .

PPSH = ppsh

# Input
SOURCES = gstplayer.c
unix {
  #VARIABLES
  isEmpty(PREFIX) {
        #PREFIX = /opt/kmplayer
        PREFIX = /opt/$${PPSH}/kmplayer++
  }
  CONFIG = link_pkgconfig
  PKGCONFIG += gio-2.0
  PKGCONFIG += gstreamer-0.10
  PKGCONFIG += gstreamer-interfaces-0.10
  PKGCONFIG += x11
BINDIR = $$PREFIX/bin
DATADIR =$$PREFIX/share
QMAKE_CFLAGS += -fwhole-program
DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

#MAKE INSTALL

INSTALLS += target

  target.path =$$BINDIR

}
