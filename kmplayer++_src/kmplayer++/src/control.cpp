//#include <config.h>

#include <string.h>
#include <errno.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>
#include <gconf/gconf-client.h>

#include "dirmodel.h"
#include "io.h"
#include "playlist.h"
#include "mediaobject.h"
#include "lists.h"
#include "kmplayer_smil.h"
#include "control.h"
#include "playmodel.h"
#include "player.h"
#include "viewarea.h"

#include <TransferUI/client.h>
#include <TransferUI/transfer.h>
#include <QDir>
#include <QUrl>
#include <QTimer>
#include <QTimerEvent>
#include <QVariant>
#include <qmdisplaystate.h>
#include <QClipboard>
#include <QApplication>
#include <QFileInfo>
#include <QDeclarativeContext>
#include <QDeclarativeView>

using namespace KMPlayer;

#ifdef KMPLAYER_DEBUG
static Application *debug_app;
static Control *debug_control;

void sig_usr1 (int) {
    debugLog () << "Start dump" << endl;
    debugLog () << debug_control->process () << " " <<
        debug_control->blanking_timer << endl;
    if (debug_control->process () && debug_control->process ()->mrl ())
        debugLog () << debug_control->process ()->playing () <<
            debug_control->process ()->mrl ()->src <<
            debug_control->process ()->mrl ()->audio_only <<
            debug_control->process ()->hasVideo () << endl;
    debugLog () << "End dump" << endl;
}
#endif

QString KMPlayer::getUserInput (Control *ctrl,
        const QString& title, const QString& desc, const QString& def)
{
    return "Not implemented";
}

namespace {

class Download : public Element, public IOJobListener
{
public:
    Download (NodePtr &doc, Control *n, int fd = -1);
    ~Download ();

    virtual void closed();
    virtual void begin();
    virtual const char *nodeName () const { return "download"; }

    void stop ();

    virtual void jobData (IOJob *job, QByteArray & data);
    virtual void jobResult (IOJob *job);
    virtual void redirected (IOJob *, const QString &) {}

    QString title;
    QString file;
    QString uri;
    IOJob *job;
    Control *notify;
    TransferUI::Transfer* transfer;
    off_t size;
    off_t bytes;
    int fd;
};

class DownloadDocument : public FileDocument
{
public:
    DownloadDocument (Control *ctrl)
        : FileDocument (id_node_download_document, "", ctrl)
    {
        readFromFile (ctrl->data_dir + "/downloads.xml");
        resolved = true;
    }

    virtual Node *childFromTag (const QString &tag)
    {
        if (tag == "download")
            return new Download (m_doc, m_control);
        if (tag == "downloads")
            return this;
        return FileDocument::childFromTag (tag);
    }
    virtual const char *nodeName () const {
        return "downloads";
    }
};

}

static bool gconf_initialized = false;

Config::Config () {
    if (!gconf_initialized) {
        gconf_initialized = true;
        g_type_init ();
    }
    client = gconf_client_get_default ();
}

Config::~Config () {
    gconf_client_clear_cache (client);
    g_object_unref (client);
}

QString Config::readEntry (const QString & key, const QString & def) {
    gchar *val = gconf_client_get_string (client, key.toUtf8 ().constData (), 0L);
    if (!val)
        return def;
    QString s = QString::fromUtf8 (val);
    g_free (val);
    return s;
}

int Config::readNumEntry (const QString & key, int def) {
    GConfValue *val = gconf_client_get (client, key.toUtf8 ().constData (), 0L);
    if (val) {
        int i = gconf_value_get_int (val);
        gconf_value_free (val);
        return i;
    }
    return def;
}

double Config::readNumEntry (const QString & key, double def) {
    GError * err;
    double val = gconf_client_get_float (client, key.toUtf8 ().constData (), &err);
    bool has_err = !!err;
    g_error_free (err);
    return has_err ? def : val;
}

void Config::writeEntry (const QString & key, const QString & val) {
    gconf_client_set_string (client,
            key.toUtf8 ().constData (), val.toUtf8 ().constData (), 0L);
}

void Config::writeEntry (const QString & key, int val) {
    gconf_client_set_int (client, key.toUtf8 ().constData (), val, 0L);
}


inline void Config::writeEntry (const QString & key, double val) {
    gconf_client_set_float (client, key.toUtf8 ().constData (), val, 0L);
}

//-----------------------------------------------------------------------------

SourceDocument::SourceDocument (Control *c, const QString &url)
    : Document (url, c), m_control (c) {}

#ifdef _HARMATTAN
SourceDocument::SourceDocument (Control *c, const QString &url, const QString &a)
    : Document (url, a, c), m_control (c) {}

#endif

void SourceDocument::message (MessageType msg, void *data) {
    switch (msg) {

    case MsgInfoString:
        //app_set_info_content (m_control->m_app,
                //data ? (const char *) *((QString *) data) : "", false);
        return;

    case MsgBannerString:
        m_control->errorMsg (*((QString *) data));
        return;

    case MsgAccessKey:
        for (Connection *c = m_KeyListeners.first(); c; c = m_KeyListeners.next ())
            if (c->payload && c->connecter) {
                KeyLoad *load = (KeyLoad *) c->payload;
                if (load->key == (int) (long) data)
                    post (c->connecter, new Posting (this, MsgAccessKey));
            }
        return;

    case MsgListUpdated: {
        PlayItem *item = m_control->model->rootItem();
        const QList<PlayItem*>::const_iterator e = item->child_items.constEnd();
        for (QList<PlayItem*>::const_iterator i = item->child_items.constBegin(); i != e; ++i)
            if ((*i)->node.ptr () == this) {
                m_control->model->updateTree (((TopPlayItem*)*i)->id, this, this);
                break;
            }
        return;
    }

    case MsgShowMeta:
        m_control->metaSelected ((Node *) data);
        return;

    default:
        break;
    }
    Document::message (msg, data);
}

void *SourceDocument::role (RoleType msg, void *data) {
    switch (msg) {

    case RoleMediaManager:
        return m_control->mediaManager ();

    case RoleChildDisplay:
        return m_control->viewArea ()->getSurface ((Mrl *) data);

    case RoleReceivers:
        switch ((MessageType) (long) data) {

        case MsgAccessKey:
            return &m_KeyListeners;

        case MsgSurfaceUpdate:
            return m_control->viewArea ()->updaters ();

        default:
            break;
        }
        // fall through

    default:
        break;
    }
    return Document::role (msg, data);
}

Control::Control (QDeclarativeView *p)
    : QObject (p),
      m_process (0L), m_doc_timer (0),
      m_view (p),
      mplayer (NULL), gstreamer_mplayer (NULL),
      dir_model (new DirModel (this)),
      file_model (new FileModel (this, dir_model)),
      model (new PlayModel (this)), m_config (new Config),
      paint_timer (0), update_tree_timer (0), blanking_timer (0),
      in_progress_update (false),
      request_download (false),
      m_display_state (NULL),
      m_keys (NULL),
      m_locks (NULL),
      m_view_area (NULL),
      media_manager (new MediaManager (this)),
      m_transfer_client (NULL),
      load_status (100),
      m_play_status (0),
      m_active (true),
      m_video_visible (false),
      m_keys_locked (false)
{
#ifdef KMPLAYER_DEBUG
    debug_control = this;
#endif
}

Control::~Control () {
    delete m_process;
    delete m_config;
    delete m_transfer_client;
    m_process = 0L;
}

static gboolean syncSchedule (Control *ctr)
{
    Recents *recents = static_cast <Recents *> (ctr->recents.ptr());
    if (!ctr->data_dir.isEmpty ()) {
        Playlist *playlist = static_cast<Playlist *>(ctr->playlists.ptr());
        FileDocument *downloads = (FileDocument*)ctr->downloads_doc.ptr ();
        if (recents)
            recents->sync (ctr->data_dir + "/recent.xml");
        if (downloads)
            downloads->sync (ctr->data_dir + "/downloads.xml");
        if (playlist)
            playlist->sync (ctr->data_dir + "/playlist.xml");
    }
    return !!recents;
}

