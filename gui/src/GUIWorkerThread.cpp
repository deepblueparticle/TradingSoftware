#include "GUIWorkerThread.h"
#include "DatabaseConnection.h"
#include "usersettings.h"
#include "GuiQuery.h"
#include <QDebug>
#include <algorithm>
#include <sstream>

GUIWorkerThread::GUIWorkerThread(const QString &query) : _query (query)
{
}

void GUIWorkerThread::run()
{
    DatabaseConnection &dbConnection = DatabaseConnection::getInstance();
    QSqlQuery query = dbConnection.executeQuery(_query);
    emit resultReady(query);
}
