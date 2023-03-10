#include "configurator.h"

Configurator* Configurator::m_Instance = 0;

bool Configurator::init(const QString &path, bool cache, quint16 port)
{
    this->cache = cache;
    this->port = port;

    bool newConfigDB = checkDatabaseFile(path);

    if(!newConfigDB)
    {
        if(!QSqlDatabase::database("config").isValid())
        {
            QSqlDatabase accesoBDD = QSqlDatabase::addDatabase("QSQLITE","config");
            accesoBDD.setDatabaseName(path);
            accesoBDD.open();
        }
    }

    if(this->cache)
    {
        qDebug() << "Init API server";
        initServer();
    }

    return true;
}

void Configurator::initServer()
{
    QHttpServer *server = new QHttpServer(this);
    connect(server, &QHttpServer::newRequest, this, &Configurator::handleRequest);
    server->listen(QHostAddress::Any, this->port);
}

void Configurator::getCacheConfigToDebug() {

    if(cache)
    {
        QMapIterator<QString,QString> i(configs);

        while (i.hasNext()) {
            i.next();
            qDebug() << i.key().leftJustified(30,' ') + " : " << i.value();
        }
    }
}

void Configurator::handleRequest(QHttpRequest* req, QHttpResponse* resp)
{
    qDebug() << "New query on config server : " <<  req->path();

    QRegExp *exp = new QRegExp();
    exp->setPattern("^/api/");

    if (exp->indexIn(req->path()) == -1)
    {
        resp->writeHead(403);
        resp->end(QByteArray("You aren't allowed here!"));
        req->deleteLater();
        return;
    }

    exp->setPattern("^/api/configurator/config");

    if (exp->indexIn(req->path()) != -1 && req->method() == QHttpRequest::HTTP_POST)
    {
        qDebug() << "update config request ...";

        connect(req,&QHttpRequest::data,this,&Configurator::setConfig);

        QByteArray body = "OK";
        resp->setHeader("Content-Length", QString::number(body.size()));
        resp->writeHead(200);
        resp->end(body);
        req->deleteLater();
        return;

    } else if(req->method() == QHttpRequest::HTTP_POST || req->method() == QHttpRequest::HTTP_DELETE || req->method() == QHttpRequest::HTTP_PUT) {

        qDebug() << "Transfert query to Adapter.";
        emit callServer(req,resp);

    } else {

        QByteArray body = "Content not allowed";
        resp->setHeader("Content-Length", QString::number(body.size()));
        resp->writeHead(400);
        resp->end(body);
        req->deleteLater();
        return;
    }
}

QMap<QString,QString> Configurator::getConfigs(QSet<QString> keys)
{
    QSqlQuery query(QSqlDatabase::database("config"));
    QMap <QString,QString> result;
    QStringList keysString;

    QString sql = "SELECT key, value FROM config WHERE ";

    foreach (QString key, keys)
        keysString << " key = '" + key + "'";

    sql += keysString.join(" OR ");

    if (!query.exec(sql))
        qCritical() << "Query Error (getConfigs) : " << query.lastError();

    while(query.next()) {
        QString key = query.value(0).toString();
        QString value = query.value(1).toString();
        result.insert(key,value);

        if(cache && !configs.contains(key))
            configs.insert(key,value);
    }

    return result;
}

QString Configurator::getConfig(const QString &key)
{
    QSqlQuery query(QSqlDatabase::database("config"));

    if(cache && configs.contains(key))
    {
#ifdef QT_DEBUG
        qDebug() << "Return key ("+key+") from cache " << configs[key];
#endif
        return configs[key];

    } else {
#ifdef QT_DEBUG
        qDebug() << "Key ("+key+") not in cache";
#endif
        QString sql = "SELECT value FROM config WHERE key=:key LIMIT 1";
        query.prepare(sql);
        query.bindValue(":key", key);

        if (!query.exec())
            qCritical() << "Query Error (getConfig) : " << query.lastError();

        if (query.first())
        {
            QString value = query.value(0).toString();
            configs.insert(key,value);
            return value;
        }

        return "";
    }
}

void Configurator::setConfig(const QByteArray &config)
{
    QJsonDocument json = QJsonDocument::fromJson(config);

    if(json.isObject())
    {
        QJsonObject object = json.object();
        QJsonObject::iterator i;

        for (i = object.begin(); i != object.end(); ++i)
        {
            QString key = QString(i.key());
            QString value = i.value().toString();

            qDebug() << "Config insert : ";
            qDebug() << "Key   : " << key;
            qDebug() << "Value : " << value;

            configs.insert(key,value);
        }

        emit configurationChanged();
    }
}

void Configurator::setConfigByKey(const QString &key,const QString &value)
{
    QSqlQuery query(QSqlDatabase::database("config"));

    QString sql = "UPDATE config SET value=:value WHERE key=:key;";
    query.prepare(sql);
    query.bindValue(":key", key);
    query.bindValue(":value", value);

    if (!query.exec())
    {
        qCritical() << "Query Error (setConfigByKey) : " << query.lastError();

    } else {
        configs.insert(key,value);
        emit configurationChanged();
    }
}

void Configurator::addConfig(const QString &key,const QString &value)
{
    configs.insert(key,value);
}

bool Configurator::checkDatabaseFile(const QString &basePath)
{
    QFile databaseFile(basePath);

    if (!databaseFile.exists())
    {
        qDebug() << "Database Configurator " << databaseFile.fileName() << " not exist";
        return true;
    }

    qDebug() << "Database Configurator " << databaseFile.fileName() << " exist";
    return false;
}
