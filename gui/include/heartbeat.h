#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <QThread>
#include <QDebug>
#include <shared/commands.h>
#include "tcpclient.h"
#include "usersettings.h"

namespace Algo {

class HeartbeatThread : public QThread
{
    Q_OBJECT

public:
    HeartbeatThread()
        :m_stop(false)
    {}

    void run()
    {
        while (true)
        {
            emit(readyHeartbeatMessage());
            sleep(Settings::getHeartbeatTimeout());

        }
    }
    void stopHearbeat()
    {
        m_stop = true;
    }

signals:
    void readyHeartbeatMessage();
private:
    bool m_stop;
};
}
#endif // HEARTBEAT_H
