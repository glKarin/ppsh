/**
  This file belong to the KMPlayer project, a movie player plugin for Konqueror
  Copyright (C) 2007  Koos Vriezen <koos.vriezen@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**/

#include <string.h>
#include <map>
#include <cairo.h>

#include "mediaobject.h"
#include "control.h"
#include "player.h"

#include <qmovie.h>
#include <QBuffer>
#include <QPainter>
#include <QtSvg/QSvgRenderer>
#include <qimage.h>
#include <qfile.h>
#include <QFileInfo>
#include <qurl.h>
#include <qtextcodec.h>
#include <qtextstream.h>

using namespace KMPlayer;

namespace {
    typedef std::map <QString, ImageDataPtrW> ImageDataMap;
    static ImageDataMap *image_data_map;
}

//------------------------%<----------------------------------------------------

MediaManager::MediaManager (Control *ctrl) : m_control (ctrl) {
    if (!image_data_map)
        image_data_map = new ImageDataMap;
}

MediaManager::~MediaManager () {
    if (image_data_map->size () == 0) {
        delete image_data_map;
        image_data_map = 0;
    }
}

//------------------------%<----------------------------------------------------

MediaObject::MediaObject (MediaManager *manager, Node *node)
 : m_manager (manager), m_node (node) {
   //manager->medias ().push_back (this);
}

MediaObject::~MediaObject () {
    //m_manager->medias ().remove (this);
}

KDE_NO_EXPORT void MediaObject::dismiss () {
    delete this;
}

//------------------------%<----------------------------------------------------

static bool isPlayListMime (const QString & mime) {
    QByteArray ba = mime.toAscii ();
    const char *mimestr = ba.constData ();
    //debugLog () << "isPlayListMime  mime:" << mimestr << endl;
    return mimestr && (!strcmp (mimestr, "audio/mpegurl") ||
            !strcmp (mimestr, "audio/x-mpegurl") ||
            !strncmp (mimestr, "video/x-ms", 10) ||
            !strncmp (mimestr, "audio/x-ms", 10) ||
            //!strcmp (mimestr, "video/x-ms-wmp") ||
            //!strcmp (mimestr, "video/x-ms-asf") ||
            //!strcmp (mimestr, "video/x-ms-wmv") ||
            //!strcmp (mimestr, "video/x-ms-wvx") ||
            //!strcmp (mimestr, "video/x-msvideo") ||
            !strcmp (mimestr, "video/x-real") ||
            !strcmp (mimestr, "audio/x-scpls") ||
            !strcmp (mimestr, "audio/x-shoutcast-stream") ||
            !strcmp (mimestr, "audio/x-pn-realaudio") ||
            !strcmp (mimestr, "audio/vnd.rn-realaudio") ||
            !strcmp (mimestr, "audio/m3u") ||
            !strcmp (mimestr, "audio/x-m3u") ||
            !strncmp (mimestr, "text/", 5) ||
            (!strncmp (mimestr, "application/", 12) &&
             strstr (mimestr + 12,"+xml")) ||
            !strncasecmp (mimestr, "application/smil", 16) ||
            !strncasecmp (mimestr, "application/xml", 15) ||
            !strcmp (mimestr, "application/news_reader") ||
            !strcmp (mimestr, "application/rss+xml") ||
            !strcmp (mimestr, "application/atom+xml") ||
            !strcmp (mimestr, "application/ram") ||
            !strcmp (mimestr, "image/vnd.rn-realpix") ||
            !strcmp (mimestr, "image/svg+xml") ||
            !strcmp (mimestr, "application/x-mplayer2") ||
            !strcmp (mimestr, "application/sdp"));
}

static bool isAudioOnlyMime (const QString& mime) {
    return mime.startsWith ("audio/");
}

MediaInfo::MediaInfo (Node *n, MediaManager::MediaType t)
 : media (NULL), type (t), node (n), job (NULL) {
}

MediaInfo::~MediaInfo () {
    clearData ();
    if (media)
        media->dismiss ();
}