void Control::init () {
    Ids::init();

    unsigned int ver, rel, req, evb, err;
    struct stat st;
    glob_t globbuf;

    use_xvideo = true;
   //media_server = new OssoMediaServer (this, m_app->osso_context, use_xvideo);
    media_server = GstreamerMPlayerProcess::createGstreamerPlayer (this);
#ifdef _HARMATTAN
    if (glob (OPTDIR "/" _HARMATTAN_KMPLAYER "/bin/mplayer", 0, NULL, &globbuf) != GLOB_NOMATCH) {
#else
    if (glob ("/usr/bin/mplayer", 0, NULL, &globbuf) != GLOB_NOMATCH) {
#endif
        mplayer = GstreamerMPlayerProcess::createMPlayer (this);
        gstreamer_mplayer = new GstreamerMPlayerProcess (this);
    }
    globfree (&globbuf);
#if 0
def __ARMEL__
    npp = new NpPlayer (this);
    ((NpPlayer *)npp)->setPlayer ("/opt/kmplayer/bin/knpplayer");
#endif
    downloader = new Downloader (this);
#ifdef _HARMATTAN
    QString p = m_config->readEntry (GCONF_KEY_PLAYER, "mplayer");
#else
    QString p = m_config->readEntry (GCONF_KEY_PLAYER, "osso-media-server");
#endif
    setProcess (p);
    selected_player = p;

    /*if (use_xvideo)
        use_xvideo = !m_config->readNumEntry (GCONF_KEY_VO, 0);
    if (!use_xvideo)
        gtk_check_menu_item_set_active (
                GTK_CHECK_MENU_ITEM (m_app->menu_item_mplayer_vo), true);*/

    const QString home = getenv ("HOME");
    if (!home.isEmpty ()) {
#ifdef _HARMATTAN
        data_dir = m_config->readEntry (GCONF_KEY_DATA_DIR, QString (home + "/." _HARMATTAN_KMPLAYER));
#else
        data_dir = m_config->readEntry (GCONF_KEY_DATA_DIR,
                QString (home + "/.kmplayer"));
#endif
        if (stat (data_dir.toLocal8Bit ().constData (), &st))
            mkdir (data_dir.toLocal8Bit ().constData (), 0755);
    }

    Playlist *pldoc = new Playlist (this, 0L);
    playlists = pldoc;
    QString path = data_dir + "/playlist.xml";
    if (stat (path.toLocal8Bit ().constData (), &st))
#ifdef _HARMATTAN
        path = OPTDIR "/" _HARMATTAN_KMPLAYER "/share/playlist.xml";
#else
        path = OPTDIR "/kmplayer/share/playlist.xml";
#endif
    pldoc->path = path;
    pldoc->title = tr ("Favorites");

    playlists_tree_id = model->addTree (pldoc, pldoc->path,
                "image://theme/icon-l-gallery-main-view",
                PlayModel::Deletable | PlayModel::AllowDrops | PlayModel::InPlaceEdit);
    GConfClient *client = (GConfClient *) *m_config;
    GSList *plists = gconf_client_all_entries (client, GCONF_KEY_LISTS, NULL );
    for (GSList *elm = plists; elm; elm = elm->next) {
        GConfEntry *entry = (GConfEntry *)elm->data;
        GConfValue *value = gconf_entry_get_value (entry);
        const char *str = gconf_value_get_string (value);
        if (str && *str) {
            Playlist *expl = new Playlist (this);
            extra_playlists.append (expl);
            expl->title = entry->key + strlen (GCONF_KEY_LISTS) + 1;
            expl->path = str;
            model->addTree (expl, expl->path,
                    "image://theme/icon-m-toolbar-directory-white", 0);
        }
        gconf_entry_free (entry);
    }
    g_slist_free (plists);

    Recents *rcdoc = new Recents (this);
    recents = rcdoc;
    path = data_dir + "/recent.xml";
    rcdoc->path = path;
    rcdoc->title = tr ("Most Recent");
    recents_tree_id = model->addTree (rcdoc, rcdoc->path,
                "file:///usr/share/themes/blanco/meegotouch/icons/icon-l-clock-main-view.png", PlayModel::Deletable);

    QString sf = m_config->readEntry (GCONF_KEY_SAVE_FOLDER, QString());
    if (!sf.isEmpty ())
        dir_model->setCurrentDirectory (sf);
    g_timeout_add (5*60*1000, GSourceFunc (syncSchedule), this);
}

void Control::deinit () {
    debugLog() << "deinit\n";
    stop ();
    setProcess ((Process *) NULL);

    syncSchedule (this);

    Recents *rec = static_cast <Recents *> (recents.ptr());
    rec->dispose ();

    Playlist *playlist = static_cast<Playlist *>(playlists.ptr());
    if (!dir_model->currentDirectory ().isEmpty ())
        m_config->writeEntry (GCONF_KEY_SAVE_FOLDER, dir_model->currentDirectory ());
    playlist->dispose ();

    for (NodePtr n = extra_playlists.first(); n; n = n->nextSibling ())
        n->document()->dispose ();

    for (NodeStoreItem *s = generators.first (); s; s = generators.first ()) {
        s->data->document ()->dispose ();
        generators.remove (s);
    }

    if (downloads_doc) {
        FileDocument *doc = (FileDocument*)downloads_doc.ptr ();
        doc->dispose ();
        downloads_doc = NULL;
    }

    if (active_list) {
        active_list->document()->dispose ();
        active_list = NULL;
    }

    viewArea ()->reset ();

    delete media_server;
    delete mplayer;
#if 0
def __ARMEL__
    delete npp;
#endif
    delete downloader;

    Ids::reset ();
}

void Control::play () {
    QModelIndex index = model->index (0, 0);
    PlayItem *pi = static_cast <PlayItem *> (index.internalPointer ());
    if (!pi->node)                // nothing to play
        return;
    if (pi->node->active ()) {    // is playing
        Mrl *cur_item_playing = m_process->mrl ();
        if (!m_process->playing () ||
                Mrl::WindowMode == m_process->mrl ()->view_mode ||
                cur_item_playing == pi->node->mrl ())
            cur_item_playing = NULL;

        if (pi->node->state == Node::state_deferred) {
            pi->node->undefer (); // unpause
            if (cur_item_playing)
                cur_item_playing->undefer ();
        } else {
            pi->node->defer ();   // pause
            if (cur_item_playing)
                cur_item_playing->defer ();
        }
        updateScreenBlanking ();
    } else {
        if (!applyBackRequest () && pi->node) {
            playCurrent ();
        }
    }
}

void Control::stop (bool keep_fullscreen) {
    bool was_document = false;
    QString url;
    Mrl *mrl = m_process->mrl ();
    if (mrl) {
        was_document = mrl->id == id_node_document;
        url = mrl->src;
    }

    back_request = 0L;

    QModelIndex index = model->index (0, 0);
    PlayItem *pi = static_cast <PlayItem *> (index.internalPointer ());
    if (pi->node && pi->node->state > Node::state_init) {
        pi->node->reset ();
        setCurrent (0L);
    }
    if (m_view_area) m_view_area->reset ();
    //m_app->aspects_ignored = false;
    setPlayStatus (STOPPED);

    if (current_generator && current_generator->active ()) {
        current_generator->deactivate ();
        current_generator = NULL;
    }

    Node *recent = recents->firstChild ();
    if (recent &&
            m_process->isLocalFile () &&
            recent->mrl ()->src == url) {
        Mrl *recent_mrl = recent->mrl ();
        QString ts = recent_mrl->getAttribute ("clipBegin");
        int pos = m_process->userStopPosition ();
        pos = pos > 50 ? pos - 50 : 0;
        if (was_document)
            mrl->clip_begin = 10 * pos;
        QString nts;
        int ds = pos % 10;
        if (ds > 0)
            nts = QString (QChar ('.')) + QString::number (ds);
        pos /= 10;
        int s = pos % 60;
        nts = QString::number (s) + nts;
        pos /= 60;
        if (pos > 0) {
            int m = pos % 60;
            nts = QString::number (m) + QString (QChar (':')) + nts;
            pos /= 60;
            if (pos > 0) {
                if (m  < 10)
                    nts = QString (QChar ('0')) + nts;
                nts = QString::number (pos) + QString (QChar (':')) + nts;
            }
        }
        if (ts != nts) {
            recent_mrl->setAttribute ("clipBegin", nts);
            mrl->setAttribute ("clipBegin", nts);
            model->topItem (recents_tree_id) ->updated_document_version =
                ++recents->document ()->m_tree_version;
            model->updateTree (recents_tree_id, recents, NULL);
        }
    }
    m_process->quit ();
    setLoading (100); // no pogress bar

    if (m_view_area) {
        m_view_area->scheduleRepaint (IRect (0, 0,
                    m_view_area->width (), m_view_area->height ()));
        if (!keep_fullscreen)
            m_view_area->setVisible (false);
    }
    updateScreenBlanking ();
    setLength (0);
    setPosition (0);
    updatePlaylistView ();
}

QString Control::currentMrl () {
    Mrl * mrl = current ? current->mrl () : 0L;
    if (!mrl)
        return QString ();
    return mrl->absolutePath ();
}

void Control::openDocument (NodePtr doc, NodePtr cur) {
    stop (true);
    doc->document()->notify_listener = this;
    current = cur ? cur : doc;
    //app_set_title ((const char *) doc->document ()->caption (), m_app);
    back_request = cur;
    setActiveList (doc);
    QTimer::singleShot (0, this, SLOT (play ()));
}

ViewArea *Control::viewArea () {

    if ( !m_view_area ) {
        m_view_area = new ViewArea (NULL);
        connect (m_view_area->scaleButton(), SIGNAL(clicked()), this, SLOT(scale()));
        connect (m_view_area->stopButton(), SIGNAL(clicked()), this, SLOT(Stop()));
        connect (m_view_area->zoomButton(), SIGNAL(clicked()), this, SLOT(panZoom()));
        connect (m_view_area->backButton(), SIGNAL(clicked()), m_view, SLOT(showFullScreen()));
        connect (m_view_area, SIGNAL(videoWidgetVisible(bool)), this, SLOT(videoWidgetVisible(bool)));
    }
    return m_view_area;
}

void Control::playCurrent () {
    QString url = currentMrl ();
    // set title url;
    aspect = 0.0;
    width = height = 0;
    //debugLog () << "playCurrent" << !!playlist_data->document << !playlist_data->document->active () << endl;
    // view ())->videoStop (); //show buttonbar
    QModelIndex index = model->index (0, 0);
    PlayItem *pi = static_cast <PlayItem *> (index.internalPointer ());
    if (pi->node &&
            !pi->node->active ()) {
        if (!current)
            setCurrent (pi->node);
        else { // ugly code duplicate w/ back_request
            for (Node *p = current->parentNode(); p; p = p->parentNode())
                p->setState (Element::state_activated);
        }
        current->activate ();
        updateViewAreaSizes ();
    } else if (!current) {
        //debugLog () << "playCurrent no current" << endl;
        stop ();
        ; //emit endOfPlayItems ();
    } else if (current->state == Element::state_deferred) {
        //debugLog () << "playCurrent set undefer" << endl;
        ;//playlist_data->current->undefer ();
    } else if (m_process->state () == Process::NotRunning) {
        //debugLog () << "playCurrent set ready" << endl;
        m_process->ready (viewArea ()->videoXId ());
    } else if (!current->active ()) {
        current->activate ();
        updateViewAreaSizes ();
    } else {
        Mrl *mrl = back_request
            ? back_request->mrl ()
            : current->mrl ();
        //debugLog () << "playCurrent " << m_process->playing () << " " << mrl->mrl ()->resolved << endl;
        if (m_process->playing ()) {
            errorLog() << "playCurrent and backend still playing" << endl;
            if (m_process->mrl () != mrl)
                m_process->stop ();
            else
                return; // FIXME
        }
        back_request = 0L;
        if (!mrl->src.isEmpty () && mrl->resolved &&
                mrl->media_info &&
                (MediaManager::Audio == mrl->media_info->type ||
                 MediaManager::AudioVideo == mrl->media_info->type)) {
            updateViewAreaSizes ();
            //if (m_process->hasXv ())
                //XSync (GDK_DISPLAY_XDISPLAY (gtk_widget_get_display (m_view_area->viewWidget ())), false);
#ifdef _HARMATTAN
						m_process->SetRequestHeaders(request_headers);
#endif
            static_cast<AudioVideoMedia *> (mrl->media_info->media)->readyToPlay (m_process);
        } else
            errorLog() << "playCurrent and mrl not resolved" << endl;
    }
}

bool Control::applyBackRequest() {
    if (!back_request || back_request->mrl ()->view_mode == Mrl::WindowMode)
        return false;
    QModelIndex index = model->index (0, 0);
    PlayItem *pi = static_cast <PlayItem *> (index.internalPointer ());
    if (current)
        pi->node->reset ();
    current = 0L;
    if (back_request->document() != pi->node) {
        //debugLog () << "playlist/recent" << endl;
        NodePtr n = back_request;
        back_request = 0L;
        n->activate();
    } else {
        setCurrent (back_request);
        for (Node *p = back_request->parentNode(); p; p = p->parentNode())
            p->setState (Element::state_activated);
        back_request = 0L;
        QTimer::singleShot (0, this, SLOT (playCurrent ()));

    }
    return true;
}

static QString processForMimetype (Control *ctr, const QString & mime)
{
    QString pname = ctr->m_config->readEntry(QString (GCONF_KEY_MIME) + mime, "");
#if 0
def __ARMEL__
    if (pname.isEmpty ()) {
        // hardcoded defaults FIXME
        if (NpPlayer::isFlashMimeType (mime))
            pname = "npp";
    }
#endif
    return pname;
}

MediaManager *Control::mediaManager () const {
    return media_manager;
}

/* dbus-send --print-reply /com/nokia/osso_browser \
 * --dest=com.nokia.osso_browser com.nokia.osso_browser.load_url \
 *  string:http://maemo.org/
 */
bool Control::requestPlayURL (Mrl *mrl) {
    debugLog () << "requestPlayURL " << mrl->src << (mrl->view_mode == Mrl::SingleMode) << endl;
    if (process ()->state () > Process::Ready) {
        if (process ()->mrl () == mrl)
            return true;
        back_request = mrl; // still playing, schedule it
        if (!setProcess (mrl))
            process ()->stop ();
    } else {
        if (mrl->view_mode == Mrl::SingleMode)
            setCurrent (mrl);
        else
            back_request = mrl;
        setProcess (mrl);
        QTimer::singleShot (0, this, SLOT (playCurrent ()));
    }
    return true;
}

void Control::setAudioVideoNode (Node *n)
{
    viewArea ()->setAudioVideoNode (n);
}

bool Control::setProcess (const QString& pname) {
    Process *old_process = m_process;
    if (mplayer && pname == mplayer->name ()) {
        m_process = mplayer;
    } else if (gstreamer_mplayer && pname == gstreamer_mplayer->name()) {
        m_process = gstreamer_mplayer;
#if 0
def __ARMEL__
    } else if (pname == npp->name ()) {
        m_process = npp;
#endif
    } else {
        m_process = media_server;
    }
    selected_player = m_process->name ();

    if (old_process) {
        if (old_process == m_process)
            old_process->stop ();
        else
            old_process->quit ();
    }
    return true;
}

bool Control::setProcess (Mrl *mrl) {
    if (request_download) {
        m_process = downloader;
        request_download = false;
        return true;
    }
    QString pname;
    bool changed = false;
    if (id_node_playlist_item == mrl->id) {
        QString p = mrl->getAttribute ("player");
        if ((mplayer && p == mplayer->name ()) || p == media_server->name ()
#if 0
def __ARMEL__
                || p == npp->name ()
#endif
           )
            pname = p;
    }
    if (pname.isEmpty () && !mrl->mimetype.isEmpty ())
        pname = processForMimetype (this, mrl->mimetype);
    if (pname.isEmpty ())
#ifdef _HARMATTAN
        pname = m_config->readEntry (GCONF_KEY_PLAYER, "mplayer");
#else
        pname = m_config->readEntry (GCONF_KEY_PLAYER,
                gstreamer_mplayer ? "gstreamer or mplayer":"osso-media-server");
#endif
    if (!m_process || pname != m_process->name ()) {
        setProcess (pname);
        changed = true;
    }
    return changed;
}

void Control::setTimeout (int ms) {
    if (m_doc_timer > 0)
        killTimer (m_doc_timer);

    m_doc_timer = ms > -1 ? startTimer (ms) : 0;
}

void Control::openUrl (const QUrl &url, const QString &, const QString & /*srv*/) {
    //osso_rpc_run_with_defaults( m_app->osso_context, "osso_browser",
    //        "open_new_window", NULL,
    //        DBUS_TYPE_STRING, (const char *)url.url (), DBUS_TYPE_INVALID);
    //errorMsg ("Open for target and mimetype is not supported");
}

void Control::enableRepaintUpdaters (bool enable, unsigned int off_time) {
    viewArea ()->enableUpdaters (enable, off_time);
}

bool Control::setCurrent (NodePtr n) {
    debugLog() << "setCurrent " << (n && n->mrl() ? n->mrl()->src : "-")<< endl;
    if (current == n)
        return true;
    current = n;
    setCurrentSelected (n);
    updatePlaylistView ();
    Mrl *mrl = n ? n->mrl () : 0L;
    if (mrl) {
        QString url;
        while (mrl) {
            if (mrl->src.startsWith ("Playlist://"))
                break;
            if (mrl->view_mode == Mrl::SingleMode)
                url = mrl->absolutePath ();
            Node *p = mrl->parentNode ();
            for (mrl = 0L; p; p = p->parentNode ())
                if (p->mrl ()) {
                    mrl = p->mrl ();
                    break;
                }
        }
        //app_insert_location (m_app, (const char *)url);
    }
    return true;
}

static const char *stateDesc [] = {
    "init", "deferred", "activated", "began", "finished",
    "deactivated", "resetting"
};

void Control::stateElementChanged (Node* n, Node::State os, Node::State ns) {
    //debugLog () << "state " << n->nodeName () << " changed to " << stateDesc [int (ns)] << endl;
    if (!m_process) // destructing
        return;
    QModelIndex index = model->index (0, 0);
    PlayItem *pi = static_cast <PlayItem *> (index.internalPointer ());
    if (ns == Node::state_deactivated &&
            n == pi->node && !back_request) {
        stop ();
        //VIEWAREA(m_app)->setEventFiltering (m_app, false);
        setPlayStatus (STOPPED);
    } else if ((ns == Node::state_deactivated || ns == Node::state_finished) &&
            n->mrl () && process ()->mrl () == n->mrl()) {
        process ()->stop ();
        updateViewAreaSizes (); // move away video widget
    } else if ((ns == Node::state_deferred ||
                (os == Node::state_deferred && ns > Node::state_deferred)) &&
            n == pi->node) {
        //VIEWAREA(m_app)->setEventFiltering (m_app, true);
        setPlayStatus (ns > Node::state_deferred ? PLAYING : PAUSED);
    } else if (ns == Node::state_activated ||
            (os == Node::state_init && ns == Node::state_deferred)) {
        if (n == pi->node) {
            //VIEWAREA(m_app)->setEventFiltering (m_app, true);
            setPlayStatus (PLAYING);
        }
        if (n->id == SMIL::id_node_body) {
            //m_app->show_playlist = false;
            videoDetected ();
            updateViewAreaSizes (); // move away video widget
        }
        if (SMIL::id_node_smil == n->id &&
                (!current ||
                 SMIL::id_node_smil != current->id))
                // make sure we don't set current to nested document
            setCurrent (n);
    }
    if (n == current &&
            (ns == Node::state_activated || ns == Node::state_deactivated))
        updatePlaylistView ();
}

void Control::updateViewAreaSizes () {
    if (m_process && m_view_area && m_view_area->videoXId ())
        m_view_area->resizeEvent (NULL);
}

void Control::updatePlaylistView (bool force) {
    if (force) {
        if (update_tree_timer) {
            killTimer (update_tree_timer);
            update_tree_timer = 0;
        }
        model->updateTree (0, active_list, current);
    } else if (!update_tree_timer) {
        update_tree_timer = startTimer (200);
    }
}

static const char * statemap [] = {
        "Not Running", "Ready", "Buffering", "Playing", "Paused"
};

void Control::stateChanged (Process * p, Process::State os, Process::State ns) {
    Mrl *mrl = p->mrl ();
    NodePtrW guard = mrl; // p->mrl is weak, needs check
    debugLog () << "Process stateChanged " << statemap [(int)os] << " -> " << statemap [(int)ns] << endl;
    if (!m_process) // destructing
        return;

    //if (os < Process::Buffering && ns >= Process::Buffering) {
        // maybe add a notification about process launch instead
        //m_app->zoom_enabled = false;
    //}

    if (!mrl && ns > Process::Ready) {
        p->stop (); // reschedule to the NotRunning state
    } else if (ns == Process::NotRunning) {
        if (p == m_process) {
            //if (playlist_data->document &&
                    //playlist_data->document->state > Node::state_init) {
                stop (); // this should not happen
            //}
            updateScreenBlanking ();
        }
        // else process changed
        //if (!playlist_data->edit_mode)
            //app_set_info_content (m_app, "", false);
    } else if (ns == Process::Ready) {
        if (load_status < 100)
            setLoading (100); // move possible buffer info dialog
        if (os > Process::Ready) {
            setLength (0);
            setPosition (0);
            viewArea ()->setAspect (0.0);
            if (guard && !applyBackRequest () &&  // if cause is no jump
                    mrl->active ()) {           // if cause is eof
                mrl->message (MsgMediaFinished);// set node to finished
                if (back_request)            // overlapping audio/video in SMIL
                    QTimer::singleShot (0, this, SLOT (playCurrent ()));
            }
        } else {
            if (p == downloader)
                updatePlaylistView (true);
            QTimer::singleShot (0, this, SLOT (playCurrent ()));
        }
    } else if (ns == Process::Buffering) {
        if (os < Process::Buffering)
            updatePlaylistView ();
        if (mrl->view_mode != Mrl::SingleMode) {
            mrl->defer ();
        }
    } else if (ns == Process::Paused /*&&
            playlist_data->document->state != Node::state_deferred &&
            !playlist_data->document->document ()->postponed ()*/) {
        //if (gtk_window_has_toplevel_focus (m_app->window))
            m_process->pause ();
        //else
            //playlist_data->document->defer ();
    } else if (ns == Process::Playing) {
        mrl->audio_only |= !p->hasVideo ();
        mrl->message (MsgMediaStarted, NULL);
        if (mrl->state == Element::state_deferred)
            mrl->undefer ();
        if (load_status < 100)
            setLoading (100); // move possible buffer info dialog
        QModelIndex index = model->index (0, 0);
        PlayItem *item = static_cast <PlayItem *> (index.internalPointer ());
        addRecent (item->node->mrl()->src,
                item->node->document ()->caption ());
        updatePlaylistView (false);
        if (mrl->audio_only &&
                current &&
                current->id != SMIL::id_node_smil)
            viewArea ()->setVisible (false);
        updateScreenBlanking ();
    }
}

void Control::addRecent (const QString &url, const QString &title, bool remove) {
    if (url == "Playlist://")
        return;
    recents->defer ();
    Node *c = recents->firstChild ();
    if (!c || c->mrl()->src != url) {
        Recent *rec = new Recent (recents, this, url);
        recents->insertBefore (rec, c);
        if (!title.isEmpty ())
            rec->setAttribute (Ids::attr_title, title);
        if (remove)
            rec->setAttribute (Ids::attr_autoremove, "true");
        rec->closed ();
        c = recents->firstChild ()->nextSibling ();
        int count = 1;
        Node *more = NULL;
        while (c) {
            if (c->id == id_node_recent_node &&
                    c->mrl ()->src == url) {
                Node *tmp = c->nextSibling ();
                if (!remove)
                    rec->setAttribute (Ids::attr_autoremove, c->mrl()->getAttribute (Ids::attr_autoremove));
                recents->removeChild (c);
                c = tmp;
            } else {
                if (c->id == id_node_recent_more)
                    more = c;
                c = c->nextSibling ();
                count++;
            }
        }
        if (!more && count > 10) {
            more = new Group (recents, this, tr ("More ..."));
            recents->appendChild (more);
        }
        if (more) {
            if (count > 10) {
                NodePtr item = more->previousSibling ();
                recents->removeChild (item);
                more->insertBefore (item.ptr (), more->firstChild ());
            }
            if (more->firstChild ())
                c = more->firstChild ()->nextSibling ();
            count = 0;
            while (c) {
                if (c->id == id_node_recent_node &&
                        c->mrl ()->src == url) {
                    Node *tmp = c->nextSibling ();
                    if (!remove)
                        rec->setAttribute (Ids::attr_autoremove, c->mrl()->getAttribute (Ids::attr_autoremove));
                    more->removeChild (c);
                    c = tmp;
                } else {
                    c = c->nextSibling ();
                    count++;
                }
            }
            if (count > 50) {
                Node *n = more->lastChild ();
                Mrl *m = n->mrl ();
                if (m && m->getAttribute (Ids::attr_autoremove) == "true") {
                    struct stat st;
                    if (!stat (m->src.toLocal8Bit().data(), &st) && S_ISREG (st.st_mode))
                        unlink (m->src.toLocal8Bit().data());
                }
                more->removeChild (n);
            }
        }
        //playListData(playlist_data, recents_tree_id)->current = NULL;
        model->updateTree (recents_tree_id, recents, NULL);
    }
}

void Control::setCurrentSelected (NodePtr node) {
    current_selected = node;
}

void Control::errorMsg (const QString & msg) {
    emit showErrorMsg (msg);
}

void Control::setLoading (int perc) {
    load_status = perc;
    emit loadStatusChanged();
}

void Control::setPosition (int p) {
    if (load_status < 100)
        setLoading (100);
    if (!m_active
            || !m_view_area
            || !m_view_area->isVisible ()
            || !m_view_area->isActiveWindow ())
        emit positionChanged (position ());
}

void Control::setPlayStatus (int s) {
    if (s != m_play_status) {
        m_play_status = s;
        if ( PLAYING == s && !m_keys) {
            m_keys = new MeeGo::QmKeys( this );
            m_locks = new MeeGo::QmLocks( this );
            connect (m_keys, SIGNAL (keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)),
                     this, SLOT (keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)));
            connect (m_locks, SIGNAL (stateChanged(MeeGo::QmLocks::Lock, MeeGo::QmLocks::State)),
                    this, SLOT (locksChanged(MeeGo::QmLocks::Lock, MeeGo::QmLocks::State)));
            m_locks->getStateAsync (MeeGo::QmLocks::TouchAndKeyboard);
        } else if (STOPPED == s) {
            delete m_keys;
            delete m_locks;
            m_keys = NULL;
            m_locks = NULL;
            m_keys_locked = false;
        }
        emit playStatusChanged ();
    }
}

