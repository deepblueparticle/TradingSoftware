#include <QDebug>
#include <QMessageBox>
#include <QCryptographicHash>
#include <shared/commands.h>
#include <shared/response.h>
#include <string.h>
#include "logindialog.h"
#include "ui_logindialog.h"
#include "usersettings.h"

namespace  Algo {

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    QObject::connect( ui->pushButtonLogin, SIGNAL(clicked()),
                      this, SLOT (checkLogin()));
}

void LoginDialog::setErrorMessage(const QString &errorMessage)
{
    _errorMessage = errorMessage;
}

const QString& LoginDialog::getErrorMessage()
{
    return _errorMessage;
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

int LoginDialog::readDataFromSocket(char *readBuffer, int size)
{
    if (TcpClient::getInstance().getSocket().waitForReadyRead(1000))
        return TcpClient::getInstance().getSocket().read(readBuffer, size);
    else
        return false;
}

bool LoginDialog::checkLogin()
{
    TcpClient::getInstance().setHost(Settings::getIP());
    TcpClient::getInstance().setPort(Settings::getPort());

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
                    tr("Login Error"),
                    tr("Network unreachable.") );

        this->reject();
        return false;
    }

    Login l(ui->lineEditLoginName->text().toStdString().c_str(),
            true,
            ui->lineEditPassword->text().toStdString().c_str(),
            Settings::getHeartbeatTimeout());

    bool ret=false;
    int retryCount = 0;
    qDebug() << "Username '" << l.GetUserName() << "', Password '" << l.GetPassword() << "'";
    while(retryCount < MAX_LOGIN_RETRY_COUNT)
    {
        ++retryCount;
        char buffer[MAX_BUF];
        qint64 bytes =  l.Serialize(buffer);

        //Sending Request to server
        qint64 bytesWritten = 0;
        TcpClient::getInstance().writeOnSocket(buffer, bytes, bytesWritten);
        if (bytesWritten == -1) {
            this->setErrorMessage("Connection lost");
            ret =  false;
            break;
        }
        qDebug()<<"Login sent to server";
        char readBuffer[MAX_BUF];
        if(readDataFromSocket( readBuffer, (sizeof(ConnectionRsp)+3)))
        {
            if((ResponseCategory)readBuffer[2] == ResponseCategory_CONNECTION)
            {
                ConnectionRsp conn(readBuffer+3);
                if(l.IsSaltRequest())
                {
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
                }
                else
                {
                    if(conn.getLoginResponse() == LoginResponse_SUCCESS)
                    {
                        Settings::setStartStrategyIndex(conn.getFirstStrategyId());
                        Settings::setBackendId(conn.getBackendId());
                        qDebug() << "Login Success :) " << "Backend Instance Id: " << Settings::getBackendId();
                        ret = true;
                    }
                    else if(conn.getLoginResponse() == LoginResponse_BAD_PASSWORD)
                    {
                        qDebug() << "Wrong password. Login Failed :(";
                        this->setErrorMessage("Wrong password");
                        ret =  false;
                    }
                    else if(conn.getLoginResponse() == LoginResponse_MULTIPLE_LOGINS)
                    {
                        qDebug() << l.GetUserName() << " is already connected!";
                        this->setErrorMessage(QString(l.GetUserName()) + " is already connected!");
                        ret = false;
                    }
                    break;
                }
            }
            else
            {
                qDebug() << "Unhandled Command :(";
                ret =  false;
                break;
            }

        }
    }
    if(ret)
    {
        this->accept();
        return true;
    }
    else
    {
        qDebug() << "Login Failed :(";
        this->reject();
        return false;
    }

}
}