KDE_NO_EXPORT void MediaInfo::killWGet () {
    if (job) {
        job->kill (); // quiet, no result signal
        job = 0L;
        clearData (); // assume data is invalid
    }
}

/**
 * Gets contents from url and puts it in data
 */
KDE_NO_EXPORT bool MediaInfo::wget (const QString &str, const QString&) {
    clearData ();
    url = str;

    if (MediaManager::Any == type || MediaManager::Image == type) {
        ImageDataMap::iterator i = image_data_map->find (str);
        if (i != image_data_map->end ()) {
            media = new ImageMedia (node, i->second);
            type = MediaManager::Image;
            ready ();
            return true;
        }
    }

    Mrl *mrl = node->mrl ();
    if (mrl && (MediaManager::Any == type || MediaManager::AudioVideo == type))
    {
        mime = mrl->mimetype;
#if 0
def __ARMEL__
        if (NpPlayer::isFlashMimeType (mime)) {
            ready ();
            return true; // FIXME
        }
#endif
    }

    if (str.startsWith ("data:")) {
        QString tmp = QUrl::fromPercentEncoding (str.mid (5).toUtf8 ());
        bool html = tmp.startsWith( "text/html" );
        int pos = tmp.indexOf (QChar (','));
        if (pos > -1)
            tmp = tmp.mid (pos + 1);
        if (html && !tmp.isEmpty ()) {
            Document *doc = new Document (QString ());
            NodePtr store = doc;
            doc->insertBefore (tmp, NULL);
            tmp = doc->innerText ();
            doc->dispose ();
        }
        if (tmp.isEmpty ()
                || (pos > 5 && !str.mid (5, pos-5).startsWith ("text/")))
            data.resize (0); //TODO
        else
            data = tmp.toUtf8 ();
        ready ();
        return true;
    }

    QUrl uri (str);
    if (Url::isLocalFile (uri)) {
        if (mime.isEmpty ()) {
            setMimetype (MimeType::findByURL (uri));
            if (mrl)
                mrl->mimetype = mime;
        } else {
            setMimetype (mime);
        }
    }

    bool only_playlist = false;
    bool maybe_playlist = false;
    if (MediaManager::Audio == type || MediaManager::AudioVideo == type) {
        only_playlist = true;
        if (!mime.isEmpty ())
            maybe_playlist = isPlayListMime (mime);
    }

    if (mrl) {
        if (mime.isEmpty ()) {
            for (Node *p = mrl->parentNode (); p; p = p->parentNode ()) {
                Mrl *m = p->mrl ();
                if (m && m->audio_only) {
                    mrl->audio_only = true;
                    break;
                }
            }
        } else {
            mrl->audio_only |= isAudioOnlyMime (mime);
        }
    }
    debugLog () << "MediaObject::wget " << str << endl;
    if (Url::isLocalFile (uri)) {
        for (Node *p = node->parentNode (); p; p = p->parentNode ()) {
            Mrl *m = p->mrl ();
            if (m && !m->src.isEmpty () &&
                    m->src != "Playlist://" &&
                    !Url::isLocalFile (QUrl (m->src))) {
                errorLog () << "local acces denied\n";
                ready ();
                return true;
            }
        }
        QFile file (uri.path ());
        if (QFileInfo (file).exists () && file.open (QIODevice::ReadOnly)) {
            if (only_playlist) {
                maybe_playlist &= file.size () < 2000000;
                if (maybe_playlist) {
                    char buf[256];
                    off_t nr = file.read (buf, sizeof (buf) - 1);
                    if (nr > 0) {
                        buf[nr] = 0;
                        int sz = nr > 512 ? 512 : nr;
                        if (MimeType::isBufferBinaryData (buf, sz) ||
                                !strncmp (buf, "RIFF", 4))
                            maybe_playlist = false;
                    }
                }
                if (!maybe_playlist) {
                    ready ();
                    return true;
                }
            }
            file.seek (0);
            data = file.readAll ();
            file.close ();
        }
        ready ();
        return true;
    }

    QString protocol = uri.scheme ();
    if (protocol == "mms" || protocol == "rtsp" ||
            protocol == "rtp" || protocol == "rtmp" ||
            (only_playlist && !maybe_playlist && !mime.isEmpty ())) {
        if (MediaManager::Any == type)
            type = MediaManager::AudioVideo;
        ready ();
        return true;
    }

    job = asyncGet (this, url);
    job->start ();
    return false;
}

