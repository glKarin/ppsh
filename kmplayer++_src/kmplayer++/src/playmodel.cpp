/**
 * Copyright (C) 2011 by Koos Vriezen <koos.vriezen@gmail.com>
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

#include "playmodel.h"
#include "lists.h"

#include <qpixmap.h>
#include <qtimer.h>

using namespace KMPlayer;

TopPlayItem *PlayItem::rootItem ()
{
    PlayItem *r = NULL;
    for (PlayItem *p = this; p->parent_item; p = p->parent_item)
        r = p;
    return static_cast <TopPlayItem *> (r);
}

Qt::ItemFlags TopPlayItem::itemFlags ()
{
    Qt::ItemFlags itemflags = Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    if (root_flags & PlayModel::AllowDrag)
        itemflags |= Qt::ItemIsDragEnabled;
    if (root_flags & PlayModel::InPlaceEdit)
        itemflags |= Qt::ItemIsEditable;
    return itemflags;
}

//-----------------------------------------------------------------------------

struct KMPLAYER_NO_EXPORT TreeUpdate {
    KDE_NO_CDTOR_EXPORT TreeUpdate (TopPlayItem *ri, NodePtr n, SharedPtr <TreeUpdate> &nx) : root_item (ri), node (n), next (nx) {}
    KDE_NO_CDTOR_EXPORT ~TreeUpdate () {}
    TopPlayItem * root_item;
    NodePtrW node;
    SharedPtr <TreeUpdate> next;
};

PlayModel::PlayModel (QObject *parent)
  : QAbstractItemModel (parent),
    auxiliary_pix ("image://theme/icon-m-toolbar-directory-white"),
    config_pix ("/usr/share/icons/hicolor/64x64/apps/configure"),
    folder_pix ("image://theme/icon-m-toolbar-directory-white"),
    music_pix ("image://theme/icon-m-toolbar-content-audio-white-selected"),
    info_pix ("image://theme/icon-m-content-attachment-inverse"),
    menu_pix ("image://theme/icon-m-toolbar-directory-selected"),
    unknown_pix ("image://theme/icon-m-content-note-inverse"),
    url_pix ("image://theme/icon-l-browser-main-view"),
    video_pix ("image://theme/icon-m-camera-video"),
    video_link_pix ("image://theme/icon-m-toolbar-favorite-mark-selected"),
    auto_remove_pix ("image://theme/icon-m-toolbar-favorite-unmark-selected"),
    root_item (new PlayItem ((Node *)NULL, NULL)),
    last_id (0)
{
    TopPlayItem *ritem = new TopPlayItem (this,
            0, NULL, PlayModel::TreeEdit);
    ritem->parent_item = root_item;
    root_item->child_items.append (ritem);
    ritem->icon = url_pix;

    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "title";
    roles[UrlRole] = "url";
    roles[DepthRole] = "depth";
    roles[Qt::DecorationRole] = "icon";
    setRoleNames(roles);
}

PlayModel::~PlayModel ()
{
    delete root_item;
}

QVariant PlayModel::data (const QModelIndex &index, int role) const
{
    if (!index.isValid ())
        return QVariant ();

    PlayItem *item = static_cast<PlayItem*> (index.internalPointer ());
    switch (role) {
    case Qt::DisplayRole:
        if (item->attribute_list) {
            return "[attributes]";
        } else if (item->attribute) {
            return QString ("%1=%2")
                .arg (item->attribute->name ().toString ())
                .arg (item->attribute->value ());
        } else if (item->node) {
            PlaylistRole *title = (PlaylistRole *) item->node->role (RolePlaylist);
            QString text (title ? title->caption () : "");
            if (text.isEmpty ()) {
                text = id_node_text == item->node->id
                    ? item->node->nodeValue () : item->node->nodeName ();
                if (item->node->isDocument ())
                    text = item->node->hasChildNodes () ? "unnamed" : "none";
            }
            return text;
        }
        return QString ("<span style=\"color:gray;font-style:italic;\">&lt;%1&gt;</span>").arg (tr ("long tap for options"));

    case Qt::DecorationRole:
        if (item->parent () == root_item)
            return static_cast <TopPlayItem *> (item)->icon;
        if (item->attribute)
            return config_pix;
        if (item->childCount() > 0)
            if (item->child (0)->attribute)
                return menu_pix;
        if (item->node) {
            Node::PlayType pt = item->node->playType ();
            switch (pt) {
            case Node::play_type_info:
                return info_pix;
            default:
                 if (item->node->isPlayable())
                     return
                         item->node->mrl ()->resolved
                         ? item->node->mrl ()->audio_only
                           ? music_pix
                           : video_pix
                         : id_node_recent_node == item->node->id
                             && item->node->mrl()->getAttribute (Ids::attr_autoremove) == "true"
                           ? auto_remove_pix
                           : video_link_pix;
                if (pt > Node::play_type_none)
                    return video_pix;
                else
                    return item->childCount ()
                        ? item->node->auxiliaryNode ()
                          ? auxiliary_pix : folder_pix
                          : unknown_pix;
            }
        }
        return unknown_pix;

    case UrlRole:
        if (item->node) {
            Mrl *mrl = item->node->mrl ();
            if (mrl && !mrl->src.isEmpty ())
                return mrl->src;
        }
        return QVariant ();

    case DepthRole:
        if (true) {
            int depth = 0;
            while (item != root_item) {
                item = item->parent ();
                depth++;
            }
            return depth;
        }
        return QVariant ();

    case Qt::EditRole:
        if (item->item_flags & Qt::ItemIsEditable)
            return data (index, Qt::DisplayRole);

    default:
        break;
    }
    return QVariant ();
}

bool PlayModel::setData (const QModelIndex& i, const QVariant& v, int role)
{
    if (role != Qt::EditRole || !i.isValid ())
        return false;

    bool changed = false;
    PlayItem *item = static_cast <PlayItem *> (i.internalPointer ());
    QString ntext = v.toString ();

    TopPlayItem *ri = item->rootItem ();
    if (ri->show_all_nodes && item->attribute) {
        int pos = ntext.indexOf (QChar ('='));
        if (pos > -1) {
            item->attribute->setName (ntext.left (pos));
            item->attribute->setValue (ntext.mid (pos + 1));
        } else {
            item->attribute->setName (ntext);
            item->attribute->setValue (QString (""));
        }
        PlayItem *pi = item->parent ();
        if (pi && pi->node) {
            pi->node->document ()->m_tree_version++;
            pi->node->closed ();
        }
        changed = true;
    } else if (item->node) {
        PlaylistRole *title = (PlaylistRole *) item->node->role (RolePlaylist);
        if (title && !ri->show_all_nodes) {
            if (ntext.isEmpty ()) {
                ntext = item->node->mrl ()
                    ? item->node->mrl ()->src
                    : title->caption ();
            }
            if (title->caption () != ntext) {
                item->node->setNodeName (ntext);
                changed = true;
            }
            //} else { // restore damage ..
            // cannot update because of crashing, shouldn't get here anyhow
            //updateTree (ri, item->node, true);
        }
    }

    if (changed) {
        //item->title = ntext;
        if (ri->updated_document_version == item->node->document ()->m_tree_version++)
            ++ri->updated_document_version;
        emit dataChanged (i, i);
        return true;
    }
    return false;
}

Qt::ItemFlags PlayModel::flags (const QModelIndex &index) const
{
    if (!index.isValid ())
        return 0;

    return static_cast<PlayItem*>(index.internalPointer())->item_flags;
}

QVariant PlayModel::headerData (int, Qt::Orientation, int) const
{
    return QVariant ();
}

static int rowCountImpl (PlayItem *item)
{
    int rows = 0;
    const QList<PlayItem*>::const_iterator e = item->child_items.constEnd();
    for (QList<PlayItem*>::const_iterator i = item->child_items.constBegin(); i != e; ++i) {
        rows++;
        if ((*i)->open)
            rows += rowCountImpl (*i);
    }
    return rows;
}

static PlayItem *itemFromRowImpl (PlayItem *item, int &row)
{
    const QList<PlayItem*>::const_iterator e = item->child_items.constEnd();
    for (QList<PlayItem*>::const_iterator i = item->child_items.constBegin(); i != e; ++i) {
        if (!row--)
            return *i;
        if ((*i)->open) {
            PlayItem *pi = itemFromRowImpl (*i, row);
            if (pi)
                return pi;
        }
    }
    return NULL;
}

static bool rowFromItemImpl (PlayItem *parent, PlayItem *item, int& row)
{
    const QList<PlayItem*>::const_iterator e = parent->child_items.constEnd();
    for (QList<PlayItem*>::const_iterator i = parent->child_items.constBegin(); i != e; ++i) {
        row++;
        if (item == *i)
            return true;
        if ((*i)->open && rowFromItemImpl (*i, item, row))
            return true;
    }
    return false;
}

PlayItem *PlayModel::itemFromRow (int row) const
{
    return itemFromRowImpl (root_item, row);
}

int PlayModel::rowFromItem (PlayItem *item) const
{
    int row = -1;
    if (rowFromItemImpl (root_item, item, row))
        return row;
    return -1;
}

QModelIndex PlayModel::index (int row, int col, const QModelIndex &parent) const
{
    if (!col && !parent.isValid ()) {
        PlayItem *item = itemFromRow (row);
        if (item)
            return createIndex (row, col, item);
    }
    return QModelIndex ();
}

TopPlayItem *PlayModel::topItem (int id) const
{
    const QList<PlayItem*>::const_iterator e = root_item->child_items.constEnd();
    for (QList<PlayItem*>::const_iterator i = root_item->child_items.constBegin(); i != e; ++i) {
        TopPlayItem *r = static_cast <TopPlayItem *>(*i);
        if (r->id == id)
            return r;
    }
    return NULL;
}

QModelIndex PlayModel::indexFromItem (PlayItem *item) const
{
    if (!item || item == root_item)
        return QModelIndex();

    return createIndex (rowFromItem (item), 0, item);
}

PlayItem *PlayModel::itemFromIndex (const QModelIndex& index) const
{
    if (!index.isValid ())
        return NULL;
    return static_cast <PlayItem*> (index.internalPointer ());
}

QModelIndex PlayModel::parent (const QModelIndex&) const
{
    return QModelIndex();
}

bool PlayModel::hasChildren (const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return false;

    if (!parent.isValid())
        return root_item->childCount();

    return false;
}

int PlayModel::rowCount (const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return rowCountImpl (root_item);
    }
    return 0;
}

int PlayModel::columnCount (const QModelIndex&) const
{
    return 1;
}

void dumpTree( PlayItem *p, const QString &indent ) {
    //qDebug( "%s%s", qPrintable(indent),qPrintable(p->title));
    for (int i=0; i < p->childCount(); i++)
        dumpTree(p->child(i), indent+"  ");
}

void TopPlayItem::add ()
{
    model->beginInsertRows (QModelIndex(), id, id);

    parent_item = model->root_item;
    if (id >= model->root_item->childCount ())
        model->root_item->child_items.append (this);
    else
        model->root_item->child_items.insert (id, this);

    model->endInsertRows();

    if (id !=row())
        qWarning("Invalid root tree");
}

void TopPlayItem ::remove ()
{
    if (id < parent_item->childCount ())
        parent_item->child_items.takeAt (id);
    else
        qWarning( "TopPlayItem::remove");
}

PlayItem *PlayModel::populate (Node *e, Node *focus,
        TopPlayItem *root, PlayItem *pitem,
        PlayItem ** curitem)
{
    root->have_dark_nodes |= !e->role (RolePlaylist);
    if (pitem && !root->show_all_nodes && !e->role (RolePlaylist)) {
        for (Node *c = e->firstChild (); c; c = c->nextSibling ())
            populate (c, focus, root, pitem, curitem);
        return pitem;
    }
    PlayItem *item = root;
    if (pitem) {
        item = new PlayItem (e, pitem);
        pitem->appendChild (item);
    }
    item->item_flags |= root->itemFlags ();
    PlaylistRole *title = (PlaylistRole *) e->role (RolePlaylist);
    if (title && !root->show_all_nodes)
        item->item_flags |= Qt::ItemIsEditable;
    if (focus == e)
        *curitem = item;
    //if (e->active ())
        //scrollToItem (item);
    for (Node *c = e->firstChild (); c; c = c->nextSibling ())
        populate (c, focus, root, item, curitem);
    if (e->isElementNode ()) {
        Attribute *a = static_cast <Element *> (e)->attributes ().first ();
        if (a) {
            root->have_dark_nodes = true;
            if (root->show_all_nodes) {
                PlayItem *as = new PlayItem (e, item);
                as->attribute_list = true;
                item->appendChild (as);
                for (; a; a = a->nextSibling ()) {
                    PlayItem * ai = new PlayItem (a, as);
                    as->appendChild (ai);
                    //pitem->setFlags(root->itemFlags() &=~Qt::ItemIsDragEnabled);
                    if (root->id > 0)
                        ai->item_flags |= Qt::ItemIsEditable;
                }
            }
        }
    }
        //if (root->flags & PlayModel::AllowDrag)
        //    item->setDragEnabled (true);
    return item;
}

int PlayModel::addTree (NodePtr doc, const QString &source, const QString &icon, int flags) {
    TopPlayItem *ritem = new TopPlayItem(this, ++last_id, doc, flags);
    ritem->source = source;
    ritem->icon = icon;
    PlayItem *curitem = 0L;
    populate (doc, 0, ritem, 0L, &curitem);
    ritem->add ();
    return last_id;
}

void PlayModel::toggleOpen (PlayItem *item)
{
    if (item && item->childCount ()) {
        int row = item->row ();
        if (item->open) {
            int start = rowFromItem (item) + 1;
            int end = -1;
            for (PlayItem *p = item->parent (); p; p = p->parent ()) {
                if (row + 1 < p->childCount ()) {
                    end = rowFromItem (p->child_items[row+1]) - 1;
                    break;
                }
                row = p->row ();
            }
            if (-1 == row)
                end = rowCount () - 1;
    debugLog() << "toggleOpen remove " << start << "-" << end << endl;
            beginRemoveRows (QModelIndex (), start, end);
            item->open = false;
            endRemoveRows();
        } else {
            item->open = true;
            int start = rowFromItem (item->child_items[0]);
            int end = -1;
            for (PlayItem *p = item->parent (); p; p = p->parent ()) {
                if (row + 1 < p->childCount ()) {
                    end = rowFromItem (p->child_items[row+1]) - 1;
                    break;
                }
                row = p->row ();
            }
            if (-1 == row)
                end = rowCount () - 1;
    debugLog() << "toggleOpen insert " << start << "-" << end << endl;
            beginInsertRows (QModelIndex (), start, end);
            endInsertRows();
            emit selectionChanged (start-1, end);
        }
    }
}

void PlayModel::updateTree (int id, NodePtr root, NodePtr active) {
    // TODO, if root is same as rootitems->node and treeversion is the same
    // and show all nodes is unchanged then only update the cells
    TopPlayItem *ritem = topItem (id);
    if (ritem) {
        if (!ritem->node && !root)
            return;
        ritem->node = root;
        for (TreeUpdate *tu = tree_update; tu; tu = tu->next)
            if (tu->root_item->id == id) {
                tu->node = active;
                return;
            }
        bool need_timer = !tree_update;
        tree_update = new TreeUpdate (ritem, active, tree_update);
        if (need_timer)
            QTimer::singleShot (0, this, SLOT (updateTrees ()));
    } else {
        errorLog () << "updateTree root item not found\n";
    }
}

KDE_NO_EXPORT void PlayModel::updateTrees () {
    for (; tree_update; tree_update = tree_update->next)
        PlayItem *cur = updateTree (tree_update->root_item, tree_update->node);
}

PlayItem *PlayModel::itemFromNode (TopPlayItem *ritem, Node *node)
{
    if (ritem->node.ptr () == node)
        return ritem;

    if (node) {
        if (!ritem->show_all_nodes && !node->role (RolePlaylist))
            return itemFromNode (ritem, node->parentNode ());

        PlayItem *pitem = itemFromNode (ritem, node->parentNode ());
        if (!pitem)
            return NULL;
        QList<PlayItem*>::const_iterator i = pitem->child_items.constBegin();
        const QList<PlayItem*>::const_iterator e = pitem->child_items.constEnd();
        for (; i != e; ++i)
            if (node == (*i)->node)
                return *i;
    }
    return NULL;
}

static PlayItem *openItem (const PlayItem *root_item, PlayItem *item)
{
    if (item && item != root_item) {
        PlayItem *pi = openItem (root_item, item->parent ());
        if (!pi)
            return item->open ? NULL : item;
        if (!item->open)
            item->open = true;
        return pi;
    }
    return NULL;
}

PlayItem *PlayModel::updateTree (TopPlayItem *ritem, NodePtr active) {
    PlayItem *curitem = 0L;

    if (ritem->node
            && ritem->node->document () == ritem->updated_document
            && ritem->updated_document_version
               == ritem->node->document ()->m_tree_version) {
        if (active) {
            PlayItem *item = itemFromNode (ritem, active);
            if (item) {
                PlayItem *pi = openItem (root_item, item->parent ());
                if (pi)
                    toggleOpen (pi);
                int row = rowFromItem (item);
                emit selectionChanged (row, row);
            } else
                errorLog () << "active not found\n";
        }
    } else {
        int start = rowFromItem (ritem);
        int end;
        if (ritem->id + 1 < root_item->childCount ())
            end = rowFromItem (root_item->child_items[ritem->id+1]) - 1;
        else
            end = rowCount () - 1;
        debugLog() << "updateTree remove " << start << "-" << end << endl;
        beginRemoveRows (QModelIndex (), start, end);
        ritem->remove ();
        endRemoveRows();
        ritem->deleteChildren ();
        if (ritem->node) {
            if (!ritem->show_all_nodes)
                for (NodePtr n = active; n; n = n->parentNode ()) {
                    active = n;
                    if (n->role (RolePlaylist))
                        break;
                }
            populate (ritem->node, active, ritem, 0L, &curitem);
        }
        if (curitem)
            for (PlayItem *p = curitem->parent (); p; p = p->parent ())
                if (p != root_item)
                    p->open = true;

        if (ritem->id >= root_item->childCount ()) {
            root_item->child_items.append (ritem);
            end = rowCount () - 1;
        } else {
            root_item->child_items.insert (ritem->id, ritem);
            end = rowFromItem (root_item->child_items[ritem->id+1]) - 1;
        }
        debugLog() << "updateTree insert " << start << "-" << end << endl;
        beginInsertRows (QModelIndex(), start, end);
        endInsertRows();
        ritem->updated_document = ritem->node->document ();
        ritem->updated_document_version = ritem->node->document ()->m_tree_version;
        if (curitem)
            emit selectionChanged (rowFromItem (curitem), start);
    }
    return curitem;
}

