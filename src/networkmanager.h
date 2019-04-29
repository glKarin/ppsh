#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QDeclarativeNetworkAccessManagerFactory>
#include <QStringList>
#include <QPair>
#include <QVariant>
//#include <QHttpRequestHeader>

namespace id
{
typedef QPair<QByteArray, QByteArray> idRequestHeader_t;
typedef QList<idRequestHeader_t> idRequestHeaders_t;
}

class idRequestHeaders
{
public:
    idRequestHeaders();
    virtual ~idRequestHeaders();
    void AddHeader(const QString &name, const QString &value);
    idRequestHeaders & operator<<(const id::idRequestHeader_t &h);
    void RemoveHeader(const QString &name);
    idRequestHeaders & operator>>(const QString &name);
    id::idRequestHeaders_t Headers() const;
    void ClearHeaders();
    bool HasHeader(const QString &name);

    bool Request(QNetworkRequest *req);

    void AddPath(const QString &path);
    void SetPaths(const QStringList &list);
    QStringList Paths() const;

    void SetMethod(const QString &m);
    QString Method() const;

public:
    static bool B_Dbg;

private:
    void SetHeaders(QNetworkRequest *req);

private:
    id::idRequestHeaders_t tHeaders;
    QStringList tPaths;
    QString sMethod;
};

class idNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    enum RequestMethod_e
    {
        Request_Get = 0,
        Request_Post,
    };

public:
    explicit idNetworkAccessManager(QObject *parent = 0);
    virtual ~idNetworkAccessManager();
    QNetworkReply * Request(const QString &url, const QByteArray &data = QByteArray(), RequestMethod_e method = Request_Get);
    void SetRequestHeaders(const QVariantMap &headers);
    void SetRequestHeaders(const QVariantList &headers);
    
signals:
    
public slots:

protected:
    virtual QNetworkReply *	createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
    virtual bool HandleRequest(QNetworkRequest *req);

private:
    void Init();

private:
    idRequestHeaders oHeaders;
    
};

class idDeclarativeNetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
{
public:
    virtual	~idDeclarativeNetworkAccessManagerFactory ();
    virtual QNetworkAccessManager *	create ( QObject * parent );
};

class idNetworkCookieJar : public QNetworkCookieJar
{
    Q_OBJECT

public:
    ~idNetworkCookieJar();
    static idNetworkCookieJar * Instance();

protected:
    virtual void Restore();
    virtual void Dump();

private:
    idNetworkCookieJar(QObject *parent = 0);

};

#endif // NETWORKMANAGER_H