KDE_NO_EXPORT bool MediaInfo::readChildDoc () {
    QTextStream textstream (&data, QIODevice::ReadOnly);
    QString line;
    NodePtr cur_elm = node;
    do {
        line = textstream.readLine ();
    } while (!line.isNull () && line.trimmed ().isEmpty ());
    if (!line.isNull ()) {
        bool pls_groupfound =
            line.startsWith ("[") && line.endsWith ("]") &&
            line.mid (1, line.length () - 2).trimmed () == "playlist";
        if ((pls_groupfound &&
                    cur_elm->mrl ()->mimetype.startsWith ("audio/")) ||
                cur_elm->mrl ()->mimetype == "audio/x-scpls") {
            int nr = -1;
            struct Entry {
                QString url, title;
#ifdef _HARMATTAN
								QString audio;
#endif
            } * entries = 0L;
            do {
                line = line.trimmed ();
                if (!line.isEmpty ()) {
                    if (line.startsWith ("[") && line.endsWith ("]")) {
                        if (line.mid (1, line.length () - 2).trimmed () == "playlist")
                            pls_groupfound = true;
                        else
                            break;
                    } else if (pls_groupfound) {
                        int eq_pos = line.indexOf (QChar ('='));
                        if (eq_pos > 0) {
                            if (line.toLower ().startsWith (QString ("numberofentries"))) {
                                nr = line.mid (eq_pos + 1).trimmed ().toInt ();
                                if (nr > 0 && nr < 1024)
                                    entries = new Entry[nr];
                                else
                                    nr = 0;
                            } else if (nr > 0) {
                                QString ll = line.toLower ();
                                if (ll.startsWith (QString ("file"))) {
                                    int i = line.mid (4, eq_pos-4).toInt ();
                                    if (i > 0 && i <= nr)
                                        entries[i-1].url = line.mid (eq_pos + 1).trimmed ();
                                } else if (ll.startsWith (QString ("title"))) {
                                    int i = line.mid (5, eq_pos-5).toInt ();
                                    if (i > 0 && i <= nr)
                                        entries[i-1].title = line.mid (eq_pos + 1).trimmed ();
                                }
                            }
                        }
                    }
                }
                line = textstream.readLine ();
            } while (!line.isNull ());
            NodePtr doc = node->document ();
            for (int i = 0; i < nr; i++)
                if (!entries[i].url.isEmpty ()) {
#ifdef _HARMATTAN
                    GenericURL *gen = new GenericURL (doc, QUrl::fromPercentEncoding (entries[i].url.toAscii ()), QUrl::fromPercentEncoding (entries[i].audio.toAscii()), entries[i].title);
#else
                    GenericURL *gen = new GenericURL (doc,
                            QUrl::fromPercentEncoding (entries[i].url.toAscii ()),
                            entries[i].title);
#endif
                    cur_elm->appendChild (gen);
                    gen->opener = cur_elm;
                }
            delete [] entries;
        } else if (line.trimmed ().startsWith (QChar ('<'))) {
            readXML (cur_elm, textstream, line);
            //cur_elm->normalize ();
        } else if (line.toLower ().startsWith ("[reference]")) {
            NodePtr doc = node->document ();
            for (line = textstream.readLine ();
                    !line.isNull ();
                    line = textstream.readLine ()) {
                if (line.toLower ().startsWith ("ref")) {
                    int p = line.indexOf ("=", 3);
                    if (p > -1) {
                        QUrl u (line.mid (p + 1));
                        if (u.scheme() == "http" &&
                                line.indexOf("MSWMExt=.asf") > -1)
                            cur_elm->appendChild (new GenericURL (doc,
                                        QString ("mms") + u.toString().mid (4)));
                        else if (u.scheme() == "mms")
                            cur_elm->appendChild (new GenericURL(doc, u.toString()));
                    }
                }
            }
        } else if (line.toLower () != "[reference]") {
            bool extm3u = line.startsWith ("#EXTM3U");
            QString title;
            NodePtr doc = node->document ();
            if (extm3u)
                line = textstream.readLine ();
            while (!line.isNull ()) {
             /* TODO && m_document.size () < 1024 / * support 1k entries * /);*/
                QString mrl = line.trimmed ();
                if (line == "--stop--")
                    break;
                if (mrl.toLower ().startsWith (QString ("asf ")))
                    mrl = mrl.mid (4).trimmed ();
                if (!mrl.isEmpty ()) {
                    if (extm3u && mrl.startsWith (QChar ('#'))) {
                        if (line.startsWith ("#EXTINF:"))
                            title = line.mid (9);
                        else
                            title = mrl.mid (1);
                    } else if (!line.startsWith (QChar ('#'))) {
#ifdef _HARMATTAN
                        GenericURL *gen = new GenericURL (doc, mrl, QString(), title);
#else
                        GenericURL *gen = new GenericURL (doc, mrl, title);
#endif
                        cur_elm->appendChild (gen);
                        gen->opener = cur_elm;
                        title.truncate (0);
                    }
                }
                line = textstream.readLine ();
            }
        }
    }
    return !cur_elm->isPlayable ();
}

