#############################################################################
# Makefile for building: ppsh
# Generated by qmake (2.01a) (Qt 4.7.4) on: ?? 4? 27 22:49:49 2019
# Project:  ppsh.pro
# Template: app
# Command: /usr/bin/qmake PREFIX=/usr -o Makefile ppsh.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -D_KARIN_MM_EXTENSIONS -D_HARMATTAN -D_MAEMO_MEEGOTOUCH_INTERFACES_DEV -DHARMATTAN_BOOSTER -DM_APPLICATION_NAME=\"${QMAKE_TARGET}\" -DQT_NO_DEBUG -DQT_DECLARATIVE_LIB -DQT_WEBKIT_LIB -DQT_DBUS_LIB -DQT_SQL_LIB -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -g -Wno-psabi -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -I/usr/include/applauncherd -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -g -Wno-psabi -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -I/usr/include/applauncherd -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-maemo -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtOpenGL -I/usr/include/qt4/QtXml -I/usr/include/qt4/QtSql -I/usr/include/qt4/QtDBus -I/usr/include/qt4/QtWebKit -I/usr/include/qt4/QtDeclarative -I/usr/include/qt4 -I/usr/include/qt4/QtMultimediaKit -I/usr/include/qt4/maemomeegotouchvideosuiteinterface -I. -Isrc -Isrc/qtm -Iqmlapplicationviewer -I/usr/include/maemo-meegotouch-interfaces -I/usr/include/meegotouch -I/usr/include/qt4 -I/usr/include/qt4/QtMobility -I/usr/X11R6/include -I.moc
LINK          = g++
LFLAGS        = -Wl,-O1 -Wl,-rpath,/usr/lib
LIBS          = $(SUBLIBS)  -L/usr/lib -L/usr/X11R6/lib -L/usr/lib -lmaemomeegotouchvideosuiteinterface -lmeegotouchcore -lmeegotouchsettings -lmeegotouchextensions -lQtMultimediaKit -lz -pie -rdynamic -lmdeclarativecache -lQtDeclarative -lQtWebKit -lQtDBus -lQtSql -lQtXml -lQtOpenGL -lQtGui -lQtNetwork -lQtCore -lGLESv2 -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = .obj/

####### Files

SOURCES       = src/qtm/qdeclarativemediabase.cpp \
		src/qtm/qdeclarativevideo.cpp \
		src/qtm/qpaintervideosurface.cpp \
		main.cpp \
		src/utility.cpp \
		src/networkmanager.cpp \
		src/networkconnector.cpp \
		src/id_std.cpp \
		src/player.cpp \
		src/qtm/qdeclarativewebview.cpp \
		qmlapplicationviewer/qmlapplicationviewer.cpp .moc/moc_qdeclarativemediametadata_p.cpp \
		.moc/moc_qmlapplicationviewer.cpp \
		.moc/moc_utility.cpp \
		.moc/moc_networkmanager.cpp \
		.moc/moc_networkconnector.cpp \
		.moc/moc_player.cpp \
		.moc/moc_qdeclarativewebview.cpp
OBJECTS       = .obj/qdeclarativemediabase.o \
		.obj/qdeclarativevideo.o \
		.obj/qpaintervideosurface.o \
		.obj/main.o \
		.obj/utility.o \
		.obj/networkmanager.o \
		.obj/networkconnector.o \
		.obj/id_std.o \
		.obj/player.o \
		.obj/qdeclarativewebview.o \
		.obj/qmlapplicationviewer.o \
		.obj/moc_qdeclarativemediametadata_p.o \
		.obj/moc_qmlapplicationviewer.o \
		.obj/moc_utility.o \
		.obj/moc_networkmanager.o \
		.obj/moc_networkconnector.o \
		.obj/moc_player.o \
		.obj/moc_qdeclarativewebview.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		qmlapplicationviewer/qmlapplicationviewer.pri \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/videosuiteinterface-maemo-meegotouch.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_defines.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mgen.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mmoc.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_common.prf \
		/usr/share/qt4/mkspecs/features/meegotouch.prf \
		/usr/share/qt4/mkspecs/features/mobility.prf \
		/usr/share/qt4/mkspecs/features/qdeclarative-boostable.prf \
		/usr/share/qt4/mkspecs/features/link_pkgconfig.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/dbusinterfaces.prf \
		/usr/share/qt4/mkspecs/features/dbusadaptors.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		ppsh.pro
