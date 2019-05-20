#include <QtGui/QApplication>
#include <QtDeclarative>
#include <QLocale>
#include <QTranslator>
#include <QTextCodec>
#include <QDebug>
#include "qmlapplicationviewer.h"

#include "id_std.h"
#include "utility.h"
#include "player.h"
#include "networkconnector.h"
#include "networkmanager.h"
#include "qtm/qdeclarativewebview.h"
#ifdef _KARIN_MM_EXTENSIONS
#include "qtm/qdeclarativevideo_p.h"
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
	QDeclarativeEngine *engine;
	QDeclarativeContext *context;
	idUtility *ut;
	idPlayer *player;
	idNetworkConnector *connector;
	QApplication *a;
	QTranslator translator;
	QString qm, qmdir;
	const QString RegisterUncreatableTypeMsg(QString("[ERROR]: %1 -> %2").arg("Can not create a single-instance object"));

	a = createApplication(argc, argv);
	QScopedPointer<QApplication> app(a);
	a->setApplicationName(ID_PKG);
	a->setApplicationVersion(ID_VER);
	a->setOrganizationName(ID_DEV);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	QString locale = QLocale::system().name();
#ifdef _HARMATTAN
#ifdef _DBG
	qm = ID_PKG ".zh_CN.qm";
	qmdir = "i18n/";
#else
	qm = QString(ID_PKG ".") + locale;
	qmdir = "/opt/" ID_PKG "/i18n/";
#endif
#else
#ifdef _DBG
	Q_INIT_RESOURCE(ID_PKG);
	qm = QString(":/i18n/" ID_PKG ".") + locale;
#else
	qm = QString("qt_") + locale;
	qmdir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif
#endif
	if(translator.load(qm, qmdir))
	{
		qDebug() << QString("[INFO]: Load i18n -> %1: %2 [%3]").arg(locale).arg(qm).arg(qmdir);
		a->installTranslator(&translator);
	}
	else
		qWarning() << QString("[DEBUG]: Not found i18n -> %1: %2 [%3]").arg(locale).arg(qm).arg(qmdir);

	QmlApplicationViewer viewer;
	idDeclarativeNetworkAccessManagerFactory factory;
	engine = viewer.engine();
	context = engine->rootContext();

	qmlRegisterType<QDeclarativeWebView>(ID_QML_URI, ID_QML_MAJOR_VER, ID_QML_MINOR_VER, ID_APP "WebView");
	qmlRegisterType<QDeclarativeWebSettings>();
#ifdef _KARIN_MM_EXTENSIONS
	qmlRegisterType<QDeclarativeVideo>(ID_QML_URI, ID_QML_MAJOR_VER, ID_QML_MINOR_VER, ID_APP "Video");
#endif
	qmlRegisterUncreatableType<idNetworkConnector>(ID_QML_URI, ID_QML_MAJOR_VER, ID_QML_MINOR_VER, ID_APP "NetworkConnector", RegisterUncreatableTypeMsg.arg("idNetworkConnector"));
	qmlRegisterUncreatableType<idPlayer>(ID_QML_URI, ID_QML_MAJOR_VER, ID_QML_MINOR_VER, ID_APP "Player", RegisterUncreatableTypeMsg.arg("idPlayer"));

	ut = idUtility::Instance();
	connector = idNetworkConnector::Instance();
	player = idPlayer::Instance();

	ut->SetEngine(engine);
	engine->setNetworkAccessManagerFactory(&factory);
	connector->SetEngine(engine);

	context->setContextProperty("_UT", ut);
	context->setContextProperty("_CONNECTOR", connector);
	context->setContextProperty("_PLAYER", player);

	viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
	viewer.setMainQmlFile(QLatin1String("qml/ppsh/main.qml"));
	viewer.showExpanded();

	return app->exec();
}
