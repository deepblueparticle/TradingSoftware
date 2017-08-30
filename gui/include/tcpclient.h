#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QNetworkSession>

#ifdef _WIN32
#include <Winsock2.h>
#endif
namespace Algo{

class TcpClient: public QObject
{
    Q_OBJECT
public:
    TcpClient();
    ~TcpClient();
    static TcpClient& getInstance();

    void setHost(QString h) { host = h; }
    void setPort(quint16 p) { port = p; }
    void connectToHost();
    bool writeToServer(const char *bytes, qint16 length);
    QTcpSocket& getSocket();
    void writeOnSocket(char *buffer, qint64 bytes, qint64 &bytesWritten);

public slots:
    void readServer();

signals:
    void socketDiconnected();
    void dataReceived(std::string);

private slots:
    void readreadyHeartbeatMessage();
private:
    void pushData(std::string data);

private:
    QTcpSocket      *socket;
    quint16         blockSize;
    QString         host;
    quint16         port;
    QNetworkSession *networkSession;
};
}
#endif // TCPCLIENT_H
