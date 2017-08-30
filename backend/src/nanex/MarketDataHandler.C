#include <algorithm>
#include <iterator>
#include <BackendConfig.H>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <boost/lexical_cast.hpp>
#include <nanex/MarketDataHandler.H>
#include <util/TimeUtil.H>

#include <shared/easylogging++.h>

#include <quote/QuoteBook.H>

namespace Algo
{
namespace Nanex
{

void MarketDataHandler::loadSymbols(std::string filename)
{
  std::ifstream ifile;
  ifile.open(filename.c_str());
  if(!ifile)
    return;
  std::string line;
  while(ifile>>line)
  {
    std::istringstream lineStream(line);
    std::string key, value;
    std::getline(lineStream,key,',');
    std::getline(lineStream,value,',');
    _symbolList[key] = value;
    LOG(INFO) << "[Nanex MD] Loaded symbol: " << key << " " << value;
  }
}

void MarketDataHandler::loadPreTradingHours(std::string filename)
{
  std::ifstream ifile;
  ifile.open(filename.c_str());
  if(!ifile)
    return;
  std::string line;
  LOG(INFO) << "[Nanex MD] Loading Nanex pre-trading hours.";
  while(ifile>>line)
  {
    std::istringstream lineStream(line);
    std::string dayStr, startMilliSecondsStr, endMilliSecondsStr;
    std::getline(lineStream,dayStr,',');
    std::getline(lineStream,startMilliSecondsStr,',');
    std::getline(lineStream,endMilliSecondsStr, ',');
    TimeRange t;
    t.startMilliSeconds = boost::lexical_cast<unsigned long>(startMilliSecondsStr);
    t.endMilliSeconds = boost::lexical_cast<unsigned long>(endMilliSecondsStr);
    int day = boost::lexical_cast<int>(dayStr);
    _preTradingHours[day] = t;
    LOG(INFO) << "[Nanex MD] Day: " << day << " " << t.startMilliSeconds << " " << t.endMilliSeconds;
  }
}

MarketDataHandler::MarketDataHandler() : _isFeedConnected(false), _isRealTime(false)
{
}

void MarketDataHandler::processMessages()
{
  loadSymbols("./config/IBNanexSymbolMap.csv");
  loadPreTradingHours("./config/NanexPreTradingHours.csv");
  if(_nxCore.LoadNxCore(NANEX_LIB))
  {
    int returnValue;
    while(1)
    {
      returnValue = _nxCore.ProcessTape("", NULL, (NxCF_EXCLUDE_QUOTES2 | NxCF_EXCLUDE_OPRA), 0, &MarketDataHandler::onNxCoreCallback);
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));
      LOG(INFO) << "[Nanex MD] Requesting tape file again. " << returnValue;
      _isFeedConnected = false;
      _isRealTime = false;
    }
    _nxCore.ProcessReturnValue(returnValue);
    _nxCore.UnloadNxCore();
  }
}

void MarketDataHandler::onNxCoreStatus(const NxCoreSystem* pNxCoreSys,const NxCoreMessage* pNxCoreMsg)
{
  switch (pNxCoreSys->Status)
  {
    case NxCORESTATUS_WAITFORCOREACCESS:
    {
      _isFeedConnected = false;
      LOG(INFO) << "[Nanex MD] NxCore Wait For Access.";
      break;
    }
    case NxCORESTATUS_RESTARTING_TAPE:
    {
      LOG(INFO) << "[Nanex MD] NxCore Restart Tape Message.";
      break;
    }
    case NxCORESTATUS_ERROR:
    {
      LOG(INFO) << "[Nanex MD] NxCore Error.";
      break;
    }
    case NxCORESTATUS_RUNNING:
    {
      _isFeedConnected = true;
      break;
    }
    case NxCORESTATUS_COMPLETE:
    {
      LOG(INFO) << "[Nanex MD] NxCore Complete Message.";
      break;
    }
    default:
    {
      LOG(INFO) << "[Nanex MD] Nanex status message " << pNxCoreSys->Status;
    }
  }
}

