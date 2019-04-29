/* This file is part of the KMPlayer project
 *
 * Copyright (C) 2005 Koos Vriezen <koos.vriezen@xs4all.nl>
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
 */
#include <math.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <glib.h>

#include "player.h"
#include "io.h"
//#include "lists.h"

using namespace KMPlayer;


Process::Process (ProcessNotify * ctrl, const QString& n, bool xv)
 : m_notify (ctrl), m_mrl_notify (NULL), m_viewer (0L), m_name (n),
      m_state (NotRunning), m_old_state (NotRunning),
      start_timer (0), m_repeat (0),
      no_video (false), has_xv (xv), is_local_file (false)
      {}

Process::~Process () {
    stop ();
    if (start_timer)
        g_source_remove (start_timer);
}

//WId Process::widget () {
//    return 0;
//}

KMPLAYER_NO_EXPORT bool Process::playing () const {
    return m_state > Ready;
}

//void Process::setAudioLang (int, const QString &) {}

//void Process::setSubtitle (int, const QString &) {}

KMPLAYER_NO_EXPORT void Process::setPosition (int p) {
    m_position = p;
    m_notify->setPosition (m_position);
}

KMPLAYER_NO_EXPORT bool Process::pause () {
    return false;
}

KMPLAYER_NO_EXPORT bool Process::seek (int /*pos*/, bool /*absolute*/) {
    return false;
}

KMPLAYER_NO_EXPORT bool Process::volume (int /*pos*/, bool /*absolute*/) {
    return false;
}

//KMPLAYER_NO_EXPORT bool Process::saturation (int /*pos*/, bool /*absolute*/) {
    //return false;
//}

//KMPLAYER_NO_EXPORT bool Process::hue (int /*pos*/, bool /*absolute*/) {
    //return false;
//}

//KMPLAYER_NO_EXPORT bool Process::contrast (int /*pos*/, bool /*absolute*/) {
    //return false;
//}

//KMPLAYER_NO_EXPORT bool Process::brightness (int /*pos*/, bool /*absolute*/) {
    //return false;
//}

void Process::toggleScale ()
{}

//bool Process::grabPicture (const KURL & /*url*/, int /*pos*/) {
//    return false;
//}

//bool Process::supports (const char * source) const {
//    for (const char ** s = m_supported_sources; s[0]; ++s) {
//        if (!strcmp (s[0], source))
//            return true;
//    }
//    return false;
//}

KMPLAYER_NO_EXPORT bool Process::stop () {
    if (!playing ()) return true;
    return false;
}

KMPLAYER_NO_EXPORT bool Process::quit () {
    debugLog() << "quit" << endl;
    stop ();
    setState (NotRunning);
    return !playing ();
}

KMPLAYER_NO_EXPORT void Process::updateAspects () {
    if (m_notify) {
        float aspect = m_aspect;
        if (aspect < 0.01)
            aspect = m_height > 0 ? 1.0 * m_width / m_height : 0;
        m_notify->setAspect (aspect);
        if (aspect > 0.01) {
            no_video = false;
            m_notify->videoDetected ();
        }
    }
}

static gboolean cb_scheduledStateChanged (void * p) {
    static_cast <Process *> (p)->scheduledStateChanged ();
    return false; // single shot
}

KMPLAYER_NO_EXPORT void Process::setState (State newstate) {
    if (m_state != newstate) {
        m_old_state = m_state;
        m_state = newstate;
        start_timer = g_timeout_add (0, cb_scheduledStateChanged, static_cast <void *> (this));
    }
}

KMPLAYER_NO_EXPORT void Process::scheduledStateChanged () {
    m_notify->stateChanged (this, m_old_state, m_state);
    start_timer = 0;
}

KMPLAYER_NO_EXPORT bool Process::play (const QString &url) {
    m_url = url;
    m_position = m_length = 0;
    m_user_stop_position = 0;
    m_width = m_height = 0;
    m_aspect = 0.0;
    is_local_file = false;
    return false;
}

KMPLAYER_NO_EXPORT bool Process::play (const QString &url, const QString &a) {
    m_url = url;
    m_position = m_length = 0;
    m_user_stop_position = 0;
    m_width = m_height = 0;
    m_aspect = 0.0;
    is_local_file = false;

		sAudio = a;

    return false;
}

bool Process::setMrlBackEndNotify (MrlBackEndNotify *mrl_notify)
{
    m_mrl_notify = mrl_notify;
    m_repeat = 0;
    no_video = false;
    return true;
}

KMPLAYER_NO_EXPORT bool Process::ready (unsigned int viewer) {
    m_viewer = viewer;
    setState (Ready);
    return true;
}

KMPLAYER_NO_EXPORT unsigned int Process::viewer () const {
    return m_viewer;
}

void Process::setUserStop () {
    if (m_state > Buffering && m_length > 0 && is_local_file)
        m_user_stop_position = m_position;
}

int Process::getPid () const
{
    return 0;
}

//-----------------------------------------------------------------------------

static void watchProcessFunc (GPid pid, gint status, gpointer data) {
#ifdef DEBUG_PROCESS_WATCH
    debugLog () << " watchProcessFunc " << endl;
#endif
    g_spawn_close_pid (pid);
    static_cast <UnixProcess *> (data)->processExited (pid);
}

