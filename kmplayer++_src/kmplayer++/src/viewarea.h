/**
  This file belong to the KMPlayer project, a movie player plugin for Konqueror
  Copyright (C) 2007  Koos Vriezen <koos.vriezen@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**/

#ifndef KMPLAYER_VIEW_AREA_H
#define KMPLAYER_VIEW_AREA_H

#include "kmplayertypes.h"
#include "surface.h"

#include <QtGui/QWidget>
#include <QtGui/QX11EmbedContainer>
#include <set>

namespace KMPlayer {

class VideoOutput;
class ViewAreaPrivate;

class KMPLAYER_NO_EXPORT Button : public QWidget
{
    Q_OBJECT

public:
    Button (QWidget *parent, const char *icon);
    void activate ();
protected:
    void mousePressEvent (QMouseEvent *);
    void paintEvent (QPaintEvent *);
Q_SIGNALS:
    void clicked ();
private:
    QString icon;
};

/*
 * The area in which the video widget and controlpanel are laid out
 */
class KMPLAYER_NO_EXPORT ViewArea : public QWidget {
    Q_OBJECT

public:
    ViewArea (QWidget *parent);
    ~ViewArea ();
    void reset ();
    KDE_NO_EXPORT bool isFullScreen () const { return m_fullscreen; }
    Surface *getSurface (Mrl *mrl);
    void setAudioVideoGeometry (const IRect &rect, unsigned int * bg);
    void setAudioVideoNode (NodePtr n);
    void scheduleRepaint (const IRect &rect);
    ConnectionList *updaters ();
    void enableUpdaters (bool enable, unsigned int off_time);
    void scale (int);
    void setAspect (float a);
    bool timeoutPaintEvent ();
    void prepareFullscreenToggle ();
    void enableVideoInputFilter (bool enable);
    KDE_NO_EXPORT IRect videoGeometry () const { return m_av_geometry; }
    unsigned long videoXId () const;
    Mrl *videoMrl () { return video_node ? video_node->mrl () : NULL; }
    void toggleUseAspects () { m_use_aspects = !m_use_aspects; }
    void resizeEvent (QResizeEvent *);
    Button *scaleButton () { return m_scale_button; }
    Button *stopButton () { return m_stop_button; }
    Button *zoomButton () { return m_zoom_button; }
    Button *backButton () { return m_back_button; }
    VideoOutput *videoOutput () { return m_video_output; }
    void mousePressEvent (QMouseEvent *);
Q_SIGNALS:
    void videoWidgetVisible (bool);
protected:
    void closeEvent (QCloseEvent *);
    void keyPressEvent (QKeyEvent *);
    void mouseDoubleClickEvent (QMouseEvent *);
    QPaintEngine *paintEngine () const;
    void paintEvent (QPaintEvent *);
    void timerEvent (QTimerEvent * e);
    bool x11Event (XEvent *xe);
    void showEvent (QShowEvent*);
    void hideEvent (QHideEvent*);
private:
    void syncVisual (const IRect & rect);
    void updateSurfaceBounds (int width, int height);
    bool scheduleRepaintTimeout (unsigned int cur_time);

    ViewAreaPrivate *d;
    VideoOutput *m_video_output;
    Button *m_stop_button;
    Button *m_zoom_button;
    Button *m_scale_button;
    Button *m_back_button;
    ConnectionList m_updaters;
    IRect m_av_geometry;
    float aspect;
    IRect m_repaint_rect;
    SurfacePtr surface;
    NodePtrW video_node;
    unsigned int last_repaint_time;
    unsigned int next_repaint_time;
    unsigned int penalty;
    int cur_timeout;
    int m_repaint_timer;
    int m_button_timer;
    bool m_fullscreen;
    bool m_event_filter_on;
    bool m_updaters_enabled;
    bool m_video_filter;
    bool m_use_aspects;
};

/*
 * The video widget
 */
class KMPLAYER_NO_EXPORT VideoOutput : public QObject {
    Q_OBJECT
public:
    enum Monitor {
        MonitorNothing = 0, MonitorMouse = 1, MonitorKey = 2 , MonitorAll = 3
    };
    VideoOutput (ViewArea *parent);
    ~VideoOutput();

    int heightForWidth (int w) const;

    WId windowHandle ();
    void setGeometry (const IRect &rect);
    IRect geometry () const { return m_rect; }
    void setAspect (float a);
    float aspect () { return m_aspect; }
    void useIndirectWidget (bool);
    void addPaintRegion (const IRect& r);
    void applyPaintRegions ();
    void initPaintRegions (const IRect& r);

    void setMonitoring (Monitor m);
    long inputMask () const KDE_NO_EXPORT { return m_input_mask; }
    void sendKeyEvent (int key);
    void setBackgroundColor (const QColor & c);
    void resetBackgroundColor ();
    void setCurrentBackgroundColor (const QColor & c);
    ViewArea * view () const KDE_NO_EXPORT { return m_view; }
private:
    std::set<IRect> m_paint_regions;
    std::set<IRect> m_tmp_regions;
    WId m_plain_window;
    IRect m_rect;
    unsigned int m_bgcolor;
    float m_aspect;
    ViewArea * m_view;
    long m_input_mask;
};

} // namespace KMPlayer

#endif
