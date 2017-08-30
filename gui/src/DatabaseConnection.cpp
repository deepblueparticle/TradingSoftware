#include "DatabaseConnection.h"
#include "usersettings.h"
#include <QDebug>
#include <QTime>

DatabaseConnection& DatabaseConnection::getInstance()
{
    static DatabaseConnection db;
    return db;
}

DatabaseConnection::DatabaseConnection()
{
    qDebug() << "Db connection start-time: " << QTime().currentTime();
    _db = QSqlDatabase::addDatabase("QMYSQL");
    _db.setHostName(Algo::Settings::getCentralDbAddress());
    _db.setPort(Algo::Settings::getCentralDbPort());
    _db.setDatabaseName(Algo::Settings::getCentralDbName());
    _db.setUserName(Algo::Settings::getCentralDbUser());
    _db.setPassword(Algo::Settings::getCentralDbPassword());
    qDebug() << "Db connection end-time: " << QTime().currentTime();
    qDebug() << "DB Opened? " << _db.open();
}

QSqlQuery DatabaseConnection::executeQuery(const QString &queryString)
{
    QSqlQuery query;
    _mutex.lock();
    query.exec(queryString);
    _mutex.unlock();
    return query;
}