static gboolean watchMPlayerStdin (GIOChannel *, GIOCondition, gpointer data) {
    static_cast <UnixProcess *> (data)->writeEvent ();
#ifdef DEBUG_PROCESS_WATCH
    debugLog () << " watchProcessStdin " << endl;
#endif
    return true;
}

static void setupProcessFunc (gpointer data) {
}

static gboolean
watchProcessStdout (GIOChannel *src, GIOCondition, gpointer data) {
#ifdef DEBUG_PROCESS_WATCH
    debugLog () << " watchProcessStdout " << endl;
#endif
    if (G_IO_IN)
        static_cast <UnixProcess *> (data)->readEvent (src);
    return true;
}

UnixProcess::UnixProcess (UnixProcessListener *l, int flags)
    : listener (l), io_flags (flags),
    pin (0L), pout (0L), win (0), wout (0) {
}

bool UnixProcess::start (const QString &pwd, const QString &cmdline) {
    int argc;
    gchar **argv;
    GError *err = 0L;
    int fdin, fdout, fderr;
    eof = false;
    debugLog () << cmdline << endl;
    if (g_shell_parse_argv (cmdline.toLocal8Bit ().constData (), &argc, &argv, &err) &&
            g_spawn_async_with_pipes (pwd.toLocal8Bit ().constData (),
                argv,
                0L,
                (GSpawnFlags)(G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD),
                setupProcessFunc,
                static_cast <void*> (this),
                &process_id,
                &fdin,
                &fdout,
                0L /*&fderr*/,
                &err)) {
        //darn glib: assertion err!=null g_error_free (err);
        g_strfreev (argv);
        g_child_watch_add(process_id, watchProcessFunc,static_cast<void*>(this));
        pin = g_io_channel_unix_new (fdin);
        g_io_channel_set_encoding (pin, NULL, NULL);
        g_io_channel_set_buffered (pin, false);
        GIOFlags flags = g_io_channel_get_flags (pin);
        g_io_channel_set_flags (pin, (GIOFlags)(flags|G_IO_FLAG_NONBLOCK), 0L);

        pout = g_io_channel_unix_new (fdout);
        g_io_channel_set_encoding (pout, NULL, NULL);
        flags = g_io_channel_get_flags (pout);
        g_io_channel_set_flags (pout, (GIOFlags)(flags|G_IO_FLAG_NONBLOCK), 0L);
        g_io_channel_set_buffer_size (pout, 0);
        wout = g_io_add_watch (pout, G_IO_IN, watchProcessStdout, this);
        return true;
    }
    errorLog () << err->message << endl;
    g_error_free (err);
    return false;
}

static void killProcessGroup (int sig)
{
    void (*oldhandler)(int) = signal (sig, SIG_IGN);
    kill (-1 * getpid (), sig);
    signal (sig, oldhandler);
}

void UnixProcess::stop () {
    closeStdIn ();
    closeStdOut ();
    if (process_id) {
        errorLog () << "interupt process" << endl;
        killProcessGroup (SIGINT);
        for (int n = 0; process_id && n < 50; n++) {
            usleep (10000); // 10ms
            while (g_main_context_iteration (0L, false))
                if (!process_id)
                    break;
        }
        if (process_id) {
            errorLog () << "terminate process" << endl;
            killProcessGroup (SIGTERM);
            for (int n = 0; process_id && n < 50; n++) {
                usleep (10000); // 10ms
                while (g_main_context_iteration (0L, false))
                    if (!process_id)
                        break;
            }
        }
        if (process_id) {
            ::kill (process_id, SIGKILL);
            processExited (process_id); // give up
        }
    }
}

bool UnixProcess::running () const {
    return process_id;
}

void UnixProcess::pollStdIn (bool b) {
    if (b) {
        if (!win)
            win = g_io_add_watch (pin, G_IO_OUT, watchMPlayerStdin, this);
    } else if (win) {
        g_source_remove (win);
        win = 0;
    }
}

int UnixProcess::readStdOut (char *buf, int len) {
    gsize nr = 0;;
    if (pout) {
        GIOStatus status = g_io_channel_read_chars (pout, buf, len, &nr, 0L);
        if (status == G_IO_STATUS_EOF || status == G_IO_STATUS_ERROR /*FIXME*/)
            eof = true;
    } else {
        eof = true;
    }
    return nr;
}

int UnixProcess::writeStdIn (const QByteArray& data) {
    gsize nr = 0;
    g_io_channel_write_chars (pin, data.constData (), data.size (), &nr, 0L);
    //g_io_channel_flush (pin, 0L);
    return nr;
}

void UnixProcess::closeStdIn () {
    pollStdIn (false);
    if (pin) {
        g_io_channel_shutdown (pin, true, 0L);
        g_io_channel_unref (pin);
        pin = 0L;
    }
}

void UnixProcess::closeStdOut () {
    if (wout) {
        g_source_remove (wout);
        wout = 0;
    }
    if (pout) {
        g_io_channel_shutdown (pout, true, 0L);
        g_io_channel_unref (pout);
        pout = 0L;
    }
}

void UnixProcess::readEvent (GIOChannel *channel) {
    listener->readEvent (this);
}

void UnixProcess::writeEvent () {
    if (pin)
        listener->writeEvent (this);
}

void UnixProcess::processExited (GPid pid) {
    if (pid == process_id) {
        process_id = 0;
        closeStdIn ();
        listener->processExited (this);
    }
}

//-----------------------------------------------------------------------------

#define NO_SEEK 0x7fffffff

