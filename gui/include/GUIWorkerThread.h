#ifndef GUIWORKERTHREAD_H
#define GUIWORKERTHREAD_H

#include <QThread>
#include <vector>
#include <QtSql/QSqlQuery>

Q_DECLARE_METATYPE(QSqlQuery)
class GUIWorkerThread : public QThread
{
public:
    GUIWorkerThread(const QString &query);
    Q_OBJECT
    void run();

private:
    QString _query;

signals:
    void resultReady(const QSqlQuery &s);
};

#endif // STRATEGYWINDOWTHREAD_H
