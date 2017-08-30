#include "mainwindow.h"
#include <QApplication>

#include "logindialog.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QThread>

#include "usersettings.h"

#include <QDate>
#include <QTime>

#define MY_ASSERT(c) if (c == false) {}
#define MY_ASSERT_X(c, where, what) if (c == false) {}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QDate cd = QDate::currentDate();

    QString logfile("AlgoModel_");
    logfile += cd.toString(Qt::ISODate);
    logfile += QString(".txt");

    QFile file(logfile);

    MY_ASSERT(file.open(QIODevice::Append | QIODevice::Text));

    QTextStream out(&file);
    out << QTime::currentTime().toString("hh:mm:ss.zzz ");

    switch (type)
    {
    case QtDebugMsg:	out << "DBG"; break;
    case QtWarningMsg:  out << "WRN"; break;
    case QtCriticalMsg: out << "CRT"; break;
    case QtFatalMsg:    out << "FTL"; break;
    }

    out << " AlgoModel [" << context.function <<" : "<<context.line <<" ] "<< msg << '\n';
    out.flush();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);
    Algo::LoginDialog ld;
    QString configFile("./config.ini");

    qDebug() << "======================== New Start ========================";
    qDebug() << "QDir::currentPath() " << QDir::currentPath() ;

    if(QFile(configFile).exists())
    {
        QSettings settings(configFile, QSettings::IniFormat);

        settings.sync();
        settings.beginGroup("Server");
        Algo::Settings::setIP(settings.value("server_ip",15).toString());
        Algo::Settings::setPort(settings.value("server_port", 5).toInt());
        Algo::Settings::setLogDir(settings.value("log_dir",100).toString());
        Algo::Settings::setHeartbeatTimeout(settings.value("heatbeat_timeout",30).toInt());
        Algo::Settings::setCentralDbAddress(settings.value("CentralDbAddress").toString());
        Algo::Settings::setCentralDbPort(settings.value("CentralDbPort").toLongLong());
        Algo::Settings::setCentralDbUser(settings.value("CentralDbUser").toString());
        Algo::Settings::setCentralDbPassword(settings.value("CentralDbPassword").toString());
        Algo::Settings::setCentralDbName(settings.value("CentralDbName").toString());
        Algo::Settings::setShouldUseTickSize(settings.value("UseTickSize").toBool());
        Algo::Settings::setReadOnly(settings.value("ReadOnly").toBool());
        settings.endGroup();
    }
    else
        qDebug() << configFile <<" not exist";

    bool flag = true;
    while(flag)
    {
        if(ld.exec() == QDialog::Accepted)
        {
            flag = false;
        }
        else
        {
            if( QMessageBox::Yes ==
                    QMessageBox(QMessageBox::Information,
                                "Login Error",
                                ld.getErrorMessage() + "Do you want to try?",
                                QMessageBox::Yes|QMessageBox::No
                                ).exec()
                    )
            {
                qDebug() << "Yes was clicked";
            }
            else
            {
                a.quit();
                return -1;
            }
        }
    }
    Algo::MainWindow w;
    qDebug() << "Requesting symbol info";
    w.show();
    w.showMaximized();
    QObject::connect(&(Algo::TcpClient::getInstance().getSocket()),
            SIGNAL(readyRead()),
            &(Algo::TcpClient::getInstance()),
            SLOT(readServer()));

    return a.exec();
}
