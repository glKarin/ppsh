#ifndef _ID_PLAYER_H
#define _ID_PLAYER_H

#include <QObject>
#include <QVariant>
#include <QProcess>

class idPlayer : public QObject
{
	Q_OBJECT
		Q_PROPERTY(QString source READ Source WRITE SetSource NOTIFY sourceChanged)
		Q_PROPERTY(QString audioSource READ AudioSource WRITE SetAudioSource NOTIFY audioSourceChanged)
		Q_PROPERTY(QVariant requestHeaders READ RequestHeaders WRITE SetRequestHeaders NOTIFY requestHeadersChanged)
		Q_PROPERTY(bool running READ Running WRITE SetRunning NOTIFY runningChanged)
		Q_PROPERTY(bool async READ Async WRITE SetAsync NOTIFY asyncChanged)
		Q_PROPERTY(idPlayerType_e mode READ Mode FINAL)
		Q_ENUMS(idPlayerType_e)

	public:
		enum idPlayerType_e
		{
			System_Player = 0,
			MPlayer_Player,
			GST_Player
		};

	public:
		virtual ~idPlayer();

		QString Source() const;
		QString AudioSource() const;
		bool Running() const;
		bool Async() const;
		QVariant RequestHeaders() const;
		idPlayerType_e Mode() const;
		void SetSource(const QString &s);
		void SetAudioSource(const QString &s);
		void SetRunning(bool b);
		void SetAsync(bool b);
		void SetRequestHeaders(const QVariant &v);
		static idPlayer * Instance();

		Q_INVOKABLE void Play(const QString &url, const QString &audio = QString(), const QVariant &headers = QVariant());
		Q_INVOKABLE void Play();
		Q_INVOKABLE void Stop();
		Q_INVOKABLE void Restart();

Q_SIGNALS:
		void sourceChanged(const QString &source);
		void audioSourceChanged(const QString &audioSource);
		void runningChanged(bool running);
		void asyncChanged(bool async);
		void requestHeadersChanged(const QVariant &requestHeaders);
		void started(const QString &pid);
		void finished(const QString &pid, int code);
		void error(const QString &pid, int errno, const QString &errstr);

	private Q_SLOTS:
		//void StateChanged(QProcess::ProcessState state);
		void Error(QProcess::ProcessError error);
		void Started();
		void Finished(int code, QProcess::ExitStatus status);

private:
		QString GenerateCmd() const;
		QStringList GenerateCmdArgs() const;
		void SetRunningState(bool b);

	private:
		idPlayer(QObject *parent = 0);

	private:
		QProcess *oProcess;
		bool bRunning;
		bool bAsync;
		QString sSource;
		QString sAudioSource;
		QStringList tRequestHeaders;
		idPlayerType_e eMode;

		Q_DISABLE_COPY(idPlayer)
};

#endif
