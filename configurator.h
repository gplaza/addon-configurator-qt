#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMutex>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QObject>
#include <QStringList>

#include <qhttpserver/qhttpserver.h>
#include <qhttpserver/qhttprequest.h>
#include <qhttpserver/qhttpresponse.h>

#include <QJsonDocument>
#include <QJsonObject>

class Configurator : public QObject
{
        Q_OBJECT
public:
    static Configurator* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
            {
                m_Instance = new Configurator;

                QHttpServer *server = new QHttpServer(m_Instance);
                connect(server, &QHttpServer::newRequest, m_Instance, &Configurator::handleRequest);
                //connect(req,&QHttpRequest::data,m_Instance,&Configurator::setConfig);

                server->listen(QHostAddress::Any, 8080);
            }

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

private:
    Configurator() {}
    Configurator(const Configurator &);
    Configurator& operator=(const Configurator &);
    static Configurator* m_Instance;
    bool checkDatabaseFile(const QString &basePath);
    void initServer();
    QMap<QString,QString> configs;
    bool cache;

public:
    bool setDB(const QString &path, bool cache);
    QString getConfig(const QString &key);
    void addConfig(const QString &key,const QString &value);
    QMap<QString,QString> getConfigs(QSet<QString> keys);

private slots:
    void handleRequest(QHttpRequest* req, QHttpResponse* resp);
    void setConfig(const QByteArray&);

signals:
    void callServer(QHttpRequest* req, QHttpResponse* resp);

};


#endif // CONFIGURATOR_H
