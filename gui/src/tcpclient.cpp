#include "tcpclient.h"
#include <shared/commands.h>

#include <QtNetwork>
#include <QDebug>

#ifdef _WIN32
#include <Mswsock.h>
#include <Ws2def.h>
#else
#include <netinet/tcp.h>
#include <sys/socket.h>
#endif
namespace  Algo {

TcpClient::TcpClient( )
    : networkSession(0)
{
    socket = new QTcpSocket( this );
#ifndef _WIN32
    int enableKeepAlive = 1;
    int fd = socket->socketDescriptor();
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive));

    int maxIdle = 10; /* seconds */
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));

    int count = 3; // send up to 3 keepalive packets out, then disconnect if no response
    setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));

    int interval = 2; // send a keepalive packet out every 2 seconds (after the 5 second idle period)
    setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
#endif
    //connect(socket, SIGNAL(writeToServer()), this, SLOT(writeToServer()));
    //connect(socket, SIGNAL(readyRead()), this, SLOT(readServer()));
}

void TcpClient::readreadyHeartbeatMessage()
{
    char buffer[MAX_BUF];
    ConnStatus cs;
    int size = cs.Serialize(buffer);
    bool socketstatus = true;
    if(!writeToServer(buffer, size))
    {
        socketstatus= false;
    }
    if(!socketstatus || getSocket().state() != QTcpSocket::ConnectedState)
    {
        qDebug() << "Socket disconnected.....";
        emit(socketDiconnected());
    }

}

TcpClient& TcpClient::getInstance()
{
    static TcpClient tcpclient;
    return tcpclient;
}

void TcpClient::connectToHost()
{
    if(socket->isOpen())
    {
        qDebug()<<"Socket is already open. Closing it before reconnect.";
        socket->close();
    }
    qDebug() << "Connecting to " << host << ":"<<port;
    socket->connectToHost(host, port);
}

bool TcpClient::writeToServer(const char* bytes, qint16 length)
{
    qint64 siz = socket->write(bytes, length);
    if(socket->waitForBytesWritten(1000))
    {
        qDebug() << "Packet size ["<<length<<"] data written ["<<siz<<"]";
        return true;
    }
    qDebug() << "Error in sending data to server";
    return false;
}

QTcpSocket &TcpClient::getSocket()
{
    return *socket;
}

void TcpClient::writeOnSocket(char *buffer, qint64 bytes, qint64 &bytesWritten)
{
    qint64 bytestoWrite = bytes;
    while(bytestoWrite !=
            (bytesWritten += getSocket().write((const char*)(buffer+bytesWritten),bytes-bytesWritten)) )
    {
        if(-1 == bytesWritten)
        {
            qDebug()<<"Error while writing on socket";
            break;
        }
    }
}


void TcpClient::readServer()
{
    while (socket->bytesAvailable()>0)
    {
        QByteArray data(socket->readAll());
        std::string str(data.data(), data.size());
        pushData(str);
    }
}

void TcpClient::pushData(std::string data)
{
    emit dataReceived(data);
}

TcpClient::~TcpClient()
{

}

}
