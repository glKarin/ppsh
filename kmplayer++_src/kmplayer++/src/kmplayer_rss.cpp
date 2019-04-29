/**
 * Copyright (C) 2005 by Koos Vriezen <koos.vriezen@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
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
 **/

#include "kmplayer_rss.h"
#include "kmplayer_atom.h"

using namespace KMPlayer;

KDE_NO_EXPORT Node *RSS::Rss::childFromTag (const QString & tag) {
    if (!strcmp (tag.toLatin1 ().constData (), "channel"))
        return new RSS::Channel (m_doc);
    return 0L;
}

void *RSS::Rss::role (RoleType msg, void *content)
{
    if (RolePlaylist == msg)
        return NULL;
    return Element::role (msg, content);
}

KDE_NO_EXPORT Node *RSS::Channel::childFromTag (const QString & tag) {
    QByteArray ba = tag.toLatin1 ();
    const char *ctag = ba.constData ();
    if (!strcmp (ctag, "item"))
        return new RSS::Item (m_doc);
    else if (!strcmp (ctag, "title"))
        return new DarkNode (m_doc, ctag, id_node_title);
    else if (!strncmp (ctag, "itunes", 6) ||
            !strncmp (ctag, "media", 5))
        return new DarkNode (m_doc, ctag, id_node_ignored);
    return 0L;
}

KDE_NO_EXPORT void RSS::Channel::closed () {
    for (Node *c = firstChild (); c; c = c->nextSibling ())
        if (c->id == id_node_title) {
            title = c->innerText ().simplified ();
            break;
        }
    Element::closed ();
}

void *RSS::Channel::role (RoleType msg, void *content)
{
    if (RolePlaylist == msg)
        return !title.isEmpty () || //return false if no title and only one
            previousSibling () || nextSibling () ? (PlaylistRole *) this : NULL;
    return Element::role (msg, content);
}

KDE_NO_EXPORT Node *RSS::Item::childFromTag (const QString & tag) {
    QByteArray ba = tag.toLatin1 ();
    const char *ctag = ba.constData ();
    if (!strcmp (ctag, "enclosure"))
        return new RSS::Enclosure (m_doc);
    else if (!strcmp (ctag, "title"))
        return new DarkNode (m_doc, ctag, id_node_title);
    else if (!strcmp (ctag, "description"))
        return new DarkNode (m_doc, ctag, id_node_description);
    else if (!strcmp (ctag, "category"))
        return new DarkNode (m_doc, ctag, id_node_category);
    else if (!strcmp (ctag, "media:group"))
        return new ATOM::MediaGroup (m_doc);
    else if (!strncmp (ctag, "itunes", 6) ||
            !strncmp (ctag, "feedburner", 10) ||
            !strcmp (ctag, "link") ||
            !strcmp (ctag, "pubDate") ||
            !strcmp (ctag, "guid") ||
            !strncmp (ctag, "media", 5))
        return new DarkNode (m_doc, ctag, id_node_ignored);
    return 0L;
}

KDE_NO_EXPORT void RSS::Item::closed () {
    if (!summary_added) {
        ATOM::MediaGroup *group = NULL;
        Enclosure *enclosure = NULL;
        for (Node *c = firstChild (); c; c = c->nextSibling ()) {
            switch (c->id) {
                case id_node_title:
                    title = c->innerText ().simplified ();
                    break;
                case id_node_enclosure:
                    enclosure = static_cast <Enclosure *> (c);
                    break;
                case id_node_description:
                    description = c->innerText ();
                    break;
                case ATOM::id_node_media_group:
                    group = static_cast <ATOM::MediaGroup *> (c);
                    break;
            }
        }
        if (enclosure) {
            enclosure->setCaption (title);
            enclosure->description = description;
            int content_links = 0;
            if (group) {
                for (Node *c = group->firstChild (); c; ) {
                    Node *next = c->nextSibling ();
                    if (ATOM::id_node_media_content == c->id) {
                        Mrl *mrl = c->mrl ();
                        if (mrl && mrl->src == enclosure->src)
                            group->removeChild (c);
                        else if (!mrl->src.isEmpty ())
                            content_links++;
                    }
                    c = next;
                }
                if (!content_links) {
                    removeChild (group);
                    group = NULL;
                }
            }
        }
        if (!group)
            title.clear ();
        summary_added = true;
    }
    Element::closed ();
}

void *RSS::Item::role (RoleType msg, void *content)
{
    if (RolePlaylist == msg)
        return title.isEmpty () ? NULL : (PlaylistRole *) this;
    if (RoleDescription == msg)
        return description.isEmpty () ? NULL : (DescriptionRole *) this;
    return Element::role (msg, content);
}

KDE_NO_EXPORT void RSS::Enclosure::activate () {
    document ()->message (MsgInfoString, &description);
    Mrl::activate ();
}

KDE_NO_EXPORT void RSS::Enclosure::deactivate () {
    document ()->message (MsgInfoString, NULL);
    Mrl::deactivate ();
}

KDE_NO_EXPORT void RSS::Enclosure::closed () {
    src = getAttribute (Ids::attr_url);
    Mrl::closed ();
}

void *RSS::Enclosure::role (RoleType msg, void *content)
{
    if (RoleDescription == msg)
        return description.isEmpty () ? NULL : (DescriptionRole *) this;
    return Mrl::role (msg, content);
}
