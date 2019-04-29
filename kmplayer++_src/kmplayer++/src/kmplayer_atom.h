/* This file is part of the KDE project
 *
 * Copyright (C) 2005-2006 Koos Vriezen <koos.vriezen@gmail.com>
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

#ifndef _KMPLAYER_ATOM_H_
#define _KMPLAYER_ATOM_H_

#include <qstringlist.h>

#include "playlist.h"

namespace KMPlayer {

namespace ATOM {

const short id_node_feed = 300;
const short id_node_entry = 301;
const short id_node_link = 302;
const short id_node_title = 303;
const short id_node_summary = 304;
const short id_node_content = 305;
const short id_node_media_group = 306;
const short id_node_media_content = 307;
const short id_node_media_title = 308;
const short id_node_media_description = 309;
const short id_node_ignored = 313;

/**
 * '<feed>' tag
 */
class KMPLAYER_NO_EXPORT Feed : public Element, public PlaylistRole {
public:
    KDE_NO_CDTOR_EXPORT Feed (NodePtr & d) : Element (d, id_node_feed) {}
    Node *childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "feed"; }
    void closed ();
    void *role (RoleType msg, void *content=NULL);
};

class KMPLAYER_NO_EXPORT Entry : public Element, public PlaylistRole, public DescriptionRole {
public:
    KDE_NO_CDTOR_EXPORT Entry (NodePtr & d) : Element (d, id_node_entry) {}
    Node *childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "entry"; }
    void closed ();
    void *role (RoleType msg, void *content=NULL);
};

class KMPLAYER_NO_EXPORT Link : public Mrl {
public:
    KDE_NO_CDTOR_EXPORT Link (NodePtr & d) : Mrl (d, id_node_link) {}
    KDE_NO_EXPORT const char * nodeName () const { return "link"; }
    PlayType playType ();
    void closed ();
};

class KMPLAYER_NO_EXPORT Content : public Mrl {
public:
    KDE_NO_CDTOR_EXPORT Content (NodePtr &d) : Mrl(d, id_node_content) {}
    KDE_NO_EXPORT const char * nodeName () const { return "content"; }
    PlayType playType ();
    void closed ();
};

class KMPLAYER_NO_EXPORT MediaGroup : public Element {
public:
    MediaGroup (NodePtr &d) : Element (d, id_node_media_group) {}
    Node *childFromTag (const QString &tag);
    void closed ();
    void message (MessageType msg, void *content=NULL);
    KDE_NO_EXPORT const char *nodeName () const { return "media:group"; }
};

class KMPLAYER_NO_EXPORT MediaContent : public Mrl {
public:
    MediaContent (NodePtr &d) : Mrl (d, id_node_media_content) {}
    KDE_NO_EXPORT const char *nodeName () const { return "media:content"; }
    void closed ();
};

} //namespace ATOM


} // namespace KMPlayer

#endif //_KMPLAYER_ATOM_H_