void MediaInfo::setMimetype (const QString &mt)
{
    mime = mt;

    Mrl *mrl = node ? node->mrl () : NULL;
    if (mrl && mrl->mimetype.isEmpty ())
        mrl->mimetype = mt;

    if (!mt.isEmpty () && MediaManager::Any == type) {
        if (mimetype ().startsWith ("image/"))
            type = MediaManager::Image;
        else if (mime.startsWith ("audio/"))
            type = MediaManager::Audio;
        else
            type = MediaManager::AudioVideo;
    }
}

KDE_NO_EXPORT QString MediaInfo::mimetype () {
    if (data.size () > 0 && mime.isEmpty ())
        mime = MimeType::findByContent (data.data (), data.size ());
    return mime;
}

KDE_NO_EXPORT void MediaInfo::clearData () {
    killWGet ();
    url.truncate (0);
    mime.truncate (0);
    data.resize (0);
}

KDE_NO_EXPORT bool MediaInfo::downloading () const {
    return !!job;
}

KDE_NO_EXPORT void MediaInfo::create () {
    MediaManager *mgr = (MediaManager *)node->document()->role(RoleMediaManager);
    if (!media) {
        switch (type) {
            case MediaManager::Audio:
            case MediaManager::AudioVideo:
                debugLog() << "create AudioVideo " << data.size () << endl;
                if (!data.size () || !readChildDoc ())
                    media = new AudioVideoMedia (mgr, node);
                break;
            case MediaManager::Image:
                if (data.size () && mime == "image/svg+xml") {
                    readChildDoc ();
                    if (node->firstChild () &&
                            id_node_svg == node->lastChild ()->id) {
                        media = new ImageMedia (node);
                        break;
                    }
                }
                if (data.size () &&
                        (!(!MimeType::isBufferBinaryData (
                                data.data (),
                                data.size () > 512 ? 512 : data.size ()) ||
                           mime == "image/vnd.rn-realpix") ||
                         !readChildDoc ()))
                    media = new ImageMedia (mgr, node, url, data);
                break;
            case MediaManager::Text:
                if (data.size ())
                    media = new TextMedia (mgr, node, data);
                break;
            default: // Any
                break;
        }
    }
    data = QByteArray ();
}

KDE_NO_EXPORT void MediaInfo::ready () {
    create ();
    node->document()->post (node, new Posting (node, MsgMediaReady));
}