QMAKE_TARGET  = ppsh
DESTDIR       = 
TARGET        = ppsh

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: ppsh.pro  /usr/share/qt4/mkspecs/linux-g++-maemo/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		qmlapplicationviewer/qmlapplicationviewer.pri \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/videosuiteinterface-maemo-meegotouch.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_defines.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mgen.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_mmoc.prf \
		/usr/share/qt4/mkspecs/features/meegotouch_common.prf \
		/usr/share/qt4/mkspecs/features/meegotouch.prf \
		/usr/share/qt4/mkspecs/features/mobility.prf \
		/usr/share/qt4/mkspecs/features/qdeclarative-boostable.prf \
		/usr/share/qt4/mkspecs/features/link_pkgconfig.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/dbusinterfaces.prf \
		/usr/share/qt4/mkspecs/features/dbusadaptors.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQtDeclarative.prl \
		/usr/lib/libQtWebKit.prl \
		/usr/lib/libQtDBus.prl \
		/usr/lib/libQtSql.prl \
		/usr/lib/libQtXml.prl \
		/usr/lib/libQtOpenGL.prl \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtNetwork.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) PREFIX=/usr -o Makefile ppsh.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
qmlapplicationviewer/qmlapplicationviewer.pri:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/videosuiteinterface-maemo-meegotouch.prf:
/usr/share/qt4/mkspecs/features/meegotouch_defines.prf:
/usr/share/qt4/mkspecs/features/meegotouch_mgen.prf:
/usr/share/qt4/mkspecs/features/meegotouch_mmoc.prf:
/usr/share/qt4/mkspecs/features/meegotouch_common.prf:
/usr/share/qt4/mkspecs/features/meegotouch.prf:
/usr/share/qt4/mkspecs/features/mobility.prf:
/usr/share/qt4/mkspecs/features/qdeclarative-boostable.prf:
/usr/share/qt4/mkspecs/features/link_pkgconfig.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/opengl.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/dbusinterfaces.prf:
/usr/share/qt4/mkspecs/features/dbusadaptors.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQtDeclarative.prl:
/usr/lib/libQtWebKit.prl:
/usr/lib/libQtDBus.prl:
/usr/lib/libQtSql.prl:
/usr/lib/libQtXml.prl:
/usr/lib/libQtOpenGL.prl:
/usr/lib/libQtGui.prl:
/usr/lib/libQtNetwork.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) PREFIX=/usr -o Makefile ppsh.pro

dist: 
	@$(CHK_DIR_EXISTS) .obj/ppsh1.0.0 || $(MKDIR) .obj/ppsh1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .obj/ppsh1.0.0/ && $(COPY_FILE) --parents src/qtm/qdeclarativemediabase_p.h src/qtm/qdeclarativevideo_p.h src/qtm/qdeclarativemediametadata_p.h src/qtm/qpaintervideosurface_p.h qmlapplicationviewer/qmlapplicationviewer.h src/utility.h src/networkmanager.h src/networkconnector.h src/id_std.h src/player.h src/qtm/qdeclarativewebview.h .obj/ppsh1.0.0/ && $(COPY_FILE) --parents src/qtm/qdeclarativemediabase.cpp src/qtm/qdeclarativevideo.cpp src/qtm/qpaintervideosurface.cpp main.cpp src/utility.cpp src/networkmanager.cpp src/networkconnector.cpp src/id_std.cpp src/player.cpp src/qtm/qdeclarativewebview.cpp qmlapplicationviewer/qmlapplicationviewer.cpp .obj/ppsh1.0.0/ && (cd `dirname .obj/ppsh1.0.0` && $(TAR) ppsh1.0.0.tar ppsh1.0.0 && $(COMPRESS) ppsh1.0.0.tar) && $(MOVE) `dirname .obj/ppsh1.0.0`/ppsh1.0.0.tar.gz . && $(DEL_FILE) -r .obj/ppsh1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_mgenerator_model_make_all:
compiler_mgenerator_model_clean:
	-$(DEL_FILE) ./gen_*
compiler_mgenerator_style_make_all:
compiler_mgenerator_style_clean:
	-$(DEL_FILE) ./gen_*
compiler_moc_header_make_all: .moc/moc_qdeclarativevideo_p.cpp .moc/moc_qdeclarativemediametadata_p.cpp .moc/moc_qpaintervideosurface_p.cpp .moc/moc_qmlapplicationviewer.cpp .moc/moc_utility.cpp .moc/moc_networkmanager.cpp .moc/moc_networkconnector.cpp .moc/moc_player.cpp .moc/moc_qdeclarativewebview.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) .moc/moc_qdeclarativevideo_p.cpp .moc/moc_qdeclarativemediametadata_p.cpp .moc/moc_qpaintervideosurface_p.cpp .moc/moc_qmlapplicationviewer.cpp .moc/moc_utility.cpp .moc/moc_networkmanager.cpp .moc/moc_networkconnector.cpp .moc/moc_player.cpp .moc/moc_qdeclarativewebview.cpp
.moc/moc_qdeclarativevideo_p.cpp: src/qtm/qdeclarativemediabase_p.h \
		src/qtm/qdeclarativevideo_p.h
	mmoc.pl $(DEFINES) $(INCPATH) src/qtm/qdeclarativevideo_p.h -o .moc/moc_qdeclarativevideo_p.cpp

.moc/moc_qdeclarativemediametadata_p.cpp: src/qtm/qdeclarativemediametadata_p.h
	mmoc.pl $(DEFINES) $(INCPATH) src/qtm/qdeclarativemediametadata_p.h -o .moc/moc_qdeclarativemediametadata_p.cpp

.moc/moc_qpaintervideosurface_p.cpp: src/qtm/qpaintervideosurface_p.h
	mmoc.pl $(DEFINES) $(INCPATH) src/qtm/qpaintervideosurface_p.h -o .moc/moc_qpaintervideosurface_p.cpp

.moc/moc_qmlapplicationviewer.cpp: qmlapplicationviewer/qmlapplicationviewer.h
	mmoc.pl $(DEFINES) $(INCPATH) qmlapplicationviewer/qmlapplicationviewer.h -o .moc/moc_qmlapplicationviewer.cpp

.moc/moc_utility.cpp: src/utility.h
	mmoc.pl $(DEFINES) $(INCPATH) src/utility.h -o .moc/moc_utility.cpp

.moc/moc_networkmanager.cpp: src/networkmanager.h
	mmoc.pl $(DEFINES) $(INCPATH) src/networkmanager.h -o .moc/moc_networkmanager.cpp

.moc/moc_networkconnector.cpp: src/networkconnector.h
	mmoc.pl $(DEFINES) $(INCPATH) src/networkconnector.h -o .moc/moc_networkconnector.cpp

.moc/moc_player.cpp: src/player.h
	mmoc.pl $(DEFINES) $(INCPATH) src/player.h -o .moc/moc_player.cpp

.moc/moc_qdeclarativewebview.cpp: src/qtm/qdeclarativewebview.h
	mmoc.pl $(DEFINES) $(INCPATH) src/qtm/qdeclarativewebview.h -o .moc/moc_qdeclarativewebview.cpp