MPlayer::MPlayer (ProcessNotify *ctrl, const QString& nm)
 : Process (ctrl, nm, true),
   unix_process (this, UnixProcess::StdIn | UnixProcess::StdOut),
#ifdef _HARMATTAN
   path(OPTDIR "/" _HARMATTAN_KMPLAYER "/bin/" "mplayer"),
#else
   path ("mplayer"),
#endif
   audio_driver (NULL),
   video_driver (NULL),
   m_transition_state (NotRunning),
   auto_sub_titles (false),
   supports_panzoom (false),
   needs_restarted (false) {}

MPlayer::~MPlayer () {
}

KMPLAYER_NO_EXPORT bool MPlayer::play (const QString &uri) {
    if (!needs_restarted && playing ()) {
        if (m_mrl_notify &&
                (m_transition_state == Paused ||
                 (Paused == m_state && m_transition_state != Playing))) {
            m_transition_state = Playing;
            if (!removeQueued ("pause"))
                sendCommand (QString ("pause"));
            return true;
        }
        return false;
    }
    debugLog () << "MPlayer::play" << endl;
    m_transition_state = NotRunning;
    if (!needs_restarted)
        panzoom_enabled = false;
    setState (Buffering);
    Process::play (uri);
    if (m_url.isEmpty ()) {
        setState (Ready);
        return false;
    }
    request_seek = NO_SEEK;
    request_quit = false;
    outbuf[0] = 0;
    QString cmdline (path);

    cmdline += " -nofs -slave -identify -vo ";
    char buf[128];
    //if (has_xv)
    //    snprintf (buf, sizeof(buf), "xv -wid %u ", m_viewer);
    //else
        snprintf (buf, sizeof(buf),
                video_driver
                ? video_driver
                : "xv");
    cmdline += QString (buf);

    if (audio_driver) {
        snprintf (buf, sizeof (buf), " -ao %s", audio_driver);
        cmdline += QString (buf);
    }

    QString wd (getenv ("HOME"));
    QUrl url (m_url);
    is_local_file = Url::isLocalFile (url);
    if (is_local_file) {
        m_url = url.path ();
        wd = url.path ();
        int pos = wd.lastIndexOf (QString ("/"));
        if (pos > -1)
            wd.truncate (pos);
#if 0
        if (auto_sub_titles) {
            pos = m_url.lastIndexOf (QString ("."));
            if (pos > -1) {
                QString sub = m_url.left (pos + 1);
                debugLog() << "check " << sub << endl;
                if (File (sub + "srt").exists ())
                    sub += "srt";
                else if (File (sub + "SRT").exists ())
                    sub += "SRT";
                else
                    sub.clear ();
                if (!sub.isEmpty ()) {
                    char *quoted = g_shell_quote ((const char *) sub);
                    cmdline += QString (" -sub ") + QString (quoted);
                    g_free (quoted);
                }
            }
        }
#endif
    }
#ifdef _HARMATTAN
		else
		{
			if(!tRequestHeaders.isEmpty())
			{
				cmdline += QString(" -http-header-fields '") + tRequestHeaders.join(",") + "'";
			}
		}
#endif

    snprintf (buf, sizeof(buf), " -wid %u ", m_viewer);
    cmdline += QString (buf);
    Mrl *mrl = this->mrl ();
    if (m_repeat > 0)
        cmdline += QString (" -loop ") + QString::number (m_repeat);
    if (is_local_file && mrl->clip_begin > 100)
        cmdline += QString (" -ss ") + QString::number ((int) mrl->clip_begin/100);
    cmdline += QChar (' ');

    const TrieString aspect_ratio ("aspectRatio");
    bool found_attr = false;
    QString crop;
    for (Node *n = mrl; n && !found_attr; n = n->parentNode ()) {
        if (n->id != id_node_group_node && n->id != id_node_playlist_item)
            continue;
        Attribute *a = static_cast <Element *> (n)->attributes ().first ();
        for (; a; a = a->nextSibling ()) {
            if (aspect_ratio == a->name ()) {
                QString asp = a->value ();
                if (asp.isEmpty ())
                    continue;
                int p = asp.indexOf (QChar (':'));
                if (p > 0) {
                    int a = asp.left (p).toInt ();
                    int b = asp.mid (p + 1).toInt ();
                    if (a > 0 && b > 0)
                        cmdline += QString ("-aspect ") +
                            QString::number (a) + QChar (':') + QString::number (b);
                } else {
                    float a = (float) asp.toDouble ();
                    if (a > 0.01) {
                        char buf[32];
                        snprintf (buf, sizeof (buf), "%0.3f", a);
                        cmdline += QString ("-aspect ") +
                            QString (buf).replace (QChar (','), QChar ('.'));
                    }
                }
                cmdline += QChar (' ');
                found_attr = true;
            } else if (a->name () == Ids::attr_panzoom) {
                QString coord[4];
                if (Mrl::parsePanZoomString (a->value (), coord)) {
                    crop = QString ("-vf crop=") +
                        QString::number (coord[2].toInt()) + QChar (':') +
                        QString::number (coord[3].toInt()) + QChar (':') +
                        QString::number (coord[0].toInt()) + QChar (':') +
                        QString::number (coord[1].toInt());
                }
                found_attr = true;
            }
        }
    }
    if (needs_restarted) {
        int x, y, w, h;
        needs_restarted = false;
        if (panzoom_enabled && calculateScale (x, y, w, h)) {
            crop = QString ("-vf crop=") +
                QString::number (w) + QChar (':') +
                QString::number (h) + QChar (':') +
                QString::number (x) + QChar (':') +
                QString::number (y);
            m_width = w;
            m_height = h;
            updateAspects ();
        }
    }
    if (!crop.isEmpty ())
        cmdline += crop + QChar (' ');

    char *quoted = g_shell_quote (m_url.toLocal8Bit ().constData ());
    cmdline += QString::fromLocal8Bit (quoted);
    g_free (quoted);
    if (unix_process.start (wd, cmdline)) {
        setState (Process::Buffering); //FIXME
        return true;
    }
    setState (Ready);
    return false;
}