void Control::setLength (int len) {
    /*hildon_seekbar_set_total_time (HILDON_SEEKBAR (m_app->progress_bar), len);
    hildon_seekbar_set_fraction (HILDON_SEEKBAR (m_app->progress_bar), len);
    m_app->show_progress = len > 50;
    app_set_playing (m_app, m_app->playing);*/
}

void Control::setAspect (float aspect) {
    viewArea ()->setAspect (aspect);
    updateViewAreaSizes ();
}

/*void cb_view_area_configure (GtkWidget *widget, GdkEventConfigure *event, Application * app) {
    //debugLog () << "cb_view_area_configure " << event->width<< "x" << event->height << endl;
    CONTROL(app)->updateViewAreaSizes ();
}*/

void Control::bitRates (int & /*prefered*/, int & /*maximal*/) {
}

static Mrl *findFirstPlayable (Node *n) {
    if (n) {
        if (n->isPlayable ())
            return n->mrl ();
        for (Node *c = n->firstChild (); c; c = c->nextSibling ()) {
            Mrl *mrl = findFirstPlayable (c);
            if (mrl)
                return mrl;
        }
    }
    return NULL;
}

void KMPlayer::jump (Node *n, Control *ctr) {
    QModelIndex index = ctr->model->index (0, 0);
    PlayItem *pi = static_cast <PlayItem *> (index.internalPointer ());
    Mrl *mrl = !pi->node || n->document() != pi->node
        ? isListItem (n->id) ||
          id_node_meta_view == n->id ||
          id_node_meta_group == n->id
              ? n->mrl ()
              : NULL
        : findFirstPlayable (n);
    if (mrl && mrl->view_mode == Mrl::SingleMode) {
        if (n->document() == pi->node && !mrl->isPlayable())
            return;
        if (ctr->process ()->playing () &&
                ctr->process ()->mrl() == mrl) {
            ctr->play (); // pause or unpause
        } else {
            //debugLog() << "jump " << mrl->src << endl;
            // FIXME: group activation doesn't work
            ctr->back_request = mrl;
            if (ctr->process ()->playing ())
                ctr->process ()->stop ();
            else
                ctr->applyBackRequest();
        }
    } else
        ctr->updatePlaylistView ();
}
/*
void app_set_info_content (Application *app, const char *s, bool plaintext) {
    if (!app->fullscreen) {
        if (!s || !*s)
            gtk_widget_hide (app->info_area);
        else
            gtk_widget_show (app->info_area);
    }
    if (!plaintext) {
        gtk_widget_hide (app->info_viewport);
        gtk_widget_show (app->html_viewport);
        if (!s[0]) {
            gtk_html_load_empty (GTK_HTML (app->html_infopanel_view));
        } else {
            int w, h;
            gtk_widget_get_size_request (app->info_area, &w, &h);
            if (h != 140) {
                gtk_widget_set_size_request (app->info_area, 400, 140);
                gtk_fixed_move (GTK_FIXED (app->window_area),
                        app->info_area, 400, 214);
            }
            gtk_html_load_from_string(GTK_HTML(app->html_infopanel_view), s,-1);
        }
    } else {
        int w, h;
        gtk_widget_get_size_request (app->info_area, &w, &h);
        if (h != 354) {
            gtk_fixed_move (GTK_FIXED (app->window_area),
                    app->info_area, 400, 0);
            gtk_widget_set_size_request (app->info_area, 400, 354);
        }
        gtk_widget_hide (app->html_viewport);
        gtk_text_buffer_set_text (gtk_text_view_get_buffer
                (GTK_TEXT_VIEW (app->infopanel_view)), s, -1);
        gtk_widget_show (app->info_viewport);
    }
}

void cbPluginAdded (GtkSocket *socket, Application *app) {
#ifdef __ARMEL__
    Control *ctr = CONTROL(app);
    debugLog () << "plugin embedded\n";
    app->show_playlist = false;
    ctr->videoDetected ();
#endif
}

void cb_playlist_prev (GtkButton * button, void *p) {
    Control *ctr = (Control *)p;
    if (ctr->playlist_data->current &&
            ctr->playlist_data->current->previousSibling ())
        jump (ctr->playlist_data->current->previousSibling (), ctr); //FIXME
}

void cb_playlist_play (GtkButton * button, void *p) {
    Control *ctr = (Control *)p;
    if (button == GTK_BUTTON (ctr->m_app->tb_pause) ||
            (ctr->playlist_data->document &&
             ctr->playlist_data->document->state == Node::state_deferred))
        ctr->play ();
    else if (ctr->current_selected)
        jump (ctr->current_selected, ctr);
    else
        QTimer::singleShot (0, this, SLOT (play ()));
}

void cb_playlist_next (GtkButton * button, void *p) {
    Control *ctr = (Control *)p;
    if (ctr->process ()->playing ())
        ctr->process ()->stop ();
}

void cb_playlist_stop (GtkButton * button, void *p) {
    Control *ctr = (Control *)p;
    if (GTK_WIDGET_VISIBLE (ctr->m_app->tb_stop))
        ctr->process ()->setUserStop ();
    ctr->stop ();
}

void cb_playlist_save (GtkButton * button, void *p) {
    Control *ctr = (Control *)p;
    if (ctr->current_selected) {
        ctr->request_download = true;
        jump (ctr->current_selected, ctr);
    } else {
        hildon_banner_show_information (GTK_WIDGET (ctr->m_app->window), NULL,
                "No Playlist Item Selected");
    }
}

void cb_favorites_add (GtkButton *, Application *app) {
    Control *ctr = CONTROL(app);
    bool valid = false;
    const gchar *msg = "Unknown failure";
    if (!ctr->current_selected) {
        msg = "No current selected";
    } else {
        Mrl *mrl = ctr->current_selected->mrl ();
        if (!mrl || mrl->src.isEmpty() || mrl->src.startsWith ("Playlist://")) {
            msg = "Not a valid link selected";
        } else {
            PlayListData *pd, *fav = NULL;
            for (pd = ctr->playlist_data; pd; pd = pd->next) {
                if (pd->id == app->playlists_tree_id)
                    fav = pd;
                if (pd->document.ptr () == mrl->document ()) {
                    if (fav == pd)
                        msg = "Already in favorites";
                    else
                        valid = true;
                }
            }
            if (fav && valid) {
                msg = "Added to favorites";
                NodePtr doc = mrl->document ();
                if (!fav->document->mrl ()->resolved)
                    fav->document->activate (); // lazy load playlist
                if (id_node_playlist_item == mrl->id ||
                        id_node_html_object == mrl->id) {
                    fav->document->insertBefore (mrl->outerXML (false), NULL);
                } else {
                    PlaylistItem *i = new PlaylistItem (doc, app, false, mrl->src);
                    if (!mrl->title.isEmpty ())
                        i->setAttribute (Ids::attr_title, mrl->title);
                    i->closed ();
                    fav->document->appendChild (i);
                }
                updateTree (app, fav->id);
            }
        }
    }
    hildon_banner_show_information (GTK_WIDGET (app->window), NULL, msg);
}
*/
void Control::setActiveList (NodePtr doc) {
    QModelIndex index = model->index (0, 0);
    PlayItem *item = static_cast <PlayItem *> (index.internalPointer ());
    if (item->node)
        item->node->document()->dispose ();
    active_list = doc;
    item->node = active_list;
    static_cast <TopPlayItem *> (item)->updated_document = NULL;
    updatePlaylistView (true);
}

