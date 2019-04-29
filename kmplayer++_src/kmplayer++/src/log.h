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

#ifndef _KMPLAYER_LOG_H_
#define _KMPLAYER_LOG_H_

#include "kmplayer_def.h"
#include <QUrl>

namespace KMPlayer {


class Log;

typedef Log & (*LOGFUNC) (Log &);

class Log {
    Log ();
    friend Log debugLog ();
    friend Log warningLog ();
    friend Log errorLog ();
public:
    ~Log ();
    Log & operator << (const QString & s);
    Log & operator << (const char * s);
    //Log & operator << (const Char & s);
    Log & operator << (const QUrl& u);
    Log & operator << (int i);
    Log & operator << (unsigned int i);
    Log & operator << (double d);
    Log & operator << (void *);
    Log & operator << (LOGFUNC manip);
};

Log debugLog ();
Log warningLog ();
Log errorLog ();


//-----------------------------------------------------------------------------

KMPLAYER_NO_CDTOR_EXPORT inline Log::Log () {}

KMPLAYER_NO_CDTOR_EXPORT inline Log::~Log () {}

KMPLAYER_NO_EXPORT inline Log & Log::operator << (const QString & s) {
    return *this << qPrintable (s);
}

KMPLAYER_NO_EXPORT inline Log & Log::operator << (const QUrl& u) {
    return *this << u.toString ();
}

KMPLAYER_NO_EXPORT inline Log & endl (Log & log) {
    log << "\n";
    return log;
}

KMPLAYER_NO_EXPORT inline Log & Log::operator << (LOGFUNC f) {
    return (*f) (*this);
}

KMPLAYER_NO_EXPORT inline Log debugLog () {
    return Log ();
}

KMPLAYER_NO_EXPORT inline Log warningLog () {
    Log log;
    return log << "[WARNING] ";
}

KMPLAYER_NO_EXPORT inline Log errorLog () {
    Log log;
    return log << "[ERROR] ";
}

} // namespace

#endif
