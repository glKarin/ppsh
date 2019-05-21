#include "utility.h"

#include <QSettings>
#include <QApplication>
#include <QClipboard>
#include <QDeclarativeEngine>
#include <QProcess>
#include <QStringList>
#include <QDebug>
#ifdef _MAEMO_MEEGOTOUCH_INTERFACES_DEV
#include <maemo-meegotouch-interfaces/videosuiteinterface.h>
#else
#define ID_VIDEO_SUITE "/usr/bin/video-suite"
#endif

#include "networkmanager.h"
#include "id_std.h"

#define ID_SETTING_VERSION 3

namespace id
{
	static QVariantHash _tDefaultSettings;
	static void InitDefaultSettings()
	{
		if(_tDefaultSettings.count() > 0)
			return;

		_tDefaultSettings.insert("setting_version", ID_SETTING_VERSION);

		_tDefaultSettings.insert("generals/orientation", 0);
		_tDefaultSettings.insert("generals/theme_color", 0);
		_tDefaultSettings.insert("generals/night_mode", false);
		_tDefaultSettings.insert("generals/default_browser", 1);
		_tDefaultSettings.insert("generals/touch_icon_drag", false);
		_tDefaultSettings.insert("generals/fullscreen", true);

		_tDefaultSettings.insert("player/open_danmaku", true);
		_tDefaultSettings.insert("player/danmaku_opacity", 1.0);
		_tDefaultSettings.insert("player/danmaku_size", 1.0);
		_tDefaultSettings.insert("player/danmaku_speed", 1.0);
		_tDefaultSettings.insert("player/danmaku_limit", 0);
		_tDefaultSettings.insert("player/video_quality", 16);
		_tDefaultSettings.insert("player/danmaku_screen_mode", 0);

		_tDefaultSettings.insert("browser/helper", false);
		_tDefaultSettings.insert("browser/dbl_zoom", false);
		_tDefaultSettings.insert("browser/load_image", false);
	}

	static bool danmakucmp(const QVariant &a, const QVariant &b)
	{
		return a.toMap()["time"].toFloat() < b.toMap()["time"].toFloat();
	}
}

idUtility::idUtility(QObject *parent) :
	QObject(parent),
	oSettings(new QSettings(this)),
	iDev(
#ifdef _DBG
			1
#else
			0
#endif
			)
{
	setObjectName("idUtility");
	Init();
}

idUtility::~idUtility()
{

}

void idUtility::Init()
{
	id::InitDefaultSettings();
}

idUtility * idUtility::Instance()
{
	static idUtility _ut;
	return &_ut;
}

int idUtility::Dev() const
{
	return iDev;
}

void idUtility::SetDev(int d)
{
	int nd = d;
	if(nd < 0)
		nd = 0;
	if(iDev != nd)
	{
		iDev = nd;
		emit devChanged(iDev);
	}
}

QVariant idUtility::GetSetting(const QString &name)
{
	if(!id::_tDefaultSettings.contains(name))
		return QVariant();

	return oSettings->value(name, id::_tDefaultSettings.value(name));
}

void idUtility::SetSetting(const QString &name, const QVariant &value)
{
	if(!id::_tDefaultSettings.contains(name))
		return;

	oSettings->setValue(name, value);
}

void idUtility::OpenPlayer(const QString &url, int t) const
{
#ifdef _HARMATTAN
#ifdef _MAEMO_MEEGOTOUCH_INTERFACES_DEV
	VideoSuiteInterface player;
	player.play(QStringList(url));
#else
	QProcess::startDetached(VIDEO_SUITE, QStringList(url));
#endif
#elif defined(_SYMBIAN)
	QString path = QDir::tempPath();
	QDir dir(path);
	if (!dir.exists()) dir.mkpath(path);
	QString ramPath = path+"/video.ram";
	QFile file(ramPath);
	if (file.exists()) file.remove();
	if (file.open(QIODevice::ReadWrite)){
		QTextStream out(&file);
		out << url;
		file.close();
		QDesktopServices::openUrl(QUrl("file:///"+ramPath));
	}
#else
	qDebug() << "[DEBUG]: Open player -> " << url << t;
#endif
}

void idUtility::CopyToClipboard(const QString &text) const
{
	QApplication::clipboard()->setText(text);
}

QVariant idUtility::ResetSetting(const QString &name)
{
	if(name.isEmpty())
	{
		for(QVariantHash::const_iterator itor = id::_tDefaultSettings.constBegin();
				itor != id::_tDefaultSettings.constEnd(); ++itor)
		{
			oSettings->setValue(itor.key(), itor.value());
		}
		return QVariant(id::_tDefaultSettings);
	}
	else
	{
		if(id::_tDefaultSettings.contains(name))
		{
			oSettings->setValue(name, id::_tDefaultSettings.value(name));
			return oSettings->value(name);
		}
	}
	return QVariant();
}

