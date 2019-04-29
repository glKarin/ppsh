/**
 * Copyright (C) 2005-2006 by Koos Vriezen <koos.vriezen@gmail.com>
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

#include "kmplayer_atom.h"
#include "kmplayer_smil.h"

#include <QTextStream>

using namespace KMPlayer;

Node *ATOM::Feed::childFromTag (const QString & tag) {
    QByteArray ba = tag.toLatin1 ();
    const char *name = ba.constData ();
    if (!strcmp (name, "entry"))
        return new ATOM::Entry (m_doc);
    else if (!strcmp (name, "link"))
        return new ATOM::Link (m_doc);
    else if (!strcmp (name, "title"))
        return new DarkNode (m_doc, tag.toUtf8 (), id_node_title);
    return NULL;
}

void ATOM::Feed::closed () {
    for (Node *c = firstChild (); c; c = c->nextSibling ())
        if (c->id == id_node_title) {
            title = c->innerText ().simplified ();
            break;
        }
    Element::closed ();
}

void *ATOM::Feed::role (RoleType msg, void *content)
{
    if (RolePlaylist == msg)
        return !title.isEmpty () ? (PlaylistRole *) this : NULL;
    return Element::role (msg, content);
}

Node *ATOM::Entry::childFromTag (const QString &tag) {
    QByteArray ba = tag.toLatin1 ();
    const char *cstr = ba.constData ();
    if (!strcmp (cstr, "link"))
        return new ATOM::Link (m_doc);
    else if (!strcmp (cstr, "content"))
        return new ATOM::Content (m_doc);
    else if (!strcmp (cstr, "title"))
        return new DarkNode (m_doc, tag.toUtf8 (), id_node_title);
    else if (!strcmp (cstr, "summary"))
        return new DarkNode (m_doc, tag.toUtf8 (), id_node_summary);
    else if (!strcmp (cstr, "media:group"))
        return new MediaGroup (m_doc);
    else if (!strcmp (cstr, "gd:rating") ||
            !strcmp (cstr, "category") ||
            !strcmp (cstr, "author:") ||
            !strcmp (cstr, "id") ||
            !strcmp (cstr, "updated") ||
            !strncmp (cstr, "yt:", 3) ||
            !strncmp (cstr, "gd:", 3))
        return new DarkNode (m_doc, tag.toUtf8 (), id_node_ignored);
    return NULL;
}

void ATOM::Entry::closed () {
    MediaGroup *group = NULL;
    Link *link = NULL;

    for (Node *c = firstChild (); c; c = c->nextSibling ())
        if (c->id == id_node_title) {
            title = c->innerText ().simplified ();
        } else if (c->id == id_node_link) {
            Link *lnk = static_cast <Link *> (c);
            if (!lnk->src.isEmpty ())
                link = lnk;
        } else if (c->id == id_node_media_group) {
            group = static_cast <MediaGroup *> (c);
        } else if (c->id == id_node_content) {
            Mrl *mrl = c->mrl ();
            if (mrl->mimetype.startsWith ("text/") ||
                    mrl->mimetype.indexOf ("html") > -1)
                description = mrl->innerText ();
        }

    if (link && group)
        for (Node *c = group->firstChild (); c; c = c->nextSibling ())
            if (id_node_media_content == c->id && c->mrl ()->src == link->src) {
                link->src.clear ();
                link->title.clear ();
                break;
            }

    Element::closed ();
}

void *ATOM::Entry::role (RoleType msg, void *content)
{
    if (RolePlaylist == msg)
        return !title.isEmpty () ? (PlaylistRole *) this : NULL;
    if (RoleDescription == msg)
        return !description.isEmpty () ? (DescriptionRole *) this : NULL;
    return Element::role (msg, content);
}

Node::PlayType ATOM::Link::playType () {
    return src.isEmpty () ? play_type_none : play_type_unknown;
}

void ATOM::Link::closed () {
    QString href;
    QString rel;
    for (Attribute *a = attributes ().first (); a; a = a->nextSibling ()) {
        if (a->name () == Ids::attr_href)
            href = a->value ();
        else if (a->name () == Ids::attr_title)
            title = a->value ();
        else if (a->name () == "rel")
            rel = a->value ();
    }
    if (!href.isEmpty () && rel == QString::fromLatin1 ("enclosure"))
        src = href;
    else if (src.isEmpty ())
        title.clear ();
    Mrl::closed ();
}

void ATOM::Content::closed () {
    for (Attribute *a = attributes ().first (); a; a = a->nextSibling ()) {
        if (a->name () == Ids::attr_src)
            src = a->value ();
        else if (a->name () == Ids::attr_type) {
            QString v = a->value ().toLower ();
            if (v == QString::fromLatin1 ("text"))
                mimetype = QString::fromLatin1 ("text/plain");
            else if (v == QString::fromLatin1 ("html"))
                mimetype = QString::fromLatin1 ("text/html");
            else if (v == QString::fromLatin1 ("xhtml"))
                mimetype = QString::fromLatin1 ("application/xhtml+xml");
            else
                mimetype = v;
        }
    }
    Mrl::closed ();
}

Node::PlayType ATOM::Content::playType () {
    if (!hasChildNodes () && !src.isEmpty ())
        return play_type_unknown;
    return play_type_none;
}

Node *ATOM::MediaGroup::childFromTag (const QString &tag) {
    QByteArray ba = tag.toLatin1 ();
    const char *cstr = ba.constData ();
    if (!strcmp (cstr, "media:content"))
        return new ATOM::MediaContent (m_doc);
    else if (!strcmp (cstr, "media:title"))
        return new DarkNode (m_doc, tag.toUtf8 (), id_node_media_title);
    else if (!strcmp (cstr, "media:description"))
        return new DarkNode (m_doc, tag.toUtf8 (), id_node_media_description);
    else if (!strncmp (cstr, "media:", 6))
        return new DarkNode (m_doc, tag.toUtf8 (), id_node_ignored);
    else if (!strcmp (cstr, "smil"))
        return new SMIL::Smil (m_doc);
    return NULL;
}

 
void ATOM::MediaGroup::closed () {
    QString title;
    for (Node *c = firstChild (); c; c = c->nextSibling ())
        if (id_node_media_title == c->id) {
            title = c->innerText ();
            break;
        }
    if (!title.isEmpty ())
        for (Node *c = firstChild (); c; c = c->nextSibling ())
            if (id_node_media_content == c->id) {
                Mrl *mrl = c->mrl ();
                if (mrl && !mrl->src.isEmpty ()) {
                    QString s = title;
                    if (!mrl->mimetype.isEmpty ())
                        s += QChar (' ') + c->mrl ()->mimetype;
                    c->mrl ()->setCaption (s);
                }
            }
}

static bool playedAnyChild (const Node *n) {
    for (Node *c = n->firstChild (); c; c = c->nextSibling ()) {
        Mrl *mrl = c->mrl ();
        if (mrl && (mrl->has_played || playedAnyChild (c)))
            return true;
    }
    return false;
}

void ATOM::MediaGroup::message (MessageType msg, void *content) {
    if (MsgChildFinished == msg &&
            id_node_media_content == ((Posting *) content)->source->id &&
            (((Posting *) content)->source->mrl ()->has_played ||
             playedAnyChild (((Posting *) content)->source.ptr ())))
        finish (); // only play one
    Element::message (msg, content);
}

void ATOM::MediaContent::closed () {
    unsigned fsize = 0;
    TrieString fs ("fileSize");
    for (Attribute *a = attributes ().first (); a; a = a->nextSibling ()) {
        if (a->name () == Ids::attr_url)
            src = a->value();
        else if (a->name () == Ids::attr_type)
            mimetype = a->value ();
        else if (a->name () == Ids::attr_height)
            size.height = a->value ().toInt ();
        else if (a->name () == Ids::attr_width)
            size.width = a->value ().toInt ();
        else if (a->name () == Ids::attr_width)
            size.width = a->value ().toInt ();
        else if (a->name () == fs)
            fsize = a->value ().toInt ();
    }
    if (!mimetype.isEmpty ()) {
        title = mimetype;
        if (fsize > 0) {
            if (fsize > 1024 * 1024)
                title += QString (" (%1 Mb)").arg (fsize / (1024 * 1024));
            else
                title += QString (" (%1 kb)").arg (fsize / 1024);
        }
    }
    Mrl::closed ();
}
