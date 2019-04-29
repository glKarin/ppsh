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

#include <math.h>

#include "viewarea.h"
#include "kmplayertypes.h"
#include "kmplayer_smil.h"
#include "control.h"
#include "kmplayer_rp.h"
#include "mediaobject.h"

#include <qmap.h>
#include <QPalette>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QTextStream>
#include <QTimerEvent>
#include <QX11Info>
#include <QPainter>
#include <QCoreApplication>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>
#include <QtGui/QAbstractTextDocumentLayout>
#include <QtGui/QImage>

#include <cairo.h>
#include <cairo-xlib.h>
//#include <X11/extensions/Xdbe.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/extensions/shape.h>
static const int XKeyPress = KeyPress;
static const int XUnsorted = Unsorted;
#undef KeyPress
#undef Always
#undef Never
#undef Status
#undef Unsorted
#undef Bool

using namespace KMPlayer;

//-------------------------------------------------------------------------

static void clearSurface (cairo_t *cr, const IRect &rect) {
    cairo_save (cr);
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (cr, rect.x (), rect.y (), rect.width (), rect.height ());
    cairo_fill (cr);
    cairo_restore (cr);
}

void ImageData::copyImage (Surface *s, const SSize &sz, cairo_surface_t *similar,
        unsigned int *penalty, CalculatedSizer *zoom) {
    cairo_surface_t *src_sf;
    bool clear = false;
    int w = sz.width;
    int h = sz.height;

    if (surface) {
        src_sf = surface;
    } else {
        if (image->depth () < 24) {
            QImage qi = image->convertToFormat (QImage::Format_RGB32);
            *image = qi;
        }
        src_sf = cairo_image_surface_create_for_data (
                image->bits (),
                has_alpha ? CAIRO_FORMAT_ARGB32:CAIRO_FORMAT_RGB24,
                width, height, image->bytesPerLine ());
        if (flags & ImagePixmap && !(flags & ImageAnimated)) {
            surface = cairo_surface_create_similar (similar,
                    has_alpha ? CAIRO_CONTENT_COLOR_ALPHA : CAIRO_CONTENT_COLOR,
                    width, height);
            cairo_pattern_t *pat = cairo_pattern_create_for_surface (src_sf);
            cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
            cairo_pattern_set_filter (pat, CAIRO_FILTER_FAST);
            cairo_t *cr = cairo_create (surface);
            cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
            cairo_set_source (cr, pat);
            cairo_paint (cr);
            cairo_destroy (cr);
            cairo_pattern_destroy (pat);
            cairo_surface_destroy (src_sf);
            cairo_surface_flush (surface);
            src_sf = surface;
            delete image;
            image = NULL;
            *penalty += width * height / (40 * 480);
        }
    }

    cairo_pattern_t *img_pat = cairo_pattern_create_for_surface (src_sf);
    cairo_pattern_set_extend (img_pat, CAIRO_EXTEND_NONE);
    if (zoom) {
        cairo_matrix_t mat;
        Single zx, zy, zw, zh;
        zoom->calcSizes (NULL, NULL, width, height, zx, zy, zw, zh);
        cairo_matrix_init_translate (&mat, zx, zy);
        cairo_matrix_scale (&mat, 1.0 * zw/w, 1.0 * zh/h);
        cairo_pattern_set_matrix (img_pat, &mat);
        *penalty += w * h / (1 * 480);
    } else if (w != width && h != height) {
        cairo_matrix_t mat;
        cairo_matrix_init_scale (&mat, 1.0 * width/w, 1.0 * height/h);
        cairo_pattern_set_matrix (img_pat, &mat);
        *penalty += w * h / (1 * 480);
    }
    if (!s->surface)
        s->surface = cairo_surface_create_similar (similar,
                has_alpha ?
                CAIRO_CONTENT_COLOR_ALPHA : CAIRO_CONTENT_COLOR, w, h);
    else
        clear = true;
    cairo_t *cr = cairo_create (s->surface);
    if (clear)
        clearSurface (cr, IRect (0, 0, w, h));
    cairo_set_source (cr, img_pat);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (cr);
    cairo_destroy (cr);

    cairo_pattern_destroy (img_pat);
    if (!surface)
        cairo_surface_destroy (src_sf);
}

//-------------------------------------------------------------------------

#define CAIRO_SET_SOURCE_RGB(cr,c)            \
    cairo_set_source_rgb ((cr),               \
            1.0 * (((c) >> 16) & 0xff) / 255, \
            1.0 * (((c) >> 8) & 0xff) / 255,  \
            1.0 * (((c)) & 0xff) / 255)

#define USE_IMG_SURFACE 0

# define CAIRO_SET_SOURCE_ARGB(cr,c)          \
    cairo_set_source_rgba ((cr),              \
            1.0 * (((c) >> 16) & 0xff) / 255, \
            1.0 * (((c) >> 8) & 0xff) / 255,  \
            1.0 * (((c)) & 0xff) / 255,       \
            1.0 * (((c) >> 24) & 0xff) / 255)
struct KMPLAYER_NO_EXPORT PaintContext
{
    PaintContext (const Matrix& m, const IRect& c)
        : matrix (m)
        , clip (c)
        , fit (fit_default)
        , bg_repeat (SMIL::RegionBase::BgRepeat)
        , bg_image (NULL)
    {}
    Matrix matrix;
    IRect clip;
    Fit fit;
    SMIL::RegionBase::BackgroundRepeat bg_repeat;
    ImageData *bg_image;
};

class KMPLAYER_NO_EXPORT CairoPaintVisitor : public Visitor, public PaintContext {
    IRect visit_rect;
    ViewArea *view_widget;
    VideoOutput *video_widget;
    cairo_surface_t * cairo_surface;
    cairo_surface_t * cairo_surface_widget;
    // stack vars need for transitions
    TransitionModule *cur_transition;
    cairo_pattern_t * cur_pat;
    cairo_matrix_t cur_mat;
    float opacity;
public:
    unsigned int penalty;
    bool toplevel;
    cairo_t * cr;

    void traverseRegion (Node *reg, Surface *s);
    void updateExternal (SMIL::MediaType *av, SurfacePtr s);
    void paint (TransitionModule *trans, MediaOpacity mopacity, Surface *s,
                const IPoint &p, const IRect &);
    void video (Mrl *mt, Surface *s);
    void addPaintRegion (const IRect& r);

    CairoPaintVisitor (ViewArea *v, cairo_surface_t * cs, Matrix m,
            const IRect & rect, bool toplevel=false);
    ~CairoPaintVisitor ();
    using Visitor::visit;
    void visit (Node * n);
    void visit (SMIL::Smil *);
    void visit (SMIL::Layout *);
    void visit (SMIL::RegionBase *);
    void visit (SMIL::Transition *);
    void visit (SMIL::TextMediaType *);
    void visit (SMIL::Brush *);
    void visit (SMIL::SmilText *);
    void visit (SMIL::RefMediaType *);
    void visit (RP::Imfl *);
    void visit (RP::Fill *);
    void visit (RP::Fadein *);
    void visit (RP::Fadeout *);
    void visit (RP::Crossfade *);
    void visit (RP::Wipe *);
    void visit (RP::ViewChange *);
};

KDE_NO_CDTOR_EXPORT
CairoPaintVisitor::CairoPaintVisitor (ViewArea *v, cairo_surface_t *cs,
        Matrix m, const IRect & rect, bool top)
 : PaintContext (m, rect), visit_rect (rect), view_widget (v), video_widget (NULL),
