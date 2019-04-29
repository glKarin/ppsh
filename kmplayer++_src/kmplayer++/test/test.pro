    QT += network
    TEMPLATE = app
    SOURCES += urltest.cpp ../src/log.cpp ../src/io.cpp
    HEADERS += test.h
    TARGET = urltest

unix {
  CONFIG += link_pkgconfig
  PKGCONFIG += gio-2.0
  PKGCONFIG += libcurl
  PKGCONFIG += conic
}