compiler_dbus_interface_source_make_all:
compiler_dbus_interface_source_clean:
compiler_dbus_adaptor_source_make_all:
compiler_dbus_adaptor_source_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_dbus_interface_header_make_all:
compiler_dbus_interface_header_clean:
compiler_dbus_interface_moc_make_all:
compiler_dbus_interface_moc_clean:
compiler_dbus_adaptor_header_make_all:
compiler_dbus_adaptor_header_clean:
compiler_dbus_adaptor_moc_make_all:
compiler_dbus_adaptor_moc_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

.obj/qdeclarativemediabase.o: src/qtm/qdeclarativemediabase.cpp src/qtm/qdeclarativemediabase_p.h \
		src/qtm/qdeclarativemediametadata_p.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/qdeclarativemediabase.o src/qtm/qdeclarativemediabase.cpp

.obj/qdeclarativevideo.o: src/qtm/qdeclarativevideo.cpp src/qtm/qdeclarativevideo_p.h \
		src/qtm/qdeclarativemediabase_p.h \
		src/qtm/qpaintervideosurface_p.h \
		.moc/moc_qdeclarativevideo_p.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/qdeclarativevideo.o src/qtm/qdeclarativevideo.cpp

.obj/qpaintervideosurface.o: src/qtm/qpaintervideosurface.cpp src/qtm/qpaintervideosurface_p.h \
		.moc/moc_qpaintervideosurface_p.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/qpaintervideosurface.o src/qtm/qpaintervideosurface.cpp

.obj/main.o: main.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/main.o main.cpp

.obj/utility.o: src/utility.cpp src/utility.h \
		src/networkmanager.h \
		src/id_std.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/utility.o src/utility.cpp

.obj/networkmanager.o: src/networkmanager.cpp src/networkmanager.h \
		src/id_std.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/networkmanager.o src/networkmanager.cpp

.obj/networkconnector.o: src/networkconnector.cpp src/networkconnector.h \
		src/id_std.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/networkconnector.o src/networkconnector.cpp

.obj/id_std.o: src/id_std.cpp src/id_std.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/id_std.o src/id_std.cpp

.obj/player.o: src/player.cpp src/player.h \
		src/id_std.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/player.o src/player.cpp

.obj/qdeclarativewebview.o: src/qtm/qdeclarativewebview.cpp src/qtm/qdeclarativewebview.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/qdeclarativewebview.o src/qtm/qdeclarativewebview.cpp

.obj/qmlapplicationviewer.o: qmlapplicationviewer/qmlapplicationviewer.cpp qmlapplicationviewer/qmlapplicationviewer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/qmlapplicationviewer.o qmlapplicationviewer/qmlapplicationviewer.cpp

.obj/moc_qdeclarativemediametadata_p.o: .moc/moc_qdeclarativemediametadata_p.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_qdeclarativemediametadata_p.o .moc/moc_qdeclarativemediametadata_p.cpp

.obj/moc_qmlapplicationviewer.o: .moc/moc_qmlapplicationviewer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_qmlapplicationviewer.o .moc/moc_qmlapplicationviewer.cpp

.obj/moc_utility.o: .moc/moc_utility.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_utility.o .moc/moc_utility.cpp

.obj/moc_networkmanager.o: .moc/moc_networkmanager.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_networkmanager.o .moc/moc_networkmanager.cpp

.obj/moc_networkconnector.o: .moc/moc_networkconnector.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_networkconnector.o .moc/moc_networkconnector.cpp

.obj/moc_player.o: .moc/moc_player.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_player.o .moc/moc_player.cpp

.obj/moc_qdeclarativewebview.o: .moc/moc_qdeclarativewebview.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_qdeclarativewebview.o .moc/moc_qdeclarativewebview.cpp

####### Install

install_splash: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/opt/ppsh/res/ || $(MKDIR) $(INSTALL_ROOT)/opt/ppsh/res/ 
	-$(INSTALL_FILE) /home/user/ppsh/ppsh/res/ppsh_splash_natasha.png $(INSTALL_ROOT)/opt/ppsh/res/


uninstall_splash:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/opt/ppsh/res/ppsh_splash_natasha.png
	-$(DEL_DIR) $(INSTALL_ROOT)/opt/ppsh/res/ 


