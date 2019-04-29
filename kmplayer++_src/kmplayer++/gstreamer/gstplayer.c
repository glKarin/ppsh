/* This file is part of the KMPlayer application
   Copyright (C) 2004 Koos Vriezen <koos.vriezen@xs4all.nl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/interfaces/colorbalance.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

typedef struct _Dimension {
    int width;
    int height;
} Dimension;

enum State {
    StateStart = 0, StatePlaying, StateBufferPaused, StateStop, StateTerminate
};

static Display             *display;
static Window               wid;
static gboolean             window_created = TRUE;
static gboolean             verbose;
static int                  movie_width;
static int                  movie_height;
static int                  movie_length;
static int                  crop_x;
static int                  crop_y;
static int                  crop_width;
static int                  crop_height;
static int                  screen;
static gchar               *mrl;
static gchar               *sub_mrl;
static const char          *ao_driver;
static const char          *vo_driver;
static const char          *playbin_name = "player";
static const char          *dvd_device;
static const char          *vcd_device;
static GstElement          *gst_elm_play;
static GstElement          *videosink;
static GstElement          *videocrop;
static GstBus              *gst_bus;
static unsigned int /*GstMessageType*/       ignore_messages_mask;
static GstXOverlay         *xoverlay;
static GstColorBalance     *color_balance;
static gulong               gst_bus_sync;
static gulong               gst_bus_async;
static GMainLoop           *event_loop;
static guint                position_tag;
static int                  seek_start;
static gboolean             has_video_codec;
static enum State           current_state;
static enum State           transition_state;

static gboolean cb_play (gpointer data);
static gboolean cb_position (gpointer data);
static gboolean cb_stop (gpointer data);
static gboolean cb_error (gpointer data);
static gboolean cb_finished (gpointer data);
static gboolean cb_video_dimension (gpointer data);
static gboolean cb_length (gpointer data);

static void transitionFinished ()
{
    current_state = transition_state;
    transition_state = StateStart;
}

static void output (const char *format, ...) {
    static gboolean need_cr;
    va_list vl;
    if (*format == '\r') {
        need_cr = TRUE;
    } else if (need_cr) {
        printf ("\r");
        need_cr = FALSE;
    }
    va_start (vl, format);
    vprintf (format, vl);
    va_end (vl);
    fflush (stdout);
}

/*static void
cb_error (GstElement * play,
          GstElement * src,
          GError     *err,
          const char *debug,
          gpointer   data)
{
    fprintf (stderr, "cb_error: %s %s\n", err->message, debug);
    if (GST_STATE (play) == GST_STATE_PLAYING)
        gst_element_set_state (play, GST_STATE_READY);
//  QApplication::postEvent (gstapp, new QEvent ((QEvent::Type)event_finished));
}*/

/* NULL -> READY -> PAUSED -> PLAYING */

static void gstCapsSet (GstPad *pad, GParamSpec *pspec, gpointer data) {
    GstCaps *caps = gst_pad_get_negotiated_caps (pad);
    const GstStructure *structure;
    (void) pspec;
    (void ) data;

    if (!caps)
        return;
  /*QApplication::postEvent (gstapp, new QEvent ((QEvent::Type) event_video));*/
    structure = gst_caps_get_structure (caps, 0);
    if (structure) {
        const GValue *par;
        Dimension *dim;

        gst_structure_get_int (structure, "width", &movie_width);
        gst_structure_get_int (structure, "height", &movie_height);
        if ((par = gst_structure_get_value (structure, "pixel-aspect-ratio"))) {
            int num = gst_value_get_fraction_numerator (par),
                den = gst_value_get_fraction_denominator (par);

            if (num > den)
                movie_width = (int) ((float) num * movie_width / den);
            else
                movie_height = (int) ((float) den * movie_height / num);
        }
        dim = (Dimension *) g_malloc (sizeof (Dimension));
        dim->width = movie_width;
        dim->height = movie_height;
        g_idle_add (cb_video_dimension, (void *) dim);
    }
    gst_caps_unref (caps);
}

static void gstStreamInfo (GObject *obj, GParamSpec *spec, gpointer data) {
    GstPad *videopad = 0L;
    GList *streaminfo = 0L;
    GstMessage *msg;
    (void) obj;
    (void) spec;
    (void ) data;

    fprintf (stderr, "gstStreamInfo\n");
    g_object_get (gst_elm_play, "stream-info", &streaminfo, NULL);
    streaminfo = g_list_copy (streaminfo);
    g_list_foreach (streaminfo, (GFunc) g_object_ref, NULL);
    for ( ; streaminfo != NULL; streaminfo = streaminfo->next) {
        GObject *info = G_OBJECT (streaminfo->data);
        gint type;
        GParamSpec *pspec;
        GEnumValue *val;

        if (!info)
            continue;
        g_object_get (info, "type", &type, NULL);
        pspec = g_object_class_find_property (G_OBJECT_GET_CLASS(info), "type");
        val = g_enum_get_value (G_PARAM_SPEC_ENUM (pspec)->enum_class, type);

        if (!g_strcasecmp (val->value_nick, "video")) {
            if (!videopad) {
                g_object_get (info, "object", &videopad, NULL);
                gstCapsSet (GST_PAD (videopad), 0L, 0L);
                g_signal_connect (videopad, "notify::caps", G_CALLBACK (gstCapsSet), 0L);
            }
        } else
            fprintf (stderr, "gstStreamInfo %s\n", val->value_nick);
    }

    msg = gst_message_new_application (GST_OBJECT (gst_elm_play),
            gst_structure_new ("notify-streaminfo", NULL));
    gst_element_post_message (gst_elm_play, msg);
    g_list_foreach (streaminfo, (GFunc) g_object_unref, NULL);
    g_list_free (streaminfo);
}

static void gstSource (GObject *obj, GParamSpec *spec, gpointer data) {
    GObject *source = 0L;
    GObjectClass *klass;
    (void) obj;
    (void) spec;
    (void ) data;
    fprintf (stderr, "gstSource\n");
    g_object_get (gst_elm_play, "source", &source, NULL);
    if (!source)
        return;
    klass = G_OBJECT_GET_CLASS (source);
    if (!strncmp (mrl, "dvd:/", 5) && dvd_device) {
        if (g_object_class_find_property (klass, "device"))
            g_object_set (source, "device", dvd_device, NULL);
    } else if (!strncmp (mrl, "vcd:/", 5) && vcd_device) {
        if (g_object_class_find_property (klass, "device"))
            g_object_set (source, "device", vcd_device, NULL);
    }
    g_object_unref (source);
}

static void gstGetDuration () {
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 len = -1; /* usec*/
    if (gst_element_query_duration (gst_elm_play, &fmt, &len))
        if (movie_length != len / (GST_MSECOND * 100)) {
            movie_length = len / (GST_MSECOND * 100);
            g_idle_add (cb_length, (void *) (long) movie_length);
        }
}

static void seek (int val /*offset_in_deciseconds*/, int abs) {
    if (gst_elm_play)
        gst_element_seek (gst_elm_play, 1.0, GST_FORMAT_TIME,
                (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                abs ? GST_SEEK_TYPE_SET : GST_SEEK_TYPE_CUR,
                val * GST_MSECOND * 100,
                GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

static void bufferingMessage (guint percentage) {
    if (current_state < StateStop) {
        GstState state;
        gst_element_get_state (gst_elm_play, &state, NULL, 0);
        if (StateBufferPaused != transition_state &&
                GST_STATE_PLAYING == state &&
                percentage < 40) {
            transition_state = StateBufferPaused;
            gst_element_set_state (gst_elm_play, GST_STATE_PAUSED);
            state = GST_STATE_PAUSED;
        }
        if (percentage >= 100 &&
                StateBufferPaused == current_state &&
                transition_state != StatePlaying) {
            transition_state = StatePlaying;
            gst_element_set_state (gst_elm_play, GST_STATE_PLAYING);
        }
        if (state != GST_STATE_PLAYING)
            output ("\rCache fill: %d%", percentage);
    }
}

static void stateChangeMessage (GstObject *src, GstState os,  GstState ns)
{
    if (GST_IS_ELEMENT (src)) {
        GstElement *elm = GST_ELEMENT (src);

        if (elm == gst_elm_play) {
            if (os == GST_STATE_PAUSED && ns >= GST_STATE_PLAYING) {
                if (StatePlaying == transition_state) {
                    if (StateStart == current_state) {
                        /*gstGetDuration ();*/
                        if (!has_video_codec)
                            output ("Video: no video\n");
                        output ("\rA:0.0\rA:0.0");
                    } else {
                        output ("\rCache fill: 100%");
                    }
                    transitionFinished ();
                }
                if (!position_tag)
                    position_tag = g_timeout_add (500, cb_position, NULL);
            } else if (os >= GST_STATE_PAUSED && ns <= GST_STATE_READY) {
                g_idle_add (cb_stop, NULL);
            } else if (ns == GST_STATE_PAUSED) {
                if (os > GST_STATE_PAUSED) {
                    if (StateBufferPaused == transition_state)
                        transitionFinished ();
                    else
                        output ("ID_PAUSED\n");
                } else if (StatePlaying == transition_state) {
                    if (StateStart == current_state)
                        gstGetDuration ();
                    if (seek_start > 0) {
                        seek (seek_start * 10, 1);
                        seek_start = 0;
                    }
                    gst_element_set_state (gst_elm_play, GST_STATE_PLAYING);
                }
            }
        } else if (os == GST_STATE_PAUSED &&
                ns >= GST_STATE_PLAYING &&
                StateStart == current_state) {
            gchar *name = gst_object_get_name (src);
            fprintf (stderr, "%s changed to PLAYING\n", name);
            if (!has_video_codec && !strcmp (name, "videosink")) {
                GstPad *pad;
                has_video_codec = TRUE;
                pad = gst_element_get_static_pad (elm, "sink");
                if (!pad)
                    pad = gst_element_get_request_pad (elm, "sink");
                if (pad) {
                    gstCapsSet (pad, NULL, NULL);
                    g_object_unref (G_OBJECT (pad));
                }
            }
            g_free (name);
        }
    }
}

/*static void
cb_expose (GtkWidget * w, GdkEventExpose * ev, GstElement * e)
{
    if (GST_IS_X_OVERLAY (e) &&
            !GTK_WIDGET_NO_WINDOW (w) && GTK_WIDGET_REALIZED (w)) {
        gst_x_overlay_set_xwindow_id (GST_X_OVERLAY (e),
                GDK_WINDOW_XWINDOW (w->window));
    }
}*/

static void gstTag (const GstTagList *lst, const gchar *tag, gpointer data) {
    (void) data;
    if (gst_tag_get_type (tag) == G_TYPE_STRING) {
        gchar *val = NULL;
        gst_tag_list_get_string (lst, tag, &val);
        if (!strcmp (tag, "title")) {
            output ("ICY Info: StreamTitle='%s';\n", val);
        } else if (!strcmp (tag, "audio-codec")) {
            output ("ID_AUDIO_CODEC=%s;\n", val);
        } else if (!strcmp (tag, "video-codec")) {
            output ("ID_VIDEO_CODEC=%s\n", val);
        } else {
            fprintf (stderr, "Tag: %s=%s\n", tag, val);
        }
        g_free (val);
    } else {
        fprintf (stderr, "Tag: %s\n", tag);
    }
}

/*static gboolean gstStructure (GQuark field_id, const GValue *value, gpointer user_data);*/

static void gstBusMessage (GstBus *bus, GstMessage *message, gpointer data) {
    GstMessageType msg_type = GST_MESSAGE_TYPE (message);
    (void) bus;
    (void) data;
    /* somebody else is handling the message, probably in gstPolForStateChange*/
    if (ignore_messages_mask & msg_type)
        return;
    switch (msg_type) {
        case GST_MESSAGE_ERROR:
            fprintf (stderr, "error msg\n");
            if (gst_elm_play) {
                gst_element_set_state (gst_elm_play, GST_STATE_NULL);
                /*gstPollForStateChange (gst_elm_play, GST_STATE_NULL);*/
            }
            g_idle_add (cb_error, NULL);
            break;
        case GST_MESSAGE_WARNING:
            fprintf (stderr, "warning msg\n");
            break;
        case GST_MESSAGE_TAG: {
            GstTagList *tag_list;
            gst_message_parse_tag (message, &tag_list);
            gst_tag_list_foreach (tag_list, gstTag, 0L);
            gst_tag_list_free (tag_list);
            break;
        }
        case GST_MESSAGE_EOS:
            fprintf (stderr, "eos msg\n");
            g_idle_add (cb_stop, NULL);
            break;
        case GST_MESSAGE_BUFFERING: {
            gint percent = 0;
            gst_structure_get_int (message->structure, "buffer-percent", &percent);
            bufferingMessage (percent);
            break;
        }
        case GST_MESSAGE_APPLICATION: {
            const char * msg = gst_structure_get_name (message->structure);
            fprintf (stderr, "app msg %s\n", msg ? msg : "<unknown>");
            /*gst_structure_foreach (message->structure, gstStructure, 0L);*/
            break;
        }
        case GST_MESSAGE_STATE_CHANGED: {
            GstState old_state, new_state;
            gst_message_parse_state_changed(message, &old_state, &new_state,0L);
            stateChangeMessage (message->src, old_state, new_state);
            break;
        }
        case GST_MESSAGE_DURATION:
            /*gstGetDuration ();
            break;*/
        case GST_MESSAGE_CLOCK_PROVIDE:
        case GST_MESSAGE_CLOCK_LOST:
        case GST_MESSAGE_NEW_CLOCK:
        case GST_MESSAGE_STATE_DIRTY:
        case GST_MESSAGE_STREAM_STATUS:
        case GST_MESSAGE_ASYNC_DONE:
        case GST_MESSAGE_QOS:
             break;
        default:
             fprintf (stderr, "Unhandled msg %s (0x%x)\n",
                     gst_message_type_get_name (msg_type), msg_type);
             break;
    }
}

static void gstMessageElement (GstBus *bus, GstMessage *msg, gpointer data) {
    (void) bus;
    (void) data;
    if (gst_structure_has_name (msg->structure, "prepare-xwindow-id")) {
        fprintf (stderr, "prepare-xwindow-id\n");
        if (xoverlay)
            gst_x_overlay_set_xwindow_id (xoverlay, wid);
    }
}

static gboolean gstPollForStateChange (GstElement *element, GstState state, gint64 timeout) {
    /*GstMessageType*/ unsigned int events, saved_events;
    GstBus *bus = gst_element_get_bus (element);
    GError **error = 0L;

    events = GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS;
    saved_events = ignore_messages_mask;

    if (element && element == gst_elm_play) {
        /* we do want the main handler to process state changed messages for
         * playbin as well, otherwise it won't hook up the timeout etc. */
        ignore_messages_mask |= (events ^ GST_MESSAGE_STATE_CHANGED);
    } else {
        ignore_messages_mask |= events;
    }

    while (TRUE) {
        GstMessage *message;
        GstElement *src;

        message = gst_bus_poll (bus, (GstMessageType) events, timeout);
        if (!message)
            goto timed_out;

        src = (GstElement*)GST_MESSAGE_SRC (message);

        switch (GST_MESSAGE_TYPE (message)) {
            case GST_MESSAGE_STATE_CHANGED: {
                GstState olds, news, pending;
                if (src == element) {
                    gst_message_parse_state_changed (message, &olds, &news, &pending);
                    if (news == state) {
                        gst_message_unref (message);
                        goto success;
                    }
                }
                break;
            }
            case GST_MESSAGE_ERROR: {
                gchar *debug = NULL;
                GError *gsterror = NULL;
                gst_message_parse_error (message, &gsterror, &debug);
                fprintf (stderr, "Error: %s (%s)\n",
                        gsterror->message, debug ? debug : "-");
                gst_message_unref (message);
                g_error_free (gsterror);
                g_free (debug);
                goto error;
            }
            case GST_MESSAGE_EOS: {
                gst_message_unref (message);
                goto error;
            }
            default:
                g_assert_not_reached ();
                break;
        }
        gst_message_unref (message);
    }
    g_assert_not_reached ();

success:
    /* state change succeeded */
    fprintf (stderr, "state change to %s succeeded\n", gst_element_state_get_name (state));
    ignore_messages_mask = saved_events;
    return TRUE;

timed_out:
    /* it's taking a long time to open -- just tell totem it was ok, this allows
     * the user to stop the loading process with the normal stop button */
    fprintf (stderr, "state change to %s timed out, returning success and handling errors asynchroneously\n", gst_element_state_get_name (state));
    ignore_messages_mask = saved_events;
    return TRUE;

error:
    fprintf (stderr, "error while waiting for state change to %s: %s\n",
            gst_element_state_get_name (state),
            (error && *error) ? (*error)->message : "unknown");
    /* already set *error */
    ignore_messages_mask = saved_events;
    g_idle_add (cb_error, NULL);
    return FALSE;
}

/*----------------------------------------------------------------------------*/

/*
gboolean Backend::isPlaying () {
    gboolean b = gst_elm_play && (GST_STATE (gst_elm_play) == GST_STATE_PLAYING);
    return b;
}
*/

static void play () {
    GstElement *element;
    GstElement *audiosink = 0L;
    gboolean success;
    fprintf (stderr, "play %s\n", !mrl || !mrl[0] ? "<empty>" : mrl);
    if (gst_elm_play) {
        if (GST_STATE (gst_elm_play) == GST_STATE_PAUSED) {
            gst_element_set_state (gst_elm_play, GST_STATE_PLAYING);
            gstPollForStateChange (gst_elm_play, GST_STATE_PLAYING, GST_SECOND/2);
        }
        return;
    }
    if (!mrl || !mrl[0])
        return;
    movie_length = movie_width = movie_height = 0;
    gst_elm_play = gst_element_factory_make ("playbin2", playbin_name);
    if (!gst_elm_play) {
        fprintf (stderr, "couldn't create playbin\n");
        goto fail;
    }
    ignore_messages_mask = 0;
    gst_bus = gst_element_get_bus (gst_elm_play);

    gst_bus_add_signal_watch (gst_bus);

    gst_bus_async = g_signal_connect (gst_bus, "message",
                G_CALLBACK (gstBusMessage), 0L);
    if (ao_driver && !strcmp (ao_driver, "alsa"))
        audiosink = gst_element_factory_make ("alsasink", "audiosink");
    else if (ao_driver && !strcmp (ao_driver, "pulse"))
        audiosink = gst_element_factory_make ("pulsesink", "audiosink");
    else if (ao_driver && !strcmp (ao_driver, "esd"))
        audiosink = gst_element_factory_make ("esdsink", "audiosink");
    else
        audiosink = gst_element_factory_make ("osssink", "audiosink");
    if (!audiosink) {
        fprintf (stderr, "couldn't create audio sink for %s\n", ao_driver);
        goto fail;
    }
    if (vo_driver && !strncmp (vo_driver, "xv", 2)) {
        videosink = gst_element_factory_make ("xvimagesink", "videosink");
        if (videosink) {
            g_object_set (videosink, "autopaint-colorkey", 0, NULL);
            g_object_set (videosink, "colorkey", 0x080810, NULL);
        }
    } else {
        videosink = gst_element_factory_make ("ximagesink", "videosink");
    }
    if (!videosink)
        goto fail;

    GstElement *bin = gst_bin_new ("cropvideosink");
    videocrop = gst_element_factory_make ("videocrop", "videocrop");
    gst_bin_add_many (GST_BIN (bin), videocrop, videosink, NULL);
    gst_element_add_pad (bin, gst_ghost_pad_new ("sink",
                gst_element_get_pad (videocrop, "sink")));
    gst_element_link_many (videocrop, videosink, NULL);

    if (GST_IS_BIN (videosink))
        element = gst_bin_get_by_interface (GST_BIN (videosink),
                GST_TYPE_X_OVERLAY);
    else
        element = videosink;
    if (GST_IS_X_OVERLAY (element)) {
        xoverlay = GST_X_OVERLAY (element);
        gst_x_overlay_set_xwindow_id (xoverlay, wid);
    }
    /*g_signal_connect (content, "expose-event", G_CALLBACK (cb_expose), videosink );*/
    gst_element_set_bus (bin, gst_bus);
    gst_element_set_state (bin, GST_STATE_READY);
    success = gstPollForStateChange (bin, GST_STATE_READY, GST_SECOND/2);
    /*if (!success) {
        / * Drop this video sink * /
        gst_element_set_state (videosink, GST_STATE_NULL);
        gst_object_unref (videosink);*/
    if (audiosink) {
        gst_element_set_bus (audiosink, gst_bus);
        gst_element_set_state (audiosink, GST_STATE_READY);
        success = gstPollForStateChange (audiosink, GST_STATE_READY, GST_SECOND/2);
    }
    g_object_set (G_OBJECT (gst_elm_play),
            "video-sink",  bin, /*videosink,*/
            "audio-sink",  audiosink,
            NULL);
    gst_bus_set_sync_handler (gst_bus, gst_bus_sync_signal_handler, 0L);
    gst_bus_sync = g_signal_connect (gst_bus, "sync-message::element",
            G_CALLBACK (gstMessageElement), 0L);
    g_signal_connect (gst_elm_play, "notify::source",
            G_CALLBACK (gstSource), 0L);
    g_signal_connect (gst_elm_play, "notify::stream-info",
            G_CALLBACK (gstStreamInfo), 0L);
    if (GST_IS_COLOR_BALANCE (videosink))
        color_balance = GST_COLOR_BALANCE (videosink);

    if (GST_STATE (gst_elm_play) > GST_STATE_READY)
        gst_element_set_state (gst_elm_play, GST_STATE_READY);

    if (mrl[0] == '/') {
        gchar *tmp = mrl;
        mrl = g_strdup_printf ("file://%s", tmp);
        g_free (tmp);
    }
    g_object_set (gst_elm_play, "uri", mrl, NULL);
    if (sub_mrl && sub_mrl[0]) {
        if (sub_mrl[0] == '/') {
            gchar *tmp = sub_mrl;
            sub_mrl = g_strdup_printf ("file://%s", tmp);
            g_free (tmp);
        }
        g_object_set (gst_elm_play, "suburi", sub_mrl, NULL);
    }
    transition_state = StatePlaying;
    gst_element_set_state (gst_elm_play, GST_STATE_PAUSED);
    return;
fail:
    if (videosink) {
        gst_element_set_state (videosink, GST_STATE_NULL);
        gst_object_unref (videosink);
        videosink = NULL;
    }
    if (audiosink) {
        gst_element_set_state (audiosink, GST_STATE_NULL);
        gst_object_unref (audiosink);
    }
    g_idle_add (cb_finished, NULL);
}

static void pause () {
    if (gst_elm_play) {
        GstState playbin_state = GST_STATE (gst_elm_play);
        GstState state = playbin_state == GST_STATE_PLAYING ?
            GST_STATE_PAUSED : GST_STATE_PLAYING;

        if (position_tag) {
            g_source_remove (position_tag);
            position_tag = 0;
        }

        if (StateBufferPaused == current_state) {
            /*StateBufferPaused: kmplayer in playing state*/
            current_state = StatePlaying;
            if (StateStart == transition_state)
                /*done with setting to 'normal' pause state*/
                return;
            if (StatePlaying == transition_state) {
                /* pause a not yet signaled playing state */
                state = GST_STATE_PAUSED;
                transition_state = StateStart;
            }
        } else if (StateBufferPaused == transition_state) {
            /* also done in playing state, so we're pausing */
            transition_state = StateStart;
            return;
        } else if (StatePlaying == transition_state) {
            /* block auto transition to playing state */
            transition_state = StateStart;
            if (GST_STATE_PAUSED != playbin_state)
                /* not yet in paused, so we're done */
                return;
        }
        gst_element_set_state (gst_elm_play, state);
    }
}

static void stop () {
    if (current_state >= StateStop)
        return;
    current_state = StateStop;
    fprintf (stderr, "stop %s\n", !mrl || !mrl[0] ? "<empty>" : mrl);
    if (position_tag) {
        g_source_remove (position_tag);
        position_tag = 0;
    }
    if (gst_elm_play) {
        GstState current_state;
        GstElement *element;
        gst_element_get_state (gst_elm_play, &current_state, NULL, 0);
        if (current_state > GST_STATE_READY) {
            gst_element_set_state (gst_elm_play, GST_STATE_READY);
            gstPollForStateChange (gst_elm_play, GST_STATE_READY, -1);
        }
        gst_element_set_state (gst_elm_play, GST_STATE_NULL);
        gst_element_get_state (gst_elm_play, NULL, NULL, -1);
        g_object_get (gst_elm_play, "video-sink", &element, NULL);
        if (element) {
            gst_element_set_state (element, GST_STATE_NULL);
            gst_object_unref (element);
        }
        g_object_get (gst_elm_play, "audio-sink", &element, NULL);
        if (element) {
            gst_element_set_state (element, GST_STATE_NULL);
            gst_object_unref (element);
        }
    }
    g_idle_add (cb_finished, NULL);
}

/*static void finished () {
 //   QTimer::singleShot (10, this, SLOT (stop ()));
}

static void adjustColorSetting (const char * channel, int val) {
    //fprintf (stderr, "adjustColorSetting %s\n", channel);
    if (color_balance) {
        for (const GList *item =gst_color_balance_list_channels (color_balance);
                item != NULL; item = item->next) {
            GstColorBalanceChannel *chan = (GstColorBalanceChannel*) item->data;

            if (!strstr (chan->label, channel))
                gst_color_balance_set_value (color_balance, chan,
                        ((val + 100) * (chan->max_value - chan->min_value)/200 + chan->min_value));
        }
    }
}

static void saturation (int s) {
    adjustColorSetting ("SATURATION", s);
}

static void hue (int h) {
    adjustColorSetting ("HUE", h);
}

static void contrast (int c) {
    adjustColorSetting ("CONTRAST", c);
}

static void brightness (int b) {
    adjustColorSetting ("BRIGHTNESS", b);
} */

static void volume (int delta) {
    if (gst_elm_play) {
        gdouble vol;
        g_object_get (gst_elm_play, "volume", &vol, NULL);
        vol += delta / 100.0;
        if (vol >= 0.0 && vol <= 1.0)
            g_object_set (G_OBJECT (gst_elm_play), "volume", vol, NULL);
    }
}

static gboolean watchX11Prepare (GSource *source, gint *timeout) {
    (void) source;
    *timeout = -1;
    return XPending (display);
}

static gboolean watchX11Check (GSource *source) {
    (void) source;
    return XPending (display);
}

static gboolean
watchX11Events (GSource *source, GSourceFunc callback, gpointer data) {
    (void) source;
    (void) callback;
    (void) data;
    while (XPending (display)) {
        XEvent   xevent;
        XNextEvent(display, &xevent);
        switch (xevent.type) {
        case KeyPress: {
            XKeyEvent  kevent;
            KeySym     ksym;
            char       kbuf[256];
            int        len;
            kevent = xevent.xkey;
            len = XLookupString(&kevent, kbuf, sizeof(kbuf), &ksym, NULL);
            fprintf(stderr, "keypressed 0x%x 0x%d\n", kevent.keycode, ksym);
            switch (ksym) {
                case XK_q:
                case XK_Q:
                    g_idle_add (cb_stop, NULL);
                    break;
            }
            break;
        }
        case Expose:
            if (!xevent.xexpose.count && xevent.xexpose.window == wid) {
                if (gst_elm_play && videosink) {
                    /*GstElement *videosink;
                    g_object_get (gst_elm_play, "video-sink", &videosink, NULL);
                    if (videosink && GST_IS_X_OVERLAY (videosink)) {*/
                        gst_x_overlay_set_xwindow_id (GST_X_OVERLAY (videosink), wid);
                        gst_x_overlay_expose (GST_X_OVERLAY (videosink));
                        /*gst_object_unref (videosink);
                    }*/
                }
            }
            break;

        case ConfigureNotify:
            if (xoverlay && GST_IS_X_OVERLAY (xoverlay))
                gst_x_overlay_expose (xoverlay);
            break;
        default:
            ; /*if (xevent.type < LASTEvent)
              fprintf (stderr, "event %d\n", xevent.type);*/
        }
    }
    return TRUE;
}

static gboolean cb_play (gpointer data) {
    (void)data;
    play ();
    return FALSE;
}

static gboolean cb_position (gpointer data) {
    gboolean ret;
    gint64 val = 0; /* usec */

    if (StateBufferPaused == current_state ||
            StateBufferPaused == transition_state)
        return TRUE;

    if (!position_tag) {
        fprintf(stderr, "cb_position  tag:%u\n", position_tag);
        return FALSE;
    }
    if (gst_elm_play) {
        do {
            GstMessage * msg = gst_bus_poll (gst_bus, (GstMessageType) (GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION), GST_MSECOND * 10);
            if (!msg)
                break;
            gst_message_unref (msg);
        } while (gst_bus);

        if (gst_elm_play) {
            GstFormat fmt = GST_FORMAT_TIME;
            if (!gst_element_query_position (gst_elm_play, &fmt, &val))
                val = 0;
            else if (val > 0 && !movie_length)
                gstGetDuration ();
        }
    }
    output ("\rA:%0.1f", (val / (GST_MSECOND * 1000.0)));
    ret = !!gst_elm_play;
    if (!ret)
        position_tag = 0;
    return ret;
}

static gboolean cb_stop (gpointer data) {
    (void)data;
    stop ();
    return FALSE;
}

static gboolean cb_error (gpointer data) {
    (void)data;
    stop ();
    return FALSE;
}

static gboolean cb_finished (gpointer data) {
    (void)data;
    if (current_state >= StateTerminate)
        return FALSE;
    current_state = StateTerminate;
    if (gst_elm_play) {
        gst_bus_set_flushing (gst_bus, TRUE);
        if (gst_bus_sync)
            g_signal_handler_disconnect (gst_bus, gst_bus_sync);
        if (gst_bus_async)
            g_signal_handler_disconnect (gst_bus, gst_bus_async);
        gst_object_unref (gst_bus);
        gst_object_unref (GST_OBJECT (gst_elm_play));
        gst_bus = 0L;
        gst_elm_play = 0L;
        color_balance = 0L;
        gst_bus_sync = gst_bus_async = 0;
        xoverlay = 0L;
    }
    g_main_loop_quit (event_loop);
    return FALSE;
}

static gboolean cb_video_dimension (gpointer data) {
    Dimension *dim = (Dimension *) data;
    int video_width = dim ? dim->width : movie_width;
    int video_height = dim ? dim->height : movie_height;
    if (gst_elm_play &&
            (crop_x > 0 || crop_y > 0 || crop_width > 0 || crop_height > 0)) {
        GObject *o = G_OBJECT (videocrop);
        g_object_set (o, "top", crop_y, NULL);
        g_object_set (o, "left", crop_x, NULL);
        g_object_set (o, "right", video_width - crop_width - crop_x, NULL);
        g_object_set (o, "bottom", video_height - crop_height - crop_y, NULL);
        video_width = crop_width;
        video_height = crop_height;
    }
    output ("ID_VIDEO_WIDTH=%d\n", video_width);
    output ("ID_VIDEO_HEIGHT=%d\n", video_height);
    if (window_created && video_width > 0 && video_height > 0) {
        XResizeWindow (display, wid, video_width, video_height);
        XFlush (display);
    }
    g_free (dim);
    return FALSE;
}

static gboolean cb_length (gpointer data) {
    output ("ID_LENGTH=%0.1f\n", ((long) data) / 10.0);
    return FALSE;
}

static gboolean watchStdin (GIOChannel *src, GIOCondition cond, gpointer data) {
    (void)data;
    if (cond == G_IO_IN) {
        static char cmdbuf[256];
        static int bufpos;
        char buf[64];
        int i;
        gsize nr = 0;
        GIOStatus status = g_io_channel_read_chars (src, buf, sizeof (buf), &nr, 0L);
        if (status == G_IO_STATUS_EOF || status == G_IO_STATUS_ERROR) {
            g_idle_add (cb_stop, NULL);
            return FALSE;
        }
        if (nr > 0) {
            for (i = 0; i < nr; i++) {
                if (buf[i] == '\n') {
                    cmdbuf[bufpos] = 0;
                    fprintf (stderr, "read '%s'\n", cmdbuf);
                    bufpos = 0;
                    if (!strncmp (cmdbuf, "quit", 4)) {
                        g_idle_add (cb_stop, NULL);
                    } else if (!strncmp (cmdbuf, "pause", 5)) {
                        pause ();
                    } else if (!strncmp (cmdbuf, "seek ", 5)) {
                        int d1, d2;
                        int nr = sscanf (cmdbuf + 5, "%d %d", &d1, &d2);
                        if (nr > 1)
                            seek (10 * d1, d2);
                    } else if (!strncmp (cmdbuf, "volume ", 7)) {
                        int d1;
                        int nr = sscanf (cmdbuf + 7, "%d", &d1);
                        if (nr > 0)
                            volume (d1);
                    } else if (!strncmp (cmdbuf, "panzoom ", 8)) {
                        int x, y, w, h;
                        char sep;
                        int nr = sscanf (cmdbuf + 8,
                                "%d%c%d%c%d%c%d",
                                &x, &sep, &y, &sep, &w, &sep, &h);
                        if (nr == 7 &&
                                x + w < movie_width &&
                                y + h < movie_height,
                                x >= 0 && y >= 0 && w > 0 && h > 0) {
                            crop_x = x;
                            crop_y = y;
                            crop_width = w;
                            crop_height = h;
                            cb_video_dimension (NULL);
                        }
                    }
                } else if (bufpos >= sizeof (cmdbuf)) {
                    fprintf (stderr, "read error\n");
                    g_idle_add (cb_stop, NULL);
                    return FALSE;
                } else {
                    cmdbuf[bufpos++] = buf[i];
                }
            }
        }
    }
    return TRUE;
}

#include <execinfo.h>
#include <signal.h>
static void sig_abort(int i) {
    void* trace[256];
    int n = backtrace(trace, 256);
    (void)i;
    if (!n)
        return;
    backtrace_symbols_fd (trace, n, 2);
    _exit(0);
}

int main (int argc, char **argv) {
    int i;
    int xpos    = 0;
    int ypos    = 0;
    int width   = 320;
    int height  = 200;
    GIOChannel *channel_in;
    int watch_in;
    GIOFlags flags;
    GSourceFuncs x11_funcs = {
        watchX11Prepare,
        watchX11Check,
        watchX11Events,
        NULL
    };
    GPollFD poll_fd;
    GSource *x11_source;

    signal(SIGABRT, sig_abort);
    signal(SIGINT, sig_abort);
    signal(SIGSEGV, sig_abort);

    if (!XInitThreads ()) {
        fprintf (stderr, "XInitThreads () failed\n");
        return 1;
    }
    display = XOpenDisplay(NULL);
    screen  = XDefaultScreen(display);

    g_set_application_name ("FMRadio"); /* BR:6694*/
    gst_init (NULL, NULL);

    event_loop = g_main_loop_new (0L, FALSE);

    for (i = 1; i < argc; i++) {
        if (!strcmp (argv [i], "-ao")) {
            ao_driver = argv [++i];
        } else if (!strcmp (argv [i], "-vo")) {
            vo_driver = argv [++i];
        } else if (!strcmp (argv [i], "-sub") && ++i < argc) {
            sub_mrl = g_strdup (argv [i]);
        } else if (!strcmp (argv [i], "-dvd-device") && ++i < argc) {
            dvd_device = argv [i];
        } else if (!strcmp (argv [i], "-vcd-device") && ++i < argc) {
            vcd_device = argv [i];
        } else if (!strcmp (argv [i], "-wid") || !strcmp (argv [i], "-window-id")) {
            wid = atol (argv [++i]);
            window_created = FALSE;
        } else if (!strcmp (argv [i], "-root")) {
            wid =  XDefaultRootWindow (display);
            window_created = FALSE;
        } else if (!strcmp (argv [i], "-window")) {
            ;
        } else if (!strcmp (argv [i], "-v")) {
            verbose = TRUE;
        } else if (!strcmp (argv [i], "-ss") && ++i < argc) {
            seek_start = strtol (argv[i], NULL, 10);
        } else if (!strncmp (argv [i], "-h", 2)) {
            fprintf (stderr, "usage: %s [-vo (xv|xshm)] [-ao <audio driver>] [-dvd-device <device>] [-vcd-device <device>] [-v] [(-wid|-window-id) <window>] [(-root|-window)]] [<url>]\n", argv[0]);
        } else if (strncmp (argv [i], "-", 1)) {
            mrl = g_strdup (argv[i]);
        }
    }

    poll_fd.fd = ConnectionNumber (display);
    poll_fd.events = G_IO_IN;
    x11_source = g_source_new (&x11_funcs, sizeof (GSource));
    g_source_add_poll (x11_source, &poll_fd);
    g_source_set_can_recurse (x11_source, TRUE);
    g_source_attach (x11_source, NULL);

    fprintf (stderr, "X11 fd:%d\n", ConnectionNumber (display));

    if (window_created)
        wid = XCreateSimpleWindow(display, XDefaultRootWindow(display),
                xpos, ypos, width, height, 1, 0, 0);
    fprintf (stderr, "init wid %u created:%d\n", (unsigned)wid, window_created);
    XSelectInput (display, wid,
                  (PointerMotionMask | ExposureMask |
                   KeyPressMask | /*ButtonPressMask |*/
                   StructureNotifyMask)); /* | SubstructureNotifyMask));*/

    if (window_created) {
        XMapRaised(display, wid);
        XSync(display, False);
    }

    g_timeout_add (50, cb_play, NULL);

    channel_in = g_io_channel_unix_new (0);
    g_io_channel_set_encoding (channel_in, NULL, NULL);
    g_io_channel_set_buffered (channel_in, FALSE);
    flags = g_io_channel_get_flags (channel_in);
    g_io_channel_set_flags(channel_in,(GIOFlags)(flags|G_IO_FLAG_NONBLOCK), 0L);
    watch_in = g_io_add_watch (channel_in, G_IO_IN, watchStdin, NULL);

    g_main_loop_run (event_loop);

    fprintf (stderr, "exit event loop\n");

    g_source_remove (watch_in);
    g_io_channel_shutdown (channel_in, TRUE, 0L);
    g_io_channel_unref (channel_in);

    stop ();
    if (window_created) {
        fprintf (stderr, "unmap %lu\n", wid);
        XUnmapWindow (display, wid);
        XDestroyWindow(display, wid);
        XSync (display, False);
    }

    fprintf (stderr, "closing display\n");
    XCloseDisplay (display);
    g_source_unref (x11_source);

    fprintf (stderr, "done\n");
    fflush (stderr);
    return 0;
}
