/* This file is part of the KMPlayer project
 *
 * Copyright (C) 2011 Koos Vriezen <koos.vriezen@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * until boost gets common, a more or less compatable one ..
 */

#ifndef _KMPLAYER_CONTROL_H_
#define _KMPLAYER_CONTROL_H_

#include <stdio.h>

#include "playlist.h"
#include "player.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qmkeys.h>
#include <qmlocks.h>

#ifndef MAEMO_NO_EXPORT
# define MAEMO_NO_EXPORT __attribute__ ((visibility("hidden")))
#endif

typedef struct _Application Application;
typedef struct _GConfClient GConfClient;

class QVariant;
class QDeclarativeView;
namespace MeeGo {
    class QmDisplayState;
}
namespace TransferUI {
    class Client;
    class Transfer;
}
using TransferUI::Transfer; // signal match workaround

namespace KMPlayer {

class MediaManager;
class DirModel;
class FileModel;
class PlayModel;
class ViewArea;
class Config;
class Control;

QString getUserInput (Control *ctrl,
        const QString& title, const QString& desc, const QString& def);

class KMPLAYER_NO_EXPORT Config {
    GConfClient *client;
public:
    Config ();
    ~Config ();
    QString readEntry (const QString& key, const QString& def);
    int readNumEntry (const QString& key, int def);
    double readNumEntry (const QString& key, double def);
    void writeEntry (const QString& key, const QString& val);
    void writeEntry (const QString& key, int val);
    void writeEntry (const QString& key, double val);
    operator GConfClient* () const { return client; }
};

class MAEMO_NO_EXPORT Control : public QObject, public PlayListNotify, public ProcessNotify {
    Q_OBJECT
    Q_PROPERTY( int loadStatus READ loadStatus WRITE setLoading NOTIFY loadStatusChanged)
    Q_PROPERTY( int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY( int playStatus READ playStatus WRITE setPlayStatus NOTIFY playStatusChanged)
    Q_PROPERTY( int videoVisible READ videoVisible NOTIFY videoVisibilityChanged)

    Process * m_process;
    IRect video_rect;
    int m_doc_timer;
public:
    enum { STOPPED = 0, PAUSED, PLAYING };

    Control (QDeclarativeView *parent);
    ~Control ();
    void init ();
    void deinit ();
    void stop (bool keep_fullscreen=false);
    void openDocument (NodePtr doc, NodePtr cur);
    QString currentMrl ();
    void updatePlaylistView (bool force=false);
    ViewArea *viewArea ();
    void addRecent (const QString &url, const QString &title, bool=false);
    void setCurrentSelected (NodePtr node);
    /**
     * PlayListNotify implementation
     */
    MediaManager *mediaManager () const;
    bool requestPlayURL (Mrl *mrl);
    bool setCurrent (NodePtr);
    bool applyBackRequest();
    void stateElementChanged (Node * n, Node::State os, Node::State ns);
    void updateViewAreaSizes ();
    void bitRates (int & prefered, int & maximal);
    void setTimeout (int ms);
    void openUrl (const QUrl &, const QString &target, const QString &srv);
    Q_INVOKABLE void openFile (const QString &filename);
    void enableRepaintUpdaters (bool enable, unsigned int off_time);

    void setAudioVideoNode (Node *n);
    void setProcess (Process * p) { m_process = p; }
    bool setProcess (const QString& pname);
    bool setProcess (Mrl *mrl);
    Process * process () { return m_process; }
    TransferUI::Client* transferClient();
    Node *downloads ();
    void updateDownload (Node *download, int percentage);
    void updateScreenBlanking ();

    // ProcessNotify
    void stateChanged (Process * p, Process::State os, Process::State ns);
    void errorMsg (const QString & msg);
    void setLoading (int perc);
    void setPosition (int pos) ;
    void setLength (int len);
    void setPlayStatus (int s);
    void setAspect (float aspect);
    void videoDetected ();
    void createDownload (const QString &url, const QString &title);

    Q_INVOKABLE void itemClicked (int i, int current);
    Q_INVOKABLE void itemContext (int i);
    Q_INVOKABLE void itemEdit (int i, const QString& text);
    Q_INVOKABLE void downloadClicked (int i);
    Q_INVOKABLE void contextSelected (int i);
    Q_INVOKABLE void itemSelected (const QString& context, int i);
    Q_INVOKABLE void menuClicked ();
    Q_INVOKABLE void saveFileAs (const QString& title, const QString &file, bool remove);
    Q_INVOKABLE void Seek (int value, bool promille);
    Q_INVOKABLE void log (const QVariant &s);
    Q_INVOKABLE void directoryChosen (const QString&);
    Q_INVOKABLE void raiseVideo ();
    Q_INVOKABLE void uiLoaded ();
    int loadStatus () const { return load_status; }
    int position () const;
    int playStatus () const { return m_play_status; }
    bool videoVisible () const { return m_video_visible; }
    void metaSelected (Node *meta, int selected=-1);
    void contextMenuSelected (Node *node, int selected=0);

    QDeclarativeView *m_view;
    Process *media_server;
    Process *mplayer;
    Process *gstreamer_mplayer;
#ifdef __ARMEL__
    Process *npp;
#endif
    Downloader *downloader;

    NodePtr active_list;
    NodePtr playlists;
    NodePtr recents;

    QStringList initial_urls;
#ifdef _HARMATTAN
		QStringList request_headers;
		QStringList initial_audios;
    Q_INVOKABLE void openFile(const QString &filename, const QString &a);
#endif

    int copyPasteTree;
    QString copyPasteXml;
    QString copyPasteUrl;

    DirModel *dir_model;
    FileModel *file_model;
    PlayModel *model;
    NodePtrW back_request;
    NodePtrW current;
    NodePtrW current_selected;
    NodePtrW current_generator;
    NodeStoreList generators;
    NodePtr downloads_doc;
    QString data_dir;
    QString selected_player;
    Config * m_config;
    int paint_timer;
    int update_tree_timer;
    int blanking_timer;
    int width, height;
    float aspect;
    IRect paint_rect;
    bool in_progress_update;
    bool request_download;
    bool use_xvideo;
public Q_SLOTS:
    void openInitialUrls ();
    void panZoom ();
    void play ();
    void playCurrent ();
    void scale ();
    void videoWidgetVisible (bool);

    void Stop();

    void transferCancelled (Transfer*);
    void transferPaused (Transfer*);
    void transferErrorRepairRequested (Transfer*);
    void transferResumed (Transfer*);

Q_SIGNALS:
    void loadStatusChanged ();
    void playStatusChanged ();
    void videoVisibilityChanged ();
    void positionChanged (int position);
    void selectContextItem (const QStringList& items);
    void selectItem (const QString& context, const QString& title, const QStringList& items);
    void showEdit (const QString& title, const QString& text);
    void showErrorMsg (const QString& text);
    void showFileOpen ();
    void showSaveFileAs (const QString& title, const QString& file);

protected:
    bool eventFilter (QObject *obj, QEvent *event);
    void timerEvent (QTimerEvent *);

private Q_SLOTS:
    void keyEvent (MeeGo::QmKeys::Key key, MeeGo::QmKeys::State state);
    void locksChanged (MeeGo::QmLocks::Lock what, MeeGo::QmLocks::State how);

private:
    void setActiveList (NodePtr doc);

    MeeGo::QmDisplayState *m_display_state;
    MeeGo::QmKeys *m_keys;
    MeeGo::QmLocks *m_locks;
    ViewArea *m_view_area;
    MediaManager *media_manager;
    TransferUI::Client* m_transfer_client;
    NodeList extra_playlists;
    int playlists_tree_id;
    int recents_tree_id;
    int load_status;
    int m_play_status;
    bool m_active;
    bool m_video_visible;
    bool m_keys_locked;
};

class MAEMO_NO_EXPORT SourceDocument : public Document {
public:
    SourceDocument (Control *c, const QString &url);
#ifdef _HARMATTAN
    SourceDocument(Control *c, const QString &url, const QString &a);
#endif

    void message (MessageType msg, void *data=NULL);
    void *role (RoleType msg, void *data=NULL);

    ConnectionList m_KeyListeners;
    Control *m_control;
};

void rowSelected (Application *app) MAEMO_NO_EXPORT;
void nextSelected (Application *app) MAEMO_NO_EXPORT;
void previousSelected (Application *app) MAEMO_NO_EXPORT;
void jump (KMPlayer::Node *n, Control *ctr) MAEMO_NO_EXPORT;

} // namespace


#endif // _KMPLAYER_CONTROL_H_
