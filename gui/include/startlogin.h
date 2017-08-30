#ifndef SATRTLOGIN_H
#define STARTLOGIN_H

#include <QCryptographicHash>
#include <QDebug>
#include <QMessageBox>
#include <shared/commands.h>
#include <shared/response.h>
#include <string>
#include <string.h>
#include "tcpclient.h"
#include "usersettings.h"

namespace Algo {

class StartLogin:public QWidget
{
    Q_OBJECT

public:
    StartLogin()
    {
    }

    StartLogin(std::string username, std::string password)
    {
        m_username = username;
        m_password = password;
        qDebug() << "Username [" << username.c_str() << "] password [" << password.c_str()<<"]";
    }

    bool requestLogin(Login &l);
    int readDataFromSocket(char *readBuffer, int size);

    bool checkLogin();

    static std::string m_username;
    static std::string m_password;

};

}
#endif // LOGIN_H
