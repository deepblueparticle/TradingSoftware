#include "startlogin.h"
namespace  Algo {

std::string StartLogin::m_username;
std::string StartLogin::m_password;

bool StartLogin::requestLogin(Login& l)
{

    bool retType = false;
    //if(QAbstractSocket::ConnectedState == TcpClient::getInstance().getSocket().state())
    {
        char buffer[MAX_BUF];
        qint64 bytes =  l.Serialize(buffer);

        //Sending Request to server
        qint64 bytesWritten = 0;
        TcpClient::getInstance().writeOnSocket(buffer, bytes, bytesWritten);

        if(bytesWritten == bytes)
        {
            retType = true;
        }
    }
    return retType;
}

int StartLogin::readDataFromSocket(char *readBuffer, int size)
{
    if (!TcpClient::getInstance().getSocket().canReadLine())
        TcpClient::getInstance().getSocket().waitForReadyRead(-1);

    return TcpClient::getInstance().getSocket().read(readBuffer, size);
}

bool StartLogin::checkLogin()
{
    QObject::disconnect(&(TcpClient::getInstance().getSocket()),
            SIGNAL(readyRead()),
            &(TcpClient::getInstance()),
            SLOT(readServer()));

    qDebug() << "Username [" << m_username.c_str() << "] password [" << m_password.c_str()<<"]";
    Login l(m_username.c_str(), true,m_password.c_str(), Settings::getHeartbeatTimeout());
    TcpClient::getInstance().connectToHost();

    if(TcpClient::getInstance().getSocket().waitForConnected(3000))
        // putting 1 as parameter isn't reasonable, using default 3000ms value
    {
        qDebug()<<"Connected to host";
    }
    else
    {
        qDebug()<<"Cannot Connected to host";
        QMessageBox::information(
                    this,
                    QString("Login Error"),
                    QString("Network unreachable.") );

        return false;
    }

    if(requestLogin(l))
    {
        //if(QAbstractSocket::ConnectedState == TcpClient::getInstance().getSocket().state())
        {
            char readBuffer[500];
            if(readDataFromSocket( readBuffer, (sizeof(ConnectionRsp)+3)))
            {
                if((ResponseCategory)readBuffer[2] == ResponseCategory_CONNECTION)
                {
                    ConnectionRsp conn(readBuffer+3);
                    l.SetSaltRequest(false);
                    // encrypting password using salt
                    QCryptographicHash hasher(QCryptographicHash::Sha1);
                    QString encryptString = conn.getPasswordSalt();
                    encryptString += l.GetPassword();
                    hasher.reset();
                    hasher.addData(encryptString.toUtf8());
                    QString encryptedPassword = QString(hasher.result().toHex());
                    l.SetPassword((char *)(encryptedPassword.toStdString().c_str()));
                    qDebug() << "Final pwd = "<< l.GetPassword();
                    qDebug() << "Authenticating Login...Please wait";
                    if(requestLogin(l))
                    {
                        memset(readBuffer,' ',500);
                        if( (sizeof(ConnectionRsp)+3) == readDataFromSocket( readBuffer, (sizeof(ConnectionRsp)+3)))
                        {
                            if(readBuffer[2] == ResponseCategory_CONNECTION)
                            {
                                ConnectionRsp conn(readBuffer+3);
                                if(conn.getLoginResponse() == LoginResponse_SUCCESS)
                                {
                                    qDebug() << "Login Success :)";
                                    return true;
                                }
                                else if(conn.getLoginResponse() == LoginResponse_BAD_PASSWORD)
                                {
                                    qDebug() << "Login Failed :(";
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            qDebug() << "Login Failed :( Unknown command from server";
                            return false;
                        }

                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

    }
    else
    {
        QMessageBox::information(
                    this,
                    tr("Login Error"),
                    tr("Network unreachable.") );

        return false;
    }
    return false;
}

}