void idUtility::Print_r(const QVariant &v) const
{
	qDebug() << v;
}

void idUtility::SetRequestHeaders(const QVariant &v)
{
	QNetworkAccessManager *qmanager;
	idNetworkAccessManager *manager;

	if(!oEngine)
		return;

	qmanager = oEngine->networkAccessManager();
	manager = dynamic_cast<idNetworkAccessManager *>(qmanager);

	if(!manager)
		return;

	if(v.canConvert<QVariantList>())
		manager->SetRequestHeaders(v.toList());
	else if(v.canConvert<QVariantMap>())
		manager->SetRequestHeaders(v.toMap());
}

void idUtility::SetEngine(QDeclarativeEngine *e)
{
	oEngine = e;
}

QDeclarativeEngine * idUtility::Engine()
{
	return oEngine;
}

QString idUtility::Sign(const QVariantMap &args, const QString &suffix, const QVariantMap &sysArgs) const
{
#define MAKE_QUERY(k, v) k + "=" + QUrl::toPercentEncoding(v)
	typedef QMap<QString, QString> idStringMap_t;

	QString r;
	idStringMap_t map;
	idStringMap_t map2;
	QStringList list;

	ID_CONST_FOREACH(QVariantMap, args)
	{
		map.insert(itor.key(), itor.value().toString());
	}
	ID_CONST_FOREACH(QVariantMap, sysArgs)
	{
		QString key = itor.key();
		if(map.contains(key))
			map2.insert(key, itor.value().toString());
		else
			map.insert(key, itor.value().toString());
	}
	ID_CONST_FOREACH(idStringMap_t, map)
	{
		QString key = itor.key();
		if(map2.contains(key))
		{
			list.push_back(MAKE_QUERY(key, map2[key]));
		}
		list.push_back(MAKE_QUERY(key, itor.value()));
	}
	r = list.join("&") + suffix;
	//qDebug() << r;
	return id::md5(r);
#undef MAKE_QUERY
}

QVariant idUtility::Get(const QString &name) const
{
#define ID_QT qVersion()
#define ID_EGG QObject::tr("Comrades, taking the Nokia MeeGo's flag of victory, to stuck in the Bilibili's highlands of Two-Dimensionas!")
#ifdef _DBG
#define ID_ICON_PATH QDir::cleanPath(QCoreApplication::applicationDirPath() + "/ppsh80.png")
#else
#define ID_ICON_PATH "/usr/share/icons/hicolor/80x80/apps/ppsh80.png"
#endif
	QVariant r;

	if(name.isEmpty())
	{
		QVariantMap map;
#define ID_M_I(x) map.insert(#x, ID_##x)
		ID_M_I(PATCH);
		ID_M_I(RELEASE);
		ID_M_I(DEV);
		ID_M_I(VER);
		ID_M_I(CODE);
		ID_M_I(STATE);
		ID_M_I(EMAIL);
		ID_M_I(GITHUB);
		ID_M_I(PAN);
		ID_M_I(OPENREPOS);
		ID_M_I(PKG);
		ID_M_I(APP);
		ID_M_I(TMO);
		ID_M_I(DESC);
		ID_M_I(EGG);
		ID_M_I(PLATFORM);
		ID_M_I(QT);
		ID_M_I(ICON_PATH);
		ID_M_I(NAME);
		ID_M_I(BUID);
#undef _NL_M_I
		r.setValue(map);
	}
	else
	{
		QString n = name.toUpper();
#define ID_I(x) if(n == #x) { r.setValue(QString(ID_##x)); }
		ID_I(PATCH)
			else ID_I(RELEASE)
			else ID_I(DEV)
			else ID_I(VER)
			else ID_I(CODE)
			else ID_I(STATE)
			else ID_I(EMAIL)
			else ID_I(GITHUB)
			else ID_I(PAN)
			else ID_I(OPENREPOS)
			else ID_I(PKG)
			else ID_I(APP)
			else ID_I(TMO)
			else ID_I(DESC)
			else ID_I(EGG)
			else ID_I(PLATFORM)
			else ID_I(QT)
			else ID_I(ICON_PATH)
			else ID_I(NAME)
			else ID_I(BUID)
			else r.setValue(QProcessEnvironment::systemEnvironment().value(name));
#undef _NL_I
	}
	return r;
#undef ID_QT
#undef ID_EGG
#undef ID_ICON_PATH
}

