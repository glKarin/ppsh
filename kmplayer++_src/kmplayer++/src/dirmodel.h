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

#ifndef KMPLAYER_DIRMODEL_H
#define KMPLAYER_DIRMODEL_H

#include "kmplayer_def.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QStringList>

namespace KMPlayer {

class KMPLAYER_NO_EXPORT DirModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString directory READ directory WRITE enterDirectory NOTIFY directoryChanged)
    Q_PROPERTY(bool canGoUp READ canGoUp NOTIFY directoryChanged)

public:
    enum { DirRole = Qt::UserRole + 1 };

    DirModel (QObject *parent);
    ~DirModel ();

    QVariant data (const QModelIndex &index, int role) const;
    Qt::ItemFlags flags (const QModelIndex &index) const;
    QVariant headerData (int section, Qt::Orientation orientation,
            int role = Qt::DisplayRole) const;
    QModelIndex index (int row, int column,
            const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent (const QModelIndex &index) const;
    bool hasChildren (const QModelIndex& parent = QModelIndex ()) const;
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount (const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE QString currentDirectory () const { return m_data.current_directory; }
    void setCurrentDirectory (const QString& dir);
    Q_INVOKABLE void goUp ();
    bool canGoUp ();
    QString directory ();
    void enterDirectory (const QString&);

Q_SIGNALS:
    void directoryChanged ();

private:
    mutable struct Data
    {
        Data (DirModel *dm)
            : current_directory (USER_ROOT_DIRECTORY)
            , dir_model (dm)
            , directories_retrieved (false) {}

        QStringList directories;
        QString current_directory;
        DirModel *dir_model;
        bool directories_retrieved;

        void updateDirectories ();
    } m_data;
};

class KMPLAYER_NO_EXPORT FileModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    FileModel (QObject *parent, DirModel*);
    ~FileModel ();

    QVariant data (const QModelIndex &index, int role) const;
    Qt::ItemFlags flags (const QModelIndex &index) const;
    QVariant headerData (int section, Qt::Orientation orientation,
            int role = Qt::DisplayRole) const;
    QModelIndex index (int row, int column,
            const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent (const QModelIndex &index) const;
    bool hasChildren (const QModelIndex& parent = QModelIndex ()) const;
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount (const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE void updateFiles () const { m_data.updateFiles (); }
private Q_SLOTS:
    void directoryChanged ();

private:
    mutable struct Data
    {
        Data (FileModel* fm, DirModel* dm)
            : file_model (fm)
            , dir_model (dm)
            , files_retrieved (false) {}

        QStringList files;
        FileModel *file_model;
        DirModel *dir_model;
        bool files_retrieved;

        void updateFiles ();
    } m_data;
};

} // namespace

#endif