void Control::openFile (const QString& filename) {
    stop ();
    setActiveList (new SourceDocument (this, filename));
    QTimer::singleShot (0, this, SLOT (play ()));
}

#ifdef _HARMATTAN
void Control::openFile (const QString& filename, const QString &a) {
    stop ();
    setActiveList (new SourceDocument (this, filename, a));
    QTimer::singleShot (0, this, SLOT (play ()));
}

#endif

void Control::openInitialUrls ()
{
    m_view->rootContext()->setContextProperty("playControl", this);
    m_view->rootContext()->setContextProperty("playModel", model);
    m_view->rootContext()->setContextProperty("dirModel", dir_model);
    m_view->rootContext()->setContextProperty("fileModel", file_model);
#ifdef _HARMATTAN
    m_view->setSource(QUrl::fromLocalFile(OPTDIR "/" _HARMATTAN_KMPLAYER "/kmplayer.qml"));
#else
    m_view->setSource(QUrl::fromLocalFile("/opt/kmplayer/kmplayer.qml"));
#endif
    if (initial_urls.size () == 1) {
#ifdef _HARMATTAN
			if(!initial_audios.isEmpty())
        openFile (initial_urls[0], initial_audios[0]);
			else
        openFile (initial_urls[0]);
#else
        openFile (initial_urls[0]);
#endif
    } else if (initial_urls.size () > 1) {
        Playlist *pl = new Playlist (this, true);
        KMPlayer::NodePtr doc = pl;
#ifdef _HARMATTAN
				for(int i = 0; i < initial_urls.size(); i++)
				{
					if(i < initial_audios.size())
            pl->appendChild(new KMPlayer::GenericURL(doc, initial_urls[i], initial_audios[i], QString()));
					else
            pl->appendChild(new KMPlayer::GenericURL(doc, initial_urls[i]));
				}
#else
        const QStringList::const_iterator e = initial_urls.constEnd ();
        for (QStringList::const_iterator i = initial_urls.constBegin (); i != e; ++i)
            pl->appendChild (new KMPlayer::GenericURL (doc, *i));
#endif
        pl->setCaption (tr ("Playlist"));
        openDocument (doc, NULL);
    }
}

