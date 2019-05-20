#ifndef ID_STD_H
#define ID_STD_H

#include <QVariant>

#define ID_APP "PPSH"
#define ID_NAME QObject::tr("PPSH")
#define ID_PKG "ppsh"
#define ID_VER "41.0.5harmattan1"
#define ID_CODE "natasha"
#define ID_DEV "karin"
#define ID_RELEASE "20140405"
#define ID_PATCH "1"
#define ID_STATE "devel"
#define ID_GITHUB "https://github.com/glKarin/ppsh"
#define ID_PAN "https://pan.baidu.com/s/1n547NjXarwva8-JrZiRk-Q g4ic"
#define ID_OPENREPOS "https://openrepos.net/content/karinzhao/ppshbilibili"
#define ID_EMAIL "beyondk2000@gmail.com"
#define ID_TMO "http://talk.maemo.org/member.php?u=70254"
#define ID_DESC QObject::tr("PPSH is a simple web-video player for Bilibili, based on Web-API.")
#define ID_BUID "14345912"
#ifdef _HARMATTAN
#define ID_PLATFORM "MeeGo 1.2 Harmattan"
#else
#define ID_PLATFORM "Symbian"
#endif

#define ID_FOREACH(T, t) for(T::iterator itor = t.begin(); itor != t.end(); ++itor)
#define ID_CONST_FOREACH(T, t) for(T::const_iterator itor = t.constBegin(); itor != t.constEnd(); ++itor)
#define ID_UNTIL(condition) while(!(condition))

#define ID_QML_URI ID_DEV"."ID_PKG
#define ID_QML_MAJOR_VER 1
#define ID_QML_MINOR_VER 0

#define ungzip(dst, src) iduncompress(dst, src, 32 + 15)
#define unz(dst, src) iduncompress(dst, src, -15)

namespace id
{
	QString md5(const QString &src);
	QString md5_b64(const QString &src);
	int iduncompress(QByteArray *dst, const QByteArray &data, int windowbits);
	QVariant qvariant_from_xml(const QString &xml);
}

#endif // ID_STD_H