KMPLAYER_NO_EXPORT bool MPlayer::play (const QString &uri, const QString &a) {
    if (!needs_restarted && playing ()) {
        if (m_mrl_notify &&
                (m_transition_state == Paused ||
                 (Paused == m_state && m_transition_state != Playing))) {
            m_transition_state = Playing;
            if (!removeQueued ("pause"))
                sendCommand (QString ("pause"));
            return true;
        }
        return false;
    }
    debugLog () << "MPlayer::play" << endl;
    m_transition_state = NotRunning;
    if (!needs_restarted)
        panzoom_enabled = false;
    setState (Buffering);
    Process::play (uri, a);
    if (m_url.isEmpty ()) {
        setState (Ready);
        return false;
    }
    request_seek = NO_SEEK;
    request_quit = false;
    outbuf[0] = 0;
    QString cmdline (path);

    cmdline += " -nofs -slave -identify -vo ";
    char buf[128];
    //if (has_xv)
    //    snprintf (buf, sizeof(buf), "xv -wid %u ", m_viewer);
    //else
        snprintf (buf, sizeof(buf),
                video_driver
                ? video_driver
                : "xv");
    cmdline += QString (buf);

    if (audio_driver) {
        snprintf (buf, sizeof (buf), " -ao %s", audio_driver);
        cmdline += QString (buf);
    }

    QString wd (getenv ("HOME"));
    QUrl url (m_url);
    is_local_file = Url::isLocalFile (url);
    if (is_local_file) {
        m_url = url.path ();
        wd = url.path ();
        int pos = wd.lastIndexOf (QString ("/"));
        if (pos > -1)
            wd.truncate (pos);
#if 0
        if (auto_sub_titles) {
            pos = m_url.lastIndexOf (QString ("."));
            if (pos > -1) {
                QString sub = m_url.left (pos + 1);
                debugLog() << "check " << sub << endl;
                if (File (sub + "srt").exists ())
                    sub += "srt";
                else if (File (sub + "SRT").exists ())
                    sub += "SRT";
                else
                    sub.clear ();
                if (!sub.isEmpty ()) {
                    char *quoted = g_shell_quote ((const char *) sub);
                    cmdline += QString (" -sub ") + QString (quoted);
                    g_free (quoted);
                }
            }
        }
#endif
    }

		else
		{
			if(!tRequestHeaders.isEmpty())
			{
				cmdline += QString(" -http-header-fields '") + tRequestHeaders.join(",") + "'";
			}
		}
		if(!sAudio.isEmpty())
		{
			cmdline += QString(" -audiofile '") + sAudio + "'";
		}

    snprintf (buf, sizeof(buf), " -wid %u ", m_viewer);
    cmdline += QString (buf);
    Mrl *mrl = this->mrl ();
    if (m_repeat > 0)
        cmdline += QString (" -loop ") + QString::number (m_repeat);
    if (is_local_file && mrl->clip_begin > 100)
        cmdline += QString (" -ss ") + QString::number ((int) mrl->clip_begin/100);
    cmdline += QChar (' ');

    const TrieString aspect_ratio ("aspectRatio");
    bool found_attr = false;
    QString crop;
    for (Node *n = mrl; n && !found_attr; n = n->parentNode ()) {
        if (n->id != id_node_group_node && n->id != id_node_playlist_item)
            continue;
        Attribute *a = static_cast <Element *> (n)->attributes ().first ();
        for (; a; a = a->nextSibling ()) {
            if (aspect_ratio == a->name ()) {
                QString asp = a->value ();
                if (asp.isEmpty ())
                    continue;
                int p = asp.indexOf (QChar (':'));
                if (p > 0) {
                    int a = asp.left (p).toInt ();
                    int b = asp.mid (p + 1).toInt ();
                    if (a > 0 && b > 0)
                        cmdline += QString ("-aspect ") +
                            QString::number (a) + QChar (':') + QString::number (b);
                } else {
                    float a = (float) asp.toDouble ();
                    if (a > 0.01) {
                        char buf[32];
                        snprintf (buf, sizeof (buf), "%0.3f", a);
                        cmdline += QString ("-aspect ") +
                            QString (buf).replace (QChar (','), QChar ('.'));
                    }
                }
                cmdline += QChar (' ');
                found_attr = true;
            } else if (a->name () == Ids::attr_panzoom) {
                QString coord[4];
                if (Mrl::parsePanZoomString (a->value (), coord)) {
                    crop = QString ("-vf crop=") +
                        QString::number (coord[2].toInt()) + QChar (':') +
                        QString::number (coord[3].toInt()) + QChar (':') +
                        QString::number (coord[0].toInt()) + QChar (':') +
                        QString::number (coord[1].toInt());
                }
                found_attr = true;
            }
        }
    }
    if (needs_restarted) {
        int x, y, w, h;
        needs_restarted = false;
        if (panzoom_enabled && calculateScale (x, y, w, h)) {
            crop = QString ("-vf crop=") +
                QString::number (w) + QChar (':') +
                QString::number (h) + QChar (':') +
                QString::number (x) + QChar (':') +
                QString::number (y);
            m_width = w;
            m_height = h;
            updateAspects ();
        }
    }
    if (!crop.isEmpty ())
        cmdline += crop + QChar (' ');

    char *quoted = g_shell_quote (m_url.toLocal8Bit ().constData ());
    cmdline += QString::fromLocal8Bit (quoted);
    g_free (quoted);
    if (unix_process.start (wd, cmdline)) {
        setState (Process::Buffering); //FIXME
        return true;
    }
    setState (Ready);
    return false;
}