void Control::timerEvent (QTimerEvent *event)
{
    if (event->timerId () == m_doc_timer) {
        QModelIndex index = model->index (0, 0);
        PlayItem *pi = static_cast <PlayItem *> (index.internalPointer ());

        m_doc_timer = -1;

        if (pi->node && pi->node->active ())
            pi->node->document ()->timer (); //can call setTimeout()
        else
            m_doc_timer = 0;

        if (-1 == m_doc_timer)
            // not called setTimeout(), keep interval
            m_doc_timer = event->timerId ();
        else
            killTimer (event->timerId ());
    } else if (event->timerId () == blanking_timer) {
        updateScreenBlanking ();
    } else {
        killTimer (event->timerId ());
        if (event->timerId () == update_tree_timer) {
            if (process ()) {
                //if (!ctr->playlist_data->document)
                //ctr->playlist_data->document = new SourceDocument (ctr, QString());
                model->updateTree (0, active_list, current);
            }
            update_tree_timer = 0;
        } else {
            errorLog () << "Control: unknown timer event\n";
        }
    }
}

bool Control::eventFilter (QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::ApplicationDeactivate) {
        m_active = false;
        debugLog () << "eventFilter: ApplicationDeactivate\n";
        updateScreenBlanking ();
    } else if (e->type() == QEvent::ApplicationActivate) {
        m_active = true;
        updateScreenBlanking ();
        debugLog () << "eventFilter: ApplicationActivate\n";
    }

    return QObject::eventFilter (obj, e);
}

