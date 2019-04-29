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

#ifndef _KMPLAYERPROCESS_H_
#define _KMPLAYERPROCESS_H_

#include <list>

#ifdef _HARMATTAN
#include <QStringList>
#endif

#include "playlist.h"

typedef struct _GIOChannel GIOChannel;

namespace KMPlayer {

class ProcessNotify;
class UnixProcess;

/*
 * Base class for all backend processes
 */
class KMPLAYER_NO_EXPORT Process {
public:
    enum State {
        NotRunning = 0, Ready, Buffering, Playing, Paused
    };
    Process (ProcessNotify * ctrl, const QString& n, bool xv);
    virtual ~Process ();
    //virtual QString menuName () const;
    //virtual void setAudioLang (int, const QString &);
    //virtual void setSubtitle (int, const QString &);
    bool playing () const;
    unsigned int viewer () const;
    //virtual bool grabPicture (const KURL & url, int pos);
    //bool supports (const char * source) const;
    State state () const { return m_state; }
    Mrl *mrl () { return m_mrl_notify ? m_mrl_notify->sourceMrl () : NULL; }
public:// slots:
    virtual bool ready (unsigned int);
    virtual bool play (const QString &url);
#ifdef _HARMATTAN
    virtual bool play(const QString &url, const QString &a);
#endif
    virtual bool stop ();
    virtual bool quit ();
    virtual bool pause ();
    /* seek (pos, abs) seek positon in deci-seconds */
    virtual bool seek (int pos, bool absolute);
    /* volume from 0 to 100 */
    virtual bool volume (int pos, bool absolute);
    /* saturation/hue/contrast/brightness from -100 to 100 */
    //virtual bool saturation (int pos, bool absolute);
    //virtual bool hue (int pos, bool absolute);
    //virtual bool contrast (int pos, bool absolute);
    //virtual bool brightness (int pos, bool absolute);
    virtual void toggleScale ();
    virtual int getPid () const;
    KMPLAYER_NO_EXPORT const QString& name () { return m_name; }
    void scheduledStateChanged ();
    ProcessNotify * processNotify () { return m_notify; }
    MrlBackEndNotify *mrlBackEndNotify () const { return m_mrl_notify; }
    bool setMrlBackEndNotify (MrlBackEndNotify *mrl_notify);
    int userStopPosition () const { return m_user_stop_position; }
    void setUserStop ();
    void setState (State newstate);
    void setWidth (int w) { m_width = w; }
    void setHeight (int h) { m_height = h; }
    void setRepeat (int r) { m_repeat = r; }
    void updateAspects ();
    bool hasVideo () const { return playing () && !no_video; }
    void setHasVideo (bool b) { no_video = !b; }
    bool hasXv () const { return has_xv; }
    void setUseXv (bool b) { has_xv = b; }
    bool isLocalFile () const { return is_local_file; }
    QString url () const { return m_url; }
    int length () const { return m_length; }
    int position () const { return m_position; }

#ifdef _HARMATTAN
		QString Audio() const { return sAudio; }
		void SetAudio(const QString &a) { sAudio = a; }
		void SetRequestHeaders(const QStringList &list) { tRequestHeaders = list; }
		QStringList RequestHeaders() const { return tRequestHeaders; }

protected:
		QStringList tRequestHeaders;
    QString sAudio;
#endif

protected:
    void setPosition (int p);
    //Source * m_source;
    //Settings * m_settings;
    State m_state;
    State m_old_state;
    QString m_url;
    ProcessNotify * m_notify;
    MrlBackEndNotify *m_mrl_notify;
    unsigned int m_viewer;
    const QString m_name;
    int start_timer;
    int m_length;
    int m_position;
    int m_width;
    int m_height;
    int m_repeat;
    int m_user_stop_position;
    float m_aspect;
    bool no_video;
    bool has_xv;
    bool is_local_file;
    //int m_request_seek;
    //const char ** m_supported_sources;
};

/*
 * Listener for Process events
 */
class KMPLAYER_NO_EXPORT ProcessNotify {
public:
    virtual void stateChanged (Process * proc, Process::State os, Process::State ns)=0;
    virtual void errorMsg (const QString & msg) = 0;
    virtual void setLoading (int perc) = 0;
    virtual void setPosition (int pos) = 0;
    virtual void setLength (int len) = 0;
    virtual void setAspect (float aspect) = 0;
    virtual void videoDetected () = 0;
    virtual void createDownload (const QString &url, const QString &title) = 0;
};

class KMPLAYER_NO_EXPORT UnixProcessListener {
public:
    virtual void readEvent (UnixProcess *) = 0;
    virtual void writeEvent (UnixProcess *) = 0;
    virtual void processExited (UnixProcess *) = 0;
};

class KMPLAYER_NO_EXPORT UnixProcess {
public:
    enum { NoIO = 0, StdIn = 0x01, StdOut = 0x02 };

