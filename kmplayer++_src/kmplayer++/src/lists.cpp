/**
 * Copyright (C) 2006 by Koos Vriezen <koos ! vriezen ? xs4all ! nl>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include <errno.h>
#include <string.h>
#include <glib.h>
#include <map>

#include "lists.h"
#include "control.h"
#include "mediaobject.h"
#include "expression.h"

#include <qfile.h>
#include <QFileInfo>
#include <QTextStream>

using namespace KMPlayer;

KDE_NO_CDTOR_EXPORT FileDocument::FileDocument (short i, const QString &s, Control *src)
 : SourceDocument (src, s), load_tree_version ((unsigned int)-1) {
    id = i;
}

KDE_NO_EXPORT KMPlayer::Node *FileDocument::childFromTag(const QString &tag) {
    if (tag == nodeName ())
        return this;
    return NULL;
}

void FileDocument::readFromFile (const QString & fn) {
    QFile file (fn);
    debugLog () << "readFromFile " << fn << endl;
    if (QFileInfo (file).exists ()) {
        file.open (QIODevice::ReadOnly);
        QTextStream xml (&file);
        xml.setCodec ("UTF-8");
        readXML (this, xml, QString (), false);
    }
    load_tree_version = m_tree_version;
}

void FileDocument::writeToFile (const QString & fn) {
    QFile file (fn);
    debugLog () << "writeToFile " << fn << m_tree_version << " " << load_tree_version << endl;
    file.open (QIODevice::WriteOnly | QIODevice::Truncate);
    file.write (outerXML ().toUtf8 ());
}

void FileDocument::sync (const QString &fn)
{
    if (resolved && load_tree_version != m_tree_version) {
        writeToFile (fn);
        load_tree_version = m_tree_version;
    }
}

KDE_NO_CDTOR_EXPORT Recents::Recents (Control *a)
    : FileDocument (id_node_recent_document, "recents://"),
      app(a) {
    title = "Most Recent";
}

KDE_NO_EXPORT void Recents::activate () {
    if (!resolved)
        defer ();
}

KDE_NO_EXPORT void Recents::defer () {
    if (!resolved) {
        resolved = true;
        readFromFile (path);
    }
}

KDE_NO_EXPORT KMPlayer::Node *Recents::childFromTag (const QString & tag) {
    // kdDebug () << nodeName () << " childFromTag " << tag << endl;
    if (tag == "item")
        return new Recent (m_doc, app);
    else if (tag == "group")
        return new Group (m_doc, app);
    return FileDocument::childFromTag (tag);
}

KDE_NO_EXPORT void Recents::message (KMPlayer::MessageType msg, void *data) {
    if (KMPlayer::MsgChildFinished == msg)
        finish ();
    else
        FileDocument::message (msg, data);
}

KDE_NO_CDTOR_EXPORT
Recent::Recent (KMPlayer::NodePtr & doc, Control * a, const QString &url)
  : KMPlayer::Mrl (doc, id_node_recent_node), app (a) {
    src = url;
    setAttribute (KMPlayer::Ids::attr_url, url);
}

KDE_NO_EXPORT void Recent::closed () {
    if (src.isEmpty ())
        src = getAttribute (KMPlayer::Ids::attr_url);
    if (title.isEmpty ())
        title = getAttribute (KMPlayer::Ids::attr_title);
    KMPlayer::Mrl::closed ();
}

KDE_NO_EXPORT void Recent::activate () {
    Document *doc = new SourceDocument (app, src);
    doc->setCaption (title);
    QString pos = getAttribute ("clipBegin");
    if (!pos.isEmpty ())
        doc->clip_begin = parseTimeString (pos);
    app->openDocument (doc, doc);
}

KDE_NO_CDTOR_EXPORT
Group::Group (KMPlayer::NodePtr & doc, Control * a, const QString & pn)
  : Element (doc, KMPlayer::id_node_recent_more), app (a) {
    title = pn;
    if (!pn.isEmpty ())
        setAttribute (KMPlayer::Ids::attr_title, pn);
}

KDE_NO_EXPORT KMPlayer::Node *Group::childFromTag (const QString & tag) {
    if (tag == "item")
        return new Recent (m_doc, app);
    else if (tag == "group")
        return new Group (m_doc, app);
    else if (tag == "object")
        return new HtmlObject (m_doc, app, false);
    return NULL;
}

KDE_NO_EXPORT void Group::closed () {
    if (title.isEmpty ())
        title = getAttribute (KMPlayer::Ids::attr_title);
    Element::closed ();
}

void *Group::role (KMPlayer::RoleType msg, void *content)
{
    if (KMPlayer::RolePlaylist == msg)
        return (KMPlayer::PlaylistRole *) this ;
    return Element::role (msg, content);
}

// gconftool -s -t string /apps/maemo/kmplayer/playlist_file /home/koos/playlist.xml

static void deleteAuxiliaries (Node *p)
{
    for (Node *c = p->firstChild (); c;) {
        Node *n = c->nextSibling ();
        if (c->auxiliaryNode ())
            p->removeChild (c);
        //else
        //    deleteAuxiliaries (c);
        c = n;
    }
}
void Playlist::show (View *view)
{
    if (!view->show)
        view->show = evaluateExpr (view->getAttribute ("show"));
    if (!open)
        deleteAuxiliaries (this);
    if (view->show) {
        TrieString aspect_ratio ("aspectRatio");
        TrieString player ("player");
        Node *before = NULL;
        if (!view->before)
            view->before = evaluateExpr (view->getAttribute ("before"));
        if (view->before) {
            view->before->setRoot (this);
            Sequence *ls = view->before->toSequence ();
            if (ls->first())
                before = ls->first()->data.node;
        }
        if (!view->before)
            before = meta->nextSibling ();

        typedef std::map <QString, PlaylistGroup *> GroupMap;
        GroupMap *groups = NULL;
        Expression *expr = evaluateExpr (view->getAttribute ("groupBy"));
        if (expr)
            groups = new GroupMap;

        view->show->setRoot (this);
        Sequence *lst = view->show->toSequence ();
        for (NodeValueItem *itm = lst->first(); itm; itm = itm->nextSibling())
            if (itm->data.node->isElementNode ()) {
                Element *e = static_cast <Element *> (itm->data.node);

                PlaylistItem *pli = new PlaylistItem (m_doc, control, false);
                NodePtr guard = pli;
                AttributeList attrs;
                for (Attribute *a = e->attributes ().first ();
                        a; a = a->nextSibling ())
                    if (a->name () == Ids::attr_url ||
                            a->name () == Ids::attr_title ||
                            a->name () == Ids::attr_panzoom ||
                            a->name () == player ||
                            a->name () == aspect_ratio)
                        attrs.append (new Attribute (TrieString (),
                                    a->name (), a->value ()));
                pli->setAttributes (attrs);
                pli->setAuxiliaryNode (true);
                pli->closed ();

                QString groupBy;
                if (expr) {
                    expr->setRoot (itm->data.node);
                    groupBy = expr->toString ().trimmed ();
                }
                if (groups && !groupBy.isEmpty ()) {
                    GroupMap::iterator it = groups->find (groupBy);
                    PlaylistGroup *group;
                    if (it == groups->end ()) {
                        PlaylistGroup *g = new PlaylistGroup(m_doc, control, false);
                        g->setAttribute (Ids::attr_title, groupBy);
                        g->setAuxiliaryNode (true);
                        g->closed ();
                        group = g;
                        it = groups->insert (std::make_pair (groupBy, g)).first;
                        Node *b = before;
                        if (++it != groups->end ())
                            b = it->second;
                        insertBefore (g, b);
                    } else {
                        group = it->second;
                    }
                    group->appendChild (pli);
                } else {
                    insertBefore (pli, before);
                }
            }
        delete groups;
        delete lst;
        delete expr;
        debugLog () << "Playlist;;show open " << open << endl;
        if (!open)
            SourceDocument::message (MsgListUpdated, NULL);
    }
}

void Playlist::closed ()
{
    PlaylistMetaGroup *m = static_cast <PlaylistMetaGroup *> (meta.ptr ());
    if (m) {
        Views *views = static_cast <Views *> (m->views.ptr ());
        if (views && views->active_view)
            show (static_cast <View *> (views->active_view.ptr ()));
    }
    FileDocument::closed();
}

KDE_NO_EXPORT void Playlist::defer () {
    if (playmode)
        KMPlayer::Document::defer ();
    else if (!resolved) {
        resolved = true;
        readFromFile (path);
    }
}

KDE_NO_EXPORT void Playlist::activate () {
    if (playmode)
        KMPlayer::Document::activate ();
    else if (!resolved)
        defer ();
}

KDE_NO_CDTOR_EXPORT Playlist::Playlist (Control *c, bool plmode)
    : FileDocument (KMPlayer::id_node_playlist_document, "Playlist://", c),
      control (c),
      playmode (plmode) {
    if (!plmode)
        title = "Favorites";
}

KDE_NO_EXPORT KMPlayer::Node *Playlist::childFromTag (const QString & tag) {
    // kdDebug () << nodeName () << " childFromTag " << tag << endl;
    QByteArray ba = tag.toUtf8 ();
    const char *name = ba.constData ();
    if (!strcmp (name, "item"))
        return new PlaylistItem (m_doc, control, playmode);
    else if (!strcmp (name, "group"))
        return new PlaylistGroup (m_doc, control, playmode);
    else if (!strcmp (name, "meta"))
        return new PlaylistMetaGroup (m_doc, control);
    else if (!strcmp (name, "object"))
        return new HtmlObject (m_doc, control, playmode);
    return FileDocument::childFromTag (tag);
}

KDE_NO_EXPORT void Playlist::message (KMPlayer::MessageType msg, void *data) {
    if (KMPlayer::MsgChildFinished == msg && !playmode)
        finish ();
    else
        FileDocument::message (msg, data);
}

KDE_NO_CDTOR_EXPORT
PlaylistItemBase::PlaylistItemBase (KMPlayer::NodePtr &d, short i, Control *a, bool pm)
  : KMPlayer::Mrl (d, i), app (a), playmode (pm), hidden (false) {
}

KDE_NO_EXPORT void PlaylistItemBase::activate () {
    if (playmode) {
        Mrl::activate ();
    } else {
        Playlist *pl = new Playlist (app, true);
        KMPlayer::NodePtr n = pl;
        QString data;
        QString pn;
        if (parentNode ()->id == KMPlayer::id_node_group_node &&
                (previousSibling() || nextSibling ())) {
            data = QString ("<playlist>") +
                parentNode ()->innerXML () +
                QString ("</playlist>");
            pn = ((PlaylistRole *)parentNode ()->role (RolePlaylist))->caption ();
        } else {
            data = outerXML ();
            pn = title.isEmpty () ? src : title;
        }
        pl->setCaption (pn);
        //debugLog () << "cloning to " << data << endl;
        pl->insertBefore (data, NULL);
        Node *cur = pl->firstChild ();
        pl->mrl ()->resolved = !!cur;
        if (parentNode ()->id == KMPlayer::id_node_group_node && cur) {
            KMPlayer::Node *sister = parentNode ()->firstChild ();
            while (sister && cur && sister != this) {
                sister = sister->nextSibling ();
                cur = cur->nextSibling ();
            }
        }
        app->openDocument (pl, cur);
    }
}

KDE_NO_EXPORT void PlaylistItemBase::closed () {
    if (title.isEmpty ())
        title = getAttribute (KMPlayer::Ids::attr_title);
    KMPlayer::Mrl::closed ();
}

void *PlaylistItemBase::role (KMPlayer::RoleType msg, void *content)
{
    if (hidden) {
        if (KMPlayer::RolePlaylist == msg)
            return NULL;
        else if (KMPlayer::RoleCaption == msg)
            return (KMPlayer::PlaylistRole *) this;
    }
    return Mrl::role (msg, content);
}
KDE_NO_CDTOR_EXPORT
PlaylistItem::PlaylistItem (KMPlayer::NodePtr & doc, Control * a, bool pm, const QString &url)
  : PlaylistItemBase (doc, KMPlayer::id_node_playlist_item, a, pm) {
    src = url;
    setAttribute (KMPlayer::Ids::attr_url, url);
}

KDE_NO_EXPORT void PlaylistItem::closed () {
    if (src.isEmpty ())
        src = getAttribute (KMPlayer::Ids::attr_url);
    PlaylistItemBase::closed ();
}

KDE_NO_EXPORT void PlaylistItem::begin () {
    if (playmode && firstChild () && id_node_ref_url != firstChild ()->id)
        firstChild ()->activate ();
    else
        Mrl::begin ();
}

void PlaylistItem::setNodeName (const QString & s) {
    title = s;
    setAttribute (KMPlayer::Ids::attr_title, s);
}

const char *PlaylistItem:: nodeName () const {
    return "item";
}

PlaylistGroupBase::PlaylistGroupBase (NodePtr & doc, short _id, Control *a, bool lm)
 : Element (doc, _id),
   app (a),
   playmode (lm) {
}

KMPlayer::Node *PlaylistGroupBase::childFromTag (const QString & tag) {
    QByteArray ba = tag.toUtf8 ();
    const char *name = ba.constData ();
    if (!strcmp (name, "item"))
        return new PlaylistItem (m_doc, app, playmode);
    else if (!strcmp (name, "group"))
        return new PlaylistGroup (m_doc, app, playmode);
    else if (!strcmp (name, "object"))
        return new HtmlObject (m_doc, app, playmode);
    return NULL;
}

KDE_NO_EXPORT void PlaylistGroupBase::closed () {
    if (title.isEmpty ())
        title = getAttribute (KMPlayer::Ids::attr_title);
    if (title.isEmpty ())
        title = "Group";
    Element::closed ();
}

void *PlaylistGroupBase::role (KMPlayer::RoleType msg, void *content)
{
    if (KMPlayer::RolePlaylist == msg)
        return (KMPlayer::PlaylistRole *) this ;
    return Element::role (msg, content);
}

void PlaylistGroup::setNodeName (const QString & s) {
    title = s;
    setAttribute (KMPlayer::Ids::attr_title, s);
}

const char *PlaylistGroup::nodeName () const
{
    return "group";
}

PlaylistMetaGroup::PlaylistMetaGroup (NodePtr &doc, Control *a)
 : Mrl (doc, KMPlayer::id_node_meta_group), app (a)
{}

void PlaylistMetaGroup::closed ()
{
    for (Node *c = firstChild (); c; c = c->nextSibling ())
        if (id_node_meta_views == c->id) {
            views = c;
            Node *p = parentNode ();
            if (p && id_node_playlist_document == p->id)
                static_cast <Playlist *> (p)->meta = this;
            break;
        }
    title = getAttribute (KMPlayer::Ids::attr_title);
    Mrl::closed ();
}

void PlaylistMetaGroup::activate ()
{
    document ()->message (MsgShowMeta, (Node *) this);
}

Node *PlaylistMetaGroup::childFromTag (const QString &tag)
{
    if (tag == "views") {
        return new Views (m_doc);
    } else if (tag == "item") {
        PlaylistItem *item = new PlaylistItem (m_doc, app, false);
        item->hidden = true;
        return item;
    }
    return NULL;
}

Views::Views (KMPlayer::NodePtr &doc) : Element (doc, id_node_meta_views)
{}

void Views::closed ()
{
    int view = 0;
    PlaylistRole *title = (PlaylistRole *) document ()->role (RolePlaylist);
    if (title)
        view = Config ().readNumEntry (
                QString (GCONF_KEY_LISTS_VIEW) + QChar ('/') + title->caption (),
                0);
    for (Node *c = firstChild (); c; c = c->nextSibling ())
        if (id_node_meta_view == c->id) {
            active_view = c;
            if (--view < 0)
                break;
        }
    Element::closed ();
}

KMPlayer::Node *Views::childFromTag (const QString &tag)
{
    if (tag == "view")
        return new View (m_doc);
    return NULL;
}

void Views::message (MessageType msg, void *content)
{
    switch (msg) {

    case MsgChildFinished: {
        Posting *post = (Posting *) content;
        if (post->source && id_node_meta_view == post->source->id)
            active_view = post->source;
        state = state_deactivated;
        break; // not forwarding to Element, preventing playlist traversal
    }
    default:
        Element::message (msg, content);
    }
}

View::View (NodePtr &doc)
 : Mrl (doc, id_node_meta_view), show (NULL), before (NULL)
{}

View::~View ()
{
    delete show;
    delete before;
}

void View::closed ()
{
    title = getAttribute (KMPlayer::Ids::attr_title);
    groupBy = getAttribute ("groupBy");
    if (title.isEmpty ()) {
        title = groupBy;
        if (title.isEmpty ())
            title = QString ("View ") + getAttribute ("show");
    }
    Element::closed ();
}

void View::activate ()
{
    debugLog () << "View::activate " << m_doc->id << endl;
    if (m_doc->id == id_node_playlist_document)
        static_cast <Playlist *> (m_doc.ptr ())->show (this);

    PlaylistRole *title = (PlaylistRole *) document ()->role (RolePlaylist);
    if (title) {
        int sibling = 0;
        for (Node *c = previousSibling(); c; c = c->previousSibling ())
            if (id_node_meta_view == c->id)
                sibling++;
        Config ().writeEntry (
                QString (GCONF_KEY_LISTS_VIEW) + QChar ('/') + title->caption (),
                sibling);
    }
    finish ();
}

void *View::role (KMPlayer::RoleType msg, void *content)
{
    if (KMPlayer::RolePlaylist == msg)
        return NULL;
    else if (KMPlayer::RoleCaption == msg)
        return (KMPlayer::PlaylistRole *) this;
    return Element::role (msg, content);
}

KDE_NO_CDTOR_EXPORT
HtmlObject::HtmlObject (KMPlayer::NodePtr &doc, Control *a, bool pm)
  : PlaylistItemBase (doc, KMPlayer::id_node_html_object, a, pm) {}

KDE_NO_EXPORT void HtmlObject::activate () {
    if (playmode)
        KMPlayer::Mrl::activate ();
    else
        PlaylistItemBase::activate ();
}

KDE_NO_EXPORT void HtmlObject::closed () {
    for (Node *n = firstChild (); n; n = n->nextSibling ()) {
        if (n->id == KMPlayer::id_node_param) {
            KMPlayer::Element *e = static_cast <KMPlayer::Element *> (n);
            QString name = e->getAttribute (KMPlayer::Ids::attr_name);
            if (name == "type")
                mimetype = e->getAttribute (KMPlayer::Ids::attr_value);
            else if (name == "movie")
                src = e->getAttribute (KMPlayer::Ids::attr_value);
        } else if (n->id == KMPlayer::id_node_html_embed) {
            KMPlayer::Element *e = static_cast <KMPlayer::Element *> (n);
            QString type = e->getAttribute (KMPlayer::Ids::attr_type);
            if (!type.isEmpty ())
                mimetype = type;
            QString asrc = e->getAttribute (KMPlayer::Ids::attr_src);
            if (!asrc.isEmpty ())
                src = asrc;
        }
    }
    PlaylistItemBase::closed ();
}

KDE_NO_EXPORT KMPlayer::Node *HtmlObject::childFromTag (const QString & tag) {
    QByteArray ba = tag.toUtf8 ();
    const char *name = ba.constData ();
    if (!strcasecmp (name, "param"))
        return new DarkNode (m_doc, name, KMPlayer::id_node_param);
    else if (!strcasecmp (name, "embed"))
        return new DarkNode (m_doc, name, id_node_html_embed);
    return NULL;
}

Node *Generator::childFromTag (const QString &tag) {
    QByteArray ba = tag.toUtf8();
    if (!strcmp (ba.constData (), "generator"))
        return new GeneratorElement (m_doc, ba, id_node_gen_generator);
    return NULL;
}

QString Generator::genReadAsk (Control *ctr, Node *n) {
    QString title;
    QString desc;
    QString type = static_cast<Element*>(n)->getAttribute(Ids::attr_type);
    QString key = static_cast<Element*>(n)->getAttribute ("key");
    QString def = static_cast<Element*>(n)->getAttribute ("default");
    QString input;
    if (!key.isEmpty ())
        def = m_control->m_config->readEntry (
                QString (GCONF_KEY_KEYS) + key, def);
    if (type == "file") {
        //input = app_file_chooser (m_control->m_app, def,
                //GTK_FILE_CHOOSER_ACTION_OPEN);
    } else if (type == "dir") {
        //input = app_file_chooser (m_control->m_app, def,
                //GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
        if (!input.isEmpty ())
            input += QChar ('/');
    } else {
        for (Node *c = n->firstChild (); c; c = c->nextSibling ())
            switch (c->id) {
                case id_node_gen_title:
                    title = c->innerText ().simplified ();
                    break;
                case id_node_gen_description:
                    desc = c->innerText ().simplified ();
                    break;
            }
        input = getUserInput (ctr, title, desc, def);
    }
    if (input.isNull ())
        canceled = true;
    else if (!key.isEmpty ())
        m_control->m_config->writeEntry (
                QString (GCONF_KEY_KEYS) + key, input);
    return input;
}

QString Generator::genReadUriGet (Control *ctr, Node *n) {
    QString str;
    bool first = true;
    for (Node *c = n->firstChild (); c && !canceled; c = c->nextSibling ()) {
        QString key;
        QString val;
        switch (c->id) {
        case id_node_gen_http_key_value: {
            Node *q = c->firstChild ();
            if (q) {
                key = genReadString (ctr, q);
                q = q->nextSibling ();
                if (q && !canceled)
                    val = genReadString (ctr, q);
            }
            break;
        }
        default:
            key = genReadString (ctr, c);
            break;
        }
        if (!key.isEmpty ()) {
            if (first) {
                str += QChar ('?');
                first = false;
            } else {
                str += QChar ('&');
            }
            str += QUrl::toPercentEncoding (key);
            if (!val.isEmpty ())
                str += QChar ('=') + QString (QUrl::toPercentEncoding (val));
        }
    }
    return str;
}

QString Generator::genReadString (Control *ctr, Node *n) {
    QString str;
    bool need_quote = quote;
    quote = false;
    for (Node *c = n->firstChild (); c && !canceled; c = c->nextSibling ())
        switch (c->id) {
        case id_node_gen_uri:
        case id_node_gen_sequence:
            str += genReadString (ctr, c);
            break;
        case id_node_gen_literal:
            str += c->innerText ().simplified ();
            break;
        case id_node_gen_predefined: {
            QString val = static_cast <Element *>(c)->getAttribute ("key");
            if (val == "data")
                str += ctr->data_dir;
            else if (val == "sysdata")
                str += "/usr/share/applications/kmplayer";
            break;
        }
        case id_node_gen_http_get:
            str += genReadUriGet (ctr, c);
            break;
        case id_node_gen_ask:
            str += genReadAsk (ctr, c);
            break;
        case id_node_text:
             str += c->nodeValue ().simplified ();
        }
    if (!static_cast <Element *>(n)->getAttribute ("encoding").isEmpty ())
        str = QUrl::toPercentEncoding (str);
    if (need_quote) {
        char *quoted = g_shell_quote (str.toLocal8Bit());
        str = quoted;
        g_free (quoted);
        quote = true;
    }
    return str;
}

QString Generator::genReadInput (Control *ctrl, Node *n) {
    quote = false;
    return genReadString (ctrl, n);
}

QString Generator::genReadProcess (Control *ctrl, Node *n) {
    QString process;
    QString str;
    quote = true;
    for (Node *c = n->firstChild (); c && !canceled; c = c->nextSibling ())
        switch (c->id) {
        case id_node_gen_program:
            process = QString (genReadString (ctrl, c));
            break;
        case id_node_gen_argument:
            process += QChar (' ') + genReadString (ctrl, c);
            break;
        }
    return process;
}

void Generator::activate () {
    QString input;
    canceled = false;
    Node *n = firstChild ();
    if (n && n->id == id_node_gen_generator) {
        title = static_cast<Element *>(n)->getAttribute (Ids::attr_name);
        for (Node *c = n->firstChild (); c && !canceled; c = c->nextSibling ())
            switch (c->id) {
            case id_node_gen_input:
                input = genReadInput (m_control, c);
                break;
            case id_node_gen_process:
                process = genReadProcess (m_control, c);
            }
    }
    if (canceled)
        return;
    if (!input.isEmpty () && process.isEmpty ()) {
        message (MsgInfoString, &input);
        m_control->openFile (input);
    } else if (!process.isEmpty ()) {
        data = new QTextStream (&data_buffer, QIODevice::ReadWrite);
        if (input.isEmpty ()) {
            message (MsgInfoString, &process);
            begin ();
        } else {
            QString cmdline (input + " | " + process);
            message (MsgInfoString, &cmdline);
            state = state_activated;
            input_job = asyncGet (this, input);
            input_job->start ();
        }
    }
}

void Generator::begin () {
    if (!unix_process)
        unix_process = new UnixProcess (
                this, UnixProcess::StdIn | UnixProcess::StdOut);
    QString info;
    info += process;
    message (MsgInfoString, &info);
    if (unix_process->start (m_control->data_dir, process)) {
        state = state_began;
        if (buffer)
            unix_process->pollStdIn (true);
    } else {
        QString err ("Couldn't start process");
        message (MsgInfoString, &err);
        deactivate ();
    }
}

void Generator::deactivate () {
    if (unix_process) {
        unix_process->stop ();
        delete unix_process;
        unix_process = NULL;
    }
    if (input_job) {
        input_job->kill ();
        input_job = NULL;
    }
    delete data;
    data = NULL;
    data_buffer.clear ();
    delete buffer;
    buffer = NULL;
    FileDocument::deactivate ();
}

void Generator::readEvent (UnixProcess *) {
    char buf[1024];
    int nr;
    State cur_state = state;
    state = state_deferred;
    do {
        nr = unix_process->readStdOut (buf, sizeof (buf) - 1);
        buf[nr] = 0;
        if (nr)
            *data << buf;
    } while (nr > 0);
    if (unix_process->eof) {
        unix_process->closeStdOut ();
        if (!data_buffer.isEmpty ()) {
            Playlist *pl = new Playlist (m_control, true);
            NodePtr n = pl;
            data->seek (0);
            readXML (pl, *data, QString (), false);
            pl->title = title;
            message (MsgInfoString, NULL);
            m_control->openDocument (n, NULL);
        } else {
            QString err ("No data received");
            message (MsgInfoString, &err);
        }
        deactivate ();
    } else {
        state = cur_state;
    }
}

void Generator::writeEvent (UnixProcess *) {
    if (buffer) {
        const gchar *start = buffer->data ();
        const gchar *end;
        int wmax = buffer->size ();
        int wcount = wmax;
        if (!g_utf8_validate (start, wmax, &end))
            wcount = end - start;
        int nr = unix_process->writeStdIn (QByteArray (start, wcount));
        if (nr == wcount && wcount < wmax)
            nr++; //skip
        if (nr < buffer->size ()) {
            if (nr) {
                int new_size = buffer->size () - nr;
                memmove (buffer->data(), buffer->data() + nr, new_size);
                buffer->resize (new_size);
            }
            return;
        }
        buffer->resize (0);
        if (input_job) {
            unix_process->pollStdIn (false);
            return;
        }
    }
    unix_process->closeStdIn ();
    message (MsgInfoString, &process);
}

void Generator::processExited (UnixProcess *) {
    debugLog() << "Generator::processExited " << endl;
    if (active () && state_deferred != state)
        readEvent (NULL);
}

void Generator::jobData (IOJob *job, QByteArray &data) {
    if (job->error ()) {
        job->kill (false);
    } else if (data.size ()) {
        if (!buffer) {
            buffer = new QByteArray (data);
            begin ();
        } else {
            int p = buffer->size ();
            buffer->resize (p + data.size ());
            memcpy (buffer->data () + p, data.data (), data.size ());
            unix_process->pollStdIn (true);
        }
    }
}

void Generator::jobResult (IOJob *) {
    input_job = NULL;
    if (!buffer) {
        QString err ("No input data received");
        message (MsgInfoString, &err);
        deactivate ();
    } else if (unix_process && !buffer->size ()) {
        unix_process->closeStdIn ();
    }
}

struct GeneratorTag {
    const char *tag;
    short id;
} gen_tags[] = {
    { "input", id_node_gen_input },
    { "process", id_node_gen_process },
    { "uri", id_node_gen_uri },
    { "literal", id_node_gen_literal },
    { "ask", id_node_gen_ask },
    { "title", id_node_gen_title },
    { "description", id_node_gen_description },
    { "process", id_node_gen_process },
    { "program", id_node_gen_program },
    { "argument", id_node_gen_argument },
    { "predefined", id_node_gen_predefined },
    { "http-get", id_node_gen_http_get },
    { "key-value", id_node_gen_http_key_value },
    { "key", id_node_gen_sequence },
    { "value", id_node_gen_sequence },
    { "sequence", id_node_gen_sequence },
    { NULL, -1 }
};

Node *GeneratorElement::childFromTag (const QString &tag) {
    QByteArray ba = tag.toUtf8 ();
    const char *ctag = ba.constData ();
    for ( GeneratorTag *t = gen_tags; t->tag; ++t)
        if (!strcmp (ctag, t->tag))
            return new GeneratorElement (m_doc, ba, t->id);
    return NULL;
}