static bool validDataFormat (MediaManager::MediaType type, const QByteArray &ba) {
    int sz = ba.size () > 512 ? 512 : ba.size();

    switch (type) {
    case MediaManager::Any:
        //TODO
    case MediaManager::Audio:
    case MediaManager::AudioVideo: {
        bool kill (ba.size () > 2000000 ||            // too big
                (ba.size () > 4 &&
                 !strncmp (ba.data (), "RIFF", 4))); // falsely seen as text
        if (!kill && ba.size() > 14) {
            int sz = ba.size () > 512 ? 512 : ba.size ();
            kill = MimeType::isBufferBinaryData (ba.data(), sz) ||
                (!strncasecmp (ba.data (), "<html", 5) || // html admin
                 !strncasecmp (ba.data (), "<!doctype html", 14));
        }
        return !kill;
    }
    default:
        return true;
    }
}

KDE_NO_EXPORT void MediaInfo::jobResult (IOJob *jb) {
    if (jb->error ())
        data.resize (0);
    job = 0L; // signal KIO::IOJob::result deletes itself
    if (mime.isEmpty ()) {
        if (data.size () > 0) {
            setMimetype (MimeType::findByContent (data.data(), data.size ()));
            if (!validDataFormat (type, data))
                data.resize (0);
        }
        if (node->mrl ()) {
            node->mrl ()->mimetype = mime;
            node->mrl ()->audio_only = isAudioOnlyMime (mime);
        }
    }
    ready ();
}

KDE_NO_EXPORT void MediaInfo::jobData (IOJob *jb, QByteArray &ba) {
    //debugLog () << "MediaTypeRuntime::jobData " << data.size () << endl;
    if (ba.size ()) {
        int old_size = data.size ();
        int newsize = old_size + ba.size ();

        data.resize (newsize);
        memcpy (data.data () + old_size, ba.data (), ba.size ());

        if (old_size < 512 && newsize >= 512) {
            setMimetype (MimeType::findByContent (data.data (), 512));
            if (!validDataFormat (type, data)) {
                data.resize (0);
                job->kill (false); // not quiet, wants jobResult
            }
        }
    }
}

KDE_NO_EXPORT void MediaInfo::redirected (IOJob *, const QString &url) {
    Mrl *mrl = node ? node->mrl () : NULL;
    if (mrl)
        mrl->src = url;
}

//------------------------%<----------------------------------------------------

AudioVideoMedia::AudioVideoMedia (MediaManager *manager, Node *node)
 : MediaObject (manager, node) {
}

AudioVideoMedia::~AudioVideoMedia () {
    stop ();
    debugLog() << "AudioVideoMedia::~AudioVideoMedia" << endl;
}

bool AudioVideoMedia::play () {
    Process *process = m_manager->control ()->process ();
    if (process && process->playing () && process->mrl () == sourceMrl ()) {
        errorLog () << "play: already playing " << sourceMrl ()->src << endl;
    } else if (m_manager->control ()->requestPlayURL (sourceMrl ())) {
        if (!sourceMrl ()->audio_only)
            m_manager->control ()->setAudioVideoNode (m_node);
        return true;
    }
    return false;
}

void AudioVideoMedia::stop () {
    Process *process = m_manager->control ()->process ();
    if (process && process->mrlBackEndNotify () == this) {
        process->stop ();
        process->setMrlBackEndNotify (NULL);
    }
    m_manager->control ()->setAudioVideoNode (NULL);
}

void AudioVideoMedia::pause () {
    Process *process = m_manager->control ()->process ();
    if (process && process->playing ())
        process->pause ();
}

void AudioVideoMedia::unpause () {
    Process *process = m_manager->control ()->process ();
    if (process && process->playing ())
        process->play (QString ());
}

Mrl *AudioVideoMedia::sourceMrl ()
{
    return m_node ? m_node->mrl () : NULL;
}

