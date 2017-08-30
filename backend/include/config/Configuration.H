/*
 * =====================================================================================
 *
 *       Filename:  Configuration.H
 *
 *    Description:  This holds all config values
 *
 *        Created:  09/17/2016 09:14:40 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef Configuration_H
#define Configuration_H

#include <string>
#include <mutex>
#include <boost/date_time/local_time/local_time.hpp>

namespace Algo
{
  class Configuration
  {
    public:
      static Configuration *getInstance ();
      std::string& getLogDir ();
      std::string& getConfigFile ();
      std::string& getConfigFileOP ();
      long& getStartOrderId ();
      int& getLogLevel ();
      int& getFXCMFeedLogLevel ();
      int& getFXCMOPLogLevel ();
      int& getServerPort ();
      std::string& getAccountNumberBuy ();
      std::string& getAccountNumberSell ();
      const std::string& getDataFeedVenueId() const;
      const std::string& getOrderVenueId() const;
      int getTExitMultiplier() const;
      int getOpenStrategyAllowance() const;
      bool shouldReattemptPrimary() const;
      std::string& getDBName ();
      const std::string& getCentralDbAddress() const;
      int getCentralDbPort() const;
      const std::string& getCentralDbUser() const;
      const std::string& getCentralDbPassword() const;
      const std::string& getCentralDbName() const;
      int& getMaxAttempts ();
      const int& getTExitInterval ();
      const int& getLogRotateHours();
      const int& getIntervalMillForPersist();
      const int& getPersistFileNum();
      const std::string& getBackendInstanceFileName() const;
      const std::string& getIbGatewayHost() const;
      int getIbGatewayPort() const;

      boost::local_time::time_zone_ptr getTZNewYork();
      void loadConfig();
      void print();

    private:
      std::string _logDir;
      std::string _configFile;
      std::string _configFileOP;
      std::string _ibConfigFile;
      long        _startClOrdrId;
      int         _LogLevel;
      int         _fxcmFeedLogLevel;
      int         _fxcmOPLogLevel;
      int         _serverPort;
      std::string _accountNumberBuy;
      std::string _accountNumberSell;
      std::string _dataFeedVenueId;
      std::string _orderVenueId;
      int         _texitMultiplier;
      int         _openStrategyAllowance;
      bool        _shouldReattemptPrimary;
      std::string _dbName;
      std::string _centralDbAddress;
      int         _centralDbPort;
      std::string _centralDbUser;
      std::string _centralDbPassword;
      std::string _centralDbName;
      int         _maxAttempts;
      int         _tExitInterval;
      int         _logRotateHours;
      int         _intervalMillForPersist;
      int         _persistFileNum;
      std::string _backendInstanceFileName;
      std::string _ibGatewayHost;
      int         _ibGatewayPort;
      boost::local_time::time_zone_ptr _tzNewYork;
      static void rolloutBackupHandler(const char* filename, std::size_t size = 0);
      static void myLoggerCrashHandler(int sig);
      static int  _curFileIndex;
      static std::mutex _mutex;
  };
}
#endif
