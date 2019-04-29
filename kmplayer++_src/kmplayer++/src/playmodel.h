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

#ifndef KMPLAYER_PLAYMODEL_H
#define KMPLAYER_PLAYMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>

#include "playlist.h"

struct TreeUpdate;

namespace KMPlayer {

class PlayModel;
class TopPlayItem;

/*
 * An item in the playlist
 */
class KMPLAYER_NO_EXPORT PlayItem
{
public:
    PlayItem (Node *e, PlayItem *parent)
        : item_flags (Qt::ItemIsEnabled | Qt::ItemIsSelectable),
          node (e), parent_item (parent),
          open (false), attribute_list (false)
    {}
    PlayItem (Attribute *a, PlayItem *pa)
        : item_flags (Qt::ItemIsEnabled | Qt::ItemIsSelectable),
          attribute (a), parent_item (pa),
          open (false), attribute_list (false)
    {}
    virtual ~PlayItem () { deleteChildren (); }

    void deleteChildren () { qDeleteAll (child_items); child_items.clear (); }
    void appendChild (PlayItem *child) { child_items.append (child); }
    PlayItem *child (unsigned i) {
        return i < (unsigned) child_items.size() ? child_items.at (i) : NULL;
    }
    int childCount () const { return child_items.count(); }
    int row () const {
        return parent_item
            ? parent_item->child_items.indexOf (const_cast <PlayItem*> (this))
            : -1;
    }
    PlayItem *parent () { return parent_item; }
    TopPlayItem *rootItem ();

    Qt::ItemFlags item_flags;

    NodePtrW node;
    AttributePtrW attribute;

    QList<PlayItem*> child_items;
    PlayItem *parent_item;
    bool open;
    bool attribute_list;
};

class KMPLAYER_NO_EXPORT TopPlayItem : public PlayItem
{
public:
    TopPlayItem (PlayModel *m, int _id, Node *e, int flags)
      : PlayItem (e, NULL),
        model (m),
        updated_document (NULL),
        id (_id),
        root_flags (flags),
        show_all_nodes (false),
        have_dark_nodes (false)
    {}
    Qt::ItemFlags itemFlags ();
    void add ();
    void remove ();
    QString icon;
    QString source;
    PlayModel *model;
    Document *updated_document;
    unsigned int updated_document_version;
    int id;
    int root_flags;
    bool show_all_nodes;
    bool have_dark_nodes;
};

class KMPLAYER_NO_EXPORT PlayModel : public QAbstractItemModel
{
    friend class TopPlayItem;

    Q_OBJECT

public:
    enum { UrlRole = Qt::UserRole + 1, DepthRole };

    enum Flags {
        AllowDrops = 0x01, AllowDrag = 0x02,
        InPlaceEdit = 0x04, TreeEdit = 0x08,
        Moveable = 0x10, Deletable = 0x20
    };

    PlayModel (QObject *parent);
    ~PlayModel ();

    QVariant data (const QModelIndex &index, int role) const;
    bool setData (const QModelIndex&, const QVariant& v, int role);
    Qt::ItemFlags flags (const QModelIndex &index) const;
    QVariant headerData (int section, Qt::Orientation orientation,
            int role = Qt::DisplayRole) const;
    QModelIndex index (int row, int column,
            const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent (const QModelIndex &index) const;
    bool hasChildren (const QModelIndex& parent = QModelIndex ()) const;
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount (const QModelIndex &parent = QModelIndex()) const;

    PlayItem *rootItem () const { return root_item; }
    TopPlayItem *topItem (int id) const;
    QModelIndex indexFromItem (PlayItem *item) const;
    PlayItem *itemFromIndex (const QModelIndex& index) const;

    int addTree (NodePtr r, const QString &src, const QString &ico, int flgs);
    void toggleOpen (PlayItem *item);
    PlayItem *updateTree (TopPlayItem *ritem, NodePtr active);
    PlayItem *itemFromRow (int row) const;
    int rowFromItem (PlayItem *item) const;
    static PlayItem *itemFromNode (TopPlayItem *ritem, Node *node);
    void emitDataChanged (const QModelIndex& i) { emit dataChanged (i, i); }

signals:
    void selectionChanged (int index, int scroll);

public slots:
    void updateTree (int id, NodePtr root, NodePtr active);

private slots:
    void updateTrees ();

private:
    PlayItem *populate (Node *e, Node *focus,
            TopPlayItem *root, PlayItem *item,
            PlayItem **curitem);

    SharedPtr <TreeUpdate> tree_update;
    QString auxiliary_pix;
    QString config_pix;
    QString folder_pix;
    QString music_pix;
    QString info_pix;
    QString menu_pix;
    QString unknown_pix;
    QString url_pix;
    QString video_pix;
    QString video_link_pix;
    QString auto_remove_pix;
    PlayItem *root_item;
    int last_id;
};

}

#endif