    UnixProcess (UnixProcessListener *l, int flags);

    bool start (const QString &pwd, const QString &cmdline);
    void stop ();
    bool running () const;
    void pollStdIn (bool b);
    void closeStdIn ();
    void closeStdOut ();
    int writeStdIn (const QByteArray& data);
    int readStdOut (char *buf, int len);

    void readEvent (GIOChannel *channel);
    void writeEvent ();
    void processExited (int pid);

    UnixProcessListener *listener;
    int io_flags;
    GIOChannel *pin, *pout, *perr;
    int win, wout, werr;
    int process_id;
    bool eof;
};

/*
 * MM backend using MPlayer
 */
class KMPLAYER_NO_EXPORT MPlayer : public Process, public UnixProcessListener {
public:
    MPlayer (ProcessNotify *ctrl, const QString& name);
    ~MPlayer ();
#ifdef _HARMATTAN
    virtual bool play(const QString &url, const QString &a);
#endif
    virtual bool play (const QString &url);
    virtual bool pause ();
    virtual bool ready (unsigned int);
    virtual bool stop ();
    virtual bool quit ();
    virtual bool seek (int pos, bool absolute);
    virtual bool volume (int pos, bool absolute);
    //virtual bool saturation (int pos, bool absolute);
    //virtual bool hue (int pos, bool absolute);
    //virtual bool contrast (int pos, bool absolute);
    //virtual bool brightness (int pos, bool absolute);
    virtual void toggleScale ();

    void setPlayer (const char *p) { path = p; }
    void setAudioDriver (const char *ao) { audio_driver = ao; }
    void setVideoDriver (const char *vo) { video_driver = vo; }
    void setAutoSubTitles (bool b) { auto_sub_titles = b; }
    void setSupportsPanzoom (bool b) { supports_panzoom = b; }

    // for UnixProcessListener functions
    void processExited (UnixProcess *);
    void writeEvent (UnixProcess *);
    void readEvent (UnixProcess *);
    int getPid () const;

protected:
    bool sendCommand (const QString &);
    bool removeQueued (const char *cmd);
    bool calculateScale (int &x, int &y, int &w, int &h);
    typedef std::list <QString> StringList;
    StringList commands;
    QString command_chunk;
    UnixProcess unix_process;
    int request_seek;
    int stream_width;
    int stream_height;
    const char *path;
    const char *audio_driver;
    const char *video_driver;
    State m_transition_state;
    bool request_quit;
    bool auto_sub_titles;
    bool supports_panzoom;
    bool panzoom_enabled;
    bool needs_restarted;
    char outbuf[512];
};

class KMPLAYER_NO_EXPORT GstreamerMPlayerProcess
    : public Process, public ProcessNotify, public MrlBackEndNotify {
public:
    GstreamerMPlayerProcess (ProcessNotify *ctrl);
    virtual ~GstreamerMPlayerProcess ();

    static MPlayer *createGstreamerPlayer (ProcessNotify *notify);
    static MPlayer *createMPlayer (ProcessNotify *notify);

    virtual bool ready (unsigned int);
    virtual bool play (const QString &url);
    virtual bool stop ();
    virtual bool quit ();
    virtual bool pause ();
    virtual bool seek (int pos, bool absolute);
    virtual bool volume (int pos, bool absolute);
    virtual void toggleScale ();

    virtual void stateChanged (Process *, Process::State os, Process::State ns);
    virtual void errorMsg (const QString & msg);
    virtual void setLoading (int perc);
    virtual void setLength (int len);
    virtual void setAspect (float aspect);
    virtual void videoDetected ();
    virtual void createDownload (const QString &url, const QString &title);

    virtual Mrl *sourceMrl ();
    virtual void referenceMrl (const QString &url);
    virtual void infoMessage (const QString &msg);
    virtual void readyToPlay (Process *process);

protected:
    virtual void setPosition (int pos);

private:
    MPlayer *gstreamer;
    MPlayer *mplayer;
    Process *process;
    State m_transition_state;
};

#if 0
def __ARMEL__

class KMPLAYER_NO_EXPORT NpPlayer : public MPlayer {
public:
    NpPlayer (ProcessNotify * ctrl);

    virtual bool play (const QString &url);

    static bool isFlashMimeType (const QString &mime);
};

#endif //__ARMEL__

/*
 * Download backend
 */

class KMPLAYER_NO_EXPORT Downloader : public Process
{
public:
    Downloader (ProcessNotify *ctrl);
    ~Downloader ();

    virtual bool play (const QString &url);
    virtual bool stop ();
};

} // namespace

#endif //_KMPLAYERPROCESS_H_
