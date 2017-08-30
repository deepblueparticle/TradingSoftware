#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <shared/commands.h>
#include "tcpclient.h"

namespace Ui {
class LoginDialog;
}

namespace Algo {


class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    void setErrorMessage(const QString&);
    const QString& getErrorMessage();
    ~LoginDialog();



public slots:
    bool checkLogin();

private:
    //TcpClient
    Ui::LoginDialog *ui;
    QString _errorMessage;
    const int MAX_LOGIN_RETRY_COUNT = 5;
    int readDataFromSocket(char *readBuffer, int size);
};
}
#endif // LOGINDIALOG_H
