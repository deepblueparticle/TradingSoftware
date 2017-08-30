#ifndef USERSETTINGS
#define USERSETTINGS

#include <QString>
namespace Algo {

class Settings
{
private:
    static QString  _ip;
    static int      _port;
    static QString  _logDir;
    static int      _heartbeatTimeout;
    static long     _lastStartStrategyIndex;
    static long     _backendId;
    static bool     _isReadOnly;

    static QString  _centralDbAddress;
    static long     _centrailDbPort;
    static QString  _centralDbUser;
    static QString  _centralDbPassword;
    static QString  _centralDbName;
    static bool     _shouldUseTickSize;
public:
    static QString& getIP();
    static void setIP(QString ip);

    static int& getPort();
    static void setPort(int port);


    static QString& getLogDir();
    static void setLogDir(QString logDir);

    static int& getHeartbeatTimeout();
    static void setHeartbeatTimeout(int t);

    static long& getStartStrategyIndex();
    static void setStartStrategyIndex(long t);

    static long getBackendId();
    static void setBackendId(long t);

    static const QString& getCentralDbAddress();
    static void setCentralDbAddress(const QString &centralDbAddress);

    static const long& getCentralDbPort();
    static void setCentralDbPort(const long &centralDbPort);

    static const QString& getCentralDbUser();
    static void setCentralDbUser(const QString &centralDbUser);

    static const QString& getCentralDbPassword();
    static void setCentralDbPassword(const QString &centralDbPassword);

    static const QString& getCentralDbName();
    static void setCentralDbName(const QString &centralDbName);

    static bool shouldUseTickSize();
    static void setShouldUseTickSize(bool shouldUseTickSize);

    static bool isReadOnly();
    static void setReadOnly(bool isReadOnly);
};
}
#endif // USERSETTINGS