install_itemfolder_01: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/opt/ppsh/qml/ || $(MKDIR) $(INSTALL_ROOT)/opt/ppsh/qml/ 
	-$(INSTALL_DIR) /home/user/ppsh/ppsh/qml/ppsh $(INSTALL_ROOT)/opt/ppsh/qml/


uninstall_itemfolder_01:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/opt/ppsh/qml/ppsh
	-$(DEL_DIR) $(INSTALL_ROOT)/opt/ppsh/qml/ 


install_itemjs: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/opt/ppsh/qml/ || $(MKDIR) $(INSTALL_ROOT)/opt/ppsh/qml/ 
	-$(INSTALL_DIR) /home/user/ppsh/ppsh/qml/js $(INSTALL_ROOT)/opt/ppsh/qml/


uninstall_itemjs:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/opt/ppsh/qml/js
	-$(DEL_DIR) $(INSTALL_ROOT)/opt/ppsh/qml/ 


install_itemkmplayer: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/opt/ppsh/./ || $(MKDIR) $(INSTALL_ROOT)/opt/ppsh/./ 
	-$(INSTALL_DIR) /home/user/ppsh/ppsh/kmplayer++ $(INSTALL_ROOT)/opt/ppsh/./


uninstall_itemkmplayer:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/opt/ppsh/./kmplayer++
	-$(DEL_DIR) $(INSTALL_ROOT)/opt/ppsh/./ 


install_itemi18n: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/opt/ppsh/./ || $(MKDIR) $(INSTALL_ROOT)/opt/ppsh/./ 
	-$(INSTALL_DIR) /home/user/ppsh/ppsh/i18n $(INSTALL_ROOT)/opt/ppsh/./


uninstall_itemi18n:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/opt/ppsh/./i18n
	-$(DEL_DIR) $(INSTALL_ROOT)/opt/ppsh/./ 


install_icon: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/usr/share/icons/hicolor/80x80/apps/ || $(MKDIR) $(INSTALL_ROOT)/usr/share/icons/hicolor/80x80/apps/ 
	-$(INSTALL_FILE) /home/user/ppsh/ppsh/ppsh80.png $(INSTALL_ROOT)/usr/share/icons/hicolor/80x80/apps/


uninstall_icon:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/usr/share/icons/hicolor/80x80/apps/ppsh80.png
	-$(DEL_DIR) $(INSTALL_ROOT)/usr/share/icons/hicolor/80x80/apps/ 


install_desktopfile: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/usr/share/applications/ || $(MKDIR) $(INSTALL_ROOT)/usr/share/applications/ 
	-$(INSTALL_FILE) /home/user/ppsh/ppsh/ppsh_harmattan.desktop $(INSTALL_ROOT)/usr/share/applications/


uninstall_desktopfile:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/usr/share/applications/ppsh_harmattan.desktop
	-$(DEL_DIR) $(INSTALL_ROOT)/usr/share/applications/ 


install_target: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/opt/ppsh/bin/ || $(MKDIR) $(INSTALL_ROOT)/opt/ppsh/bin/ 
	-$(INSTALL_PROGRAM) "$(QMAKE_TARGET)" "$(INSTALL_ROOT)/opt/ppsh/bin/$(QMAKE_TARGET)"

uninstall_target:  FORCE
	-$(DEL_FILE) "$(INSTALL_ROOT)/opt/ppsh/bin/$(QMAKE_TARGET)"
	-$(DEL_DIR) $(INSTALL_ROOT)/opt/ppsh/bin/ 


install:  install_splash install_itemfolder_01 install_itemjs install_itemkmplayer install_itemi18n install_icon install_desktopfile install_target  FORCE

uninstall: uninstall_splash uninstall_itemfolder_01 uninstall_itemjs uninstall_itemkmplayer uninstall_itemi18n uninstall_icon uninstall_desktopfile uninstall_target   FORCE

FORCE:

