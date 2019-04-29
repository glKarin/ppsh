#include "player.h"

#include <QDebug>

#include "id_std.h"

#ifdef _DBG
#define ID_KMPLAYER_PATH "./kmplayer++/kmplayer++/src/kmplayer++"
#else
#define ID_KMPLAYER_PATH "/opt/ppsh/kmplayer++/bin/kmplayer++"
#endif

idPlayer::idPlayer(QObject *parent)
	: QObject(parent),
	oProcess(0),
	bRunning(false),
	bAsync(true),
	eMode(idPlayer::MPlayer_Player)
{
}

idPlayer::~idPlayer()
{
	if(oProcess && oProcess->state() != QProcess::NotRunning)
		oProcess->kill();
}


QString idPlayer::Source() const
{
	return sSource;
}

QString idPlayer::AudioSource() const
{
	return sAudioSource;
}

bool idPlayer::Running() const
{
	//return oProcess && oProcess->state() != QProcess::NotRunning;
	return bRunning;
}

QVariant idPlayer::RequestHeaders() const
{
	return QVariant(tRequestHeaders);
}

idPlayer::idPlayerType_e idPlayer::Mode() const
{
	return eMode;
}

void idPlayer::SetSource(const QString &s)
{
	if(sSource != s)
	{
		sSource = s;
		emit sourceChanged(sSource);
	}
}

void idPlayer::SetAudioSource(const QString &s)
{
	if(sAudioSource != s)
	{
		sAudioSource = s;
		emit audioSourceChanged(sAudioSource);
	}
}

void idPlayer::SetRunning(bool b)
{
	if(b)
	{
		if(oProcess)
		{
			if(oProcess->state() == QProcess::NotRunning)
				Play();
		}
	}
	else
	{
		if(oProcess)
		{
			if(oProcess->state() != QProcess::NotRunning)
				Stop();
		}
	}
}

void idPlayer::SetRequestHeaders(const QVariant &v)
{
	bool b;

	b = tRequestHeaders.isEmpty();

	tRequestHeaders.clear();
	if(v.canConvert<QStringList>())
	{
		tRequestHeaders = v.toStringList();
		b = true;
	}
	else if(v.canConvert<QVariantMap>())
	{
		QVariantMap m = v.toMap();
		ID_CONST_FOREACH(QVariantMap, m)
			tRequestHeaders.push_back(itor.key() + ": " + itor.value().toString());
		b = true;
	}
	else if(v.canConvert<QVariantList>())
	{
		QVariantList l = v.toList();
		ID_CONST_FOREACH(QVariantList, l)
		{
			QVariantMap m = itor->toMap();
			tRequestHeaders.push_back(m["name"].toString() + ": " + m["value"].toString());
		}
		b = true;
	}
	else
		b = (b != tRequestHeaders.isEmpty());

	if(b)
	{
		emit requestHeadersChanged(QVariant(tRequestHeaders));
	}
}

void idPlayer::Play(const QString &url, const QString &audio, const QVariant &headers)
{
	Stop();
	SetSource(url);
	SetAudioSource(audio);
	SetRequestHeaders(headers);
	Play();
}

void idPlayer::Play()
{
	if(sSource.isEmpty())
		return;
	if(!oProcess)
	{
		oProcess = new QProcess(this);
		connect(oProcess, SIGNAL(started()), this, SLOT(Started()));
		connect(oProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(Finished(int, QProcess::ExitStatus)));
		connect(oProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(Error(QProcess::ProcessError)));
		//connect(oProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(StateChanged(QProcess::ProcessState)));
	}
	if(oProcess->state() != QProcess::NotRunning)
		return;
	//oProcess->start(GenerateCmd());
	oProcess->start(ID_KMPLAYER_PATH, GenerateCmdArgs());
}

void idPlayer::Stop()
{
	if(!oProcess)
		return;
	if(oProcess->state() != QProcess::NotRunning)
	{
		oProcess->kill();
		oProcess->waitForFinished(-1);
	}
}

QString idPlayer::GenerateCmd() const
{
	QString cmd(ID_KMPLAYER_PATH);

	if(!sAudioSource.isEmpty())
		cmd += " -a '" + sAudioSource + "'";
	if(!tRequestHeaders.isEmpty())
		cmd += " -H '" + tRequestHeaders.join(",") + "'";
	cmd += " -u '" + sSource + "'";

	qDebug() << cmd;

	return cmd;
}

QStringList idPlayer::GenerateCmdArgs() const
{
	QStringList r;

	if(!sAudioSource.isEmpty())
		r << "-a" << sAudioSource;
	if(!tRequestHeaders.isEmpty())
		r << "-H" << tRequestHeaders.join(",");
	r << "-u" << sSource;

	qDebug() << r;
	return r;
}

idPlayer * idPlayer::Instance()
{
	static idPlayer Player;
	return &Player;
}

/*
void idPlayer::StateChanged(QProcess::ProcessState state)
{
}
*/

void idPlayer::Error(QProcess::ProcessError err)
{
	QString str;
	switch(err)
	{
		case QProcess::FailedToStart:
			str = tr("Failed to start");
			break;
		case QProcess::Crashed:
			str = tr("Crashed");
			break;
		case QProcess::Timedout:
			str = tr("Timed out");
			break;
		case QProcess::ReadError:
			str = tr("Read error");
			break;
		case QProcess::WriteError:
			str = tr("Write error");
			break;
		case QProcess::UnknownError:
		default:
			str = tr("Unknow error");
			break;
	}
	emit error(QString::number(oProcess->pid()), err, str);
}

void idPlayer::Started()
{
	SetRunningState(true);
	emit started(QString::number(oProcess->pid()));
}

void idPlayer::Finished(int code, QProcess::ExitStatus status)
{
	Q_UNUSED(status);
	SetRunningState(false);
	emit finished(QString::number(oProcess->pid()), code);
}

void idPlayer::SetRunningState(bool b)
{
	if(bRunning != b)
	{
		bRunning = b;
		emit runningChanged(bRunning);
	}
}

void idPlayer::Restart()
{
	Stop();
	oProcess->waitForFinished(0);
	Play();
}

bool idPlayer::Async() const
{
	return bAsync;
}

void idPlayer::SetAsync(bool b)
{
	if(bAsync != b)
	{
		bAsync = b;
		emit asyncChanged(bAsync);
	}
}
