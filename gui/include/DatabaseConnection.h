#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMutex>

class DatabaseConnection
{
public:
    static DatabaseConnection& getInstance();
    QSqlQuery executeQuery(const QString &query);

private:
    DatabaseConnection();
    QSqlDatabase _db;
    QMutex _mutex;
};

#endif // DATABASECONNECTION_H