/*
void cb_openUrls (void *app, int argc, gchar **argv) {
}

static QString makeM3u (Node *n, const QString &group, const QString &fn) {
    QString str;
    if (id_node_playlist_item == n->id || id_node_recent_node == n->id) {
        Mrl *mrl = n->mrl ();
        if (!mrl->src.isEmpty () && fn != mrl->src) {
            if (!mrl->caption ().isEmpty ())
                str = QString ("#EXTINF:,") +
                    (id_node_playlist_item == n->id && !group.isEmpty ()
                     ? group + " - "
                     : QString ()) +
                    mrl->caption () + QString ("\r\n");
            str += mrl->src + QString ("\r\n");
        }
    } else {
        QString title;
        if (id_node_group_node == n->id) {
            PlaylistRole *title_role = (PlaylistRole *) n->role (RolePlaylist);
            if (title_role)
                title = title_role->caption ();
        }
        for (Node *c = n->firstChild (); c; c = c->nextSibling ())
            str += makeM3u (c, title, fn);
    }
    return str;
}

void saveFile (Application *app, bool xml) {
    Control *ctr = CONTROL(app);
    QString msg;
    if (ctr->current_selected) {
        Document *doc = ctr->current_selected->document ();
        gchar *filename = app_file_chooser (app, save_folder,
                GTK_FILE_CHOOSER_ACTION_SAVE);
        if (filename) {
            QString fn (filename);
            QString str;
            if (xml) {
                if (!fn.toLower ().endsWith (".xml"))
                    fn += ".xml";
                str = doc->outerXML ();
            } else {
                if (!fn.toLower ().endsWith (".m3u"))
                    fn += ".m3u";
                str = QString ("#EXTM3U\r\n") + makeM3u (doc, QString (), fn);
            }
            int fd = open ((const char*)fn, O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (fd < 0) {
                msg = QString( "Failed creating ") + fn + QChar ('\n') + strerror (errno);
            } else {
                const char *txt = (const char *) str;
                ::write (fd, txt, strlen (txt));
                msg = QString ("Saved ") + fn;
                ::close (fd);
            }
            app_store_save_directory (app, filename); // free's filename
        }
    } else {
        msg = "No list selected";
    }
    if (!msg.isEmpty ())
        hildon_banner_show_information (GTK_WIDGET (app->window), NULL,
                (const char *)msg);
}

void newFile (Application * app) {
    PlayListData *pd = CONTROL(app)->playlist_data;
    if (pd->document) {
        pd->document->reset ();
        pd->document->document ()->dispose ();
    }
    pd->document = new SourceDocument (CONTROL(app), QString ());
    CONTROL(app)->updatePlaylistView (true);
}

gboolean cb_mainview_delete (GtkObject *, GdkEvent *, Application *app) {
    CONTROL(app)->deinit ();
    gtk_main_quit ();
    return false;
}

static bool testFullscreenPlaylist (Application *app) {
    Control *ctr = CONTROL(app);
    return ((ctr->process () != ctr->mplayer || !ctr->process ()->hasXv ()) &&
            app->fullscreen);
}

gboolean cb_keyPressed (GtkWidget *, GdkEventKey * e, Application *app) {
    switch (e->keyval) {
        case GDK_F6: //HILDON_HARDKEY_FULLSCREEN:
            //if (app->control->process ()->playing ()) {
             //   int w, h;
            VIEWAREA(app)->prepareFullscreenToggle ();
            if (!app->fullscreen) {
                if (CONTROL(app)->playlist_data->document &&
                        CONTROL(app)->playlist_data->document->active ())
                    app_full_screen (app);
            } else {
                app_normal_screen (app);
            }
            break;
        / *case GDK_F7: //HILDON_HARDKEY_INCREASE
            if (CONTROL(app)->process ()->playing ())
                CONTROL(app)->process ()->volume (2, false);
            break;
        case GDK_F8: //HILDON_HARDKEY_DECREASE
            if (CONTROL(app)->process ()->playing ())
                CONTROL(app)->process ()->volume (-2, false);
            break;* /
        case GDK_Left:
            if (gtk_window_get_focus (app->window) == app->playlist_view ||
                    testFullscreenPlaylist (app))
                collapseSelectedList (app);
            else if (CONTROL(app)->process ()->playing ())
                CONTROL(app)->process ()->seek (-200, false); // 20s
            break;
        case GDK_Right:
            if (gtk_window_get_focus (app->window) == app->playlist_view ||
                    testFullscreenPlaylist (app))
                expandSelectedList (app);
            else if (CONTROL(app)->process ()->playing ())
                CONTROL(app)->process ()->seek (200, false); // 20s
            break;
        case GDK_KP_Enter:
        case GDK_Return: // HILDON_HARDKEY_SELECT
            if (gtk_window_get_focus (app->window) == app->playlist_view ||
                    testFullscreenPlaylist (app))
                rowSelected (app);
            break;
        case GDK_Up:
            if (gtk_window_get_focus (app->window) == app->playlist_view ||
                    testFullscreenPlaylist (app))
                previousSelected (app);
            break;
        case GDK_Down:
            if (gtk_window_get_focus (app->window) == app->playlist_view ||
                    testFullscreenPlaylist (app))
                nextSelected (app);
            break;
        case GDK_Escape:
            return true;
        default:
            KMPlayer::debugLog () << "Keypress " << e->keyval << KMPlayer::endl;
    }
    return false;
}

void app_set_suspend (Application *a, bool suspend) {
    PlayListData *pd = CONTROL(a)->playlist_data;
    if (pd->document &&
            pd->document->active () &&
            (!CONTROL(a)->process ()->playing () ||
             CONTROL(a)->process()->mrl()->view_mode == Mrl::WindowMode)) {
        if (pd->document->state == Node::state_deferred) {
            if (!suspend)
                pd->document->undefer ();
        } else if (suspend)
            pd->document->defer ();
    }
    app_update_screen_blanking (a);
}

static bool locationbar_initialized;

gboolean
locationbarGainFocus (GtkWidget *, GdkEventFocus *, Application * app) {
    int n = gtk_toolbar_get_n_items (GTK_TOOLBAR (app->main_toolbar));
    for (int i = 5; i < n; i++)
        gtk_widget_hide (GTK_WIDGET (gtk_toolbar_get_nth_item (GTK_TOOLBAR (app->main_toolbar), i)));
    app_set_playing (app, app->playing);
    if (!locationbar_initialized) {
        const char * defs[6] = {
            "http://gdata.youtube.com/feeds/api/videos?vq=MeeGo",
            "http://homepages.cwi.nl/~media/SMIL/fiets/fiets.smil",
            "http://www.theworld.org/rss/tech.xml",
            "http://www.etherbeat.com/shoutcast/64.pls",
            "http://www.swissgroove.ch/listen.m3u",
            "file:///home/user/MyDocs/.videos/Nokia_N900.mp4"
        };
        int defpos = 0;
        locationbar_initialized = true;
        KMPlayer::Config * config = CONTROL(app)->m_config;
        QString cfg_prefix (GCONF_KEY_LOCATION_URL);
        GtkTreeIter iterator;
        GtkListStore * model = GTK_LIST_STORE (gtk_combo_box_get_model (
                    GTK_COMBO_BOX (app->location_bar)));
        for (int i = 1; i < 7; i++) {
            gtk_list_store_append (model, &iterator);
            QString cfg = config->readEntry (cfg_prefix +
                    QString::number(i), "");
            if (cfg.isEmpty ())
                cfg = defs[defpos++];
            gtk_list_store_set (model, &iterator,
                0, (const char *) cfg, -1);
        }
    }
    gtk_editable_select_region (GTK_EDITABLE (GTK_BIN (app->location_bar)->child), 0, -1);
    //g_print ("callback_locationbar_gain_focus %d\n", n);
    return false;
}

void save_locationbar (Application *app) {
    if (!locationbar_initialized)
        return;
    KMPlayer::Config * config = CONTROL(app)->m_config;
    int idx = 1;
    QString cfg_pfx (GCONF_KEY_LOCATION_URL);
    GtkTreeIter iterator;
    GtkTreeModel * model = GTK_TREE_MODEL (gtk_combo_box_get_model (
                GTK_COMBO_BOX (app->location_bar)));
    for (bool b = gtk_tree_model_get_iter_first (model, &iterator);
            b && idx < 7;
            b = gtk_tree_model_iter_next (model, &iterator)) {
        GValue value = { 0 , };
        gtk_tree_model_get_value (model, &iterator, 0, &value);
        QString val (g_value_get_string (&value));
        g_value_unset (&value);
        if (!val.isEmpty ())
            config->writeEntry (cfg_pfx + QString::number(idx++), val);
    }
}

static void getGenerators (Control *ctrl, const QString &dir) {
    GDir *gdir = g_dir_open ((const char *) dir, 0, NULL);
    if (gdir) {
        const gchar *file = g_dir_read_name (gdir);
        while (file) {
            Generator *gen = new Generator (ctrl);
            NodePtr doc = gen;
            gen->readFromFile (dir + QChar ('/') + file);
            Node *n = gen->firstChild ();
            if (n && n->isElementNode ()) {
                QString txt = static_cast <Element *> (n)->getAttribute (
                        Ids::attr_name);
                if (txt.isEmpty ())
                    txt = file;
                gen->setAttribute (Ids::attr_name, txt);
                ctrl->generators.append (new NodeStoreItem (doc));
            } else {
                gen->dispose ();
            }
            file = g_dir_read_name (gdir);
        }
        g_dir_close (gdir);
    }
}

static void
generatorResponse (GtkDialog *dialog, gint response, gpointer data) {
    Control *ctrl = (Control *) data;
    HildonTouchSelector *sel = hildon_picker_dialog_get_selector (
            HILDON_PICKER_DIALOG (dialog));

    if (response == GTK_RESPONSE_OK) {
        gchar *selection = hildon_touch_selector_get_current_text (sel);
        for (NodeStoreItem *s = ctrl->generators.first(); s; s = s->nextSibling()) {
            QString txt = static_cast <Element *> (s->data.ptr())->getAttribute (
                    Ids::attr_name);
            if (txt == selection) {
                ctrl->current_generator = s->data;
                return;
            }
        }
    }
    ctrl->current_generator = NULL;
}

void generatorMenuActivate (GtkMenuItem *item, Application *app) {
    Control *ctrl = CONTROL(app);
    GtkWidget *dialog = hildon_picker_dialog_new (app->window);
    GtkWidget *selector = hildon_touch_selector_new_text();

    hildon_picker_dialog_set_selector (HILDON_PICKER_DIALOG (dialog),
            HILDON_TOUCH_SELECTOR (selector));
    g_signal_connect (G_OBJECT (dialog), "response",
            G_CALLBACK (generatorResponse), ctrl);

    if (!ctrl->generators.first ()) {
        getGenerators (ctrl, "/usr/share/applications/kmplayer/generators");
        getGenerators (ctrl, ctrl->data_dir + "/generators");
    } else if (ctrl->current_generator) {
        if (ctrl->current_generator->active ())
            ctrl->current_generator->deactivate ();
        ctrl->current_generator = NULL;
    }

    for (NodeStoreItem *s = ctrl->generators.first(); s; s = s->nextSibling()) {
        QString txt = static_cast <Element *> (s->data.ptr ())->getAttribute (
                Ids::attr_name);
        hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (selector),
                                txt);
    }
    gtk_window_set_title (GTK_WINDOW (dialog), "Generate Playlist");
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);

    if (ctrl->current_generator)
        ctrl->current_generator->activate ();
}

static void
clearHistoryResponse (GtkDialog *dialog, gint response, gpointer data) {
    *((bool *) data) = response == GTK_RESPONSE_ACCEPT;
}

#include <hildon/hildon-check-button.h>

static GtkWidget *clearButton (GtkWidget *box, const char *title)
{
    GtkWidget *button = hildon_check_button_new ((HildonSizeType)
            (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
    gtk_button_set_label (GTK_BUTTON (button), title);
    hildon_check_button_set_active (HILDON_CHECK_BUTTON (button), true);
    gtk_box_pack_start (GTK_BOX (box), button, true, true, 1);
    return button;
}

void clearHistory (GtkMenuItem *item, Application *app)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons ("Clear History",
            app->window,
            GtkDialogFlags(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT),
            GTK_STOCK_OK,
            GTK_RESPONSE_ACCEPT,
            NULL);
    GtkWidget *content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    GtkWidget *recents = clearButton (content, "Clear Recent List");
    GtkWidget *locations = clearButton (content, "Clear Location Bar");
    GtkWidget *generators = clearButton (content, "Clear Generators Input");
    gtk_widget_show_all (content);

    bool clear = false;
    g_signal_connect (G_OBJECT (dialog), "response",
            G_CALLBACK (clearHistoryResponse), &clear);

    gtk_dialog_run (GTK_DIALOG (dialog));

    if (clear) {
        Control *ctr = CONTROL(app);
        KMPlayer::Config *config = ctr->m_config;
        if (hildon_check_button_get_active (HILDON_CHECK_BUTTON (locations))) {
            locationbar_initialized = false;
            gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (
                            GTK_COMBO_BOX (app->location_bar))));
            QString cfg_pfx (GCONF_KEY_LOCATION_URL);
            for (int i = 1; i < 7; i++)
                config->writeEntry (cfg_pfx + QString::number(i), "");
            gtk_combo_box_set_active_iter (GTK_COMBO_BOX (app->location_bar), NULL);
        }
        if (hildon_check_button_get_active (HILDON_CHECK_BUTTON (generators))) {
            GConfClient *gcnf = (GConfClient *) *config;
            GSList *keys = gconf_client_all_entries (gcnf, GCONF_KEY_KEY_DIR, NULL);
            for (GSList *elm = keys; elm; elm = elm->next) {
                GConfEntry *entry = (GConfEntry *)elm->data;
                gconf_client_unset (gcnf, entry->key, NULL);
                gconf_entry_free (entry);
            }
            g_slist_free (keys);
        }
        if (hildon_check_button_get_active (HILDON_CHECK_BUTTON (recents))) {
            if (ctr->recents) {
                ctr->recents->defer ();
                ctr->recents->clear ();
            }
            updateTree (app, app->recents_tree_id);
            syncSchedule (ctr);
        }
    }
    gtk_widget_destroy (dialog);
}

static void
playlistResponse (GtkDialog *dialog, gint response, gpointer data) {
    if (response == GTK_RESPONSE_OK) {
        HildonTouchSelector *sel = hildon_picker_dialog_get_selector (
                HILDON_PICKER_DIALOG (dialog));
        *(int *) data = hildon_touch_selector_get_active (
                HILDON_TOUCH_SELECTOR (sel), 0);
    } else {
        *(int *) data = -1;
    }
}

void playlistMenuActivate (GtkMenuItem *, Application *app) {
    Control *ctrl = CONTROL(app);
    if (ctrl->current_selected) {
        GtkWidget *dialog = hildon_picker_dialog_new (app->window);
        GtkWidget *selector = hildon_touch_selector_new_text();
        int idx = 0;

        hildon_picker_dialog_set_selector (HILDON_PICKER_DIALOG (dialog),
                HILDON_TOUCH_SELECTOR (selector));
        g_signal_connect (G_OBJECT (dialog), "response",
                G_CALLBACK (playlistResponse), &idx);
        hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (selector),
                "Export as XML");
        hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (selector),
                "Export as m3u");
        hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (selector),
                "Change Title");
        hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (selector),
                "Show all XML Nodes");
        hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (selector),
                "Edit XML");
        gtk_window_set_title (GTK_WINDOW (dialog), "Playlist Options");
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        switch (idx) {
        case 0:
            saveFile (app, true);
            break;
        case 1:
            saveFile (app, false);
            break;
        case 2:
            cb_playlistTitle (app);
            break;
        case 3:
            togglePlaylistShowAll (app);
            break;
        case 4:
            togglePlaylistEdit (app);
            break;
        }
    } else {
        hildon_banner_show_information (GTK_WIDGET (app->window), NULL,
                "No playlist item selected");
    }
}

#include <hildon/hildon-pannable-area.h>
#include <hildon/hildon-program.h>
#include <hildon/hildon-window.h>
#include <hildon/hildon-defines.h>
#include <hildon/hildon-gtk.h>
*/

Download::Download (NodePtr &d, Control *n, int _fd)
    : Element (d, id_node_download),
      job (NULL), notify (n), transfer (NULL),
      size (0), bytes (0), fd (_fd)
{}

Download::~Download ()
{
    stop ();
    if (transfer) {
        notify->transferClient ()->removeTransfer (transfer->transferId ());
        transfer->markCompleted (!unfinished ());
        delete transfer;
    }
}

void Download::closed ()
{
    file = getAttribute ("file");
    uri = getAttribute (Ids::attr_src);
    title = getAttribute (Ids::attr_title);
    QString sz = getAttribute ("size");
    if (!sz.isEmpty ())
        size = strtoll (sz.toAscii ().constData (), NULL, 10);
    if (!transfer) {
        transfer = notify->transferClient()->registerTransfer (title,
                TransferUI::Client::TRANSFER_TYPES_DOWNLOAD, "");
        transfer->setCanPause (true);
        if (!sz.isEmpty ()) {
            transfer->setSize (size);
            transfer->markPaused();
        }
    }
    Element::closed ();
}

void Download::begin ()
{
    setState (state_began);
    bytes = 0;
    size = 0;
    debugLog() << "Download " << uri << " as " << file << " cap:" << title << endl;
    if (fd > -1) {
        job = asyncGet (this, uri);
        job->start ();
    } else {
        QString sz = getAttribute ("size");
        if (sz.isEmpty ()) {
            fd = ::open (file.toLocal8Bit ().constData (), O_WRONLY | O_CREAT | O_TRUNC, 0664);
        } else {
            size = strtoll (sz.toAscii ().constData (), NULL, 10);
            transfer->setSize (size);
            transfer->setActive (size > 0 ? bytes/(1.0*size) : 0);
            fd = ::open (file.toLocal8Bit ().constData (), O_WRONLY | O_APPEND, 0664);
            bytes = ::lseek (fd, 0, SEEK_END);
        }
        if (fd > 0) {
            if (bytes > 0 && size == bytes) {
                notify->updateDownload (this, 100);
                return;
            }
            job = asyncGet (this, uri);
            job->start (bytes);
        }
    }
    notify->updateDownload (this, size > 0 ? int (100.0 * bytes / size) : 0);
}

