#include <shared/client_utils.h>

#include <QStringList>

namespace Algo {

    //Verify time in HH:MM:SS and between 00:00:00 and 24:00:00
    bool verifyTime(const QString &time)
    {
        if(time.length()!=8)
        {
            return false;
        }
        int count =0;
        for(auto ch : time)
            if(ch==':')
                ++count;

        if(count != 2)
            return false;
        QStringList splitted = time.split(":");
        int HH = splitted[0].toInt();
        int MM = splitted[1].toInt();
        int SS = splitted[2].toInt();
        if(HH>24)
        {
            return false;
        }
        else if(HH==24)
        {
            if(MM!=0 && SS!=0)
                return false;
            else
                return true;
        }
        else if(HH>=0&&HH<=23)
        {
            if(MM>59 || SS >59)
                return false;
            else
                return true;
        }
        else
        {
            return false;
        }
    }
}
