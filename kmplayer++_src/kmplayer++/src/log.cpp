/*
 * Copyright (C) 2011 Koos Vriezen <koos.vriezen@gmail.com>
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
 *
 */

#include <stdio.h>
#include <unistd.h>

#include "log.h"

using namespace KMPlayer;


static FILE *log_out;

static FILE *getLog () {
    /*if (!log_out) {
        log_out = isatty (2) ? stderr : fopen ("/home/user/.kmplayer/log.txt", "w+");
        FILE *f = fopen ("/home/user/.kmplayer/err.txt", "w+");
        fprintf(f, "isaaty %d out %p stderr %p\n", isatty(2), log_out, stderr);
        fclose(f);
    }
    return log_out;*/
    return stderr;
}

KMPLAYER_NO_EXPORT Log & Log::operator << (const char * s) {
    FILE *out = getLog ();
    if (out)
        fprintf (out, "%s", s ? s : "<NULL>");
    return *this;
}

KMPLAYER_NO_EXPORT Log & Log::operator << (int i) {
    FILE *out = getLog ();
    if (out)
        fprintf (out, "%d", i);
    return *this;
}

KMPLAYER_NO_EXPORT Log & Log::operator << (unsigned int i) {
    FILE *out = getLog ();
    if (out)
        fprintf (out, "%u", i);
    return *this;
}

KMPLAYER_NO_EXPORT Log & Log::operator << (double d) {
    FILE *out = getLog ();
    if (out)
        fprintf (out, "%f", d);
    return *this;
}

KMPLAYER_NO_EXPORT Log & Log::operator << (void * d) {
    FILE *out = getLog ();
    if (out)
        fprintf (out, "0x%x", d);
    return *this;
}
