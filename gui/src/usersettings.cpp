#include "usersettings.h"
namespace  Algo {

QString Settings::_ip=QString("127.0.0.1");
int     Settings::_port=12345;
QString Settings::_logDir=QString("");
int     Settings::_heartbeatTimeout=30;
long    Settings::_lastStartStrategyIndex=0;
long    Settings::_backendId=-1;

QString  Settings::_centralDbAddress = QString("127.0.0.1");
long     Settings::_centrailDbPort = 1234;
QString  Settings::_centralDbUser = QString("root");
QString  Settings::_centralDbPassword = QString("1234");
QString  Settings::_centralDbName = QString("1234");
bool     Settings::_shouldUseTickSize = true;
bool     Settings::_isReadOnly = false;

QString& Settings::getIP()
{
    return _ip;
}

void Settings::setIP(QString ip)
{
    _ip=ip;
}

int& Settings::getPort()
{
   return _port;
}

void Settings::setPort(int port)
{
    _port = port;
}

QString& Settings::getLogDir()
{
   return _logDir;
}

void Settings::setLogDir(QString logDir)
{
    _logDir = logDir;
}

int& Settings::getHeartbeatTimeout()
{
    return _heartbeatTimeout;
}

void Settings::setHeartbeatTimeout(int t)
{
   _heartbeatTimeout = t;
}

long& Settings::getStartStrategyIndex()
{
    return _lastStartStrategyIndex;
}

void Settings::setStartStrategyIndex(long t)
{
   _lastStartStrategyIndex = t;
}

long Settings::getBackendId()
{
    return _backendId;
}

void Settings::setBackendId(long t)
{
    _backendId = t;
}

const QString& Settings::getCentralDbAddress()
{
    return _centralDbAddress;
}

void Settings::setCentralDbAddress(const QString &centralDbAddress)
{
    _centralDbAddress = centralDbAddress;
}

const long& Settings::getCentralDbPort()
{
    return _centrailDbPort;
}

void Settings::setCentralDbPort(const long &centralDbPort)
{
    _centrailDbPort = centralDbPort;
}

const QString& Settings::getCentralDbUser()
{
    return _centralDbUser;
}

void Settings::setCentralDbUser(const QString &centralDbUser)
{
    _centralDbUser = centralDbUser;
}

const QString& Settings::getCentralDbPassword()
{
    return _centralDbPassword;
}

void Settings::setCentralDbPassword(const QString &centralDbPassword)
{
    _centralDbPassword = centralDbPassword;
}

const QString& Settings::getCentralDbName()
{
    return _centralDbName;
}

void Settings::setCentralDbName(const QString &centralDbName)
{
    _centralDbName = centralDbName;
}

bool Settings::shouldUseTickSize()
{
    return _shouldUseTickSize;
}

void Settings::setShouldUseTickSize(bool shouldUseTickSize)
{
    _shouldUseTickSize  = shouldUseTickSize;
}

bool Settings::isReadOnly()
{
    return _isReadOnly;
}

void Settings::setReadOnly(bool isReadOnly)
{
    _isReadOnly = isReadOnly;
}
}
