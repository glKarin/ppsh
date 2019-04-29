/*
* Copyright (C) 2010  Koos Vriezen <koos.vriezen@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _KMPLAYER_IO_H_
#define _KMPLAYER_IO_H_

#include <qstring.h>

class QUrl;

namespace KMPlayer {

class IOJob;
class IOJobListener;
typedef class CurlGetJob IOJobPrivate;

IOJob *asyncGet (IOJobListener *receiver, const QString &url);

class IOJob {
    friend IOJob *asyncGet (IOJobListener *receiver, const QString &url);
    friend class CurlGetJob;

public:
    ~IOJob ();

    void setHeader (const QString &header);
    void setHttpPostData (const QByteArray &data);
    void start (off_t pos=0);
    void kill (bool quiet = true);
    bool error ();
    void setError ();
    unsigned long contentLength() const;
    QString contentType () const;

private:
    IOJob (IOJobPrivate *);

    IOJobPrivate *d;
};

class IOJobListener {
public:
    virtual void jobData (IOJob *job, QByteArray& data) = 0;
    virtual void jobResult (IOJob *job) = 0;
    virtual void redirected (IOJob *job, const QString& uri) = 0;
};

namespace MimeType {
    QString findByContent (const char * data, int size);
    QString findByURL (const QUrl & url);
    bool isBufferBinaryData (const char * data, int size);
}

namespace Url {
    bool isLocalFile (const QUrl &url);
}

} // namespace

#endif
