/* This file is part of the KMPlayer project
 *
 * Copyright (C) 2010 Koos Vriezen <koos.vriezen@gmail.com>
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
 * until boost gets common, a more or less compatable one ..
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <conic.h>
#include <curl/curl.h>
#include <gio/gio.h>

#include <qurl.h>
#include <qfileinfo.h>

#include "io.h"
#include "log.h"

using namespace KMPlayer;


static int easyCompare (gconstpointer a, gconstpointer b) {
    return (long)a - (long)b;
}

namespace KMPlayer {
    class CurlGetJob {
    public:
        CurlGetJob (IOJobListener * rec, const QString & u);
        ~CurlGetJob ();

        void init (IOJob *job, off_t pos);
        void start ();

        static size_t headerLine( void *p, size_t sz, size_t n, void *strm);
        static size_t dataReceived (void *p, size_t sz, size_t n, void *strm);
        static size_t writePost (void *p, size_t sz, size_t n, void *strm);
        static gboolean findKilledJob (gpointer key, gpointer val, gpointer d);

        IOJobListener * m_receiver;

        static bool global_init;
        static CURLM *multi;

        QString url;
        QString content_type;
        QByteArray post_data;
        curl_slist *headers;
        CURL *easy;
        bool quiet;
        bool killed;
        bool redirected;
        int error_code;
        unsigned long content_length;
    };
}

namespace {

    struct FdWatch {
        int fd;
        int chan_read_watch;
        int chan_write_watch;
    };

}

namespace KMPlayer {

    IOJob *asyncGet (IOJobListener *receiver, const QString &url) {
        return new IOJob (new CurlGetJob (receiver, url));
    }
}

inline gboolean CurlGetJob::findKilledJob (gpointer key, gpointer value, gpointer data)
{
    IOJob *job = (IOJob *)value;
    CurlGetJob *self = job->d;

    if (self->killed) {
        *(IOJob **)data = job;
        return true;
    }
    return false;
}

static gboolean curlChannelRead (GIOChannel *src, GIOCondition c, gpointer d);
static gboolean curlChannelWrite (GIOChannel *src, GIOCondition c, gpointer d);
static gboolean curlTimeOut (void *);
static void curlPerformMulti ();
bool CurlGetJob::global_init = false;
CURLM *CurlGetJob::multi = NULL;
static GTree *easy_list = NULL;
static FdWatch *fd_watch_array;
static int fd_watch_array_len;
static int curl_timeout_timer;
static bool in_perform_multi;
static bool curl_job_killed;
static ConIcConnection *con_ic_connection;
#ifdef __ARMEL__
static ConIcConnectionStatus ic_status = CON_IC_STATUS_DISCONNECTED;
#else
static ConIcConnectionStatus ic_status = CON_IC_STATUS_CONNECTED;
#endif
static GSList *ic_connect_waiters;
static bool ic_connect_request;

static
void icConnection (ConIcConnection *conn, ConIcConnectionEvent *ev, gpointer) {
    ic_connect_request = false;
    ic_status = con_ic_connection_event_get_status (ev);
    if (CON_IC_STATUS_CONNECTED == ic_status && ic_connect_waiters) {
        ic_connect_waiters = g_slist_reverse (ic_connect_waiters);
        for (GSList *sl = ic_connect_waiters; sl; sl = sl->next)
            ((CurlGetJob *)sl->data)->start ();
        g_slist_free (ic_connect_waiters);
        ic_connect_waiters = NULL;
        curlPerformMulti ();
    } else if (CON_IC_STATUS_DISCONNECTED == ic_status) {
        ConIcConnectionError err = con_ic_connection_event_get_error (ev);
        if (CON_IC_CONNECTION_ERROR_NONE != err)
            for (GSList *sl = ic_connect_waiters; sl; sl = sl->next) {
                CurlGetJob *curl_job = (CurlGetJob *)ic_connect_waiters->data;
                curl_job->error_code = 1;
                curl_job->killed = true;
            }
        curlPerformMulti ();
    }
}

static void curlChannelDestroy (gpointer) {
#ifdef DEBUG_TIMERS
    debugLog() << "curlChannelDestroy" << endl;
#endif
}

static void updateReadWatches() {
    fd_set fd_read, fd_write, fd_excep;
    FD_ZERO (&fd_read);
    FD_ZERO (&fd_write);
    FD_ZERO (&fd_excep);
    int maxfd = 0;
    curl_multi_fdset (CurlGetJob::multi, &fd_read, &fd_write, &fd_excep, &maxfd);
    ++maxfd;
    if (maxfd > 0 && !fd_watch_array) {
        fd_watch_array = new FdWatch[maxfd];
        memset (fd_watch_array, 0, maxfd * sizeof (FdWatch));
        fd_watch_array_len = maxfd;
    } else if (maxfd > fd_watch_array_len) {
        FdWatch *tmp = fd_watch_array;
        fd_watch_array = new FdWatch[maxfd];
        memset (fd_watch_array, 0, maxfd * sizeof (FdWatch));
        memcpy (fd_watch_array, tmp, fd_watch_array_len * sizeof (FdWatch));
        delete[] tmp;
        fd_watch_array_len = maxfd;
    }
    for (int i = 3; i < fd_watch_array_len; i++) {
        if (FD_ISSET (i, &fd_read)) {
            //debugLog () << "update watches for read fd=" << i << endl;
            if (!fd_watch_array[i].chan_read_watch) {
                fd_watch_array[i].fd = i;
                GIOChannel *channel = g_io_channel_unix_new (i);
                g_io_channel_set_encoding (channel, NULL, NULL);
                fd_watch_array[i].chan_read_watch = g_io_add_watch_full (channel, G_PRIORITY_DEFAULT, G_IO_IN, curlChannelRead, NULL, curlChannelDestroy);
                g_io_channel_unref (channel);
            }
        } else if (fd_watch_array[i].chan_read_watch) {
            g_source_remove (fd_watch_array[i].chan_read_watch);
            fd_watch_array[i].chan_read_watch = 0;
        }
        if (!fd_watch_array[i].chan_read_watch && FD_ISSET (i, &fd_write)) {
            // connecting ..
            //debugLog () << "update watches for write fd=" << i << endl;
            if (!fd_watch_array[i].chan_write_watch) {
                fd_watch_array[i].fd = i;
                GIOChannel *channel = g_io_channel_unix_new (i);
                g_io_channel_set_encoding (channel, NULL, NULL);
                fd_watch_array[i].chan_write_watch = g_io_add_watch_full (channel, G_PRIORITY_DEFAULT, G_IO_OUT, curlChannelWrite, NULL, curlChannelDestroy);
                g_io_channel_unref (channel);
            }
        } else if (fd_watch_array[i].chan_write_watch) {
            g_source_remove (fd_watch_array[i].chan_write_watch);
            fd_watch_array[i].chan_write_watch = 0;
        }
    }
}

static void curlPerformMulti () {

    in_perform_multi = true;

    int running_handles = 0;
    CURLMcode curlCode;

    do {
        curl_job_killed = false;
        curlCode = curl_multi_perform (CurlGetJob::multi, &running_handles);
	if (curl_job_killed)
            do {
                IOJob *job = NULL;
                g_tree_traverse (easy_list, CurlGetJob::findKilledJob, G_IN_ORDER, &job);
                if (job)
                    delete job;
                else
                    break;
            } while (true);
    } while (CURLM_CALL_MULTI_PERFORM == curlCode && running_handles > 0);

    int messages;
    CURLMsg* msg = curl_multi_info_read (CurlGetJob::multi, &messages);
    while (msg) {
        if (CURLMSG_DONE == msg->msg) {
            IOJob *job = (IOJob *)g_tree_lookup (easy_list, msg->easy_handle);
            if (job) {
                if (CURLE_OK != msg->data.result &&
                        CURLE_PARTIAL_FILE != msg->data.result) {
                    debugLog() << "curl error " << msg->data.result << endl;
                    job->setError ();
                }
                delete job;
            }
        }
        msg = curl_multi_info_read (CurlGetJob::multi, &messages);
    }
    updateReadWatches();

    in_perform_multi = false;

    if (!curl_timeout_timer && g_tree_height (easy_list))
        curl_timeout_timer = g_timeout_add (250, curlTimeOut, NULL);
}

static gboolean curlTimeOut (void *) {
#ifdef DEBUG_TIMERS
    debugLog() << "curlTimeOut" << endl;
#endif
    curlPerformMulti ();
    if (g_tree_height (easy_list))
        return 1; // continue
    curl_timeout_timer = 0;
    debugLog() << "clearing polling" << endl;
    return 0; // stop
}

static gboolean curlChannelRead (GIOChannel *src, GIOCondition c, gpointer d) {
#ifdef DEBUG_TIMERS
    debugLog() << "curlChannelRead" << endl;
#endif
    curlPerformMulti ();
    return TRUE;
}

static gboolean curlChannelWrite (GIOChannel *src, GIOCondition c, gpointer d) {
#ifdef DEBUG_TIMERS
    debugLog() << "curl connected" << endl;
#endif
    curlPerformMulti ();
    return TRUE;
}

size_t CurlGetJob::headerLine( void *ptr, size_t size, size_t nmemb, void *stream) {
    IOJob *job = (IOJob *) stream;
    CurlGetJob *self = job->d;
    //char buf[256];
    //snprintf (buf, sizeof (buf), "%s", (char *)ptr);
    //debugLog() << "header: " << buf << endl;
    // Last-Modified:
    if (!self->killed) {
        if (!strncmp ((char *)ptr, "HTTP/", 5)) {
            char *p = strchr ((char *)ptr + 5, ' ');
            if (p) {
                int rc = strtol (p +1, NULL, 10);
                if (rc >= 400) {
                    self->error_code = rc;
                    gchar *buf = g_strndup (p + 1, size * nmemb);
                    debugLog() << "CurlGetJob err: " << buf << endl;
                    g_free (buf);
                }  else if (rc >= 300)
                    self->redirected = true;
            }
        } else if (self->redirected && !strncasecmp ((char *)ptr, "location:", 9)) {
            QString nuri = QString::fromUtf8 ((char *) ptr + 9, size * nmemb - 9);
            self->redirected = false;
            self->m_receiver->redirected (job, nuri.trimmed ());
        } else if (!strncasecmp ((char *)ptr, "content-length:", 15)) {
            self->content_length = strtol ((char *)ptr + 15, NULL, 10);
        } else if (!strncasecmp ((char *)ptr, "content-type:", 13)) {
            self->content_type = QString::fromUtf8 (
                    (char *)ptr + 13, size * nmemb - 13).trimmed ();
            int pos = self->content_type.indexOf (QChar (';'));
            if (pos > 0)
                self->content_type = self->content_type.left (pos);
        }
    }
    return size * nmemb;
}

size_t CurlGetJob::dataReceived (void *ptr, size_t size, size_t nmemb, void *stream) {
    IOJob *job = (IOJob *) stream;
    CurlGetJob *self = job->d;
    QByteArray data ((char *)ptr, size * nmemb);
    //debugLog() << "dataReceived " << (size * nmemb) << endl;
    if (!self->killed)
        self->m_receiver->jobData (job, data);
    return size * nmemb;
}

size_t CurlGetJob::writePost (void *p, size_t sz, size_t n, void *stream) {
    IOJob *job = (IOJob *) stream;
    CurlGetJob *self = job->d;
    int plen = self->post_data.size ();
    if (!plen)
        return 0;
    int count = plen;
    if (count > sz * n)
        count = sz * n;
    memcpy (p, self->post_data.data (), count);
    if (plen > count) {
        memmove (self->post_data.data(), self->post_data.data() + count, plen - count);
    }
    self->post_data.resize (plen - count);
    return count;
}

static int job_count;

KMPLAYER_NO_CDTOR_EXPORT
CurlGetJob::CurlGetJob (IOJobListener * rec, const QString & u)
 : m_receiver (rec), url (u), headers (NULL), easy (NULL),
   quiet (false), killed (false), redirected (false),
   error_code (0), content_length (0)
{}

void CurlGetJob::init (IOJob *job, off_t pos) {
    if (!global_init) {
        global_init = !curl_global_init (CURL_GLOBAL_ALL);
        easy_list = g_tree_new (easyCompare);
    }
    if (!multi)
        multi = curl_multi_init ();
    easy = curl_easy_init ();
    //debugLog() << "CurlGetJob::CurlGetJob " << easy << " " << ++job_count << endl;
    g_tree_insert (easy_list, easy, job);
    //curl_easy_setopt (easy, CURLOPT_VERBOSE, true);
    curl_easy_setopt (easy, CURLOPT_NOPROGRESS, true);
    curl_easy_setopt (easy, CURLOPT_WRITEFUNCTION, CurlGetJob::dataReceived);
    curl_easy_setopt (easy, CURLOPT_WRITEDATA, job);
    curl_easy_setopt (easy, CURLOPT_HEADERFUNCTION, CurlGetJob::headerLine);
    curl_easy_setopt (easy, CURLOPT_WRITEHEADER, job);
    if (pos)
        curl_easy_setopt (easy, CURLOPT_RESUME_FROM_LARGE, (unsigned long long) pos);
    if (post_data.size ()) {
        curl_easy_setopt (easy, CURLOPT_POST, 1);
        curl_easy_setopt (easy, CURLOPT_POSTFIELDSIZE, post_data.size ());
        curl_easy_setopt (easy, CURLOPT_READFUNCTION, CurlGetJob::writePost);
        curl_easy_setopt (easy, CURLOPT_READDATA, job);
    }
    curl_easy_setopt (easy, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt (easy, CURLOPT_MAXREDIRS, 10);
    //CURLSH *curl_share_init( );
    //curl_easy_setopt (easy, CURLOPT_SHARE, m_curlShareHandle);
    //curl_share_setopt(share_handle,CURL_LOCK_DATA_DNS);
    //curl_share_cleanup(CURLSH * share_handle );
    curl_easy_setopt (easy, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt (easy, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
    curl_easy_setopt (easy, CURLOPT_ENCODING, "");
    curl_easy_setopt (easy, CURLOPT_USERAGENT, "KMPlayer/" VERSION);
    curl_easy_setopt (easy, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt (easy, CURLOPT_URL, url.toUtf8 ().data ());
    if (headers)
        curl_easy_setopt (easy, CURLOPT_HTTPHEADER, headers);
    //if (!strncmp (u, "http", 4))
    //    curl_easy_setopt (easy, CURLOPT_HTTPGET, true);
    debugLog() << "Curl init connected:" << (CON_IC_STATUS_CONNECTED == ic_status) << endl;
    if (CON_IC_STATUS_CONNECTED == ic_status || Url::isLocalFile (QUrl (url))) {
        start ();
    } else if (ic_connect_request) {
        ic_connect_waiters = g_slist_prepend (ic_connect_waiters, this);
    } else {
        if (!con_ic_connection) {
            con_ic_connection = con_ic_connection_new ();
            g_signal_connect (G_OBJECT (con_ic_connection), "connection-event",
                    G_CALLBACK (icConnection), NULL);
        }
        ic_connect_request = true;
        if (con_ic_connection_connect (con_ic_connection,
                CON_IC_CONNECT_FLAG_NONE)) {
            if (!ic_connect_request)
                start ();
            else
                ic_connect_waiters = g_slist_prepend (ic_connect_waiters, this);
        } else {
            ic_connect_request = false;
            job->kill (false);
        }
    }
}

void CurlGetJob::start () {
    CURLMcode type = curl_multi_add_handle (multi, easy);
    if (type && type != CURLM_CALL_MULTI_PERFORM)
        debugLog() << "CurlGetJob add to multi failure" << endl;
    else if (!ic_connect_waiters)
        curlPerformMulti ();
}

KMPLAYER_NO_CDTOR_EXPORT CurlGetJob::~CurlGetJob () {
    //debugLog() << "CurlGetJob::~CurlGetJob " << easy << " " << --job_count << endl;
    if (ic_connect_waiters)
        ic_connect_waiters = g_slist_remove (ic_connect_waiters, this);
    if (easy) {
        curl_multi_remove_handle (multi, easy);
        curl_easy_cleanup (easy);
        g_tree_remove (easy_list, easy);
    }
    if (headers)
        curl_slist_free_all (headers);
    if (0) {
        curl_multi_cleanup (multi);
        multi = NULL;
    }
}

KMPLAYER_NO_EXPORT IOJob::IOJob (IOJobPrivate *priv) : d (priv) {
}

KMPLAYER_NO_EXPORT IOJob::~IOJob () {
    IOJobListener *listener = d->easy && !d->quiet ? d->m_receiver : NULL;

    if (listener)
        listener->jobResult (this);

    delete d;
}

KMPLAYER_NO_EXPORT void IOJob::setHeader (const QString &header) {
    d->headers = curl_slist_append (d->headers, header.toUtf8 ().data ());
}

KMPLAYER_NO_EXPORT void IOJob::setHttpPostData (const QByteArray &data) {
    d->post_data = data;
}

KMPLAYER_NO_EXPORT void IOJob::start (off_t pos) {
    d->init (this, pos);
}

KMPLAYER_NO_EXPORT void IOJob::kill (bool q) {
    d->quiet = q;
    d->killed = true;
    curl_job_killed = true;
    if (!in_perform_multi) {
        delete this;
	curlPerformMulti ();
    }
}

KMPLAYER_NO_EXPORT bool IOJob::error () {
    return d->error_code;
}

KMPLAYER_NO_EXPORT void IOJob::setError () {
    d->error_code = 1;
}

unsigned long IOJob::contentLength() const {
    return d->content_length;
}

QString IOJob::contentType () const {
    return d->content_type;
}

bool MimeType::isBufferBinaryData (const char *data, int size)
{
    const gchar *end;
    if (g_utf8_validate ((gchar *) data, size, &end))
        return false;
    int valid = end - data;
    if (size - valid < 6)
        return g_utf8_get_char_validated (end, size - valid) != (gunichar)-2;
    return true;
}

QString MimeType::findByContent (const char *data, int nr) {
    debugLog() << "findByContent " << nr << endl;
    if (nr <= 0)
        return QString();
    gboolean pb;
    char *type = g_content_type_guess (NULL, (const guchar *)data, nr, &pb);
    QString s (type);
    g_free (type);
    return s;
}

QString MimeType::findByURL (const QUrl &url) {
    gboolean pb;
    if (Url::isLocalFile (url)) {
        QFileInfo finfo (url.path ());
        if (finfo.exists()) {
            char *type = g_content_type_guess (finfo.absoluteFilePath ().toLocal8Bit().data(), NULL, 200, &pb);
            debugLog() << "findByUrl " << url.path() << ": " << type << endl;
            QString s (type);
            g_free (type);
            return s;
        }
    }
    debugLog() << "findByUrl " << url.toString() << endl;
    warningLog () << "MimeType::findByURL not supported on remote links\n";
    return QString ();
}

bool Url::isLocalFile (const QUrl &url)
{
    QString scheme = url.scheme ();
    return scheme.isEmpty() || scheme == "file";
}
