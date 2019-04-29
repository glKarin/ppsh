/* This file is part of the KDE project
 *
 * Copyright (C) 2006-2007 Koos Vriezen <koos.vriezen@gmail.com>
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
 */

#ifndef _KMPLAYER_RP_H_
#define _KMPLAYER_RP_H_

#include <qobject.h>
#include <qstring.h>

#include "playlist.h"
#include "surface.h"

namespace KMPlayer {

/**
 * RealPix support classes
 */
namespace RP {

const short id_node_imfl = 150;
const short id_node_head = 151;
const short id_node_image = 152;
const short id_node_crossfade = 153;
const short id_node_fill = 154;
const short id_node_wipe = 155;
const short id_node_fadein = 156;
const short id_node_fadeout = 157;
const short id_node_viewchange = 158;
const short id_node_animate = 159;
const short id_node_first = id_node_imfl;
const short id_node_last = 160;

class KMPLAYER_NO_EXPORT Imfl : public Mrl {
public:
    Imfl (NodePtr & d);
    ~Imfl ();
    KDE_NO_EXPORT virtual const char * nodeName () const { return "imfl"; }
    virtual Node *childFromTag (const QString & tag);
    virtual void closed ();
    virtual void defer ();      // start loading the images if not yet done
    virtual void activate ();   // start timings, handle paint events
    virtual void finish ();     // end the timings
    virtual void deactivate (); // stop handling paint events
    KDE_NO_EXPORT virtual PlayType playType () { return play_type_image; }
    virtual void message (MessageType msg, void *content=NULL);
    virtual void accept (Visitor *);
    Surface *surface ();
    void repaint (); // called whenever something changes on image
    Fit fit;        // how to layout images
    unsigned int duration; // cached attributes of head
    Posting *duration_timer;
    SurfacePtrW rp_surface;
    int needs_scene_img;
};

class KMPLAYER_NO_EXPORT TimingsBase  : public Element {
public:
    TimingsBase (NodePtr & d, const short id);
    KDE_NO_CDTOR_EXPORT ~TimingsBase () {}
    virtual void activate ();    // start the 'start_timer'
    virtual void begin ();       // start_timer has expired
    virtual void finish ();      // ?duration_timer has expired?
    virtual void deactivate ();  // disabled
    virtual void message (MessageType msg, void *content=NULL);
    int progress;
    Single x, y, w, h;
    Single srcx, srcy, srcw, srch;
    NodePtrW target;
protected:
    void update (int percentage);
    void cancelTimers ();
    unsigned int start, duration;
    int steps, curr_step;
    Posting *start_timer;
    Posting *duration_timer;
    Posting *update_timer;
    ConnectionLink document_postponed;
};

class KMPLAYER_NO_EXPORT Crossfade : public TimingsBase {
public:
    KDE_NO_CDTOR_EXPORT Crossfade (NodePtr & d)
        : TimingsBase (d, id_node_crossfade) {}
    KDE_NO_CDTOR_EXPORT ~Crossfade () {}
    KDE_NO_EXPORT virtual const char * nodeName () const { return "crossfade"; }
    virtual void activate ();
    virtual void begin ();
    virtual void accept (Visitor *);
};

class KMPLAYER_NO_EXPORT Fadein : public TimingsBase {
public:
    KDE_NO_CDTOR_EXPORT Fadein (NodePtr & d) : TimingsBase(d, id_node_fadein) {}
    KDE_NO_CDTOR_EXPORT ~Fadein () {}
    KDE_NO_EXPORT virtual const char * nodeName () const { return "fadein"; }
    virtual void activate ();
    virtual void begin ();
    virtual void accept (Visitor *);
    unsigned int from_color;
};

class KMPLAYER_NO_EXPORT Fadeout : public TimingsBase {
public:
    KDE_NO_CDTOR_EXPORT Fadeout(NodePtr &d) : TimingsBase(d, id_node_fadeout) {}
    KDE_NO_CDTOR_EXPORT ~Fadeout () {}
    KDE_NO_EXPORT virtual const char * nodeName () const { return "fadeout"; }
    virtual void activate ();
    virtual void begin ();
    virtual void accept (Visitor *);
    unsigned int to_color;
};

class KMPLAYER_NO_EXPORT Fill : public TimingsBase {
public:
    KDE_NO_CDTOR_EXPORT Fill (NodePtr & d) : TimingsBase (d, id_node_fill) {}
    KDE_NO_CDTOR_EXPORT ~Fill () {}
    KDE_NO_EXPORT virtual const char * nodeName () const { return "fill"; }
    virtual void activate ();
    virtual void begin ();
    unsigned int fillColor () const { return color; }
    virtual void accept (Visitor *);
    unsigned int color;
};

class KMPLAYER_NO_EXPORT Wipe : public TimingsBase {
public:
    KDE_NO_CDTOR_EXPORT Wipe (NodePtr & d) : TimingsBase (d, id_node_wipe) {}
    KDE_NO_CDTOR_EXPORT ~Wipe () {}
    KDE_NO_EXPORT virtual const char * nodeName () const { return "wipe"; }
    virtual void activate ();
    virtual void begin ();
    virtual void accept (Visitor *);
    enum { dir_right, dir_left, dir_up, dir_down } direction;
};

class KMPLAYER_NO_EXPORT ViewChange : public TimingsBase {
public:
    KDE_NO_CDTOR_EXPORT ViewChange (NodePtr & d)
        : TimingsBase (d, id_node_viewchange) {}
    KDE_NO_CDTOR_EXPORT ~ViewChange () {}
    KDE_NO_EXPORT virtual const char * nodeName() const { return "viewchange"; }
    virtual void activate ();
    virtual void begin ();
    virtual void finish ();
    virtual void accept (Visitor *);
};

class KMPLAYER_NO_EXPORT Image : public Mrl {
    PostponePtr postpone_lock;
public:
    Image (NodePtr & d);
    ~Image ();
    KDE_NO_EXPORT virtual const char * nodeName () const { return "image"; }
    virtual void activate ();
    virtual void begin ();
    virtual void deactivate ();
    virtual void closed ();
    virtual void message (MessageType msg, void *content=NULL);
    bool isReady (bool postpone_if_not = false); // is downloading ready
    Surface *surface ();
    SurfacePtrW img_surface;
protected:
    void dataArrived ();
};

} // RP namespace

}  // KMPlayer namespace

#endif //_KMPLAYER_RP_H_