KMPLAYER_NO_EXPORT bool MPlayer::pause () {
    if (Paused != m_transition_state) {
        m_transition_state = Paused;
        if (!removeQueued ("pause"))
            sendCommand (QString ("pause"));
        return true;
    }
    return false;
}

KMPLAYER_NO_EXPORT bool MPlayer::ready (unsigned int viewer) {
    if (m_state < Ready)
        return Process::ready (viewer);
    return false;
}

KMPLAYER_NO_EXPORT bool MPlayer::stop () {
    debugLog() << "stop mplayer" << endl;
    needs_restarted = false;
    return sendCommand (QString ("quit"));
}

KMPLAYER_NO_EXPORT bool MPlayer::quit () {
    if (playing ()) {
        needs_restarted = false;
        request_quit = true;
        stop ();
        while (g_main_context_iteration (0L, false))
            if (!unix_process.running ())
                break;
        for (int n = 0; unix_process.running () && n < 50; n++) {
            usleep (10000); // 10ms
            while (g_main_context_iteration (0L, false))
                if (!unix_process.running ())
                    break;
        }
        if (unix_process.running ())
            unix_process.stop ();

        commands.clear ();
        request_quit = false;
    }
    setState (NotRunning);
    return true;
}

KMPLAYER_NO_EXPORT bool MPlayer::seek (int pos, bool absolute) {
    if (removeQueued ("seek"))
        request_seek = NO_SEEK;
    if (request_seek != NO_SEEK)
        return false;
    if (!absolute && m_length > 0) {
        pos += m_position;
        absolute = true;
    }
    if (absolute && pos < 0)
        pos = 0;
    if (pos < m_length)
        m_position = pos;
    request_seek = pos;
    char buf[32];
    snprintf (buf, sizeof (buf), "seek %d %d", pos/10, absolute ? 2 : 0);
    return sendCommand (QString (buf));
}

KMPLAYER_NO_EXPORT bool MPlayer::volume (int pos, bool /*absolute*/) {
    return sendCommand (QString ("volume ") + QString::number (pos));
}

//KMPLAYER_NO_EXPORT bool MPlayer::saturation (int pos, bool absolute) {
    //return true;
//}

//KMPLAYER_NO_EXPORT bool MPlayer::hue (int pos, bool absolute) {
    //return true;
//}

//KMPLAYER_NO_EXPORT bool MPlayer::contrast (int pos, bool absolute) {
    //return true;
//}

//KMPLAYER_NO_EXPORT bool MPlayer::brightness (int pos, bool absolute) {
    //return true;
//}

bool MPlayer::calculateScale (int &x, int &y, int &w, int &h)
{
    if (stream_width > 0 && stream_height > 0) {
        if ((1.0 * stream_height) / stream_width > 0.6) {
            x = 0;
            h = stream_width * 480 / 854;
            if (h <= 0)
                return false;
            y = (stream_height - h) / 2;
            w = stream_width;
        } else {
            y = 0;
            w = stream_height * 854 / 480;
            if (w <= 0)
                return false;
            x = (stream_width - w) / 2;
            h = stream_height;
        }
        return true;
    }
    return false;
}

void MPlayer::toggleScale ()
{
    if (supports_panzoom && stream_width > 0 && stream_height > 0) {
        char buf[64];
        if (panzoom_enabled) {
            panzoom_enabled = false;
            snprintf (buf, sizeof (buf),
                    "panzoom 0 0 %d %d", stream_width, stream_height);
        } else {
            int x, y, w, h;
            if (!calculateScale (x, y, w, h))
                return;
            panzoom_enabled = true;
            snprintf (buf, sizeof (buf), "panzoom %d %d %d %d", x, y, w, h);
        }
        sendCommand (QString (buf));
    } else {
        panzoom_enabled = !panzoom_enabled;
        needs_restarted = true;
        sendCommand (QString ("quit"));
    }
}

KMPLAYER_NO_EXPORT void MPlayer::processExited (UnixProcess *) {
    unix_process.closeStdOut ();
    commands.clear ();
    if (needs_restarted) {
        int pos = m_position;
        play (m_url);
        seek (pos, true);
        if (Paused == m_state)
            pause ();
    } else if (m_state > Ready && !request_quit) {
        setState (Ready);
    }
}