QVariant idUtility::Changelog(const QString &version) const
{
	QVariantMap m;
	QStringList list;

	if(version.isEmpty())
	{
		list 
			<< QObject::tr("Add live.")
			<< QObject::tr("Some fixed.")
			<< QObject::tr("Videos of dash format will decode by MPlayer, others using internal player.")
			<< QObject::tr("If playing with KMPlayer and not work, set player to MPlayer.");
	}

	// read from changelog?
	m.insert("CHANGES", list);
	m.insert("PKG_NAME", QVariant());
	m.insert("RELEASE", QVariant());
	m.insert("DEVELOPER", QVariant());
	m.insert("EMAIL", QVariant());
	m.insert("URGENCY", QVariant());
	m.insert("STATE", QVariant());
	m.insert("VERSION", QVariant());

	return QVariant::fromValue<QVariantMap>(m);
}

QString idUtility::Uncompress(const QString &src, int windowbits)
{
	QByteArray b;

	if(id::iduncompress(&b, QByteArray::fromBase64(QByteArray().append(src)), windowbits) == 0)
		return b;
	return QString();
}

QVariant idUtility::XML_Parse(const QString &xml)
{
	return id::qvariant_from_xml(xml);
}

QVariant idUtility::MakeDanmaku_cpp(const QString &xml, int limit)
{
	int c;
	QVariantList r;

	QVariant v = XML_Parse(xml);
	QVariantMap map = v.toMap();

	c = 0;
	if(map["tag"].toString() != "i")
	{
		return QVariant(r);
	}
	const QVariantList list = map["children"].toList();
	QString content;
	QVariantMap m;
	QVariantMap p;
	QVariantMap item;
	QStringList ps;
	const QString Color_Fmt("#%1");
	ID_CONST_FOREACH(QVariantList, list)
	{
		if(limit > 0 && c >= limit)
			break;

		m = itor->toMap();
		if(m["tag"].toString() != "d")
			continue;
		if(!m["children"].canConvert<QString>())
			continue;
		content = m["children"].toString();
		p = m["params"].toMap();
		if(!p.contains("p"))
			continue;
		ps = p["p"].toString().split(",");
		//item.insert("time", ps[0].toFloat()); // second
		item.insert("time", (uint)(ps[0].toFloat() * 1000)); // float precision?, ms
		item.insert("mode", ps[1].toInt()); // 1 - slide, 5 - top, 4 - bottom 
		// 7 - special [0.98,0.97,"1-1",7,"na ku ye-fu ra-se-ya~",0,0,0.05,0.68,500,0,1,"SimHei",true]
		item.insert("size", ps[2].toInt()); // 25 - normal, 18 - small
		item.insert("color", Color_Fmt.arg(ps[3].toUInt(), 6, 16).replace(' ', '0')); // color(10)
		if(0)
		{
			item.insert("timestamp", ps[4].toLongLong()); // second
			item.insert("__5", ps[5]); // 0
			item.insert("uid", ps[6]); // UID(16) ?
			item.insert("id", ps[7]); // ID ?
		}
		item.insert("content", content);

		r.push_back(item);
		c++;
	}
	qSort(r.begin(), r.end(), id::danmakucmp);
	return QVariant(r);
}

QVariant idUtility::GetDefaultSetting(const QString &name)
{
	if(name.isEmpty())
		return QVariant(id::_tDefaultSettings);
	return id::_tDefaultSettings.value(name);
}

QString idUtility::FormatUrl(const QString &u)
{
	int dot, slash;
	QUrl url(u);

	if(url.isValid())
	{
		if(url.scheme().isEmpty())
		{
			if(url.isRelative())
			{
				if(u.at(0) == '.')
					return QString("file://") + QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + u);
				else if(u.at(0) == '/')
					return QString("file://") + u;
				else
				{
					dot = u.indexOf('.');
					slash = u.indexOf('/');
					if(dot != -1 || slash != -1)
					{
						if(slash == -1 || dot < slash - 1)
							return QString("http://") + u;
					}
					else if(u.indexOf("localhost") == 0)
						return QString("http://") + u;
				}
			}
			else
				return QString("http://") + u;
		}
		else
			return url.toString();
	}
	return QString();
}

qint64 idUtility::System(const QString &path, const QVariant &args, bool async)
{
	qint64 pid;
	QStringList list;

	list = args.toStringList();

	if(async)
	{
		if(QProcess::startDetached(path, list, QString(), &pid))
			return pid;
		return -1;
	}
	else
		return QProcess::execute(path, list);
}

void idUtility::CheckUpdate()
{
	int ver;
	bool u;

	ver = 0;
	u = true;
	if(oSettings->contains("setting_version"))
	{
		ver = oSettings->value("setting_version").toInt();
		if(ver >= ID_SETTING_VERSION)
			u = false;
	}
	
	if(u)
	{
		oSettings->setValue("setting_version", ID_SETTING_VERSION);
		emit hasUpdate(ID_SETTING_VERSION);
	}
}
