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

#ifndef _KMPLAYER__MEDIA_OBJECT_H_
#define _KMPLAYER__MEDIA_OBJECT_H_

#include "playlist.h"
#include "io.h"

class QMovie;
class QImage;
class QSvgRenderer;
class QBuffer;
class QByteArray;

namespace KMPlayer {

class Control;
class ImageData;
class CalculatedSizer;
class Surface;

/*
 * Class that creates MediaObject and keeps track objects
 */
class KMPLAYER_NO_EXPORT MediaManager {
public:
    enum MediaType { Any, Audio, AudioVideo, Image, Text };
    //typedef QValueList <MediaObject *> MediaList;

    MediaManager (Control *ctrl);
    ~MediaManager ();

    //MediaList &medias () { return m_media_objects; }
    Control *control () const { return m_control; }

private:
    //MediaList m_media_objects;
    Control *m_control;
};


//------------------------%<----------------------------------------------------

/*
 * Abstract base of MediaObject types, handles downloading
 */

class KMPLAYER_NO_EXPORT MediaObject {
    friend class MediaManager;
public:
    virtual MediaManager::MediaType type () const = 0;

    virtual bool play () = 0;
    virtual void pause () {}
    virtual void unpause () {}
    virtual void stop () {}
    /*virtual*/ void dismiss ();

protected:
    MediaObject (MediaManager *manager, Node *node);
    virtual ~MediaObject ();

    MediaManager *m_manager;
    NodePtrW m_node;
};

//------------------------%<----------------------------------------------------

class KMPLAYER_NO_EXPORT MediaInfo : public IOJobListener {
public:
    MediaInfo (Node *node, MediaManager::MediaType type);
    ~MediaInfo ();

    bool wget (const QString &url, const QString &from_domain=QString ());
    void killWGet ();
    void clearData ();
    QString mimetype ();
    bool downloading () const;
    void create ();

    QByteArray &rawData () { return data; }
    MediaObject *media;
    QString url;
    QByteArray data;
    QString mime;
    MediaManager::MediaType type;

private:
    virtual void jobData (IOJob *job, QByteArray &data);
    virtual void jobResult (IOJob *job);
    virtual void redirected (IOJob * job, const QString & uri);

    void ready ();
    bool readChildDoc ();
    void setMimetype (const QString &);

    Node *node;
    IOJob *job;
};


//------------------------%<----------------------------------------------------

/*
 * MediaObject for audio/video, groups Mrl, Process and Viewer
 */

class KMPLAYER_NO_EXPORT AudioVideoMedia : public MediaObject, public MrlBackEndNotify
{
    friend class MediaManager;
public:
    //enum Request { ask_nothing, ask_play, ask_stop, ask_delete };

    AudioVideoMedia (MediaManager *manager, Node *node);

    MediaManager::MediaType type () const { return MediaManager::AudioVideo; }

    virtual bool play ();
    virtual void stop ();
    virtual void pause ();
    virtual void unpause ();

    virtual Mrl *sourceMrl ();
    virtual void referenceMrl (const QString &url);
    virtual void infoMessage (const QString &msg);
    virtual void readyToPlay (Process *process);

protected:
    ~AudioVideoMedia ();
};


//------------------------%<----------------------------------------------------

/*
 * MediaObject for (animated)images
 */

struct KMPLAYER_NO_EXPORT ImageData {
    enum ImageFlags {
        ImageAny=0, ImagePixmap=0x1, ImageAnimated=0x2, ImageScalable=0x4
    };
    ImageData( const QString & img);
    ~ImageData();
    void setImage (QImage *img);
    void setImage (cairo_surface_t *s, int w, int h);
    void copyImage (Surface *s, const SSize &sz, cairo_surface_t *similar,
            unsigned int *penalty, CalculatedSizer *zoom=NULL);
    bool isEmpty () const {
         return !image && !surface; }

    unsigned short width;
    unsigned short height;
    short flags;
    bool has_alpha;
private:
    QImage *image;
    cairo_surface_t *surface;
    QString url;
};

typedef SharedPtr <ImageData> ImageDataPtr;
typedef WeakPtr <ImageData> ImageDataPtrW;

class KMPLAYER_NO_EXPORT ImageMedia : public MediaObject {
public:
    ImageMedia (MediaManager *manager, Node *node,
            const QString &url, const QByteArray &data);
    ImageMedia (Node *node, ImageDataPtr id = NULL);

    MediaManager::MediaType type () const { return MediaManager::Image; }

    bool play ();
    void stop ();
    void pause ();
    void unpause ();

    bool isEmpty () const;
    unsigned render (ISize size);
    void sizes (SSize &size);
    void updateRender ();
    bool movieTimer ();

    ImageDataPtr cached_img;

protected:
    ~ImageMedia ();

private:
    void setupImage (const QString &url, const QByteArray &ba);

    QBuffer *buffer;
    QMovie *img_movie;
    QSvgRenderer *svg_renderer;
    int frame_nr;
    bool update_render;
};

//------------------------%<----------------------------------------------------

/*
 * MediaObject for text
 */
class KMPLAYER_NO_EXPORT TextMedia : public MediaObject {
public:
    TextMedia (MediaManager *manager, Node *node, const QByteArray &ba);

    MediaManager::MediaType type () const { return MediaManager::Text; }

    bool play ();

    QString text;
    static int defaultFontSize ();

protected:
    ~TextMedia ();
};

} // namespace

#endif