void AudioVideoMedia::referenceMrl (const QString &uri)
{
    Mrl *_mrl = sourceMrl ();
    if (_mrl && !uri.isEmpty ()) {
        for (Node *n = _mrl; n; n = n->parentNode ()) {
            Mrl *m = n->mrl ();
            if (m && m->src == uri)
                return;
        }
        for (Node *n = _mrl->firstChild (); n; n = n->nextSibling ()) {
            Mrl *m = n->mrl ();
            if (m && m->src == uri)
                return;
        }

        NodePtr doc = _mrl->document ();
#ifdef _HARMATTAN
        Node *n = new GenericURL (doc, uri, QString(), QString ());
#else
        Node *n = new GenericURL (doc, uri, QString ());
#endif
        n->id = id_node_ref_url;
        n->setAuxiliaryNode (true);
        _mrl->appendChild (n);

        m_manager->control ()->updatePlaylistView ();
    }
}

void AudioVideoMedia::infoMessage (const QString &msg) {
    if (sourceMrl ())
        sourceMrl ()->document ()->message (MsgInfoString, (void *) &msg);
}

void AudioVideoMedia::readyToPlay (Process *process)
{
    Mrl *mrl = sourceMrl ();
    if (mrl) {
        process->setMrlBackEndNotify (this);
        process->setRepeat (mrl->repeat);
        process->setHasVideo (!mrl->audio_only);
        // TODO set aspect/crop/http_object args
#ifdef _HARMATTAN
        process->play(mrl->absolutePath(), mrl->audio);
#else
        process->play (mrl->absolutePath ());
#endif
    }
}

//------------------------%<----------------------------------------------------

ImageData::ImageData( const QString & img)
 : width (0),
   height (0),
   flags (0),
   has_alpha (false),
   image (0L),
   surface (NULL),
   url (img) {
    //if (img.isEmpty ())
    //    //debugLog() << "New ImageData for " << this << endl;
    //else
    //    //debugLog() << "New ImageData for " << img << endl;
}

ImageData::~ImageData() {
    if (!url.isEmpty ())
        image_data_map->erase (url);
    if (surface)
        cairo_surface_destroy (surface);
    delete image;
}

void ImageData::setImage (QImage *img) {
    if (image != img) {
        delete image;
        if (surface) {
            cairo_surface_destroy (surface);
            surface = NULL;
        }
        image = img;
        if (img) {
            width = img->width ();
            height = img->height ();
            has_alpha = img->hasAlphaChannel ();
        } else {
            width = height = 0;
        }
    }
}

void ImageData::setImage (cairo_surface_t *s, int w, int h) {
    delete image;
    image = NULL;
    if (surface)
        cairo_surface_destroy (surface);
    surface = s;
    width = w;
    height = h;
    has_alpha = true;
}

ImageMedia::ImageMedia (MediaManager *manager, Node *node,
        const QString &url, const QByteArray &ba)
 : MediaObject (manager, node),
   buffer (NULL),
   img_movie (NULL),
   svg_renderer (NULL),
   update_render (false) {
    setupImage (url, ba);
}

ImageMedia::ImageMedia (Node *node, ImageDataPtr id)
 : MediaObject ((MediaManager *)node->document()->role(RoleMediaManager),
         node),
   buffer (NULL),
   img_movie (NULL),
   svg_renderer (NULL),
   update_render (false) {
    if (!id) {
        Node *c = findChildWithId (node, id_node_svg);
        if (c) {
            svg_renderer = new QSvgRenderer (c->outerXML().toUtf8 ());
            if (svg_renderer->isValid ()) {
                cached_img = new ImageData (QString ());
                cached_img->flags = ImageData::ImageScalable;
            } else {
                delete svg_renderer;
                svg_renderer = NULL;
            }
        }
    } else {
        cached_img = id;
    }
}

ImageMedia::~ImageMedia () {
    stop ();
    delete img_movie;
    delete svg_renderer;
    delete buffer;
}

KDE_NO_EXPORT bool ImageMedia::play () {
    if (!img_movie)
        return false;
    if (img_movie->state () == QMovie::Paused)
        img_movie->setPaused (false);
    else if (img_movie->state () != QMovie::Running)
        img_movie->start ();
    return true;
}

