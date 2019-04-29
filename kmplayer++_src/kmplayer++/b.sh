#!/bin/sh

cd src
g++ -Wl,-O1 -o kmplayer main.o io.o log.o dirmodel.o control.o playlist.o playmodel.o player.o lists.o mediaobject.o surface.o viewarea.o triestring.o expression.o kmplayer_asx.o kmplayer_atom.o kmplayer_rss.o kmplayer_xspf.o kmplayer_rp.o kmplayer_smil.o kmplayer_opml.o moc_dirmodel.o moc_control.o moc_playmodel.o moc_viewarea.o    -L/usr/lib -L/usr/X11R6/lib -lgio-2.0 -lgmodule-2.0 -lXrender -lcairo -lX11 -lcurl -lconic -pthread -lgconf-2 -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lgthread-2.0 -lrt -lglib-2.0 -L/usr/lib/ -lqmsystem2 -ltuiclient -lQtDeclarative -lQtOpenGL -lQtGui -lQtCore -lGLESv2 -lpthread