#if USE_IMG_SURFACE
   cairo_surface_widget (cs),
   toplevel (top) {
    if (toplevel)
        cairo_surface = cairo_image_surface_create
            (CAIRO_FORMAT_RGB24, rect.width (), rect.height ());
    else
        cairo_surface = cs;
    cr = cairo_create (cairo_surface);
    if (toplevel) {
        cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_tolerance (cr, 0.5 );
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
        cairo_rectangle (cr, 0, 0, rect.width (), rect.height ());
        cairo_fill (cr);
        cairo_translate (cr, -rect.x (), -rect.y ());
#else
# warning xrender
   cairo_surface (cs), toplevel (top)
{
    cr = cairo_create (cs);
    if (toplevel) {
        penalty = rect.width () * rect.height () / (40 * 480);
        cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_tolerance (cr, 0.5 );
        cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
        //cairo_push_group (cr);
        cairo_set_source_rgb (cr, 8.0 / 255, 12.0 / 255, 12.0 / 255);
        cairo_rectangle (cr, rect.x(), rect.y(), rect.width(), rect.height());
        cairo_fill (cr);
#endif
    } else {
        penalty = 0;
        clearSurface (cr, rect);
    }
}

KDE_NO_CDTOR_EXPORT CairoPaintVisitor::~CairoPaintVisitor () {
#if USE_IMG_SURFACE
    if (toplevel) {
        cairo_pattern_t *pat = cairo_pattern_create_for_surface (cairo_surface);
        cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
        cairo_matrix_t mat;
        cairo_matrix_init_translate (&mat, -clip.x (), -clip.y ());
        cairo_pattern_set_matrix (pat, &mat);
        cairo_t * wcr = cairo_create (cairo_surface_widget);
        cairo_set_source (wcr, pat);
        cairo_rectangle (wcr, clip.x(), clip.y(), clip.width(), clip.height());
        cairo_fill (wcr);
        cairo_destroy (wcr);
        cairo_pattern_destroy (pat);
        cairo_surface_destroy (cairo_surface);
    }
#endif
    cairo_destroy (cr);
    if (video_widget)
        video_widget->applyPaintRegions ();
}

void CairoPaintVisitor::addPaintRegion (const IRect& r) {
    if (video_widget)
        video_widget->addPaintRegion (r);
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (Node * n) {
    warningLog() << "Paint called on " << n->nodeName() << endl;
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::Smil *s) {
    if (s->active () && s->layout_node)
        s->layout_node->accept (this);
}

KDE_NO_EXPORT void CairoPaintVisitor::traverseRegion (Node *node, Surface *s) {
    ConnectionList *nl = nodeMessageReceivers (node, MsgSurfaceAttach);
    if (nl) {
        for (Connection *c = nl->first(); c; c = nl->next ())
            if (c->connecter)
                c->connecter->accept (this);
    }
    /*for (SurfacePtr c = s->lastChild (); c; c = c->previousSibling ()) {
        if (c->node && c->node->id != SMIL::id_node_region &&
        c->node && c->node->id != SMIL::id_node_root_layout)
            c->node->accept (this);
        else
            break;
    }*/
    // finally visit region children
    for (SurfacePtr c = s->firstChild (); c; c = c->nextSibling ()) {
        if (c->node && c->node->id == SMIL::id_node_region)
            c->node->accept (this);
        else
            break;
    }
    s->dirty = false;
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::Layout *layout) {
    if (layout->root_layout)
        layout->root_layout->accept (this);
}

#define REGION_SCROLLBAR_WIDTH 30

static void cairoDrawRect (cairo_t *cr, unsigned int color,
        int x, int y, int w, int h) {
    CAIRO_SET_SOURCE_ARGB (cr, color);
    cairo_rectangle (cr, x, y, w, h);
    cairo_fill (cr);
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::RegionBase *reg) {
    Surface *s = (Surface *) reg->role (RoleDisplay);
    if (s) {
        SRect rect = s->bounds;

        IRect scr = matrix.toScreen (rect);
        if (clip.intersect (scr).isEmpty ())
            return;
        PaintContext ctx_save = *(PaintContext *) this;
        matrix = Matrix (rect.x(), rect.y(), s->xscale, s->yscale);
        matrix.transform (ctx_save.matrix);
        clip = clip.intersect (scr);
        if (SMIL::RegionBase::BgInherit != reg->bg_repeat)
            bg_repeat = reg->bg_repeat;
        cairo_save (cr);

        Surface *cs = s->firstChild ();
        if (!s->virtual_size.isEmpty ())
            matrix.translate (-s->x_scroll, -s->y_scroll);

        if (fit_default != reg->fit)
            fit = reg->fit;

        ImageMedia *im = reg->media_info
             ? (ImageMedia *) reg->media_info->media
             : NULL;

        ImageData *bg_img = im && !im->isEmpty() ? im->cached_img.ptr () : NULL;
        if (reg->background_image == "inherit")
            bg_img = bg_image;
        else
            bg_image = bg_img;
        unsigned int bg_alpha = s->background_color & 0xff000000;
        if ((SMIL::RegionBase::ShowAlways == reg->show_background ||
                    reg->m_AttachedMediaTypes.first ()) &&
                (bg_alpha || bg_img)) {
            cairo_save (cr);
            if (bg_alpha) {
                cairo_rectangle (cr,
                        clip.x (), clip.y (), clip.width (), clip.height ());
                if (bg_alpha < 0xff000000) {
                    CAIRO_SET_SOURCE_ARGB (cr, s->background_color);
                    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
                    cairo_fill (cr);
                    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
                } else {
                    CAIRO_SET_SOURCE_RGB (cr, s->background_color);
                    cairo_fill (cr);
                }
            }
            if (bg_img) {
                Single w = bg_img->width;
                Single h = bg_img->height;
                matrix.getWH (w, h);
                if (!s->surface)
                    bg_img->copyImage (s, SSize(w, h), cairo_surface, &penalty);
                if (bg_img->has_alpha)
                    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
                cairo_pattern_t *pat = cairo_pattern_create_for_surface (s->surface);
                cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);
                cairo_matrix_t mat;
                cairo_matrix_init_translate (&mat, -scr.x (), -scr.y ());
                cairo_pattern_set_matrix (pat, &mat);
                cairo_set_source (cr, pat);
                int cw = clip.width ();
                int ch = clip.height ();
                switch (bg_repeat) {
                case SMIL::RegionBase::BgRepeatX:
                    if (h < ch)
                        ch = h;
                    break;
                case SMIL::RegionBase::BgRepeatY:
                    if (w < cw)
                        cw = w;
                    break;
                case SMIL::RegionBase::BgNoRepeat:
                    if (w < cw)
                        cw = w;
                    if (h < ch)
                        ch = h;
                    break;
                default:
                    break;
                }
                cairo_rectangle (cr, clip.x (), clip.y (), cw, ch);
                cairo_fill (cr);
                cairo_pattern_destroy (pat);
                if (bg_img->has_alpha)
                    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
            }
            cairo_restore (cr);

            addPaintRegion (s->toScreen (s->bounds.size));
        }
        traverseRegion (reg, s);
        cs = s->firstChild ();
        if (cs && (s->scroll || cs->scroll) && cs == s->lastChild ()) {
            SRect r = cs->bounds;
            if (r.width () > rect.width () || r.height () > rect.height ()) {
                if (s->virtual_size.isEmpty ())
                    s->x_scroll = s->y_scroll = 0;
                s->virtual_size = r.size;
                matrix.getWH (s->virtual_size.width, s->virtual_size.height);
                s->virtual_size.width += REGION_SCROLLBAR_WIDTH;
                s->virtual_size.height += REGION_SCROLLBAR_WIDTH;
                const int vy = s->virtual_size.height;
                const int vw = s->virtual_size.width;
                int sbw = REGION_SCROLLBAR_WIDTH;
                int sbx = scr.x () + scr.width () - sbw;
                int sby = scr.y ();
                int sbh = scr.height () - REGION_SCROLLBAR_WIDTH;
                IRect sb_clip = clip.intersect (IRect (sbx, sby, sbw, sbh));
                if (!sb_clip.isEmpty ()) {
                    int knob_h = sbh * scr.height () / vy;
                    int knob_y = scr.y () + s->y_scroll * sbh / vy;
                    IRect knob (sbx, knob_y, sbw, knob_h);
                    cairo_save (cr);
                    cairo_rectangle (cr, sb_clip.x (), sb_clip.y (),
                            sb_clip.width (), sb_clip.height ());
                    cairo_clip (cr);
                    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
                    cairo_set_line_width (cr, 2);
                    CAIRO_SET_SOURCE_ARGB (cr, 0x80A0A0A0);
                    cairo_rectangle (cr, sbx + 1, sby + 1, sbw - 2, sbh - 2);
                    cairo_stroke (cr);
                    if (s->y_scroll)
                        cairoDrawRect (cr, 0x80000000,
                                sbx + 2, sby + 2,
                                sbw - 4, knob.y() - sby - 2);
                    cairoDrawRect (cr, 0x80808080,
                            knob.x() + 2, knob.y(),
                            knob.width() - 4, knob.height());
                    if (sby + sbh - knob.y() - knob.height() - 2 > 0)
                        cairoDrawRect (cr, 0x80000000,
                                sbx + 2, knob.y() + knob.height(),
                                sbw - 4, sby + sbh - knob.y() -knob.height()-2);
                    cairo_restore (cr);
                }
                sbh = REGION_SCROLLBAR_WIDTH;
                sbx = scr.x ();
                sby = scr.y () + scr.height () - sbh;
                sbw = scr.width () - REGION_SCROLLBAR_WIDTH;
                sb_clip = clip.intersect (IRect (sbx, sby, sbw, sbh));
                if (!sb_clip.isEmpty ()) {
                    int knob_w = sbw * scr.width () / vw;
                    int knob_x = scr.x () + s->x_scroll * sbw / vw;
                    IRect knob (knob_x, sby, knob_w, sbh);
                    cairo_save (cr);
                    cairo_rectangle (cr, sb_clip.x (), sb_clip.y (),
                            sb_clip.width (), sb_clip.height ());
                    cairo_clip (cr);
                    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
                    cairo_set_line_width (cr, 2);
                    CAIRO_SET_SOURCE_ARGB (cr, 0x80A0A0A0);
                    cairo_rectangle (cr, sbx + 1, sby + 1, sbw - 2, sbh - 2);
                    cairo_stroke (cr);
                    if (s->x_scroll)
                        cairoDrawRect (cr, 0x80000000,
                                sbx + 2, sby + 2,
                                knob.x() - sbx - 2, sbh - 4);
                    cairoDrawRect (cr, 0x80808080,
                            knob.x(), knob.y() + 2,
                            knob.width(), knob.height() - 4);
                    if (sbx + sbw - knob.x() - knob.width() - 2 > 0)
                        cairoDrawRect (cr, 0x80000000,
                                knob.x() + knob.width(), sby + 2,
                                sbx + sbw - knob.x() - knob.width()-2, sbh - 4);
                    cairo_restore (cr);
                }
            }
        }
        cairo_restore (cr);
        *(PaintContext *) this = ctx_save;
        s->dirty = false;
    }
}

#define CAIRO_SET_PATTERN_COND(cr,pat,mat)                      \
    if (pat) {                                                  \
        cairo_pattern_set_extend (cur_pat, CAIRO_EXTEND_NONE);  \
        cairo_pattern_set_matrix (pat, &mat);                   \
        cairo_pattern_set_filter (pat, CAIRO_FILTER_FAST);      \
        cairo_set_source (cr, pat);                             \
    }

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::Transition *trans) {
    float perc = trans->start_progress + (trans->end_progress - trans->start_progress)*cur_transition->trans_gain;
    if (cur_transition->trans_out_active)
        perc = 1.0 - perc;
    if (SMIL::Transition::Fade == trans->type) {
        CAIRO_SET_PATTERN_COND(cr, cur_pat, cur_mat)
        cairo_rectangle (cr, clip.x(), clip.y(), clip.width(), clip.height());
        opacity = perc;
    } else if (SMIL::Transition::BarWipe == trans->type) {
        IRect rect;
        if (SMIL::Transition::SubTopToBottom == trans->sub_type) {
            if (SMIL::Transition::dir_reverse == trans->direction) {
                int dy = (int) ((1.0 - perc) * clip.height ());
                rect = IRect (clip.x (), clip.y () + dy,
                        clip.width (), clip.height () - dy);
            } else {
                rect = IRect (clip.x (), clip.y (),
                        clip.width (), (int) (perc * clip.height ()));
            }
        } else {
            if (SMIL::Transition::dir_reverse == trans->direction) {
                int dx = (int) ((1.0 - perc) * clip.width ());
                rect = IRect (clip.x () + dx, clip.y (),
                        clip.width () - dx, clip.height ());
            } else {
                rect = IRect (clip.x (), clip.y (),
                        (int) (perc * clip.width ()), clip.height ());
            }
        }
        cairo_rectangle (cr, rect.x(), rect.y(), rect.width(), rect.height());
        CAIRO_SET_PATTERN_COND(cr, cur_pat, cur_mat)
    } else if (SMIL::Transition::PushWipe == trans->type) {
        int dx = 0, dy = 0;
        if (SMIL::Transition::SubFromTop == trans->sub_type)
            dy = -(int) ((1.0 - perc) * clip.height ());
        else if (SMIL::Transition::SubFromRight == trans->sub_type)
            dx = (int) ((1.0 - perc) * clip.width ());
        else if (SMIL::Transition::SubFromBottom == trans->sub_type)
            dy = (int) ((1.0 - perc) * clip.height ());
        else //if (SMIL::Transition::SubFromLeft == trans->sub_type)
            dx = -(int) ((1.0 - perc) * clip.width ());
        cairo_matrix_translate (&cur_mat, -dx, -dy);
        IRect rect = clip.intersect (IRect (clip.x () + dx, clip.y () + dy,
                    clip.width (), clip.height ()));
        cairo_rectangle (cr, rect.x(), rect.y(), rect.width(), rect.height());
        CAIRO_SET_PATTERN_COND(cr, cur_pat, cur_mat)
    } else if (SMIL::Transition::IrisWipe == trans->type) {
        CAIRO_SET_PATTERN_COND(cr, cur_pat, cur_mat)
        if (SMIL::Transition::SubDiamond == trans->sub_type) {
            cairo_rectangle (cr, clip.x(), clip.y(),clip.width(),clip.height());
            cairo_clip (cr);
            int dx = (int) (perc * clip.width ());
            int dy = (int) (perc * clip.height ());
            int mx = clip.x () + clip.width ()/2;
            int my = clip.y () + clip.height ()/2;
            cairo_new_path (cr);
            cairo_move_to (cr, mx, my - dy);
            cairo_line_to (cr, mx + dx, my);
            cairo_line_to (cr, mx, my + dy);
            cairo_line_to (cr, mx - dx, my);
            cairo_close_path (cr);
        } else { // SubRectangle
            int dx = (int) (0.5 * (1 - perc) * clip.width ());
            int dy = (int) (0.5 * (1 - perc) * clip.height ());
            cairo_rectangle (cr, clip.x () + dx, clip.y () + dy,
                    clip.width () - 2 * dx, clip.height () -2 * dy);
        }
    } else if (SMIL::Transition::ClockWipe == trans->type) {
        cairo_rectangle (cr, clip.x(), clip.y(), clip.width(), clip.height());
        cairo_clip (cr);
        int mx = clip.x () + clip.width ()/2;
        int my = clip.y () + clip.height ()/2;
        cairo_new_path (cr);
        cairo_move_to (cr, mx, my);
        float hw = 1.0 * clip.width ()/2;
        float hh = 1.0 * clip.height ()/2;
        float radius = sqrtf (hw * hw + hh * hh);
        float phi;
        switch (trans->sub_type) {
            case SMIL::Transition::SubClockwiseThree:
                phi = 0;
                break;
            case SMIL::Transition::SubClockwiseSix:
                phi = M_PI / 2;
                break;
            case SMIL::Transition::SubClockwiseNine:
                phi = M_PI;
                break;
            default: // Twelve
                phi = -M_PI / 2;
                break;
        }
        if (SMIL::Transition::dir_reverse == trans->direction)
            cairo_arc_negative (cr, mx, my, radius, phi, phi - 2 * M_PI * perc);
        else
            cairo_arc (cr, mx, my, radius, phi, phi + 2 * M_PI * perc);
        cairo_close_path (cr);
        CAIRO_SET_PATTERN_COND(cr, cur_pat, cur_mat)
    } else if (SMIL::Transition::BowTieWipe == trans->type) {
        cairo_rectangle (cr, clip.x(), clip.y(), clip.width(), clip.height());
        cairo_clip (cr);
        int mx = clip.x () + clip.width ()/2;
        int my = clip.y () + clip.height ()/2;
        cairo_new_path (cr);
        cairo_move_to (cr, mx, my);
        float hw = 1.0 * clip.width ()/2;
        float hh = 1.0 * clip.height ()/2;
        float radius = sqrtf (hw * hw + hh * hh);
        float phi;
        switch (trans->sub_type) {
            case SMIL::Transition::SubHorizontal:
                phi = 0;
                break;
            default: // Vertical
                phi = -M_PI / 2;
                break;
        }
        float dphi = 0.5 * M_PI * perc;
        cairo_arc (cr, mx, my, radius, phi - dphi, phi + dphi);
        cairo_close_path (cr);
        cairo_new_sub_path (cr);
        cairo_move_to (cr, mx, my);
        if (SMIL::Transition::SubHorizontal == trans->sub_type)
            cairo_arc (cr, mx, my, radius, M_PI + phi - dphi, M_PI + phi +dphi);
        else
            cairo_arc (cr, mx, my, radius, -phi - dphi, -phi + dphi);
        cairo_close_path (cr);
        CAIRO_SET_PATTERN_COND(cr, cur_pat, cur_mat)
    } else if (SMIL::Transition::EllipseWipe == trans->type) {
        cairo_rectangle (cr, clip.x(), clip.y(), clip.width(), clip.height());
        cairo_clip (cr);
        int mx = clip.x () + clip.width ()/2;
        int my = clip.y () + clip.height ()/2;
        float hw = (double) clip.width ()/2;
        float hh = (double) clip.height ()/2;
        float radius = sqrtf (hw * hw + hh * hh);
        cairo_save (cr);
        cairo_new_path (cr);
        cairo_translate (cr, (int) mx, (int) my);
        cairo_move_to (cr, - Single (radius), 0);
        if (SMIL::Transition::SubHorizontal == trans->sub_type)
            cairo_scale (cr, 1.0, 0.6);
        else if (SMIL::Transition::SubVertical == trans->sub_type)
            cairo_scale (cr, 0.6, 1.0);
        cairo_arc (cr, 0, 0, perc * radius, 0, 2 * M_PI);
        cairo_close_path (cr);
        cairo_restore (cr);
        CAIRO_SET_PATTERN_COND(cr, cur_pat, cur_mat)
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::video (Mrl *m, Surface *s) {
    if (m->media_info &&
            m->media_info->media &&
            (MediaManager::Audio == m->media_info->type ||
             MediaManager::AudioVideo == m->media_info->type)) {
        AudioVideoMedia *avm = static_cast<AudioVideoMedia *> (m->media_info->media);
        if (s && avm && strcmp (m->nodeName (), "audio")) {
            s->xscale = s->yscale = 1; // either scale width/heigt or use bounds
            view_widget->setAudioVideoGeometry (s->toScreen (s->bounds.size),
                    NULL);
            video_widget = view_widget->videoOutput ();
            video_widget->initPaintRegions (visit_rect);
            IRect scr = matrix.toScreen (s->bounds);
            IRect clip_rect = clip.intersect (scr);
            if (clip_rect.isEmpty ())
                return;
            cairo_save (cr);
            cairo_set_source_rgb (cr, 8.0 / 255, 8.0 / 255, 16.0 / 255);
            cairo_rectangle (cr, clip_rect.x(), clip_rect.y(), clip_rect.width(), clip_rect.height());
            cairo_fill (cr);
            cairo_restore (cr);
        } else {
            view_widget->setAudioVideoGeometry (IRect (-60, -60, 50, 50), NULL);
        }
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::RefMediaType *ref) {
    Surface *s = ref->surface ();
    if (s && ref->external_tree) {
        updateExternal (ref, s);
        return;
    }
    if (!ref->media_info)
        return;
    if (fit_default != fit
            && fit_default == ref->fit
            && fit != ref->effective_fit) {
        ref->effective_fit = fit;
        s->resize (ref->calculateBounds(), false);
    }
    if (ref->media_info->media &&
            ref->media_info->media->type () == MediaManager::Image) {
        if (!s)
            return;

        IRect scr = matrix.toScreen (s->bounds);
        IRect clip_rect = clip.intersect (scr);
        if (clip_rect.isEmpty ())
            return;

        ImageMedia *im = static_cast <ImageMedia *> (ref->media_info->media);
        ImageData *id = im ? im->cached_img.ptr () : NULL;
        if (id && id->flags == ImageData::ImageScalable)
            penalty += im->render (scr.size);
        if (!id || im->isEmpty () || ref->size.isEmpty ()) {
            s->remove();
            return;
        }
        if (!s->surface || s->dirty)
            id->copyImage (s, SSize (scr.width (), scr.height ()), cairo_surface, &penalty, ref->pan_zoom);
        paint (&ref->transition, ref->media_opacity, s, scr.point, clip_rect);
        s->dirty = false;

    } else {
        video (ref, s);
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::paint (TransitionModule *trans,
        MediaOpacity mopacity, Surface *s,
        const IPoint &point, const IRect &rect) {
    cairo_save (cr);
    opacity = 1.0;
    cairo_matrix_init_translate (&cur_mat, -point.x, -point.y);
    cur_pat = cairo_pattern_create_for_surface (s->surface);
    if (trans->active_trans) {
        IRect clip_save = clip;
        clip = rect;
        penalty += rect.width() * rect.height() / (20 * 480);
        cur_transition = trans;
        trans->active_trans->accept (this);
        clip = clip_save;
    } else {
        cairo_pattern_set_extend (cur_pat, CAIRO_EXTEND_NONE);
        cairo_pattern_set_matrix (cur_pat, &cur_mat);
        cairo_pattern_set_filter (cur_pat, CAIRO_FILTER_FAST);
        cairo_set_source (cr, cur_pat);
        cairo_rectangle (cr, rect.x(), rect.y(), rect.width(), rect.height());
    }
    opacity *= mopacity.opacity / 100.0;
    bool over = opacity < 0.99 ||
                CAIRO_CONTENT_COLOR != cairo_surface_get_content (s->surface);
    cairo_operator_t op;
    if (over) {
        penalty += rect.width () * rect.height () / (40 * 480);
        op = cairo_get_operator (cr);
        cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    }
    if (opacity < 0.99) {
        cairo_clip (cr);
        cairo_paint_with_alpha (cr, opacity);
    } else {
        cairo_fill (cr);
    }
    if (over)
        cairo_set_operator (cr, op);
    cairo_pattern_destroy (cur_pat);
    cairo_restore (cr);

    addPaintRegion (s->toScreen (s->bounds.size));
}

static Mrl *findActiveMrl (Node *n, bool *rp_or_smil) {
    Mrl *mrl = n->mrl ();
    if (mrl) {
        *rp_or_smil = (mrl->id >= SMIL::id_node_first &&
                mrl->id < SMIL::id_node_last) ||
            (mrl->id >= RP::id_node_first &&
             mrl->id < RP::id_node_last);
        if (*rp_or_smil ||
                (mrl->media_info &&
                 (MediaManager::Audio == mrl->media_info->type ||
                  MediaManager::AudioVideo == mrl->media_info->type)))
            return mrl;
    }
    for (Node *c = n->firstChild (); c; c = c->nextSibling ())
        if (c->active ()) {
            Mrl *m = findActiveMrl (c, rp_or_smil);
            if (m)
                return m;
        }
    return NULL;
}

KDE_NO_EXPORT
void CairoPaintVisitor::updateExternal (SMIL::MediaType *av, SurfacePtr s) {
    bool rp_or_smil = false;
    Mrl *ext_mrl = findActiveMrl (av->external_tree.ptr (), &rp_or_smil);
    if (!ext_mrl)
        return;
    if (!rp_or_smil) {
        video (ext_mrl, s.ptr ());
        return;
    }
    IRect scr = matrix.toScreen (s->bounds);
    IRect clip_rect = clip.intersect (scr);
    if (clip_rect.isEmpty ())
        return;
    if (!s->surface || s->dirty) {
        Matrix m = matrix;
        m.translate (-scr.x (), -scr.y ());
        m.scale (s->xscale, s->yscale);
        IRect r (clip_rect.x() - scr.x () - 1, clip_rect.y() - scr.y () - 1,
                clip_rect.width() + 3, clip_rect.height() + 3);
        if (!s->surface) {
            s->surface = cairo_surface_create_similar (cairo_surface,
                    CAIRO_CONTENT_COLOR_ALPHA, scr.width (), scr.height ());
            r = IRect (0, 0, scr.size);
        }
        CairoPaintVisitor visitor (view_widget, s->surface, m, r);
        ext_mrl->accept (&visitor);
        penalty += visitor.penalty;
        s->dirty = false;
    }
    paint (&av->transition, av->media_opacity, s.ptr (), scr.point, clip_rect);
}

static void setAlignment (QTextDocument &td, unsigned char align) {
    QTextOption opt = td.defaultTextOption();
    if (SmilTextProperties::AlignLeft == align)
        opt.setAlignment (Qt::AlignLeft);
    else if (SmilTextProperties::AlignCenter == align)
        opt.setAlignment (Qt::AlignCenter);
    else if (SmilTextProperties::AlignRight == align)
        opt.setAlignment (Qt::AlignRight);
    td.setDefaultTextOption (opt);
}

static void calculateTextDimensions (const QFont& font,
        const QString& text, Single w, Single h, Single maxh,
        int *pxw, int *pxh, bool markup_text,
        unsigned char align = SmilTextProperties::AlignLeft) {
    QTextDocument td;
    td.setDefaultFont( font );
    td.setDocumentMargin (0);
    QImage img (QSize ((int)w, (int)h), QImage::Format_RGB32);
    td.setPageSize (QSize ((int)w, (int)maxh));
    td.documentLayout()->setPaintDevice (&img);
    if (markup_text)
        td.setHtml( text );
    else
        td.setPlainText( text );
    setAlignment (td, align);
    QRectF r = td.documentLayout()->blockBoundingRect (td.lastBlock());
    *pxw = (int)td.idealWidth ();
    *pxh = (int)(r.y() + r.height());
}

static cairo_t *createContext (cairo_surface_t *similar, Surface *s, int w, int h) {
    unsigned int bg_alpha = s->background_color & 0xff000000;
    bool clear = s->surface;
    if (!s->surface)
        s->surface = cairo_surface_create_similar (similar,
                bg_alpha < 0xff000000
                ? CAIRO_CONTENT_COLOR_ALPHA
                : CAIRO_CONTENT_COLOR,
                w, h);
    cairo_t *cr = cairo_create (s->surface);
    if (clear)
        clearSurface (cr, IRect (0, 0, w, h));
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

    if (bg_alpha) {
        if (bg_alpha < 0xff000000)
            CAIRO_SET_SOURCE_ARGB (cr, s->background_color);
        else
            CAIRO_SET_SOURCE_RGB (cr, s->background_color);
        cairo_paint (cr);
    }
    return cr;
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::TextMediaType * txt) {
    if (!txt->media_info || !txt->media_info->media)
        return;
    TextMedia *tm = static_cast <TextMedia *> (txt->media_info->media);
    Surface *s = txt->surface ();
    if (!s)
        return;
    if (!s->surface) {
        txt->size = SSize ();
        s->bounds = txt->calculateBounds ();
    }
    IRect scr = matrix.toScreen (s->bounds);
    if (!s->surface || s->dirty) {

        int w = scr.width ();
        int pxw, pxh;
        Single ft_size = w * txt->font_size / (double)s->bounds.width ();
        bool clear = s->surface;

        QFont font (txt->font_name, ft_size);
        if (clear) {
            pxw = scr.width ();
            pxh = scr.height ();
        } else {
            calculateTextDimensions (font, tm->text,
                    w, 2 * ft_size, scr.height (), &pxw, &pxh, false);
        }
        QTextDocument td;
        td.setDocumentMargin (0);
        td.setDefaultFont (font);
        bool have_alpha = (s->background_color & 0xff000000) < 0xff000000;
        QImage img (QSize (pxw, pxh), have_alpha ? QImage::Format_ARGB32 : QImage::Format_RGB32);
        img.fill (s->background_color);
        td.setPageSize (QSize (pxw, pxh + (int)ft_size));
        td.documentLayout()->setPaintDevice (&img);
        setAlignment (td, 1 + (int)txt->halign);
        td.setPlainText (tm->text);
        QPainter painter;
        painter.begin (&img);
        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.clip = QRect (0, 0, pxw, pxh);
        ctx.palette.setColor (QPalette::Text, QColor (QRgb (txt->font_color)));
        td.documentLayout()->draw (&painter, ctx);
        painter.end();

        cairo_t *cr_txt = createContext (cairo_surface, s, pxw, pxh);
        cairo_surface_t *src_sf = cairo_image_surface_create_for_data (
                img.bits (),
                have_alpha ? CAIRO_FORMAT_ARGB32:CAIRO_FORMAT_RGB24,
                img.width(), img.height(), img.bytesPerLine ());
        cairo_pattern_t *pat = cairo_pattern_create_for_surface (src_sf);
        cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
        cairo_set_operator (cr_txt, CAIRO_OPERATOR_SOURCE);
        cairo_set_source (cr_txt, pat);
        cairo_paint (cr_txt);
        cairo_pattern_destroy (pat);
        cairo_surface_destroy (src_sf);
        cairo_destroy (cr_txt);

        // update bounds rect
        SRect rect = matrix.toUser (IRect (scr.point, ISize (pxw, pxh)));
        txt->size = rect.size;
        s->bounds = txt->calculateBounds ();

        // update coord. for painting below
        scr = matrix.toScreen (s->bounds);
    }
    IRect clip_rect = clip.intersect (scr);
    if (!clip_rect.isEmpty ())
        paint (&txt->transition, txt->media_opacity, s, scr.point, clip_rect);
    s->dirty = false;
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::Brush * brush) {
    Surface *s = brush->surface ();
    if (s) {
        opacity = 1.0;
        IRect clip_rect = clip.intersect (matrix.toScreen (s->bounds));
        if (clip_rect.isEmpty ())
            return;
        cairo_save (cr);
        if (brush->transition.active_trans) {
            cur_transition = &brush->transition;
            cur_pat = NULL;
            brush->transition.active_trans->accept (this);
        } else {
            cairo_rectangle (cr, clip_rect.x (), clip_rect.y (),
                    clip_rect.width (), clip_rect.height ());
        }
        unsigned int color = brush->color.color;
        if (!color) {
            color = brush->background_color.color;
            opacity *= brush->background_color.opacity / 100.0;
        } else {
            opacity *= brush->color.opacity / 100.0;
        }
        opacity *= brush->media_opacity.opacity / 100.0;
        if (opacity < 0.99) {
            cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
            cairo_set_source_rgba (cr,
                    1.0 * ((color >> 16) & 0xff) / 255,
                    1.0 * ((color >> 8) & 0xff) / 255,
                    1.0 * (color & 0xff) / 255,
                    opacity);
        } else {
            CAIRO_SET_SOURCE_RGB (cr, color);
        }
        cairo_fill (cr);
        if (opacity < 0.99)
            cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        s->dirty = false;
        cairo_restore (cr);

        addPaintRegion (s->toScreen (s->bounds.size));
    }
}

struct SmilTextBlock {
    SmilTextBlock (const QFont& f, const QString &t,
            IRect r, unsigned char a)
        : font (f), rich_text (t), rect (r), align (a), next (NULL) {}

    QFont font;
    QString rich_text;
    IRect rect;
    unsigned char align;

    SmilTextBlock *next;
};

struct KMPLAYER_NO_EXPORT SmilTextInfo {
    SmilTextInfo (const SmilTextProperties &p) : props (p) {}

    void span (float scale);

    SmilTextProperties props;
    QString span_text;
};

class KMPLAYER_NO_EXPORT SmilTextVisitor : public Visitor {
public:
    SmilTextVisitor (int w, float s, const SmilTextProperties &p)
        : first (NULL), last (NULL), width (w), voffset (0),
          scale (s), max_font_size (0), info (p) {
         info.span (scale);
    }
    using Visitor::visit;
    void visit (TextNode *);
    void visit (SMIL::TextFlow *);
    void visit (SMIL::TemporalMoment *);

    void addRichText (const QString &txt);
    void push ();

    SmilTextBlock *first;
    SmilTextBlock *last;

    int width;
    int voffset;
    float scale;
    float max_font_size;
    SmilTextInfo info;
    QString rich_text;
};

void SmilTextInfo::span (float scale) {
    QString s = "<span style=\"";
    if (props.font_size.size () > -1)
        s += "font-size:" + QString::number ((int)(scale * props.font_size.size ())) + "px;";
    s += "font-family:" + props.font_family + ";";
    if (props.font_color > -1)
        s += QString().sprintf ("color:#%06x;", props.font_color);
    if (props.background_color > -1)
        s += QString().sprintf ("background-color:#%06x;", props.background_color);
    if (SmilTextProperties::StyleInherit != props.font_style) {
        s += "font-style:";
        switch (props.font_style) {
            case SmilTextProperties::StyleOblique:
                s += "oblique;";
                break;
            case SmilTextProperties::StyleItalic:
                s += "italic;";
                break;
            default:
                s += "normal;";
                break;
        }
    }
    if (SmilTextProperties::WeightInherit != props.font_weight) {
        s += "font-weight:";
        switch (props.font_weight) {
            case SmilTextProperties::WeightBold:
                s += "bold;";
                break;
            default:
                s += "normal;";
                break;
        }
    }
    s += "\">";
    span_text = s;
}

void SmilTextVisitor::addRichText (const QString &txt) {
    if (!info.span_text.isEmpty ())
        rich_text += info.span_text;
    rich_text += txt;
    if (!info.span_text.isEmpty ())
        rich_text += "</span>";
}

void SmilTextVisitor::push () {
    if (!rich_text.isEmpty ()) {
        int pxw, pxh;
        float fs = info.props.font_size.size ();
        if (fs < 0)
            fs = TextMedia::defaultFontSize ();
        float maxfs = max_font_size;
        if (maxfs < 1.0)
            maxfs = fs;
        fs *= scale;
        maxfs *= scale;

        QFont font ("Sans", (int)fs);
        calculateTextDimensions (font, rich_text.toUtf8 ().constData (),
                width, 2 * maxfs, 1024, &pxw, &pxh, true, info.props.text_align);
        int x = 0;
        if (SmilTextProperties::AlignCenter == info.props.text_align)
            x = (width - pxw) / 2;
        else if (SmilTextProperties::AlignRight == info.props.text_align)
            x = width - pxw;
        SmilTextBlock *block = new SmilTextBlock (font, rich_text,
                IRect (x, voffset, pxw, pxh), info.props.text_align);
        voffset += pxh;
        max_font_size = 0;
        rich_text.clear();
        if (!first) {
            first = last = block;
        } else {
            last->next = block;
            last = block;
        }
    }
}

void SmilTextVisitor::visit (TextNode *text) {
    QString buf;
    QTextStream out (&buf, QIODevice::WriteOnly);
    out << XMLStringlet (text->nodeValue ());
    addRichText (buf);
    if (text->nextSibling ())
        text->nextSibling ()->accept (this);
}

void SmilTextVisitor::visit (SMIL::TextFlow *flow) {
    bool new_block = SMIL::id_node_p == flow->id ||
        SMIL::id_node_br == flow->id ||
        SMIL::id_node_div == flow->id;
    if ((new_block && !rich_text.isEmpty ()) || flow->firstChild ()) {
        float fs = info.props.font_size.size ();
        if (fs < 0)
            fs = TextMedia::defaultFontSize ();
        int par_extra = SMIL::id_node_p == flow->id
            ? (int)(scale * fs) : 0;
        voffset += par_extra;

        SmilTextInfo saved_info = info;
        if (new_block)
            push ();

        info.props.mask (flow->props);
        if ((float)info.props.font_size.size () > max_font_size)
            max_font_size = info.props.font_size.size ();
        info.span (scale);

        if (flow->firstChild ())
            flow->firstChild ()->accept (this);

        if (rich_text.isEmpty ())
            par_extra = 0;
        if (new_block && flow->firstChild ())
            push ();
        voffset += par_extra;

        info = saved_info;
    }
    if (flow->nextSibling ())
        flow->nextSibling ()->accept (this);
}

void SmilTextVisitor::visit (SMIL::TemporalMoment *tm) {
    if (tm->state >= Node::state_began
            && tm->nextSibling ())
        tm->nextSibling ()->accept (this);
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (SMIL::SmilText *txt) {
    Surface *s = txt->surface ();
    if (!s)
        return;

    SRect rect = s->bounds;
    IRect scr = matrix.toScreen (rect);

    if (!s->surface) {

        int w = scr.width ();
        float scale = 1.0 * w / (double)s->bounds.width ();
        SmilTextVisitor info (w, scale, txt->props);

        Node *first = txt->firstChild ();
        for (Node *n = first; n; n = n->nextSibling ())
            if (SMIL::id_node_clear == n->id) {
                if (n->state >= Node::state_began)
                    first = n->nextSibling ();
                else
                    break;
            }
        if (first)
            first->accept (&info);

        info.push ();
        if (info.first) {
            cairo_t *cr_txt = createContext (cairo_surface, s, (int) w, info.voffset);

            CAIRO_SET_SOURCE_RGB (cr_txt, 0);
            SmilTextBlock *b = info.first;
            int hoff = 0;
            int voff = 0;
            while (b) {
                cairo_translate (cr_txt, b->rect.x() - hoff, b->rect.y() - voff);
                QTextDocument td;
                td.setDocumentMargin (0);
                td.setDefaultFont (b->font);
                bool have_alpha = (s->background_color & 0xff000000) < 0xff000000;
                QImage img (QSize (b->rect.width(), b->rect.height()), have_alpha ? QImage::Format_ARGB32 : QImage::Format_RGB32);
                img.fill (s->background_color);
                td.setPageSize (QSize (b->rect.width(), b->rect.height() + 10));
                setAlignment (td, b->align);
                td.documentLayout()->setPaintDevice (&img);
                td.setHtml (b->rich_text);
                QPainter painter;
                painter.begin (&img);
                QAbstractTextDocumentLayout::PaintContext ctx;
                ctx.clip = QRect (QPoint (0, 0), img.size ());
                td.documentLayout()->draw (&painter, ctx);
                painter.end();

                cairo_surface_t *src_sf = cairo_image_surface_create_for_data (
                        img.bits (),
                        have_alpha ? CAIRO_FORMAT_ARGB32:CAIRO_FORMAT_RGB24,
                        img.width(), img.height(), img.bytesPerLine ());
                cairo_pattern_t *pat = cairo_pattern_create_for_surface (src_sf);
                cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
                cairo_set_operator (cr_txt, CAIRO_OPERATOR_SOURCE);
                cairo_set_source (cr_txt, pat);
                cairo_rectangle (cr_txt, 0, 0, b->rect.width(), b->rect.height());
                cairo_fill (cr_txt);
                cairo_pattern_destroy (pat);
                cairo_surface_destroy (src_sf);

                hoff = b->rect.x ();
                voff = b->rect.y ();

                SmilTextBlock *tmp = b;
                b = b->next;
                delete tmp;
            }
            cairo_destroy (cr_txt);

            // update bounds rect
            s->bounds = matrix.toUser(IRect(scr.point, ISize(w, info.voffset)));
            txt->size = s->bounds.size;
            txt->updateBounds (false);

            // update coord. for painting below
            scr = matrix.toScreen (s->bounds);
        }
    }
    IRect clip_rect = clip.intersect (scr);
    if (s->surface && !clip_rect.isEmpty ())
        paint (&txt->transition, txt->media_opacity, s, scr.point, clip_rect);
    s->dirty = false;
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (RP::Imfl * imfl) {
    if (imfl->surface ()) {
        cairo_save (cr);
        Matrix m = matrix;
        IRect scr = matrix.toScreen (SRect (0, 0, imfl->rp_surface->bounds.size));
        int w = scr.width ();
        int h = scr.height ();
        cairo_rectangle (cr, scr.x (), scr.y (), w, h);
        //cairo_clip (cr);
        cairo_translate (cr, scr.x (), scr.y ());
        cairo_scale (cr, 1.0*w/(double)imfl->size.width, 1.0*h/(double)imfl->size.height);
        if (imfl->needs_scene_img)
            cairo_push_group (cr);
        for (NodePtr n = imfl->firstChild (); n; n = n->nextSibling ())
            if (n->state >= Node::state_began &&
                    n->state < Node::state_deactivated) {
                RP::TimingsBase * tb = convertNode<RP::TimingsBase>(n);
                switch (n->id) {
                    case RP::id_node_viewchange:
                        if (!(int)tb->srcw)
                            tb->srcw = imfl->size.width;
                        if (!(int)tb->srch)
                            tb->srch = imfl->size.height;
                        // fall through
                    case RP::id_node_crossfade:
                    case RP::id_node_fadein:
                    case RP::id_node_fadeout:
                    case RP::id_node_fill:
                    case RP::id_node_wipe:
                        if (!(int)tb->w)
                            tb->w = imfl->size.width;
                        if (!(int)tb->h)
                            tb->h = imfl->size.height;
                        n->accept (this);
                        break;
                }
            }
        if (imfl->needs_scene_img) {
            cairo_pattern_t * pat = cairo_pop_group (cr);
            cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
            cairo_set_source (cr, pat);
            cairo_paint (cr);
            cairo_pattern_destroy (pat);
        }
        cairo_restore (cr);
        matrix = m;
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (RP::Fill * fi) {
    CAIRO_SET_SOURCE_RGB (cr, fi->color);
    if ((int)fi->w && (int)fi->h) {
        cairo_rectangle (cr, fi->x, fi->y, fi->w, fi->h);
        cairo_fill (cr);
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (RP::Fadein * fi) {
    if (fi->target && fi->target->id == RP::id_node_image) {
        RP::Image *img = convertNode <RP::Image> (fi->target);
        ImageMedia *im = img && img->media_info
            ? static_cast <ImageMedia*> (img->media_info->media) : NULL;
        if (im && img->surface ()) {
            Single sx = fi->srcx, sy = fi->srcy, sw = fi->srcw, sh = fi->srch;
            if (!(int)sw)
                sw = img->size.width;
            if (!(int)sh)
                sh = img->size.height;
            if ((int)fi->w && (int)fi->h && (int)sw && (int)sh) {
                if (!img->img_surface->surface)
                    im->cached_img->copyImage (img->img_surface,
                            img->size, cairo_surface, &penalty);
                cairo_matrix_t matrix;
                cairo_matrix_init_identity (&matrix);
                float scalex = 1.0 * sw / fi->w;
                float scaley = 1.0 * sh / fi->h;
                cairo_matrix_scale (&matrix, scalex, scaley);
                cairo_matrix_translate (&matrix,
                        1.0*sx/scalex - (double)fi->x,
                        1.0*sy/scaley - (double)fi->y);
                cairo_save (cr);
                cairo_rectangle (cr, fi->x, fi->y, fi->w, fi->h);
                cairo_pattern_t *pat = cairo_pattern_create_for_surface (img->img_surface->surface);
                cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
                cairo_pattern_set_matrix (pat, &matrix);
                cairo_set_source (cr, pat);
                cairo_clip (cr);
                cairo_paint_with_alpha (cr, 1.0 * fi->progress / 100);
                cairo_restore (cr);
                cairo_pattern_destroy (pat);
            }
        }
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (RP::Fadeout * fo) {
    if (fo->progress > 0) {
        CAIRO_SET_SOURCE_RGB (cr, fo->to_color);
        if ((int)fo->w && (int)fo->h) {
            cairo_save (cr);
            cairo_rectangle (cr, fo->x, fo->y, fo->w, fo->h);
            cairo_clip (cr);
            cairo_paint_with_alpha (cr, 1.0 * fo->progress / 100);
            cairo_restore (cr);
        }
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (RP::Crossfade * cf) {
    if (cf->target && cf->target->id == RP::id_node_image) {
        RP::Image *img = convertNode <RP::Image> (cf->target);
        ImageMedia *im = img && img->media_info
            ? static_cast <ImageMedia*> (img->media_info->media) : NULL;
        if (im && img->surface ()) {
            Single sx = cf->srcx, sy = cf->srcy, sw = cf->srcw, sh = cf->srch;
            if (!(int)sw)
                sw = img->size.width;
            if (!(int)sh)
                sh = img->size.height;
            if ((int)cf->w && (int)cf->h && (int)sw && (int)sh) {
                if (!img->img_surface->surface)
                    im->cached_img->copyImage (img->img_surface,
                            img->size, cairo_surface, &penalty);
                cairo_save (cr);
                cairo_matrix_t matrix;
                cairo_matrix_init_identity (&matrix);
                float scalex = 1.0 * sw / cf->w;
                float scaley = 1.0 * sh / cf->h;
                cairo_matrix_scale (&matrix, scalex, scaley);
                cairo_matrix_translate (&matrix,
                        1.0*sx/scalex - (double)cf->x,
                        1.0*sy/scaley - (double)cf->y);
                cairo_rectangle (cr, cf->x, cf->y, cf->w, cf->h);
                cairo_pattern_t *pat = cairo_pattern_create_for_surface (img->img_surface->surface);
                cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
                cairo_pattern_set_matrix (pat, &matrix);
                cairo_set_source (cr, pat);
                cairo_clip (cr);
                cairo_paint_with_alpha (cr, 1.0 * cf->progress / 100);
                cairo_restore (cr);
                cairo_pattern_destroy (pat);
            }
        }
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (RP::Wipe * wipe) {
    if (wipe->target && wipe->target->id == RP::id_node_image) {
        RP::Image *img = convertNode <RP::Image> (wipe->target);
        ImageMedia *im = img && img->media_info
            ? static_cast <ImageMedia*> (img->media_info->media) : NULL;
        if (im && img->surface ()) {
            Single x = wipe->x, y = wipe->y;
            Single tx = x, ty = y;
            Single w = wipe->w, h = wipe->h;
            Single sx = wipe->srcx, sy = wipe->srcy, sw = wipe->srcw, sh = wipe->srch;
            if (!(int)sw)
                sw = img->size.width;
            if (!(int)sh)
                sh = img->size.height;
            if (wipe->direction == RP::Wipe::dir_right) {
                Single dx = w * 1.0 * wipe->progress / 100;
                tx = x -w + dx;
                w = dx;
            } else if (wipe->direction == RP::Wipe::dir_left) {
                Single dx = w * 1.0 * wipe->progress / 100;
                tx = x + w - dx;
                x = tx;
                w = dx;
            } else if (wipe->direction == RP::Wipe::dir_down) {
                Single dy = h * 1.0 * wipe->progress / 100;
                ty = y - h + dy;
                h = dy;
            } else if (wipe->direction == RP::Wipe::dir_up) {
                Single dy = h * 1.0 * wipe->progress / 100;
                ty = y + h - dy;
                y = ty;
                h = dy;
            }

            if ((int)w && (int)h) {
                if (!img->img_surface->surface)
                    im->cached_img->copyImage (img->img_surface,
                            img->size, cairo_surface, &penalty);
                cairo_matrix_t matrix;
                cairo_matrix_init_identity (&matrix);
                float scalex = 1.0 * sw / wipe->w;
                float scaley = 1.0 * sh / wipe->h;
                cairo_matrix_scale (&matrix, scalex, scaley);
                cairo_matrix_translate (&matrix,
                        1.0*sx/scalex - (double)tx,
                        1.0*sy/scaley - (double)ty);
                cairo_pattern_t *pat = cairo_pattern_create_for_surface (img->img_surface->surface);
                cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
                cairo_pattern_set_matrix (pat, &matrix);
                cairo_set_source (cr, pat);
                cairo_rectangle (cr, x, y, w, h);
                cairo_fill (cr);
                cairo_pattern_destroy (pat);
            }
        }
    }
}

KDE_NO_EXPORT void CairoPaintVisitor::visit (RP::ViewChange * vc) {
    if (vc->unfinished () || vc->progress < 100) {
        cairo_pattern_t * pat = cairo_pop_group (cr); // from imfl
        cairo_pattern_set_extend (pat, CAIRO_EXTEND_NONE);
        cairo_push_group (cr);
        cairo_save (cr);
        cairo_set_source (cr, pat);
        cairo_paint (cr);
        if ((int)vc->w && (int)vc->h && (int)vc->srcw && (int)vc->srch) {
            cairo_matrix_t matrix;
            cairo_matrix_init_identity (&matrix);
            float scalex = 1.0 * vc->srcw / vc->w;
            float scaley = 1.0 * vc->srch / vc->h;
            cairo_matrix_scale (&matrix, scalex, scaley);
            cairo_matrix_translate (&matrix,
                    1.0*vc->srcx/scalex - (double)vc->x,
                    1.0*vc->srcy/scaley - (double)vc->y);
            cairo_pattern_set_matrix (pat, &matrix);
            cairo_set_source (cr, pat);
            cairo_rectangle (cr, vc->x, vc->y, vc->w, vc->h);
            cairo_fill (cr);
        }
        cairo_pattern_destroy (pat);
        cairo_restore (cr);
    }
}

//-----------------------------------------------------------------------------

namespace KMPlayer {

class KMPLAYER_NO_EXPORT MouseVisitor : public Visitor {
    ViewArea *view_area;
    Matrix matrix;
    NodePtrW source;
    const MessageType event;
    int x, y;
    bool handled;
    bool bubble_up;

    bool deliverAndForward (Node *n, Surface *s, bool inside, bool deliver);
    void surfaceEvent (Node *mt, Surface *s);
public:
    MouseVisitor (ViewArea *v, MessageType evt, Matrix m, int x, int y);
    KDE_NO_CDTOR_EXPORT ~MouseVisitor () {}
    using Visitor::visit;
    void visit (Node * n);
    void visit (Element *);
    void visit (SMIL::Smil *);
    void visit (SMIL::Layout *);
    void visit (SMIL::RegionBase *);
    void visit (SMIL::MediaType * n);
    void visit (SMIL::SmilText * n);
    void visit (SMIL::Anchor *);
    void visit (SMIL::Area *);

};

} // namespace

KDE_NO_CDTOR_EXPORT
MouseVisitor::MouseVisitor (ViewArea *v, MessageType evt, Matrix m, int a, int b)
  : view_area (v), matrix (m), event (evt), x (a), y (b),
    handled (false), bubble_up (false) {
}

KDE_NO_EXPORT void MouseVisitor::visit (Node * n) {
    debugLog () << "Mouse event ignored for " << n->nodeName () << endl;
}

KDE_NO_EXPORT void MouseVisitor::visit (SMIL::Smil *s) {
    if (s->active () && s->layout_node)
        s->layout_node->accept (this);
}

KDE_NO_EXPORT void MouseVisitor::visit (SMIL::Layout * layout) {
    if (layout->root_layout)
        layout->root_layout->accept (this);
}

KDE_NO_EXPORT void MouseVisitor::visit (SMIL::RegionBase *region) {
    Surface *s = (Surface *) region->role (RoleDisplay);
    if (s) {
        SRect rect = s->bounds;
        IRect scr = matrix.toScreen (rect);
        int rx = scr.x(), ry = scr.y(), rw = scr.width(), rh = scr.height();
        handled = false;
        bool inside = x > rx && x < rx+rw && y > ry && y< ry+rh;
        if (!inside && (event == MsgEventClicked || !s->has_mouse))
            return;

        if (event == MsgEventClicked && !s->virtual_size.isEmpty () &&
                x > rx + rw - REGION_SCROLLBAR_WIDTH) {
            const int sbh = rh - REGION_SCROLLBAR_WIDTH;
            const int vy = s->virtual_size.height;
            const int knob_h = sbh * rh / vy;
            int knob_y = y - ry - 0.5 * knob_h;
            if (knob_y < 0)
                knob_y = 0;
            else if (knob_y + knob_h > sbh)
                knob_y = sbh - knob_h;
            s->y_scroll = vy * knob_y / sbh;
            view_area->scheduleRepaint (scr);
            return;
        }
        if (event == MsgEventClicked && !s->virtual_size.isEmpty () &&
                y > ry + rh - REGION_SCROLLBAR_WIDTH) {
            const int sbw = rw - REGION_SCROLLBAR_WIDTH;
            const int vw = s->virtual_size.width;
            const int knob_w = sbw * rw / vw;
            int knob_x = x - rx - 0.5 * knob_w;
            if (knob_x < 0)
                knob_x = 0;
            else if (knob_x + knob_w > sbw)
                knob_x = sbw - knob_w;
            s->x_scroll = vw * knob_x / sbw;
            view_area->scheduleRepaint (scr);
            return;
        }

        NodePtrW src = source;
        source = region;
        Matrix m = matrix;
        matrix = Matrix (rect.x(), rect.y(), 1.0, 1.0);
        matrix.transform (m);
        if (!s->virtual_size.isEmpty ())
            matrix.translate (-s->x_scroll, -s->y_scroll);
        bubble_up = false;

        bool child_handled = false;
        if (inside || s->has_mouse)
            for (SurfacePtr c = s->firstChild (); c; c = c->nextSibling ()) {
                if (c->node && c->node->id == SMIL::id_node_region) {
                    c->node->accept (this);
                    child_handled |= handled;
                    if (!source || !source->active ())
                        break;
                } else {
                    break;
                }
            }
        child_handled &= !bubble_up;
        bubble_up = false;
        if (source && source->active ())
            deliverAndForward (region, s, inside, !child_handled);

        handled = inside;
        matrix = m;
        source = src;
    }
}

static void followLink (SMIL::LinkingBase * link) {
    debugLog () << "link to " << link->href << " clicked" << endl;
    if (link->href.startsWith ("#")) {
        SMIL::Smil * s = SMIL::Smil::findSmilNode (link);
        if (s)
            s->jump (link->href.mid (1));
        else
            errorLog() << "In document jumps smil not found" << endl;
    } else {
        PlayListNotify *notify = link->document ()->notify_listener;
        if (notify && !link->target.isEmpty ()) {
             notify->openUrl (link->href, link->target, QString ());
        } else {
            NodePtr n = link;
            for (NodePtr p = link->parentNode (); p; p = p->parentNode ()) {
                if (n->mrl () && n->mrl ()->opener == p) {
                    p->setState (Node::state_deferred);
                    p->mrl ()->setParam (Ids::attr_src, link->href, 0L);
                    p->activate ();
                    break;
                }
                n = p;
            }
        }
    }
}

KDE_NO_EXPORT void MouseVisitor::visit (SMIL::Anchor * anchor) {
    if (event == MsgEventPointerMoved)
        ;// set cursor to hand
    else if (event == MsgEventClicked)
        followLink (anchor);
}

KDE_NO_EXPORT void MouseVisitor::visit (SMIL::Area * area) {
    NodePtr n = area->parentNode ();
    Surface *s = (Surface *) n->role (RoleDisplay);
    if (s) {
        SRect rect = s->bounds;
        IRect scr = matrix.toScreen (rect);
        int w = scr.width (), h = scr.height ();
        if (area->nr_coords > 1) {
            Single left = area->coords[0].size (rect.width ());
            Single top = area->coords[1].size (rect.height ());
            matrix.getXY (left, top);
            if (x < left || x > left + w || y < top || y > top + h)
                return;
            if (area->nr_coords > 3) {
                Single right = area->coords[2].size (rect.width ());
                Single bottom = area->coords[3].size (rect.height ());
                matrix.getXY (right, bottom);
                if (x > right || y > bottom)
                    return;
            }
        }
        if (event == MsgEventPointerMoved)
            ;//cursor.setShape (Qt::PointingHandCursor);
        else {
            ConnectionList *nl = nodeMessageReceivers (area, event);
            if (nl)
                for (Connection *c = nl->first(); c; c = nl->next ()) {
                    if (c->connecter)
                        c->connecter->accept (this);
                    if (!source || !source->active ())
                        return;
                }
            if (event == MsgEventClicked && !area->href.isEmpty ())
                followLink (area);
        }
    }
}

KDE_NO_EXPORT void MouseVisitor::visit (Element *elm) {
    Runtime *rt = (Runtime *) elm->role (RoleTiming);
    if (rt) {
        Posting mouse_event (source, event);
        rt->message (event, &mouse_event);
    }
}

bool MouseVisitor::deliverAndForward (Node *node, Surface *s, bool inside, bool deliver) {
    bool forward = deliver;
    MessageType user_event = event;
    if (event == MsgEventPointerMoved) {
        forward = true; // always pass move events
        if (inside && !s->has_mouse) {
            deliver = true;
            user_event = MsgEventPointerInBounds;
        } else if (!inside && s->has_mouse) {
            deliver = true;
            user_event = MsgEventPointerOutBounds;
        } else if (!inside) {
            return false;
        } else {
            deliver = false;
        }
    }
    s->has_mouse = inside;

    if (event != MsgEventPointerMoved && !inside)
        return false;

    NodePtrW node_save = node;

    if (forward) {
        ConnectionList *nl = nodeMessageReceivers (node, MsgSurfaceAttach);
        if (nl) {
            NodePtr node_save = source;
            source = node;

            for (Connection *c = nl->first(); c; c = nl->next ()) {
                if (c->connecter)
                    c->connecter->accept (this);
                if (!source || !source->active ())
                    break;
            }
            source = node_save;
        }
    }
    if (!node_save || !node->active ())
        return false;
    if (deliver) {
        Posting mouse_event (node, user_event);
        node->deliver (user_event, &mouse_event);
    }
    if (!node_save || !node->active ())
        return false;
    return true;
}

void MouseVisitor::surfaceEvent (Node *node, Surface *s) {
    if (!s)
        return;
    if (s->node && s->node.ptr () != node) {
        s->node->accept (this);
        return;
    }
    SRect rect = s->bounds;
    IRect scr = matrix.toScreen (rect);
    int rx = scr.x(), ry = scr.y(), rw = scr.width(), rh = scr.height();
    const bool inside = x > rx && x < rx+rw && y > ry && y< ry+rh;
    const bool had_mouse = s->has_mouse;
    if (deliverAndForward (node, s, inside, true) &&
            (inside || s->has_mouse) &&
            s->firstChild () && s->firstChild ()->node) {
        Matrix m = matrix;
        matrix = Matrix (rect.x(), rect.y(), s->xscale, s->yscale);
        matrix.transform (m);
        s->firstChild ()->node->accept (this);
        matrix = m;
    }
}

KDE_NO_EXPORT void MouseVisitor::visit (SMIL::MediaType *mt) {
    if (mt->sensitivity == SMIL::MediaType::sens_transparent)
        bubble_up = true;
    else
        surfaceEvent (mt, mt->surface ());
}

KDE_NO_EXPORT void MouseVisitor::visit (SMIL::SmilText *st) {
    surfaceEvent (st, st->surface ());
}

//-----------------------------------------------------------------------------

namespace KMPlayer {
class KMPLAYER_NO_EXPORT ViewAreaPrivate {
public:
    ViewAreaPrivate (ViewArea *v)
     : m_view_area (v), backing_store (0), have_gc (false) {
    }

    ~ViewAreaPrivate() {
        destroyBackingStore ();
        if (have_gc)
            XFreeGC (QX11Info::display(), gc);
    }
    void clearSurface (Surface *s) {
        if (s->surface) {
            cairo_surface_destroy (s->surface);
            s->surface = 0L;
        }
        destroyBackingStore ();
    }
    void resizeSurface (Surface *s, int w, int h) {
        if (s->surface && (w != width || h != height)) {
                //cairo_xlib_surface_set_size (s->surface, w, h);
            clearSurface (s);
            width = w;
            height = h;
        }
    }
    cairo_surface_t *createSurface (int w, int h) {
        Display *dpy = QX11Info::display ();
        destroyBackingStore ();
        width = w;
        height = h;
        backing_store = XCreatePixmap (dpy, m_view_area->winId(), w, h, QX11Info::appDepth ());
        return cairo_xlib_surface_create (dpy, backing_store,
                DefaultVisual (dpy, DefaultScreen (dpy)), w, h);
        /*return cairo_xlib_surface_create_with_xrender_format (
            QX11Info::display (),
            id,
            DefaultScreenOfDisplay (QX11Info::display ()),
            XRenderFindVisualFormat (QX11Info::display (),
                DefaultVisual (QX11Info::display (),
                    DefaultScreen (QX11Info::display ()))),
            w, h);*/
    }
    void swapBuffer (int sx, int sy, int sw, int sh, int dx, int dy) {
        if (!have_gc) {
            XGCValues values;
            values.graphics_exposures = false;
            values.function = GXcopy;
            values.fill_style = FillSolid;
            values.subwindow_mode = ClipByChildren;
            gc = XCreateGC (QX11Info::display(), backing_store,
                    GCGraphicsExposures | GCFunction |
                    GCFillStyle | GCSubwindowMode, &values);
            have_gc = true;
        }
        XCopyArea (QX11Info::display(), backing_store, m_view_area->winId(),
                gc, sx, sy, sw, sh, dx, dy);
        XFlush (QX11Info::display());
    }
    void destroyBackingStore () {
        if (backing_store)
            XFreePixmap (QX11Info::display (), backing_store);
        backing_store = 0;
    }
    ViewArea *m_view_area;
    Drawable backing_store;
    GC gc;
    int width;
    int height;
    bool have_gc;
};

}

//-----------------------------------------------------------------------------

Button::Button (QWidget *parent, const char *icon_path)
  : QWidget (parent), icon (icon_path)
{
    setAttribute (Qt::WA_DontCreateNativeAncestors);
    setAttribute (Qt::WA_NativeWindow);
    QPalette pal = palette ();
    pal.setColor(backgroundRole(), QColor (0x08, 0x08, 0x10));
    setPalette (pal);
    setVisible (false);
}

void Button::activate ()
{
    emit clicked();
}

void Button::mousePressEvent (QMouseEvent *)
{
    activate ();
}

void Button::paintEvent (QPaintEvent *e)
{
    QPainter p;
    p.begin (this);
    p.drawImage (QPoint (8, 8), QImage (icon));
    p.end ();
}

//-----------------------------------------------------------------------------

static QCoreApplication::EventFilter old_X11_event_filter;
static WId x11_embed_window;
static ViewArea *current_view_area;

static bool x11EventFilter(void *msg, long *result)
{
    XEvent *xe = reinterpret_cast<XEvent *>(msg);
    switch (xe->type) {
        case ButtonPress:
            if (xe->xany.window == x11_embed_window) {
                QMouseEvent qev (QEvent::MouseButtonPress,
                        current_view_area->mapFromGlobal (QPoint (xe->xbutton.x_root, xe->xbutton.y_root)),
                        QPoint (xe->xbutton.x_root, xe->xbutton.y_root),
                        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                current_view_area->mousePressEvent (&qev);
                return true;
            }
            break;
        case ButtonRelease:
            if (xe->xany.window == x11_embed_window) {
                /*QMouseEvent qev (QEvent::MouseButtonRelease,
                        current_view_area->mapFromGlobal (QPoint (xe->xbutton.x_root, xe->xbutton.y_root)),
                        QPoint (xe->xbutton.x_root, xe->xbutton.y_root),
                        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                current_view_area->mouseReleaseEvent (&qev);*/
                XSetInputFocus (xe->xany.display, xe->xany.window, RevertToParent, CurrentTime);
                XFlush (xe->xany.display);
                return true;
            }
            if (xe->xany.window == current_view_area->winId ()) {
                XSetInputFocus (xe->xany.display, xe->xany.window, RevertToParent, CurrentTime);
                XFlush (xe->xany.display);
            }
            break;
        default:
            if (xe->xany.window == x11_embed_window) {
                return true;
            }
    }

    if (old_X11_event_filter && old_X11_event_filter != &x11EventFilter)
        return old_X11_event_filter (msg, result);
    else
        return false;
}

KDE_NO_CDTOR_EXPORT ViewArea::ViewArea (QWidget *parent)
 : //QWidget (parent),
   d (new ViewAreaPrivate (this)),
   m_scale_button (new Button (this, "/usr/share/themes/blanco/meegotouch/icons/icon-m-image-edit-resize.png")),
   m_stop_button (new Button (this, "/usr/share/themes/blanco/meegotouch/icons/icon-m-input-methods-close.png")),
   m_zoom_button (new Button (this, "/usr/share/themes/blanco/meegotouch/icons/icon-m-input-add.png")),
   m_back_button (new Button (this, "/usr/share/themes/blanco/meegotouch/icons/icon-m-toolbar-previous-selected.png")),
   aspect (0.0),
   //video_input (NULL),
   surface (new Surface (this)),
   last_repaint_time (0),
   next_repaint_time (0),
   penalty (0),
   cur_timeout (0),
   m_repaint_timer (0),
   m_button_timer (0),
   m_fullscreen (false),
   m_event_filter_on (false),
   m_updaters_enabled (true),
   m_video_filter (true),
   m_use_aspects (true)
{
    current_view_area = this;
    setWindowState (windowState() | Qt::WindowFullScreen);
    setAttribute (Qt::WA_StaticContents);
    setAttribute (Qt::WA_DontCreateNativeAncestors);
    setAttribute (Qt::WA_PaintOnScreen);
    setAttribute (Qt::WA_NativeWindow);
    setAttribute (Qt::WA_DeleteOnClose, false);
    setAttribute (Qt::WA_OpaquePaintEvent);
    setAttribute (Qt::WA_NoSystemBackground);
    QPalette pal = palette ();
    pal.setColor(backgroundRole(), QColor (8, 12, 12));
    setPalette (pal);
    old_X11_event_filter = QCoreApplication::instance()->setEventFilter(x11EventFilter);
    setVisible (false);
    m_video_output = new VideoOutput (this);
    //kmplayer_app->m_view_area = this;
}

KDE_NO_CDTOR_EXPORT ViewArea::~ViewArea () {
    reset ();
    delete d;
    current_view_area = NULL;
}

KDE_NO_EXPORT void ViewArea::reset () {
    if (m_repaint_timer) {
        killTimer (m_repaint_timer);
        m_repaint_timer = 0;
    }
    if (m_button_timer) {
        m_scale_button->setVisible (false);
        m_stop_button->setVisible (false);
        m_zoom_button->setVisible (false);
        m_back_button->setVisible (false);
        killTimer (m_button_timer);
        m_button_timer = 0;
    }
    aspect = 0.0;
    m_use_aspects = true;
}

void ViewArea::closeEvent (QCloseEvent *event)
{
    m_stop_button->activate ();
    return QWidget::closeEvent (event);
}

void ViewArea::mousePressEvent (QMouseEvent *e) {
    if (surface->node) {
        struct timeval tv;
        surface->node->document ()->timeOfDay (tv); // update last_event_time
        MouseVisitor visitor (this, MsgEventClicked,
                Matrix (surface->bounds.x (), surface->bounds.y (),
                    surface->xscale, surface->yscale),
                e->x(), e->y());
        surface->node->accept (&visitor);
    }
    int w = width (), h = height ();
    if ((e->x () < w - 70 || e->y () > h/2 + 70)
            && (e->x () > 70 || e->y () < h - 70)) {
        if (!m_button_timer) {
            m_stop_button->setGeometry (w - 64, 0, 64, 64);
            m_stop_button->setVisible (true);
            m_stop_button->raise();
            m_back_button->setGeometry (0, h - 64, 64, 64);
            m_back_button->setVisible (true);
            m_back_button->raise();
            if (video_node && Mrl::SingleMode == video_node->mrl ()->view_mode) {
                m_scale_button->setGeometry (w - 64, h/2 - 64, 64, 64);
                m_scale_button->setVisible (true);
                m_scale_button->raise();
                m_zoom_button->setGeometry (w - 64, h/2, 64, 64);
                m_zoom_button->setVisible (true);
                m_zoom_button->raise();
            }
        } else {
            killTimer (m_button_timer);
        }
        m_button_timer = startTimer (2000);
    }
}

void ViewArea::mouseDoubleClickEvent (QMouseEvent *) {
}

KDE_NO_EXPORT void ViewArea::keyPressEvent (QKeyEvent *e) {
    if (surface->node) {
        QString txt = e->text ();
        if (!txt.isEmpty ())
            surface->node->document ()->message (MsgAccessKey,
                    (void *)(long) txt[0].unicode ());
    }
}

KDE_NO_EXPORT void ViewArea::syncVisual (const IRect & rect) {
    int ex = rect.x ();
    if (ex > 0)
        ex--;
    int ey = rect.y ();
    if (ey > 0)
        ey--;
    int ew = rect.width () + 2;
    int eh = rect.height () + 2;
    if (!surface->surface)
        surface->surface = d->createSurface (width (), height ());
    {
        CairoPaintVisitor visitor (this, surface->surface,
                Matrix (surface->bounds.x(), surface->bounds.y(),
                    surface->xscale, surface->yscale),
                IRect (ex, ey, ew, eh), true);
        if (surface->node)
            surface->node->accept (&visitor);
        penalty = visitor.penalty;
    }
    cairo_surface_flush (surface->surface);
    d->swapBuffer (ex, ey, ew, eh, ex, ey);

    m_repaint_rect = IRect ();
}

QPaintEngine *ViewArea::paintEngine () const
{
    return NULL;
}

void ViewArea::paintEvent (QPaintEvent *e) {
    scheduleRepaint (IRect (e->rect ().x (), e->rect ().y (), e->rect ().width (), e->rect ().height ()));
}

KDE_NO_EXPORT void ViewArea::updateSurfaceBounds (int w, int h) {
    d->resizeSurface (surface.ptr (), w, h);
    surface->resize (SRect (0, 0, w, h));
    if (surface->node) {
        surface->node->message (MsgSurfaceBoundsUpdate, (void *) true);
    }
    scheduleRepaint (IRect (0, 0, w, h));
}

KDE_NO_EXPORT void ViewArea::resizeEvent (QResizeEvent *) {
    int w = width (), h = height ();
    if (w <= 0 || h <= 0)
        return;
    updateSurfaceBounds (w, h);
    if (video_node && Mrl::SingleMode == video_node->mrl ()->view_mode)
        setAudioVideoGeometry (IRect (0, 0, w, h), 0L);
}

KDE_NO_EXPORT
void ViewArea::setAudioVideoGeometry (const IRect &rect, unsigned int * bg_color) {
    int x = rect.x (), y = rect.y (), w = rect.width (), h = rect.height ();
    if (m_use_aspects && !surface->node && aspect > 0.01) {
        int w1 = w;
        int h1 = h;
        w = int (h * aspect);
        if (w > w1) {
            h = int (w1 / aspect);
            w = w1;
        }
        x += (w1 - w) / 2;
        y += (h1 - h) / 2;
    }
    IRect wrect = IRect (x, y, w, h);
    if (m_av_geometry != wrect) {
        if (m_av_geometry.width () > 0 && m_av_geometry.height () > 0)
            scheduleRepaint (IRect (m_av_geometry.x (), m_av_geometry.y (),
                    m_av_geometry.width (), m_av_geometry.height ()));
        m_av_geometry = IRect (x, y, w, h);
        debugLog() << "ViewArea::setAudioVideoGeometry " << x << "," << y << " "
            << w << "x" << h << endl;
        m_video_output->setGeometry (IRect (x, y , w, h));
        enableVideoInputFilter (m_video_filter);
    }
}

void ViewArea::enableVideoInputFilter (bool enable) {
    int x, y, w, h;
    m_video_filter = enable;
    if (enable) {
        x = m_av_geometry.x ();
        y = m_av_geometry.y ();
        w = m_av_geometry.width();
        h = m_av_geometry.height ();
    } else {
        x = y = -100;
        w = h = 50;
    }
    //if (video_input)
        //gdk_window_move_resize (video_input, x, y, w, h);

}

KDE_NO_EXPORT void ViewArea::setAudioVideoNode (NodePtr n) {
    video_node = n;
    m_av_geometry = IRect ();
    if (!n || Mrl::WindowMode == n->mrl ()->view_mode)
        setAudioVideoGeometry (IRect (-100, -100, 50, 50), 0L);
}

KDE_NO_EXPORT
void ViewArea::setAspect (float a) {
    aspect = a;
    /*if (aspect > 0.01) {
        int w, h, x, y;
        gdk_drawable_get_size (video, &w, &h);
        gdk_window_get_position (video, &x, &y);
        scheduleRepaint (x, y, w, h);
        int w1 = int (h * aspect);
        int h1 = h;
        if (w1 > w) {
            h1 = int (w / aspect);
            w1 = w;
        }
        int xoff = (w - w1) / 2;
        int yoff = (h - h1) / 2;
        if (xoff > 0 && yoff > 0)
            gdk_window_move_resize (video, x + xoff, y + yoff, w1, h1);
    }*/
}

KDE_NO_EXPORT Surface *ViewArea::getSurface (Mrl *mrl) {
    int w = width (), h = height ();
    surface->clear ();
    surface->node = mrl;
    //m_view->viewer()->resetBackgroundColor ();
    if (mrl) {
        updateSurfaceBounds (w, h);
        return surface.ptr ();
    }
    d->clearSurface (surface.ptr ());
    scheduleRepaint (IRect (0, 0, w, h));
    return 0L;
}

KDE_NO_EXPORT void ViewArea::scheduleRepaint (const IRect &rect) {
    if (m_repaint_timer) {
        m_repaint_rect = m_repaint_rect.unite (rect);
    } else {
        m_repaint_rect = rect;
        scheduleRepaintTimeout (surface->node
                ? surface->node->document ()->last_event_time
                : 0);
    }
}

KDE_NO_EXPORT ConnectionList *ViewArea::updaters () {
    if (!m_repaint_timer)
        scheduleRepaintTimeout (0);
    return &m_updaters;
}

KDE_NO_EXPORT
void ViewArea::enableUpdaters (bool enable, unsigned int skip) {
    m_updaters_enabled = enable;
    Connection *connect = m_updaters.first ();
    if (enable && connect) {
        UpdateEvent event (connect->connecter->document (), skip);
        for (; connect; connect = m_updaters.next ())
            if (connect->connecter)
                connect->connecter->message (MsgSurfaceUpdate, &event);
        if (!m_repaint_timer)
            scheduleRepaintTimeout (event.cur_event_time);
    } else if (!enable && m_repaint_timer && m_repaint_rect.isEmpty ()) {
        killTimer (m_repaint_timer);
        m_repaint_timer = 0;
    }
}

KDE_NO_EXPORT void ViewArea::timerEvent (QTimerEvent * e) {
    if (e->timerId() == m_repaint_timer) {
        Connection *connect = m_updaters.first ();
        int count = 0;
        if (m_updaters_enabled && connect) {
            UpdateEvent event (connect->connecter->document (), 0);
            next_repaint_time = event.cur_event_time;
            for (; connect; count++, connect = m_updaters.next ())
                if (connect->connecter)
                    connect->connecter->message (MsgSurfaceUpdate, &event);
        }
        if (!m_repaint_rect.isEmpty ())
            syncVisual (m_repaint_rect);
        last_repaint_time = next_repaint_time;
        if (m_updaters_enabled && m_updaters.first ()) {
            scheduleRepaintTimeout (last_repaint_time);
        } else {
            killTimer (m_repaint_timer);
            m_repaint_timer = 0;
        }
    } else if (e->timerId() == m_button_timer) {
        m_scale_button->setVisible (false);
        m_stop_button->setVisible (false);
        m_zoom_button->setVisible (false);
        m_back_button->setVisible (false);
        killTimer (m_button_timer);
        m_button_timer = 0;
    } else {
        errorLog () << "ViewArea invalid timer\n";
        killTimer(e->timerId());
    }
}

bool ViewArea::scheduleRepaintTimeout (unsigned int cur_time) {
    int timeout;
    if (!cur_time) {
        timeout = 20;
    } else {
        Document *doc = surface->node ? surface->node->document () : NULL;
        if (doc && doc->last_event_time < last_repaint_time) {
            last_repaint_time = doc->last_event_time;
            penalty = 30;
        }
        if (last_repaint_time + penalty < cur_time)
            timeout = 20;
        else
            timeout = 20 + penalty - (cur_time - last_repaint_time);
    }
    next_repaint_time = cur_time + timeout;
    if (m_repaint_timer && abs (cur_timeout - timeout) < 5)
        return true;
    cur_timeout = timeout;
    if (m_repaint_timer)
        killTimer (m_repaint_timer);
    m_repaint_timer = startTimer (cur_timeout);
    return false;
}

void ViewArea::prepareFullscreenToggle () {
    // MPlayer+omapfb stays black in window mode if having the focus
    /*Window w = GDK_DRAWABLE_XID (GDK_DRAWABLE (widget->window));
    Display *dpy = gdk_x11_display_get_xdisplay(gtk_widget_get_display(widget));

    XSetInputFocus (dpy, w, RevertToParent, CurrentTime);
    XFlush (dpy);*/
}

static void setXSelectInput (Display *dpy, Window wid, long mask) {
    XWindowAttributes attr;
    XGetWindowAttributes (dpy, wid, &attr);
    long m = mask | attr.your_event_mask;
    XSelectInput (dpy, wid, m);

    Window r, p, *c;
    unsigned int nr;
    if (XQueryTree (dpy, wid, &r, &p, &c, &nr) && c) {
        for (int i = 0; i < nr; ++i)
            setXSelectInput (dpy, c[i], mask);
        XFree (c);
    }
}

static bool findWindowAt (Display *dpy, Window wid, long mask, int *x, int *y,
        Window *window, Window *sub_window) {
    XWindowAttributes attr;
    XGetWindowAttributes (dpy, wid, &attr);
    Window r, p, *c;
    unsigned int nr;
    *window = 0;
    if (XQueryTree (dpy, wid, &r, &p, &c, &nr)) {
        for (int i = nr-1; i >= 0; --i) {
            XWindowAttributes cattr;
            XGetWindowAttributes (dpy, c[i], &cattr);
            if (*x >= cattr.x && *x <= cattr.x + cattr.width &&
                    *y >= cattr.y && *y <= cattr.y + cattr.height) {
                int x1 = *x - cattr.x;
                int y1 = *y - cattr.y;
                *sub_window = c[i];
                if (findWindowAt(dpy, c[i], mask, &x1, &y1, window, sub_window))
                {
                    *x = x1;
                    *y = y1;
                    break;
                }
            }
        }
        XFree (c);
    }
    if (!*window && attr.all_event_masks & mask)
        *window = wid;
    return *window;

}

static void setXSelectInput (WId wid, long mask) {
    WId r, p, *c;
    unsigned int nr;
    XSelectInput (QX11Info::display (), wid, mask);
    if (XQueryTree (QX11Info::display (), wid, &r, &p, &c, &nr)) {
        for (int i = 0; i < nr; ++i)
            setXSelectInput (c[i], mask);
        XFree (c);
    }
}

bool ViewArea::x11Event (XEvent *xe) {
    switch (xe->type) {
        case UnmapNotify:
            if (m_video_output->windowHandle () == xe->xunmap.event) {
                //m_view->videoStart ();
                return false;
            }
            break;
        case XKeyPress:
            if (m_video_output->windowHandle () == xe->xkey.window) {
                if (m_video_output->inputMask() & KeyPressMask) {
                    KeySym ksym;
                    char kbuf[16];
                    XLookupString (&xe->xkey, kbuf, sizeof(kbuf), &ksym, NULL);
                    switch (ksym) {
                        case XK_f:
                        case XK_F:
                            //m_view->fullScreen ();
                            break;
                    }
                }
                return false;
            }
            break;
        /*case ColormapNotify:
            fprintf (stderr, "colormap notify\n");
            return true;*/
        case MotionNotify:
            /*if (m_view->controlPanelMode () == View::CP_AutoHide) {
                const VideoWidgetList::iterator e = video_widgets.end ();
                for (VideoWidgetList::iterator i=video_widgets.begin(); i != e; ++i) {
                    QPoint p = mapToGlobal (QPoint (0, 0));
                    int x = xe->xmotion.x_root - p.x ();
                    int y = xe->xmotion.y_root - p.y ();
                    m_view->mouseMoved (x, y);
                    if (x > 0 && x < width () && y > 0 && y < height ())
                        mouseMoved ();
                }
            }*/
            break;
        case MapNotify:
            if (!xe->xmap.override_redirect) {
                WId p = xe->xmap.event;
                WId w = xe->xmap.window;
                WId v = m_video_output->windowHandle ();
                WId va = winId ();
                WId root = 0;
                WId *children;
                unsigned int nr;
                while (p != v &&
                        XQueryTree (QX11Info::display (), w, &root,
                            &p, &children, &nr)) {
                    if (nr)
                        XFree (children);
                    if (p == va || p == v || p == root)
                        break;
                    w = p;
                }
                if (p == v)
                    setXSelectInput (xe->xmap.window, m_video_output->inputMask ());
            }
            /*if (e->xmap.event == m_viewer->clientWinId ()) {
              show ();
              QTimer::singleShot (10, m_viewer, SLOT (sendConfigureEvent ()));
              }*/
            break;
        /*case ConfigureNotify:
            break;
            //return true;*/
        default:
            break;
    }
    return false;
}

void ViewArea::showEvent (QShowEvent*) {
    emit videoWidgetVisible (true);
}

void ViewArea::hideEvent (QHideEvent*) {
    emit videoWidgetVisible (false);
}

/*KDE_NO_EXPORT void ViewArea::setOutputWidget (Application *app, GtkWidget * w) {
    widget = w;
    gdk_window_clear (w->window);
    gdk_window_focus (widget->window, CurrentTime);
    if (surface->surface)
        d->clearSurface (surface.ptr ());
    if (!video_input) {
        GdkWindowAttr attributes;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.x = w->allocation.x;
        attributes.y = w->allocation.y;
        attributes.width = w->allocation.width;
        attributes.height = w->allocation.height;
        attributes.wclass = GDK_INPUT_ONLY;
        attributes.visual = gtk_widget_get_visual (w);
        attributes.event_mask = gtk_widget_get_events (w) |
            GDK_EXPOSURE_MASK | GDK_STRUCTURE_MASK |
            GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
        attributes.event_mask &= ~(GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
        gint mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_WMCLASS;
        video_input = gdk_window_new (w->window, &attributes, mask);
        gdk_window_show (video_input);

        setAudioVideoGeometry (IRect (-100, -100, 50, 50), 0L);

        setEventFiltering (app, true);
    }
    //resizeEvent ();
}*/

unsigned long ViewArea::videoXId () const {
    return m_video_output->windowHandle ();
}

//------------------------------------------------------------------------------

namespace std {
    // used to sort the paint regions
    template<>
    bool less<IRect>::operator () (const IRect &r1, const IRect &r2) const {
        int d = r1.size.width * r1.size.height - r2.size.width * r2.size.height;
        if (d < 0)
            return true;
        if (d > 0)
            return false;
        if (r1.point.x == r2.point.x)
            return r1.point.y < r2.point.y;
        return r1.point.x < r2.point.x;
    }
}

KDE_NO_CDTOR_EXPORT VideoOutput::VideoOutput (ViewArea *view)
  : QObject (view),
    m_plain_window (0),
    m_bgcolor (0), m_aspect (0.0),
    m_view (view) {
    /*XWindowAttributes xwa;
    XGetWindowAttributes (QX11Info::display(), winId (), &xwa);
    XSetWindowAttributes xswa;
    xswa.background_pixel = 0;
    xswa.border_pixel = 0;
    xswa.colormap = xwa.colormap;
    create (XCreateWindow (QX11Info::display()), parent->winId (), 0, 0, 10, 10, 0, 
                           x11Depth (), InputOutput, (Visual*)x11Visual (),
                           CWBackPixel | CWBorderPixel | CWColormap, &xswa));*/
    debugLog () << "VideoOutput::VideoOutput" << endl;
    useIndirectWidget (true);
    setMonitoring (MonitorAll);
    setXSelectInput (windowHandle (), m_input_mask);
    //setProtocol (QXEmbed::XPLAIN);
}

KDE_NO_CDTOR_EXPORT VideoOutput::~VideoOutput () {
    debugLog () << "VideoOutput::~VideoOutput" << endl;
}

void VideoOutput::useIndirectWidget (bool inderect) {
    debugLog  () << "setIntermediateWindow " << !!m_plain_window << "->" << inderect << endl;
    if (!windowHandle () || !!m_plain_window != inderect) {
        if (inderect) {
            if (!m_plain_window) {
                XSetWindowAttributes xswa;
                xswa.background_pixel = 0x000842;
                xswa.border_pixel = 0;
                xswa.backing_store = NotUseful;
                xswa.bit_gravity = StaticGravity;
                m_plain_window = XCreateWindow (
                        QX11Info::display(),
                        m_view->winId (),
                        0, 0, 50, 50,
                        0, CopyFromParent, InputOutput,
                        CopyFromParent,
                        CWBackPixel|CWBorderPixel|CWBitGravity|CWBackingStore,
                        &xswa);
                x11_embed_window = m_plain_window;
                XMapWindow (QX11Info::display(), m_plain_window);
                XSync (QX11Info::display (), false);
                //embedClient (m_plain_window);
            }
            XClearWindow (QX11Info::display(), m_plain_window);
        } else {
            if (m_plain_window) {
                XUnmapWindow (QX11Info::display(), m_plain_window);
                XFlush (QX11Info::display());
                XDestroyWindow (QX11Info::display(), m_plain_window);
                m_plain_window = 0;
                //XSync (QX11Info::display (), false);
            }
        }
    }
}

WId VideoOutput::windowHandle () {
    return m_plain_window;
}

void VideoOutput::setGeometry (const IRect &rect) {
    int x = rect.x (), y = rect.y (), w = rect.width (), h = rect.height ();
    //if (m_view->keepSizeRatio ()) {
        // scale video widget inside region
        int hfw = heightForWidth (w);
        if (hfw > 0) {
            if (hfw > h) {
                int old_w = w;
                w = int ((1.0 * h * w)/(1.0 * hfw));
                x += (old_w - w) / 2;
            } else {
                y += (h - hfw) / 2;
                h = hfw;
            }
        }
    //}
    if (geometry () != IRect (x, y, w, h)) {
        if (m_paint_regions.size ()) {
            XShapeCombineMask (QX11Info::display (), windowHandle (), ShapeBounding, 0, 0, None, ShapeSet);
            m_paint_regions.clear ();
        }
        m_rect = IRect (x, y, w, h);
        m_tmp_regions.clear ();
        XMoveResizeWindow (QX11Info::display(), windowHandle (), x, y, w, h);
        XMapRaised (QX11Info::display(), windowHandle ());
        if (m_view->stopButton()->isVisible ()) {
            m_view->stopButton()->raise();
            m_view->backButton()->raise();
            if (m_view->scaleButton()->isVisible ()) {
                m_view->scaleButton()->raise();
                m_view->zoomButton()->raise();
            }
        }
        XFlush (QX11Info::display());
    }
}

void VideoOutput::setAspect (float a) {
    m_aspect = a;
    IRect r = m_view->videoGeometry ();
    m_view->scheduleRepaint (
            IRect (r.x (), r.y (), r.width (), r.height ()));
}

void VideoOutput::initPaintRegions (const IRect& r) {
    IRect ir = geometry ().intersect (r);
    const std::set<IRect>::iterator e = m_paint_regions.end();
    for (std::set<IRect>::iterator it = m_paint_regions.begin(); it != e; ++it)
        if (it->intersect (ir).isEmpty ())
            m_tmp_regions.insert (*it);
}

void VideoOutput::addPaintRegion (const IRect& r) {
    IRect ir = geometry ().intersect (r);
    if (ir.isEmpty ())
        return;
    const std::set<IRect>::iterator e = m_tmp_regions.end();
    for (std::set<IRect>::iterator it = m_tmp_regions.begin(); it != e; ++it) {
        IRect u = ir.unite (*it);
        if (*it == u)
            return;
    }
    m_tmp_regions.insert (ir);
}

void VideoOutput::applyPaintRegions () {
    bool update = false;
    if (m_tmp_regions.size ()) {
        if (m_tmp_regions.size () != m_paint_regions.size ()) {
            update = true;
        } else {
            std::set<IRect>::iterator ti = m_tmp_regions.begin();
            std::set<IRect>::iterator pi = m_paint_regions.begin();
            const std::set<IRect>::iterator te = m_tmp_regions.end();
            for (; ti != te; ++ti, ++pi)
                if (*ti != *pi) {
                    update = true;
                    break;
                }
        }
    }
    if (update
            || (!m_tmp_regions.size ()
                && m_paint_regions.size () > 0)) {
        XShapeCombineMask (QX11Info::display (), windowHandle (), ShapeBounding, 0, 0, None, ShapeSet);
    }
    if (update) {
        IRect geom = geometry ();
        XRectangle *xrects = new XRectangle[m_tmp_regions.size ()];
        const std::set<IRect>::iterator e = m_tmp_regions.end();
        int i = 0;
        for (std::set<IRect>::iterator it = m_tmp_regions.begin(); it != e; ++i, ++it) {
            xrects[i].x = it->x() - geom.point.x;
            xrects[i].y = it->y() - geom.point.y;
            xrects[i].width = it->width();
            xrects[i].height = it->height();
        }
        XShapeCombineRectangles (QX11Info::display (),
                windowHandle (),
                ShapeBounding,
                0, 0,
                xrects, m_tmp_regions.size (),
                ShapeSubtract,
                XUnsorted);
        delete[] xrects;
    }
    m_paint_regions = m_tmp_regions;
    m_tmp_regions.clear();
}

KDE_NO_EXPORT void VideoOutput::setMonitoring (Monitor m) {
    m_input_mask =
        ButtonPressMask | ButtonReleaseMask |
        //KeyPressMask | KeyReleaseMask |
        //EnterWindowMask | LeaveWindowMask |
        //FocusChangeMask |
        ExposureMask |
        //StructureNotifyMask |
        SubstructureNotifyMask;
    //if (m & MonitorMouse)
        //m_input_mask |= PointerMotionMask;
    if (m & MonitorKey)
        m_input_mask |= KeyPressMask;
    if (windowHandle ())
        setXSelectInput (windowHandle (), m_input_mask);
}

KDE_NO_EXPORT int VideoOutput::heightForWidth (int w) const {
    if (m_aspect <= 0.01)
        return 0;
    return int (w/m_aspect);
}

/*
*/
void VideoOutput::sendKeyEvent (int key) {
    WId w = windowHandle ();
    if (w) {
        char buf[2] = { char (key), '\0' };
        KeySym keysym = XStringToKeysym (buf);
        XKeyEvent event = {
            XKeyPress, 0, true,
            QX11Info::display (), w, QX11Info::appRootWindow(), w,
            /*time*/ 0, 0, 0, 0, 0,
            0, XKeysymToKeycode (QX11Info::display (), keysym), true
        };
        XSendEvent (QX11Info::display(), w, false, KeyPressMask, (XEvent *) &event);
        XFlush (QX11Info::display ());
    }
}

KDE_NO_EXPORT void VideoOutput::setBackgroundColor (const QColor & c) {
    if (m_bgcolor != c.rgb ()) {
        m_bgcolor = c.rgb ();
        setCurrentBackgroundColor (c);
    }
}

KDE_NO_EXPORT void VideoOutput::resetBackgroundColor () {
    setCurrentBackgroundColor (m_bgcolor);
}

KDE_NO_EXPORT void VideoOutput::setCurrentBackgroundColor (const QColor & c) {
    WId w = windowHandle ();
    if (w) {
        XSetWindowBackground (QX11Info::display (), w, c.rgb ());
        XFlush (QX11Info::display ());
    }
}

