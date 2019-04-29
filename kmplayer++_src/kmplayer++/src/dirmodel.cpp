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

#include "dirmodel.h"
#include "log.h"
#include <QDir>

using namespace KMPlayer;

DirModel::DirModel (QObject *parent)
    : QAbstractItemModel (parent)
    , m_data (this)
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "caption";
    roles[DirRole] = "path";
    setRoleNames(roles);
}

DirModel::~DirModel ()
{}

QVariant DirModel::data (const QModelIndex &index, int role) const
{
    m_data.updateDirectories ();
    if (index.column () > 0
            || !index.isValid ()
            || index.row () >= m_data.directories.length ())
        return QVariant ();
    QString entry = m_data.directories[index.row ()];
    switch (role) {
    case Qt::DisplayRole:
        return entry;
    case DirRole:
        return m_data.current_directory + QChar ('/') + entry;
    default:
        break;
    }
    return QVariant ();
}

Qt::ItemFlags DirModel::flags (const QModelIndex &index) const
{
    if (index.isValid ())
        return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    return 0;
}

QVariant DirModel::headerData (int, Qt::Orientation, int) const
{
    return QVariant ();
}

QModelIndex DirModel::index (int row, int col, const QModelIndex& parent) const
{
    if (col > 0 || parent.isValid ())
        return QModelIndex();
    m_data.updateDirectories ();
    if (row >= m_data.directories.length ())
        return QModelIndex();
    return createIndex (row, col, NULL);
}

QModelIndex DirModel::parent (const QModelIndex&) const
{
    return QModelIndex();
}

bool DirModel::hasChildren (const QModelIndex& parent) const
{
    return rowCount (parent);
}

int DirModel::rowCount (const QModelIndex &parent) const
{
    if (parent.column() > 0 || parent.isValid ())
        return 0;
    m_data.updateDirectories ();
    return m_data.directories.length ();
}

int DirModel::columnCount (const QModelIndex&) const
{
    return 1;
}

void DirModel::Data::updateDirectories ()
{
    if (!directories_retrieved) {
        directories = QDir (current_directory).entryList (QDir::AllDirs | QDir::Hidden | QDir::Writable | QDir::NoDot | QDir::NoDotDot);
        debugLog() << "retrieving " << directories.length () << " directories" << endl;
        directories_retrieved = true;
        if (directories.length ()) {
            dir_model->beginInsertRows (QModelIndex(), 0, directories.length ()-1);
            dir_model->endInsertRows();
        }
    }
}

void DirModel::setCurrentDirectory (const QString& dir)
{
    QString path = dir;
    while (path.endsWith (QChar ('/')))
        path.chop (1);
    if ( !QDir (path).exists() )
        path = USER_ROOT_DIRECTORY;
    if (path == m_data.current_directory)
        return;
    m_data.current_directory = path;
    m_data.directories_retrieved = false;
    if (m_data.directories.length ()) {
        beginRemoveRows (QModelIndex (), 0, m_data.directories.length ()-1);
        m_data.directories.clear ();
        endRemoveRows();
    }
    emit directoryChanged ();
}

KMPLAYER_NO_EXPORT void DirModel::goUp ()
{
    if (m_data.current_directory == USER_ROOT_DIRECTORY)
        return;
    QDir dir (m_data.current_directory);
    if (dir.cdUp ()) {
        setCurrentDirectory (dir.absolutePath ());
        m_data.updateDirectories ();
    }
}

KMPLAYER_NO_EXPORT bool DirModel::canGoUp ()
{
    return m_data.current_directory != USER_ROOT_DIRECTORY;
}

KMPLAYER_NO_EXPORT QString DirModel::directory ()
{
    if (m_data.current_directory == USER_ROOT_DIRECTORY)
        return tr ("Phone");
    QString entry = QDir (m_data.current_directory).dirName ();
    return entry;
}

KMPLAYER_NO_EXPORT void DirModel::enterDirectory (const QString& dirname)
{
    QString entry = dirname;
    QDir dir (m_data.current_directory);
    if (dir.cd (entry) || dir.cd (dirname)) {
        setCurrentDirectory (dir.absolutePath ());
        m_data.updateDirectories ();
    }
}


FileModel::FileModel (QObject *parent, DirModel* dm)
    : QAbstractItemModel (parent)
    , m_data (this, dm)
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "caption";
    setRoleNames(roles);
    connect (dm, SIGNAL (directoryChanged()), SLOT (directoryChanged()));
}

FileModel::~FileModel ()
{}

QVariant FileModel::data (const QModelIndex &index, int role) const
{
    m_data.updateFiles ();
    if (index.column () > 0
            || !index.isValid ()
            || index.row () >= m_data.files.length ()
            || Qt::DisplayRole != role)
        return QVariant ();
    return m_data.files[index.row ()];
}

Qt::ItemFlags FileModel::flags (const QModelIndex &index) const
{
    if (index.isValid ())
        return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    return 0;
}

QVariant FileModel::headerData (int, Qt::Orientation, int) const
{
    return QVariant ();
}

QModelIndex FileModel::index (int row, int col, const QModelIndex& parent) const
{
    if (col > 0 || parent.isValid ())
        return QModelIndex();
    m_data.updateFiles ();
    if (row >= m_data.files.length ())
        return QModelIndex();
    return createIndex (row, col, NULL);
}

QModelIndex FileModel::parent (const QModelIndex&) const
{
    return QModelIndex();
}

bool FileModel::hasChildren (const QModelIndex& parent) const
{
    return rowCount (parent);
}

int FileModel::rowCount (const QModelIndex &parent) const
{
    if (parent.column() > 0 || parent.isValid ())
        return 0;
    m_data.updateFiles ();
    return m_data.files.length ();
}

int FileModel::columnCount (const QModelIndex&) const
{
    return 1;
}

void FileModel::Data::updateFiles ()
{
    if (!files_retrieved) {
        files = QDir (dir_model->currentDirectory ()).entryList (QDir::Files | QDir::Readable | QDir::NoDot | QDir::NoDotDot);
        debugLog() << "retrieving " << files.length () << " files" << endl;
        files_retrieved = true;
        if (files.length ()) {
            file_model->beginInsertRows (QModelIndex(), 0, files.length ()-1);
            file_model->endInsertRows();
        }
    }
}

KDE_NO_EXPORT void FileModel::directoryChanged ()
{
    m_data.files_retrieved = false;
    if (m_data.files.length ()) {
        beginRemoveRows (QModelIndex (), 0, m_data.files.length ()-1);
        m_data.files.clear ();
        endRemoveRows();
    }
}
