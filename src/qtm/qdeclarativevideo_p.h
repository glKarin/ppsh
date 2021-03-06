/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef _KARIN_QDECLARATIVEVIDEO_H
#define _KARIN_QDECLARATIVEVIDEO_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdeclarativemediabase_p.h"

#include <qgraphicsvideoitem.h>

#include <QtCore/qbasictimer.h>
#include <QtDeclarative/qdeclarativeitem.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QTimerEvent;
class QVideoSurfaceFormat;


class QDeclarativeVideo : public QDeclarativeItem, public QDeclarativeMediaBase
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool autoLoad READ isAutoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool hasAudio READ hasAudio NOTIFY hasAudioChanged)
    Q_PROPERTY(bool hasVideo READ hasVideo NOTIFY hasVideoChanged)
    Q_PROPERTY(int bufferProgress READ bufferProgress NOTIFY bufferProgressChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode)
    Q_PROPERTY(QDeclarativeMediaMetaData *metaData READ metaData CONSTANT)
    Q_ENUMS(FillMode)
    Q_ENUMS(Status)
    Q_ENUMS(Error)

#ifdef _KARIN_MM_EXTENSIONS
		Q_PROPERTY(QVariantList requestHeaders READ RequestHeaders WRITE SetRequestHeaders NOTIFY requestHeadersChanged)
		Q_PROPERTY(bool headersEnabled READ HeadersEnabled WRITE SetHeadersEnabled NOTIFY headersEnabledChanged)

	public:
		QVariantList RequestHeaders() const;
		void SetRequestHeaders(const QVariantList &headers);

		bool HeadersEnabled() const;
		void SetHeadersEnabled(bool b);

Q_SIGNALS:
		void requestHeadersChanged();
		void headersEnabledChanged();

	private:
		QVariantList tRequestHeaders;
#endif

public:
    enum FillMode
    {
        Stretch            = Qt::IgnoreAspectRatio,
        PreserveAspectFit  = Qt::KeepAspectRatio,
        PreserveAspectCrop = Qt::KeepAspectRatioByExpanding
    };

    enum Status
    {
        UnknownStatus = QMediaPlayer::UnknownMediaStatus,
        NoMedia       = QMediaPlayer::NoMedia,
        Loading       = QMediaPlayer::LoadingMedia,
        Loaded        = QMediaPlayer::LoadedMedia,
        Stalled       = QMediaPlayer::StalledMedia,
        Buffering     = QMediaPlayer::BufferingMedia,
        Buffered      = QMediaPlayer::BufferedMedia,
        EndOfMedia    = QMediaPlayer::EndOfMedia,
        InvalidMedia  = QMediaPlayer::InvalidMedia
    };

    enum Error
    {
        NoError        = QMediaPlayer::NoError,
        ResourceError  = QMediaPlayer::ResourceError,
        FormatError    = QMediaPlayer::FormatError,
        NetworkError   = QMediaPlayer::NetworkError,
        AccessDenied   = QMediaPlayer::AccessDeniedError,
        ServiceMissing = QMediaPlayer::ServiceMissingError
    };

    QDeclarativeVideo(QDeclarativeItem *parent = 0);
    ~QDeclarativeVideo();

    bool hasAudio() const;
    bool hasVideo() const;

    FillMode fillMode() const;
    void setFillMode(FillMode mode);

    Status status() const;
    Error error() const;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    void classBegin();
    void componentComplete();

public Q_SLOTS:
    void play();
    void pause();
    void stop();

Q_SIGNALS:
    void sourceChanged();
    void autoLoadChanged();
    void playingChanged();
    void pausedChanged();
    void loopCountChanged();

    void started();
    void resumed();
    void paused();
    void stopped();

    void statusChanged();

    void durationChanged();
    void positionChanged();

    void volumeChanged();
    void mutedChanged();
    void hasAudioChanged();
    void hasVideoChanged();

    void bufferProgressChanged();

    void seekableChanged();
    void playbackRateChanged();

    void errorChanged();
    void error(QDeclarativeVideo::Error error, const QString &errorString);

protected:
    void geometryChanged(const QRectF &geometry, const QRectF &);

private Q_SLOTS:
    void _q_nativeSizeChanged(const QSizeF &size);
    void _q_error(int, const QString &);

private:
    Q_DISABLE_COPY(QDeclarativeVideo)

    QGraphicsVideoItem *m_graphicsItem;

    Q_PRIVATE_SLOT(mediaBase(), void _q_statusChanged())

    inline QDeclarativeMediaBase *mediaBase() { return this; }
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeVideo))

QT_END_HEADER

#endif