KDE_NO_EXPORT void ImageMedia::stop () {
    pause ();
}

void ImageMedia::pause () {
    if (img_movie && !img_movie->state () == QMovie::Paused)
        img_movie->setPaused (true);
}

KDE_NO_EXPORT unsigned ImageMedia::render (ISize sz) {
    unsigned penalty = 0;
    if (svg_renderer && update_render) {
        delete svg_renderer;
        svg_renderer = NULL;
        Node *c = findChildWithId (m_node, id_node_svg);
        if (c) {
            QString xml = c->outerXML();
            QSvgRenderer *r = new QSvgRenderer (xml.toUtf8 ());
            if (r->isValid ()) {
                cached_img->setImage (NULL);
                svg_renderer = r;
            } else {
                delete r;
            }
            penalty += xml.length () / 1024;
        }
        update_render = false;
    }
    if (svg_renderer &&
           (cached_img->width != sz.width || cached_img->height != sz.height)) {
        QImage *img = new QImage (sz.width, sz.height,
                QImage::Format_ARGB32_Premultiplied);
        img->fill (0x0);
        QPainter paint (img);
        paint.setViewport (QRect (0, 0, sz.width, sz.height));
        svg_renderer->render (&paint);
        cached_img->setImage (img);
        penalty += sz.width * sz.height / (10 * 480);
    }
    return penalty;
}

KDE_NO_EXPORT void ImageMedia::updateRender () {
    update_render = true;
    if (m_node)
        m_node->document()->post(m_node, new Posting (m_node, MsgMediaUpdated));
}

KDE_NO_EXPORT void ImageMedia::sizes (SSize &size) {
    if (svg_renderer) {
        QSize s = svg_renderer->defaultSize ();
        size.width = s.width ();
        size.height = s.height ();
    } else if (cached_img) {
        size.width = cached_img->width;
        size.height = cached_img->height;
    } else {
        size.width = 0;
        size.height = 0;
    }
}

void ImageMedia::unpause () {
    if (img_movie && img_movie->state () == QMovie::Paused)
        img_movie->setPaused (false);
}

KDE_NO_EXPORT
void ImageMedia::setupImage (const QString &url, const QByteArray &data) {
    if (data.size ()) {
        QImage pix = QImage::fromData (data);
        if (!pix.isNull ()) {
            cached_img = ImageDataPtr (new ImageData (url));
            cached_img->setImage (new QImage (pix));
        }
        /*buffer = new QBuffer (&data);
        img_movie = new QMovie (buffer);
        //kDebug() << img_movie->frameCount ();
        if (img_movie->frameCount () > 1) {
            cached_img->flags |= (short)ImageData::ImagePixmap | ImageData::ImageAnimated;
            connect (img_movie, SIGNAL (updated (const QRect &)),
                    this, SLOT (movieUpdated (const QRect &)));
            connect (img_movie, SIGNAL (stateChanged (QMovie::MovieState)),
                    this, SLOT (movieStatus (QMovie::MovieState)));
            connect (img_movie, SIGNAL (resized (const QSize &)),
                    this, SLOT (movieResize (const QSize &)));
        } else {
            delete img_movie;
            img_movie = 0L;
            delete buffer;
            buffer = 0L;
            frame_nr = 0; */
            cached_img->flags |= (short)ImageData::ImagePixmap;
            (*image_data_map)[url] = ImageDataPtrW (cached_img);
        //}
    }
}

bool ImageMedia::isEmpty () const {
    return !cached_img || (!svg_renderer && cached_img->isEmpty ());
}

//------------------------%<----------------------------------------------------

static int default_font_size = 11;

TextMedia::TextMedia (MediaManager *manager, Node *node, const QByteArray &ba)
 : MediaObject (manager, node) {
    if (!ba.isEmpty ())
        text = QString::fromUtf8 (ba);
}

TextMedia::~TextMedia () {
}

bool TextMedia::play () {
    return !text.isEmpty ();
}

int TextMedia::defaultFontSize () {
    return default_font_size;
}