int MarketDataHandler::onNxCoreCallback(const NxCoreSystem* pNxCoreSys,const NxCoreMessage* pNxCoreMsg)
{
  MarketDataHandler &handler = MarketDataHandler::getInstance();
  switch (pNxCoreMsg->MessageType)
  {
    case NxMSG_STATUS:
    {
      handler.onNxCoreStatus(pNxCoreSys, pNxCoreMsg);
      break;
    }
    case NxMSG_EXGQUOTE:
    {
      handler.onGetExgQuoteStates(pNxCoreSys, pNxCoreMsg);
      break;
    }
    case NxMSG_SYMBOLSPIN:
    {
      if (pNxCoreMsg->coreHeader.pnxStringSymbol->String[0]=='f')
      {
        std::string symbol = pNxCoreMsg->coreHeader.pnxStringSymbol->String;
        if (handler._symbolList.find(symbol) != handler._symbolList.end())
        {
          pNxCoreMsg->coreHeader.pnxStringSymbol->UserData1=1;
          LOG(INFO) << "[Nanex MD] Interest set for symbol: " << symbol;
        }
      }
      break;
    }
    default:
    {
      LOG(DEBUG) << "[Nanex MD] Nanex message type " << pNxCoreMsg->MessageType;
    }
  }
  return NxCALLBACKRETURN_CONTINUE;
}

bool MarketDataHandler::isRealTime(const NxCoreSystem* pNxCoreSys)
{
  if (!_isRealTime)
  {
    NxDate date = pNxCoreSys->nxDate;
    NxTime time = pNxCoreSys->nxTime;
    int day, month, year;
    int nxDay, nxMonth, nxYear;
    TimeUtil::getDayMonthYear(day, month, year);
    nxDay = (int)date.Day;
    nxMonth = (int)date.Month;
    nxYear = (int)date.Year;
    if ((day == nxDay) && (month == nxMonth) && (year == nxYear))
    {
      _isRealTime = ((TimeUtil::getMilliSecondsFromStartOfDay() - time.MsOfDay) <= 250);
      if (_isRealTime)
      {
        LOG(INFO) << "[Nanex MD] Matched with Realtime feed now.";
      }
      else
      {
        std::cout << "[Nanex MD] NxTime " << (int)date.Day << "-" << (int)date.Month << "-" << date.Year << " "
                  << (int)time.Hour << ":" << (int)time.Minute << ":" << (int)time.Second << ":"
                  << time.Millisecond << " " << time.MsOfDay << " " << TimeUtil::getMilliSecondsFromStartOfDay() << std::endl;
      }
    }
  }
  return _isRealTime;
}

bool MarketDataHandler::isUnderPreTradingHours(const NxCoreSystem* pNxCoreSys)
{
  int dayOfWeek = pNxCoreSys->nxDate.DayOfWeek;
  unsigned long msOfDay = pNxCoreSys->nxTime.MsOfDay;
  TimeRange t = _preTradingHours[dayOfWeek];
  if (t.startMilliSeconds <= msOfDay && msOfDay <= t.endMilliSeconds)
  {
    LOG(INFO) << "[Nanex MD] Realtime data. " << msOfDay << " Day " << dayOfWeek
              << ". Ticks fall under pre-trading hours now. [" << t.startMilliSeconds
              << ":" << t.endMilliSeconds << "]";
    QuoteBook::getInstance()->clear();
    return true;
  }
  return false;
}

void MarketDataHandler::onGetExgQuoteStates(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
  if(pNxCoreMsg->coreHeader.pnxStringSymbol->UserData1 && isRealTime(pNxCoreSys) && !isUnderPreTradingHours(pNxCoreSys))
  {
    std::string symbol = pNxCoreMsg->coreHeader.pnxStringSymbol->String;
    std::map<std::string, std::string>::iterator iter = _symbolList.find(symbol);
    if (iter != _symbolList.end())
    {
      NxCoreExgQuote* Quote = (NxCoreExgQuote*) &pNxCoreMsg->coreData.ExgQuote;
      double bid = _nxCore.PriceToDouble(Quote->coreQuote.BidPrice, Quote->coreQuote.PriceType);
      double ask = _nxCore.PriceToDouble(Quote->coreQuote.AskPrice, Quote->coreQuote.PriceType);
      Quotes q;
      q.setSecurityId(iter->second);
      q.setBidPrice(bid);
      q.setBidQty(0);
      q.setAskPrice(ask);
      q.setAskQty(0);
      QuoteBook::getInstance()->add(q.getSecurityId(), q);
      LOG(DEBUG) << "[Nanex MD] Ticker " << iter->second << " BID price: " << bid << " ASK price: " << ask; 
    }
  }
}

MarketDataHandler::~MarketDataHandler() {}

bool MarketDataHandler::subscribeImplementation(const std::string& symbol) {}

bool MarketDataHandler::isConnected()
{
  return _isFeedConnected && _isRealTime;
}

bool MarketDataHandler::unsubscribeImplementation(const std::string& symbol) {}
}
}
