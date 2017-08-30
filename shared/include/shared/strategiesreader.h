#ifndef STRATEGIESREADER_H
#define STRATEGIESREADER_H

#include <string>
#include <functional>
#include <memory>


namespace  Algo {

struct algomodel1;

struct SymbolData {
 std::string secType;
 std::string exchange;
 std::string currency;
};

typedef std::function<void(std::shared_ptr<algomodel1>)> InsertAlgo;

void getStrategiesFromExcel(const std::string &excelFileName, InsertAlgo insertAlgo, int &strategyAdded, std::string upname="");

}

#endif // STRATEGIESREADER_H