KMPLAYER_NO_EXPORT void MPlayer::writeEvent (UnixProcess *) {
    QString data;
    if (command_chunk.isEmpty ()) {
        if (commands.size ()) {
            data = *commands.begin ();
            commands.pop_front ();
        }
    } else
        data = command_chunk;
    debugLog () << "eval " << data /*<< endl*/;
    gsize nr = 0;
    if (!data.isEmpty ())
        nr = unix_process.writeStdIn (data.toUtf8 ());
    if (nr < data.length ()) {
        //debugLog () << "partial " << nr << " " << data.length() << endl;
        command_chunk = data.mid (nr);
    } else {
        command_chunk.clear ();
        if (!commands.size ())
            // FIXME: always remove and readd when new data from stdin arrives
            unix_process.pollStdIn (false);
    }
}

KMPLAYER_NO_EXPORT void MPlayer::readEvent (UnixProcess *) {
    int inbuf = strlen (outbuf);
    inbuf += unix_process.readStdOut (outbuf + inbuf, sizeof (outbuf) -inbuf-1);
    //debugLog() << "read " << nr << " chars had " << inbuf << endl;
    outbuf[inbuf] = 0;
    char *tok = outbuf;
    for (int i = 0; i < inbuf; i++) {
        if (outbuf[i] == '\r' && outbuf[i+1] != '\n') {
            outbuf[i] = 0;
            char * c = strstr (tok, "A:");
            if (!c)
                c = strstr (tok, "V:");
            if (c) {
                if (request_seek == NO_SEEK)
                    setPosition ((int)(10 * strtof (c + 2, 0L)));
                request_seek = NO_SEEK;
                if (Playing == m_transition_state)
                    m_transition_state = NotRunning;
                setState (Playing);
            } else {
                c = strstr (tok, "Cache fill:");
                if (c) {
                    char *p = strchr (tok + 11, '%');
                    if (p)
                        *p = 0;
                    //debugLog() << "c " << (c + 11) << endl;
                    m_notify->setLoading ((int)strtof (c + 11, 0L));
                }
                if (m_state == Paused) {
                    if (Playing == m_transition_state)
                        m_transition_state = NotRunning;
                    setState (Playing);
                }
            }
            tok = outbuf + i + 1;
        } else if (outbuf[i] == '\n') {
            bool handled = false;
            outbuf[i] = 0;
            if (strncmp (tok, "dsp_thread", 10))
                debugLog() << "> " << tok << endl;
            if (m_state == Buffering) {
                char * p = strstr (tok, "Start");
                if (p && strstr (p, "play")) { // find a 'Start.*play' pattern
                    m_notify->setLoading (99);
                    handled = true;
                    if (hasVideo ())
                        m_notify->videoDetected ();
                } else {
                    char * c = strstr (tok, "Cache fill:");
                    if (c) {
                        char *p = strchr (tok + 11, '%');
                        if (p)
                            *p = 0;
                        //debugLog() << "c2 " << (c + 11) << endl;
                        m_notify->setLoading ((int)strtof (c + 11, 0L));
                        handled = true;
                    } else {
                        char * v = strstr (tok, "Video: no video");
                        if (v) {
                            setHasVideo (false);
                            handled = true;
                        }
                    }
                }
            }
            if (!handled) {
                if (!strncmp (tok, "ID_", 3)) {
                    handled = true;
                    if (!strncmp (tok + 3, "LENGTH", 6)) {
                        m_length = (int)(10 * strtof (tok + 10, 0L));
                        m_notify->setLength (m_length);
                    } else if (!strncmp (tok + 3, "PAUSED", 6)) {
                        if (Paused == m_transition_state) {
                            m_transition_state = NotRunning;
                            setState (Paused);
                        } // else ignore, a second 'pause' cmd should follow
                    } else if (!strncmp (tok + 3, "VIDEO_WIDTH", 11)) {
                        m_width = (int) strtof (tok + 15, 0L);
                        if (!panzoom_enabled)
                            stream_width = m_width;
                        updateAspects ();
                    } else if (!strncmp (tok + 3, "VIDEO_HEIGHT", 12)) {
                        m_height = (int) strtof (tok + 16, 0L);
                        if (!panzoom_enabled)
                            stream_height = m_height;
                        updateAspects ();
                    } else if (!strncmp (tok + 3, "VIDEO_ASPECT", 12)) {
                        if (!panzoom_enabled) {
                            // convert in C locale
                            m_aspect = (float) QString (tok + 16).toDouble ();
                            if (m_aspect > 0.01)
                                updateAspects ();
                        }
                    } else if (!strncmp (tok + 3, "REF", 3) && m_mrl_notify) {
                        m_mrl_notify->referenceMrl(QUrl::fromPercentEncoding(tok + 7));
                    }
                } else {
                    char *icy = strstr (tok, "ICY Info");
                    if (icy) {
                        //debugLog () << "icy found" << endl;
                        char *t = strstr (icy + 8, "StreamTitle=");
                        if (t) {
                            char *e = strchr (t + 12, ';');
                            if (e)
                                *e = 0;
                            if (m_mrl_notify)
                                m_mrl_notify->infoMessage (QString (t + 12));
                        }
                        handled = true;
                    } else {
                        // Crop: 320 x 200, 0 ; 20
                        char *crop = strstr (tok, "Crop:");
                        if (crop) {
                            char *by = strchr (tok + 5, 'x');
                            if (by) {
                                char *endptr = NULL;
                                int w = strtol (tok + 5, &endptr, 10);
                                if (w > 0 && endptr != tok + 5) {
                                    int h = strtol (by + 1, &endptr, 10);
                                    if (h > 0 && endptr != by + 1) {
                                        m_width = w;
                                        m_height = h;
                                        updateAspects ();
                                        handled = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            tok = outbuf + i + 1;
        }
    }
    if (tok == outbuf && inbuf == sizeof (outbuf) - 1) {
        inbuf = 0; //skip this line
    } else {
        inbuf -= tok - outbuf;
        if (inbuf > 0)
            memmove (outbuf, tok, inbuf);
    }
    outbuf[inbuf] = 0;
}

int MPlayer::getPid () const
{
    return unix_process.process_id;
}

bool MPlayer::removeQueued (const char *cmd)
{
    for (StringList::iterator i = commands.begin (); i != commands.end (); ++i)
        if (i->startsWith (cmd)) {
            commands.erase (i);
            return true;
        }
    return false;
}

KMPLAYER_NO_EXPORT bool MPlayer::sendCommand (const QString & cmd) {
    if (playing ()) {
        commands.push_back (cmd + QChar ('\n'));
        unix_process.pollStdIn (true);
        return true;
    }
    return false;
}

MPlayer *GstreamerMPlayerProcess::createGstreamerPlayer (ProcessNotify *notify)
{
    MPlayer *gst = new MPlayer (notify, "osso-media-server");
#ifdef _HARMATTAN
    gst->setPlayer (OPTDIR "/" _HARMATTAN_KMPLAYER "/bin/kgstplayer");
#else
    gst->setPlayer (OPTDIR "/kmplayer/bin/kgstplayer");
#endif
    gst->setAudioDriver ("pulse");
    gst->setVideoDriver ("xv");
    gst->setAutoSubTitles (true);
    gst->setSupportsPanzoom (true);
    return gst;
}

MPlayer *GstreamerMPlayerProcess::createMPlayer (ProcessNotify *notify)
{
    MPlayer *mp = new MPlayer (notify, "mplayer");
    mp->setAudioDriver ("pulse");
    mp->setVideoDriver ("xv:ck-method=bg:ck=set -colorkey 2114");
    return mp;
}

GstreamerMPlayerProcess::GstreamerMPlayerProcess (ProcessNotify *ctrl)
 : Process (ctrl, "gstreamer or mplayer", true),
   gstreamer (createGstreamerPlayer (this)),
   mplayer (createMPlayer (this)),
   process (gstreamer),
   m_transition_state (NotRunning)
{}

GstreamerMPlayerProcess::~GstreamerMPlayerProcess ()
{
    delete gstreamer;
    delete mplayer;
}

bool GstreamerMPlayerProcess::ready (unsigned int xid)
{
    process = gstreamer;
    m_viewer = xid;
    return process->ready (xid);
}

bool GstreamerMPlayerProcess::play (const QString &url)
{
    Process::play (url);
    if (m_transition_state < Playing) {
        m_transition_state = Playing;
        process = gstreamer;
        process->setMrlBackEndNotify (this);
        process->setRepeat (m_repeat);
        process->setHasVideo (!no_video);
    }
    return process->play (url);
}

bool GstreamerMPlayerProcess::stop ()
{
    m_transition_state = Ready;
    return process->stop ();
}

bool GstreamerMPlayerProcess::quit ()
{
    m_transition_state = NotRunning;
    return process->quit ();
}

bool GstreamerMPlayerProcess::pause ()
{
    return process->pause ();
}

bool GstreamerMPlayerProcess::seek (int pos, bool absolute)
{
    return process->seek (pos, absolute);
}

bool GstreamerMPlayerProcess::volume (int pos, bool absolute)
{
    return process->volume (pos, absolute);
}

void GstreamerMPlayerProcess::toggleScale ()
{
    return process->toggleScale ();
}

void GstreamerMPlayerProcess::stateChanged (Process *p,
        Process::State os, Process::State ns)
{
    if (p != process)
        return;

    if (ns == Process::Playing) {
        m_transition_state = NotRunning;
        is_local_file = process->isLocalFile ();
    } else if (ns == Process::Ready) {
        if (Playing == m_transition_state &&
                os > Process::Ready &&
                process == gstreamer) {
            process->quit ();
            process = mplayer;
            process->ready (m_viewer);
            process->setMrlBackEndNotify (this);
            process->setRepeat (m_repeat);
            process->setHasVideo (!no_video);
            process->play (m_url);
            return;
        }
    } else if (ns == Process::Buffering) {
        is_local_file = process->isLocalFile ();
    }
    m_state = ns;

    processNotify ()->stateChanged (this, os, ns);
}

void GstreamerMPlayerProcess::errorMsg (const QString & msg)
{
    processNotify ()->errorMsg (msg);
}

void GstreamerMPlayerProcess::setLoading (int perc)
{
    processNotify ()->setLoading (perc);
}

void GstreamerMPlayerProcess::setPosition (int pos)
{
    m_position = pos;
    processNotify ()->setPosition (pos);
}

void GstreamerMPlayerProcess::setLength (int len)
{
    m_length = len;
    processNotify ()->setLength (len);
}

void GstreamerMPlayerProcess::setAspect (float aspect)
{
    m_aspect = aspect;
    processNotify ()->setAspect (aspect);
}

void GstreamerMPlayerProcess::videoDetected ()
{
    no_video = false;
    processNotify ()->videoDetected ();
}

void GstreamerMPlayerProcess::createDownload (const QString &u, const QString &t)
{
    processNotify ()->createDownload (u, t);
}

Mrl *GstreamerMPlayerProcess::sourceMrl ()
{
    return m_mrl_notify ? m_mrl_notify->sourceMrl () : NULL;
}

void GstreamerMPlayerProcess::referenceMrl (const QString &url)
{
    if (m_mrl_notify)
        m_mrl_notify->referenceMrl (url);
}

void GstreamerMPlayerProcess::infoMessage (const QString &msg)
{
    if (m_mrl_notify)
        m_mrl_notify->infoMessage (msg);
}

void GstreamerMPlayerProcess::readyToPlay (Process *)
{
    if (m_mrl_notify)
        m_mrl_notify->readyToPlay (this);
}


#if 0
//def __ARMEL__

KDE_NO_CDTOR_EXPORT NpPlayer::NpPlayer (ProcessNotify * ctrl)
 : MPlayer (ctrl, "npp")
{}

bool NpPlayer::isFlashMimeType (const QString &mime)
{
    return mime == "application/x-shockwave-flash" ||
        mime == "application/futuresplash";
}

static gboolean copyParams (gpointer key, gpointer value, gpointer data) {
    QString *buf = (QString *) data;

    QString str;
    str += (char *)key;
    str += QChar ('=');
    str += (char *)value;

    g_free ((char *)key);
    g_free ((char *)value);

    char *quoted = g_shell_quote ((const char *) str);
    *buf += QChar (' ');
    *buf += quoted;
    g_free (quoted);

    return false; // continue
}

KMPLAYER_NO_EXPORT bool NpPlayer::play (const QString &uri) {
    if (playing ())
        return pause ();

    setState (Buffering);

    Process::play (uri);
    debugLog () << "NpPlayer::play " << mrl()->mimetype << endl;

    if (m_url.isEmpty ()) {
        setState (Ready);
        return false;
    }

    request_quit = false;
    outbuf[0] = 0;
    no_video = false;

    QString cmdline (path);
    char buf[32];
    snprintf (buf, sizeof(buf), " -wid %u ", m_viewer);
    cmdline += QString (buf);
    cmdline += " -m application/x-shockwave-flash"
        " -p /usr/lib/browser/plugins/libflashplayer.so ";
    char *quoted = g_shell_quote ((const char *) m_url);
    cmdline += quoted;
    g_free (quoted);

    GTree *params = g_tree_new ((GCompareFunc)::strcmp);
    Mrl *mrl = this->mrl ();
    if (mrl->id == id_node_html_object) {
        for (Node *n = mrl->firstChild (); n; n = n->nextSibling ()) {
            /*if (n->id == KMPlayer::id_node_param) {
                KMPlayer::Element *e = static_cast <KMPlayer::Element *> (n);
                QString name = e->getAttribute (KMPlayer::Ids::attr_name);
                if (!g_tree_lookup (params, (const char *) name))
                    g_tree_insert (params,
                            strdup ((const char *) name),
                            strdup ((const char *) e->getAttribute (KMPlayer::Ids::attr_value)));
            } else*/ if (n->id == KMPlayer::id_node_html_embed) {
                KMPlayer::Element *e = static_cast <KMPlayer::Element *> (n);
                Attribute *a = e->attributes ().first ();
                for (; a; a = a->nextSibling ()) {
                    QString nm = a->name().toString();
                    if (!g_tree_lookup (params, (const char *) nm))
                        g_tree_insert (params,
                                strdup ((const char *) nm),
                                strdup ((const char *) a->value ()));
                }
            }
        }
    } else if (NpPlayer::isFlashMimeType (mrl->mimetype)) {
        g_tree_insert (params, strdup ("SRC"), strdup (m_url));
    } else {
        QString flashvars = QString ("file=") + URL::encode_string (m_url);
        m_url = "http://video.linuxfoundation.org/sites/all/modules/custom/os_video_player/mediaplayer.swf";
        g_tree_insert (params, strdup ("SRC"), strdup (m_url));
        g_tree_insert (params, strdup ("flashvars"), strdup (flashvars));
    }
    int argc = g_tree_nnodes (params);
    if (argc) {
        QString str;
        g_tree_traverse (params, copyParams, G_IN_ORDER, &str);
        cmdline += " -args";
        cmdline += str;
    }
    g_tree_destroy (params);

    QString wd (getenv ("HOME"));
    QUrl url (m_url);
    is_local_file = Url::isLocalFile (url);
    if (is_local_file) {
        m_url = url.path ();
        wd = url.path ();
        int pos = wd.lastIndexOf (QString ("/"));
        if (pos > -1)
            wd.truncate (pos);
    }

    if (unix_process.start (wd, cmdline)) {
        setState (Process::Buffering); //FIXME
        return true;
    }
    setState (Ready);
    return false;
}

#endif //__ARMEL__

Downloader::Downloader (ProcessNotify *pn)
 : Process (pn, "downloader", false)
{}

Downloader::~Downloader ()
{}

bool KMPLAYER_NO_EXPORT Downloader::play (const QString &url) {
    Process::play (url);
    if (!m_url.isEmpty ()) {
        QString title;
        PlaylistRole *title_role = (PlaylistRole*) mrl ()->role (RolePlaylist);
        if (title_role)
            title = title_role->caption ();
        processNotify ()->createDownload (m_url, title);
    }
    setState (NotRunning);
    return false;
}

KMPLAYER_NO_EXPORT bool Downloader::stop () {
    return true;
}
