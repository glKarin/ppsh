/* This file is part of the KMPlayer project
 *
 * Copyright (C) 2006 Koos Vriezen <koos.vriezen@gmail.com>
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

#ifndef _KMPLAYER_LISTS_H_
#define _KMPLAYER_LISTS_H_

#include "playlist.h"
#include "control.h"
#include "io.h"
#include "player.h"

namespace KMPlayer {

class UnixProcess;
class UnixProcessListener;
class View;
class Expression;

static const short id_node_meta_group = 22;
static const short id_node_meta_views = 23;
static const short id_node_meta_view = 24;
//static const short id_node_group_node = 25;
//static const short id_node_playlist_document = 26;
//static const short id_node_playlist_item = 27;
static const short id_node_recent_document = 30;
static const short id_node_recent_node = 31;
static const short id_node_recent_more = 32;
static const short id_node_disk_document = 33;
static const short id_node_disk_node = 34;
//static const short id_node_html_object = 34;
//static const short id_node_html_embed = 35;
static const short id_node_gen_generator = 36;
static const short id_node_gen_input = 37;
static const short id_node_gen_uri = 38;
static const short id_node_gen_literal = 39;
static const short id_node_gen_ask = 40;
static const short id_node_gen_title = 41;
static const short id_node_gen_description = 42;
static const short id_node_gen_process = 43;
static const short id_node_gen_program = 44;
static const short id_node_gen_argument = 45;
static const short id_node_gen_predefined = 46;
static const short id_node_gen_document = 47;
static const short id_node_gen_http_get = 48;
static const short id_node_gen_http_key_value = 49;
static const short id_node_gen_sequence = 50;

inline bool isListItem (short id) {
    return id_node_recent_node == id ||
        id_node_playlist_item == id ||
        id_node_html_object == id;
}

class KMPLAYER_NO_EXPORT FileDocument : public KMPlayer::SourceDocument {
public:
    FileDocument (short id, const QString &, KMPlayer::Control * notify = 0L);
    KMPlayer::Node *childFromTag (const QString & tag);
    void readFromFile (const QString & file);
    void writeToFile (const QString & file);
    void sync (const QString & file);
    unsigned int load_tree_version;
};

class KMPLAYER_NO_EXPORT Recents : public FileDocument {
public:
    Recents (Control *a);
    void defer ();
    void activate ();
    void message (KMPlayer::MessageType msg, void *content=NULL);
    KMPlayer::Node *childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "playlist"; }
    QString path;
    Control * app;
};

class KMPLAYER_NO_EXPORT Recent : public KMPlayer::Mrl {
public:
    Recent (KMPlayer::NodePtr & doc, Control *a, const QString &url = QString());
    void activate ();
    void closed ();
    KDE_NO_EXPORT const char * nodeName () const { return "item"; }
    Control * app;
};

class KMPLAYER_NO_EXPORT Group : public Element, public PlaylistRole
{
public:
    Group (KMPlayer::NodePtr &doc, Control *a, const QString &pn=QString());
    KMPlayer::Node *childFromTag (const QString & tag);
    void closed ();
    void *role (KMPlayer::RoleType msg, void *content=NULL);
    KDE_NO_EXPORT const char * nodeName () const { return "group"; }
    Control * app;
};

class KMPLAYER_NO_EXPORT Playlist : public FileDocument {
public:
    Playlist (Control *c, bool plmod = false);

    void show (View *view);

    void closed ();
    void message (KMPlayer::MessageType msg, void *content=NULL);
    void defer ();
    void activate ();
    KMPlayer::Node *childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "playlist"; }
    Control *control;
    NodePtrW meta;
    QString path;
    bool playmode;
};

class KMPLAYER_NO_EXPORT PlaylistItemBase : public KMPlayer::Mrl {
public:
    PlaylistItemBase (KMPlayer::NodePtr &d, short id, Control *a, bool pm);
    void activate ();
    void closed ();
    void *role (KMPlayer::RoleType msg, void *content=NULL);
    Control * app;
    bool playmode;
    bool hidden;
};

class KMPLAYER_NO_EXPORT PlaylistItem : public PlaylistItemBase {
public:
    PlaylistItem (KMPlayer::NodePtr & doc, Control *a, bool playmode, const QString &url = QString());
    void closed ();
    void begin ();
    void setNodeName (const QString&);
    const char * nodeName () const;
};

class KMPLAYER_NO_EXPORT PlaylistGroupBase : public Element, public PlaylistRole
{
public:
    PlaylistGroupBase (KMPlayer::NodePtr &d, short id, Control *a, bool pm);
    KMPlayer::Node *childFromTag (const QString & tag);
    void closed ();
    void *role (KMPlayer::RoleType msg, void *content=NULL);
    Control *app;
    bool playmode;
};

class KMPLAYER_NO_EXPORT PlaylistGroup : public PlaylistGroupBase
{
public:
    PlaylistGroup (KMPlayer::NodePtr &doc, Control *a, bool plmode=false)
        : PlaylistGroupBase (doc, KMPlayer::id_node_group_node, a, plmode) {}
    void setNodeName (const QString&);
    const char * nodeName () const;
};

class KMPLAYER_NO_EXPORT PlaylistMetaGroup : public Mrl
{
public:
    PlaylistMetaGroup (KMPlayer::NodePtr &doc, Control *a);
    void closed ();
    void activate ();
    KMPlayer::Node *childFromTag (const QString &tag);
    Node::PlayType playType () { return play_type_unknown; }
    KDE_NO_EXPORT const char *nodeName () const { return "meta"; }
    NodePtrW views;
    Control *app;
};

class KMPLAYER_NO_EXPORT Views : public Element
{
public:
    Views (KMPlayer::NodePtr &doc);
    void closed ();
    KMPlayer::Node *childFromTag (const QString &tag);
    KDE_NO_EXPORT const char * nodeName () const { return "views"; }
    void message (KMPlayer::MessageType msg, void *content=NULL);
    NodePtr active_view;
};

class KMPLAYER_NO_EXPORT View : public Mrl
{
public:
    View (NodePtr &doc);
    ~View ();

    void activate ();
    void closed ();
    void *role (KMPlayer::RoleType msg, void *content=NULL);
    KDE_NO_EXPORT const char * nodeName () const { return "view"; }
    QString groupBy;
    Expression *show;
    Expression *before;
};

class KMPLAYER_NO_EXPORT HtmlObject : public PlaylistItemBase {
public:
    HtmlObject (KMPlayer::NodePtr & doc, Control *a, bool playmode);
    void activate ();
    void closed ();
    KMPlayer::Node *childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "object"; }
};

class KMPLAYER_NO_EXPORT Generator
    : public FileDocument, public UnixProcessListener, public IOJobListener {
public:
    Generator (KMPlayer::Control *src)
        : FileDocument (id_node_gen_document, QString (), src),
          unix_process (NULL), input_job (NULL), data (NULL), buffer (NULL) {}
    void activate ();
    void begin ();
    void deactivate ();
    Node *childFromTag (const QString & tag);
    KDE_NO_EXPORT const char *nodeName () const { return "generator"; }

    void readEvent (UnixProcess *);
    void writeEvent (UnixProcess *);
    void processExited (UnixProcess *);

    void jobData (IOJob *job, QByteArray &data);
    void jobResult (IOJob *job);
    void redirected (IOJob*, const QString&) {}

private:
    QString genReadProcess (Control *ctrl, Node *n);
    QString genReadInput (Control *ctrl, Node *n);
    QString genReadString (Control *ctr, Node *n);
    QString genReadUriGet (Control *ctr, Node *n);
    QString genReadAsk (Control *ctr, Node *n);

    QString data_buffer;
    UnixProcess *unix_process;
    IOJob *input_job;
    QTextStream *data;
    QByteArray *buffer;
    QString process;
    bool canceled;
    bool quote;
};

class KMPLAYER_NO_EXPORT GeneratorElement : public Element {
public:
    GeneratorElement (NodePtr &doc, const QByteArray &t, short id)
        : Element (doc, id), tag (t) {}
    Node *childFromTag (const QString & tag);
    KDE_NO_EXPORT const char *nodeName () const { return tag.constData (); }
    QByteArray tag;
};

}  // KMPlayer namespace

#endif //_KMPLAYER_LISTS_H_