void Download::jobData (IOJob *jb, QByteArray &data)
{
    int old_perc = size > 0 ? int (100.0 * bytes / size) : 0;
    if (!bytes) {
        size = jb->contentLength ();
        if (size > 0) {
            transfer->setSize (size);
            transfer->setActive (0);
            setAttribute ("size", QString::number (size));
        }
    }
    bytes += data.size ();
    int new_perc = size > 0 ? int (100.0 * bytes / size) : 0;
    if (new_perc >= 100)
        new_perc = 99;
    if (old_perc != new_perc)
        notify->updateDownload (this, new_perc);
    if (data.size ()) {
        int nr = write (fd, data.constData (), data.size ());
        if (nr < data.size ()) {
            QString err ("Couldn't write data");
            if (nr < 0)
                err += QChar (' ') + QString (strerror (errno));
            stop ();
            document ()->message (MsgBannerString, &err);
        }
    }
}

void Download::stop ()
{
    if (job) {
        IOJob* jb = job;
        ::close (fd);
        fd = -1;
        job = NULL;
        jb->kill ();
    }
}

void Download::jobResult (IOJob *jb)
{
    if (fd > 0)
        close (fd);
    fd = -1;
    job = NULL;
    if (!jb->error ())
        notify->updateDownload (this, 100); // deletes this
}

TransferUI::Client* Control::transferClient() {
    if (!m_transfer_client) {
        m_transfer_client = new TransferUI::Client;
        if (!m_transfer_client->init())
            errorLog() << "Failed to inititialize TransferUI::Client\n";
        connect (m_transfer_client, SIGNAL (cancelTransfer(Transfer*)), this,
                SLOT (transferCancelled(Transfer*)));
        connect (m_transfer_client, SIGNAL (pauseTransfer(Transfer*)), this,
                SLOT (transferPaused(Transfer*)));
        connect (m_transfer_client, SIGNAL (repairErrorTransfer(Transfer*)), this,
                SLOT (transferErrorRepairRequested(Transfer*)));
        connect (m_transfer_client, SIGNAL (startTransfer(Transfer*)), this,
                SLOT (transferResumed(Transfer*)));
    }
    return m_transfer_client;
}

Node *Control::downloads ()
{
    if (!downloads_doc)
        downloads_doc = new DownloadDocument (this);
    return downloads_doc.ptr ();
}

void Control::transferCancelled (TransferUI::Transfer* transfer) {
    debugLog() << "Control::transferCancelled\n";
    for (Node *c = downloads ()->firstChild (); c; c = c->nextSibling ()) {
        Download *download = static_cast <Download *> (c);
        if (download->transfer == transfer) {
            download->stop ();
            struct stat st;
            if (!stat (download->file.toLocal8Bit().data(), &st) && S_ISREG (st.st_mode))
                unlink (download->file.toLocal8Bit().data());
            download->parentNode ()->removeChild (download);
            break;
        }
    }
}

void Control::transferPaused (TransferUI::Transfer* transfer) {
    debugLog() << "Control::transferPaused\n";
    for (Node *c = downloads ()->firstChild (); c; c = c->nextSibling ()) {
        Download *download = static_cast <Download *> (c);
        if (download->transfer == transfer) {
            download->stop ();
            transfer->markPaused();
            break;
        }
    }
}

void Control::transferErrorRepairRequested (TransferUI::Transfer*) {
    debugLog() << "Control::transferRequested\n";
}

void Control::transferResumed (TransferUI::Transfer* transfer) {
    debugLog() << "Control::transferResumed\n";
    for (Node *c = downloads ()->firstChild (); c; c = c->nextSibling ()) {
        Download *download = static_cast <Download *> (c);
        if (download->transfer == transfer) {
            download->begin ();
            transfer->markResumed();
            break;
        }
    }
}

void Control::updateDownload (Node *node, int percentage)
{
    Download *download = static_cast <Download *> (node);
    download->transfer->setProgress (percentage / 100.0);
    if (100 == percentage) {
        download->finish ();
        addRecent (download->file, download->title,
                download->getAttribute ("remove") == "true");
        download->parentNode ()->removeChild (download);
    }
}

static QString genFileName (const QString &str, bool skip_on_invalid) {
    QString name;
    for (int i = 0; i < str.length (); ++i)
        switch (str[i].unicode ()) {
            case '\n':
            case ';':
            case ':':
            case '*':
            case '?':
            case ',':
            case '&':
            case '>':
            case '<':
            case '|':
            case '"':
            case '\'':
            case '/':
                if (!skip_on_invalid)
                    return QString ();
                break;
            default:
                name += str[i];
        }
    return name;
}

void Control::createDownload (const QString &url, const QString &target_title) {
    QString name, err;
    QString title = target_title;
    QUrl uri (url);

    if (Url::isLocalFile (uri)) {
        err = tr ("Refuse to download a local file");
    } else if (uri.scheme () != "http" &&
            uri.scheme () != "https" &&
            uri.scheme () != "ftp" &&
            uri.scheme () != "sftp") {
        err = QString (tr ("Cannot download from protocol ")) + uri.scheme ();
    } else {
        int pos = url.lastIndexOf (QChar ('/'));
        if (pos > 0)
            name = genFileName (url.mid (pos + 1), false);
        if (name.isEmpty ())
            name = title;
        if (!name.isEmpty ()) {
            if (title.isEmpty ())
                title = name;
            name = genFileName (name, true);
        } else {
            name = tr ("Unnamed");
        }
        QString dir = dir_model->currentDirectory ();
        if (!dir.endsWith (QChar ('/')))
            dir += QChar ('/');
        QString path = dir + name;
        if (QFileInfo (path).exists ()) {
            int i = 1;
            pos = path.lastIndexOf (QChar ('.'));
            QString prefix (path);
            QString suffix ("");
            QString tmpl (" (%1)");
            if (pos > dir_model->currentDirectory ().length ()) {
                prefix = path.left (pos);
                suffix = path.mid (pos);
            }
            path = prefix + tmpl.arg (i) + suffix;
            while (QFileInfo (path).exists ())
                path = prefix + tmpl.arg (++i) + suffix;
        }
        emit showSaveFileAs (title, path.mid (dir.length ()));
        /*gchar *filename = app_file_chooser (m_app, (const char *) path,
                GTK_FILE_CHOOSER_ACTION_SAVE);
        }*/
    }
    if (!err.isEmpty ())
        errorMsg (err);
}

void Control::updateScreenBlanking ()
{
    bool needs_blanking_pause = m_view_area && m_view_area->isVisible ();
    if (needs_blanking_pause && !blanking_timer) {
        blanking_timer = startTimer (50000);
    } else if (!needs_blanking_pause && blanking_timer) {
        killTimer (blanking_timer);
        blanking_timer = 0;
        m_display_state->cancelBlankingPause ();
    }
    if (blanking_timer) {
        if (!m_display_state)
            m_display_state = new MeeGo::QmDisplayState (this);
        m_display_state->setBlankingPause ();
    }
}

void Control::keyEvent(MeeGo::QmKeys::Key key, MeeGo::QmKeys::State state)
{
    if (MeeGo::QmKeys::KeyDown != state
            || !m_keys_locked
            || !m_process
            || !m_process->playing ())
        return;
    switch (key) {
    case MeeGo::QmKeys::VolumeUp:
        m_process->volume (2, false);
        break;
    case MeeGo::QmKeys::VolumeDown:
        m_process->volume (-2, false);
        break;
    default:
        break;
    }
}

void Control::locksChanged (MeeGo::QmLocks::Lock what, MeeGo::QmLocks::State how)
{
    if (MeeGo::QmLocks::TouchAndKeyboard == what) {
        m_keys_locked = MeeGo::QmLocks::Locked == how;
    }
}

Q_DECLARE_METATYPE(QModelIndex);

void Control::itemClicked (int i, int current)
{
    QModelIndex index = model->index (i, 0);
    fprintf( stderr, "clicked %d curr %d\n", i, current);
    if (index.isValid ()) {
        PlayItem *item = static_cast <PlayItem *> (index.internalPointer ());

        int count = item->childCount();
        if (!count && item->parent_item == model->rootItem ()) {
            TopPlayItem *ritem = static_cast <TopPlayItem *> (item);
            if (ritem->id > 0 && !item->node->mrl()->resolved) {
                ritem->node->defer ();
                if (item->node->mrl()->resolved) {
                    model->updateTree (ritem, NULL);
                    index = model->index (i, 0);
                    item = static_cast <PlayItem *> (index.internalPointer ());
                    count = item->childCount();
                }
            }
        }
        fprintf( stderr, "found %s\n", item->node ? item->node->nodeName() : "-");
        if (count) {
            model->toggleOpen (item);
            return;
        }
        if (i == current && item->node && item->node->mrl()) {
            setCurrentSelected (item->node);
            jump(item->node, this);
            return;
        }
    }
}

