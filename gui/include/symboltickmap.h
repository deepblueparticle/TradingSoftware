#ifndef SYMBOLTICKMAP
#define SYMBOLTICKMAP

#include <QDebug>
#include <map>
#include <string>
#include <tuple>

namespace Algo {

class SymolTickMap
{
public:
    static SymolTickMap& getInstance()
    {
        static SymolTickMap sm;
        return sm;
    }

    void add(std::string str, float tick, long long lot, int precission)
    {
        qDebug() << "Adding Instrument ["<< QString(str.c_str()) <<"] tick [" << tick <<"] Lot ["
                 << lot << "] Precision ["<<precission << "] in Map";
        m_symbolmap.insert(std::pair<std::string, std::tuple<float, long long, int>>
                                (str, std::tuple<float, long long, int>(tick, lot, precission)));
    }
    bool get(std::string sym, float& tick, long long& lot, int& precission)
    {
        if(m_symbolmap.find(sym) == m_symbolmap.end())
        {
            qDebug() << "Instrument [" << QString(sym.c_str()) << "] not found in the map.";
            return false;
        }
        tick = std::get<0>(m_symbolmap[sym]);
        lot = std::get<1>(m_symbolmap[sym]);
        precission = std::get<2>(m_symbolmap[sym]);
        return true;
    }

    bool isExist(std::string sym)
    {
        if(m_symbolmap.find(sym) == m_symbolmap.end())
        {
            return false;
        }
        return true;
    }

private:
    SymolTickMap() {}
    std::map<std::string, std::tuple<float, long long, int>> m_symbolmap;
};

}
#endif // SYMBOLTICKMAP

