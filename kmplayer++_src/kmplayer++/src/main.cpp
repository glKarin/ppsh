#include <unistd.h>
#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus.h>
#include <QApplication>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeView>
#include <QTimer>
#include <QtCore/QtGlobal>
#include <QtGui/QDesktopWidget>
#ifndef QT_NO_OPENGL
#include <QtOpenGL/QGLWidget>
#endif
#include <QtCore/QTranslator>
#include <QtCore/QLocale>

#include "control.h"
#include "playmodel.h"
#include "dirmodel.h"

#ifdef _HARMATTAN
#include <QDebug>
static void print_help(const char *path)
{
#define _H_P qDebug // printf
#define _H_ENDL // "\n"
	const char *p;

	p = path ? path : _HARMATTAN_KMPLAYER;
	_H_P("Usage: " _H_ENDL);
	_H_P("    %s url1, url2, ..." _H_ENDL, p);
	_H_P("    %s -a audio1, audio2, ... -H <request headers> -u url1, url2, ..." _H_ENDL, p);
	_H_P("Arguments: " _H_ENDL);
	_H_P("    -u: " _H_ENDL);
	_H_P("    --url: Special video url or path" _H_ENDL);
	_H_P("    -H: " _H_ENDL);
	_H_P("    --header: Special request headers for remote url" _H_ENDL);
	_H_P("    -a: " _H_ENDL);
	_H_P("    --audio: Special audio url or path for external audio" _H_ENDL);
	_H_P("    -h: " _H_ENDL);
	_H_P("    --help: Print this info" _H_ENDL);
#undef _H_P
#undef _H_ENDL
}

static QHash<QString, QStringList> parse_args(int argc, char *argv[])
{
	int mode;
	QStringList headers;
	QStringList urls;
	QStringList audios;
	QHash<QString, QStringList> r;
	bool help;

	help = false;
	mode = 1;
	for(int i = 1; i < argc; i++)
	{
		QString s = QString::fromLocal8Bit(argv[i]);
		if(s == "--url" || s == "-u")
		{
			if(mode == 1)
				continue;
			else
			{
				if(mode != 0)
				{
					mode = 1;
					continue;
				}
			}
		}
		else if(s == "--header" || s == "-H")
		{
			if(mode == 2)
				continue;
			else
			{
				if(mode != 0)
				{
					mode = 2;
					continue;
				}
			}
		}
		else if(s == "--audio" || s == "-a")
		{
			if(mode == 3)
				continue;
			else
			{
				if(mode != 0)
				{
					mode = 3;
					continue;
				}
			}
		}
		else if(s == "--help" || s == "-h")
		{
			if(mode != 0)
			{
				help = true;
				continue;
			}
		}
		else if(s == "--")
		{
			if(mode != 0)
			{
				mode = 0;
				continue;
			}
		}

		if(mode == 0 || mode == 1)
			urls.push_back(s);
		else if(mode == 2)
			headers.push_back(s);
		else if(mode == 3)
			audios.push_back(s);
	}

	r.insert("url", urls);
	r.insert("header", headers);
	r.insert("audio",audios);

	if(help)
		print_help(_HARMATTAN_KMPLAYER);

	qDebug() << r;

	return r;
}
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    setsid();
    g_type_init();
    DBusError error;
    dbus_error_init (&error);
    DBusConnection* bus = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
    if (!bus) {
        fprintf (stderr, "Failed to connect to dbus, %s\n", error.message);
        dbus_error_free (&error);
    }

    QApplication *app = new QApplication (argc, argv);
    dbus_connection_setup_with_g_main( bus, NULL);

    QTranslator translator;
#ifdef _HARMATTAN
    translator.load ("kmplayer_" + QLocale::system ().name (), OPTDIR "/" _HARMATTAN_KMPLAYER "/i18n");
#else
    translator.load ("kmplayer_" + QLocale::system ().name (), "/opt/kmplayer/i18n");
#endif
    app->installTranslator (&translator);

    QDeclarativeView *view = new QDeclarativeView();
    view->setResizeMode (QDeclarativeView::SizeRootObjectToView);
    view->setAttribute (Qt::WA_NoSystemBackground);
    view->setAttribute (Qt::WA_OpaquePaintEvent);
#ifndef QT_NO_OPENGL
#warning using opengl
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);
    QGLWidget *glWidget = new QGLWidget (format);
    glWidget->setAutoFillBackground (false);
    view->setViewport (glWidget);
#endif
    view->setGeometry (QApplication::desktop()->screenGeometry ());

    KMPlayer::Control *control = new KMPlayer::Control (view);
    control->init ();
    QApplication::instance ()->installEventFilter (control);

#ifdef _HARMATTAN
    view->setSource(QUrl::fromLocalFile(OPTDIR "/" _HARMATTAN_KMPLAYER "/splash.qml"));
#else
    view->setSource(QUrl::fromLocalFile("/opt/kmplayer/splash.qml"));
#endif
    ///view->setSource(QUrl::fromLocalFile("/home/koos/kmplayer/branches/harmattan/src/kmplayer.qml"));
    view->showFullScreen();

#ifdef _HARMATTAN
		QHash<QString, QStringList> params = parse_args(argc, argv);
		control->initial_urls = params["url"];
		control->request_headers = params["header"];
		control->initial_audios = params["audio"];
#else
    for (int i = 1; i < argc; ++i)
        control->initial_urls << QString::fromLocal8Bit (argv[i]);
#endif
    QTimer::singleShot (0, control, SLOT (openInitialUrls()));

    int result = app->exec();

    control->deinit ();

    return result;
}