void Control::contextMenuSelected (Node *node, int selected)
{
    QStringList menu;
    Mrl *mrl = node ? node->mrl () : NULL;
    if (mrl && (mrl->src.isEmpty () || mrl->src.startsWith ("Playlist:/")))
        mrl = NULL;
    int flags = 0;
    TopPlayItem *ritem = NULL;
    if (node) {
        ritem = model->topItem (copyPasteTree);
        if (ritem) {
            if (ritem->id && ritem->node.ptr () == node)
                return;
            flags = ritem->root_flags;
        }
    }
    if (node) {
        if (selected == 'a') {
            if (mrl) {
                copyPasteUrl = mrl->src;
                QApplication::clipboard()->setText (mrl->src);
                if (node->id == id_node_playlist_item || node->id == id_node_recent_node)
                    copyPasteXml = node->outerXML ();
                else
                    copyPasteXml.clear ();
            } else if (id_node_group_node == node->id) {
                copyPasteXml = node->outerXML ();
                copyPasteUrl.clear ();
            }
            return;
        }
        if (!selected && (mrl || id_node_group_node == node->id))
            menu << QChar ('a') + tr ("Copy");
    }
    if (node == active_list || (flags & PlayModel::AllowDrops)) {
        QString txt = copyPasteUrl;
        if (txt.isEmpty ())
            txt = QApplication::clipboard()->text ();
        bool valid_txt = !txt.isEmpty ()
            && txt.indexOf (QChar ('\n')) < 0
            && (txt.indexOf ("://") > 0 || txt.startsWith (QChar ('/')));
        if (node == active_list) {
            if (valid_txt && !copyPasteXml.isEmpty () == !copyPasteUrl.isEmpty ()) {
                if (selected == 'b') {
                    openFile (txt);
                    return;
                }
                if (!selected)
                    menu << QChar ('b') + tr ("Paste");
            }
        } else if (!copyPasteUrl.isEmpty () || !copyPasteXml.isEmpty ()) {
            if (selected) {
                NodePtr doc = node->document ();
                Node *parent = node;
                Node *before = node->nextSibling ();
                PlayItem *item = ritem ? model->itemFromNode (ritem, node) : NULL;
                if (id_node_group_node == node->id
                        && (!item || item->open || !item->childCount ()))
                    before = node->firstChild ();
                else
                    parent = node->parentNode ();
                if (selected == 'b') {
                    if (!copyPasteXml.isEmpty ())
                        parent->insertBefore (copyPasteXml, before);
                    else if (valid_txt)
                        parent->insertBefore (new PlaylistItem (doc, this, false, txt), before);
                    Node *active = node->nextSibling ();
                    if (id_node_group_node == node->id)
                        active = node->firstChild ();
                    model->updateTree (copyPasteTree, doc, active);
                    return;
                }
                if (selected == 'c') {
                    NodePtr group = new PlaylistGroup (doc, this);
                    parent->insertBefore (group, before);
                    if (!copyPasteXml.isEmpty ())
                        group->insertBefore (copyPasteXml, NULL);
                    else if (valid_txt)
                        group->insertBefore (new PlaylistItem (doc, this, false, txt), NULL);
                    model->updateTree (copyPasteTree, doc, group->firstChild ());
                    return;
                }
            } else { // !selected
                menu << QChar ('b') + tr ("Paste");
                menu << QChar ('c') + tr ("Paste in new Group");
            }
        }
    }
    if (flags & PlayModel::Deletable && (mrl || id_node_group_node == node->id)) {
        if (selected == 'd') {
            NodePtr doc = node->document ();
            Node *active = node->nextSibling ()
                ? node->nextSibling ()
                : node->previousSibling ()
                    ? node->previousSibling ()
                    : node->parentNode ();
            if (mrl) {
                copyPasteUrl = mrl->src;
                QApplication::clipboard()->setText (mrl->src);
                if (node->id == id_node_playlist_item || node->id == id_node_recent_node)
                    copyPasteXml = node->outerXML ();
                else
                    copyPasteXml.clear ();
            } else if (id_node_group_node == node->id) {
                copyPasteXml = node->outerXML ();
                copyPasteUrl.clear ();
            }
            node->parentNode ()->removeChild (node);
            model->updateTree (copyPasteTree, doc, active);
            return;
        } else if (ritem->id == recents_tree_id) {
            bool local = Url::isLocalFile (mrl->src);
            if (selected == 'f') {
                mrl->setAttribute (Ids::attr_autoremove, QString());
                model->topItem (recents_tree_id) ->updated_document_version =
                    ++recents->document ()->m_tree_version;
                if (ritem)
                    model->emitDataChanged (model->indexFromItem (model->itemFromNode (ritem, node)));
            } else if (selected == 'g') {
                mrl->setAttribute (Ids::attr_autoremove, "true");
                model->topItem (recents_tree_id) ->updated_document_version =
                    ++recents->document ()->m_tree_version;
                if (ritem)
                    model->emitDataChanged (model->indexFromItem (model->itemFromNode (ritem, node)));
            } else if (local && mrl->getAttribute (Ids::attr_autoremove) == "true") {
                menu << QChar ('f') + tr ("Unset auto remove");
            } else {
                menu << QChar ('d') + tr ("Cut");
                if (local)
                    menu << QChar ('g') + tr ("Auto remove from Phone");
            }
        } else {
            menu << QChar ('d') + tr ("Cut");
        }
    }
    if (node == active_list
            || (flags & PlayModel::InPlaceEdit
                && !node->isDocument ())) {
        if (selected == 'e') {
            QString title;
            QString text;
            if (!node || node == active_list) {
                title = tr ("Open Link");
                text = "http://";
            } else {
                title = tr ("Change Title");
                PlaylistRole *role = (PlaylistRole *) node->role (RolePlaylist);
                if (role)
                    text = role->title;
                else if (mrl)
                    text = mrl->src;
                if (text.isEmpty ())
                   text = node->nodeName ();
            }
            emit showEdit (title, text);
        } else {
            menu << QChar ('e') + (node == active_list ? tr ("Open Link") : tr ("Edit Title"));
        }
    }
    if (!selected && menu.size ())
        emit selectContextItem (menu);
}

void Control::itemContext (int i)
{
    QModelIndex index = model->index (i, 0);
    fprintf( stderr, "context %d\n", i);
    if (index.isValid ()) {
        PlayItem *item = static_cast <PlayItem *> (index.internalPointer ());
        setCurrentSelected (item->node);
        copyPasteTree = item->rootItem ()->id;
        contextMenuSelected (item->node);
    }
}

void Control::itemEdit (int i, const QString& text)
{
    if (text.isEmpty ())
        return;
    QModelIndex index = model->index (i, 0);
    if (i == 0) {
        openFile (text);
    } else if (model->flags (index) & Qt::ItemIsEditable) {
        model->setData (index, text, Qt::EditRole);
    }
}

void Control::downloadClicked (int i)
{
    QModelIndex index = model->index (i, 0);
    fprintf( stderr, "download %d\n", i);
    if (index.isValid ()) {
        PlayItem *item = static_cast <PlayItem *> (index.internalPointer ());
        if (item
                && item->node
                && (item->node->mrl() || isListItem (item->node->id))
                && (item->parent_item != model->rootItem ()
                    || static_cast <TopPlayItem *> (item)->id == 0)) {
            request_download = true;
            jump (item->node, this);
            return;
        }
    }
    if (downloads ()->firstChild ())
        m_transfer_client->showUI ();
    else
        errorMsg (tr ("No Playlist Item Selected"));
}

void Control::contextSelected (int i)
{
    debugLog () << "selected context " << i << endl;
    if ('m' == i) {
        emit showFileOpen ();
    } else if ('n' == i) {
        if (mplayer) {
            QStringList items;
            items << tr ("GStreamer or MPlayer") << "GStreamer" << "MPlayer";
            emit selectItem ("player", tr ("Select Player"), items);
        }
    } else {
        contextMenuSelected (current_selected, i);
    }
}

void Control::itemSelected (const QString& context, int i)
{
    if (context == "meta") {
        if (i > -1 && current_selected)
            metaSelected (current_selected, i);
    } else if (context == "player") {
        if (i >= 0 && mplayer) {
            Process *p = gstreamer_mplayer;
            if (1 == i)
                p = media_server;
            else if (2 == i)
                p = mplayer;
            if (p->name () != selected_player) {
                setProcess (p->name ());
                m_config->writeEntry (GCONF_KEY_PLAYER, QString (p->name ()));
            }
        }
    }
}

void Control::menuClicked ()
{
    QStringList menu;
    menu << QChar ('m') + tr ("Open File");
    if (mplayer)
        menu << QChar ('n') + tr ("Select Player");
    emit selectContextItem (menu);
}

void Control::saveFileAs (const QString& title, const QString& filename, bool remove)
{
    if (filename.isEmpty ())
        return;
    QString file = dir_model->currentDirectory () + QChar ('/') + filename;
    fprintf( stderr, "saveFileAs %s %d\n", file.toLocal8Bit().constData(), process () == downloader);
    if (process () == downloader) {
        int fd = open (file.toLocal8Bit().constData (), O_WRONLY | O_CREAT | O_TRUNC, 0664);
        if (fd > 0) {
            NodePtr doc = downloads ();
            Element *n = new Download (doc, this, fd);
            doc->appendChild (n);
            n->setAttribute (Ids::attr_src, downloader->url ());
            n->setAttribute (Ids::attr_title, title);
            n->setAttribute ("file", file);
            n->setAttribute ("remove", remove ? "true" : "false");
            n->closed ();
            n->begin ();
        } else {
            QString err = tr ("Couldn't open file ");
            err += QString (file) + QChar (' ');
            err += strerror (errno);
            errorMsg (err);
        }
        //app_store_save_directory (m_app, file);
    }
}

void Control::Seek (int value, bool promille)
{
    if (m_process && m_process->playing ()) {
        int len = m_process->length ();
        if (len) {
            if (promille)
                m_process->seek (len * value / 1000, true);
            else
                m_process->seek (value, false);
        }
    }
}

void Control::log (const QVariant &s)
{
    fprintf( stderr, "log %s", s.toString().toUtf8().constData());
}

void Control::directoryChosen (const QString& dir)
{
    debugLog() << "directoryChosen " << dir << endl;
}

void Control::raiseVideo ()
{
    if (m_view_area && m_view_area->isVisible ())
        m_view_area->raise ();
}

void Control::uiLoaded ()
{
    if (downloads ()->firstChild ())
        errorMsg (tr ("Tap on title bar to manage pending transfers"));
}

int Control::position () const
{
    if (m_process && m_process->playing ()) {
        int len = m_process->length ();
        if (len)
            return 1000*m_process->position()/len;
    }
    return 0;
}

void Control::metaSelected (Node *meta, int selected)
{
    QString title;
    QStringList items;
    PlaylistRole *pr = (PlaylistRole *) meta->role (RolePlaylist);
    if (pr)
        title = pr->caption ();
    for (Node *c = meta->firstChild (); c; c = c->nextSibling ()) {
        if (id_node_meta_views == c->id) {
            for (Node *v = c->firstChild (); v; v = v->nextSibling ()) {
                pr = (PlaylistRole *) v->role (RoleCaption);
                if (pr) {
                    if (selected > -1) {
                        if (!selected--) {
                            jump (v, this);
                            return;
                        }
                    } else {
                        items << pr->caption ();
                    }
                }
            }
        } else if (id_node_playlist_item == c->id) {
            pr = (PlaylistRole *) c->role (RoleCaption);
            if (pr) {
                if (selected > -1) {
                    if (!selected--) {
                        jump (c, this);
                        return;
                    }
                } else {
                    items << pr->caption ();
                }
            }
        }
    }
    if (selected < 0)
        emit selectItem ("meta", title, items);
}

void Control::panZoom ()
{
    if (m_process)
        m_process->toggleScale ();
}

void Control::scale ()
{
    if ( m_view_area ) {
        m_view_area->toggleUseAspects ();
        updateViewAreaSizes ();
    }
}

void Control::videoWidgetVisible (bool b)
{
    m_video_visible = b;
    emit videoVisibilityChanged ();
}

void Control::Stop ()
{
    process ()->setUserStop ();
    stop ();
}

void Control::videoDetected ()
{
    Mrl *mrl = m_process ? m_process->mrl () : NULL;
    if (mrl && mrl->audio_only) {
        mrl->audio_only = false;
        viewArea ()->setAudioVideoNode (mrl);
        updateViewAreaSizes ();
    }
    viewArea ()->showFullScreen ();
    //app_full_screen (m_app);
    updateScreenBlanking ();
}

/*
 * TODO
static void cb_iap (struct iap_event_t * event, void * arg) {
    if (event->type == OSSO_IAP_CONNECTED) {
    }
}

osso_iap_cb (cb_iap);
osso_iap_connect (OSSO_IAP_ANY, OSSO_IAP_REQUESTED_CONNECT, 0L);

'dbus-send --print-reply --dest=com.nokia.osso_browser /usr/bin/browser com.nokia.osso_browser.load_url string:%s' % url
*/
