/*
 * =====================================================================================
 *
 *       Filename:  Configuration.C
 *
 *    Description:
 *
 *        Created:  09/22/2016 06:17:54 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */


#include <shared/easylogging++.h>
#include <config/Configuration.H>
#include <constant/ServerConstant.H>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <thread>

INITIALIZE_EASYLOGGINGPP
namespace Algo
{
  namespace bpo = boost::program_options;
  int Configuration::_curFileIndex = 0;
  std::mutex Configuration::_mutex;

  Configuration* Configuration::getInstance()
  {
    static Configuration conf;
    return &conf;
  }

  std::string& Configuration::getLogDir()
  {
    return _logDir;
  }

  std::string& Configuration::getConfigFile()
  {
    return _configFile;
  }

  std::string& Configuration::getConfigFileOP()
  {
    return _configFileOP;
  }

  long& Configuration::getStartOrderId()
  {
    return _startClOrdrId;
  }

  int& Configuration::getLogLevel()
  {
    return _LogLevel;
  }

  int& Configuration::getFXCMFeedLogLevel()
  {
    return _fxcmFeedLogLevel;
  }

  int& Configuration::getFXCMOPLogLevel()
  {
    return _fxcmOPLogLevel;
  }
  int& Configuration::getServerPort()
  {
    return _serverPort;
  }

  std::string& Configuration::getAccountNumberBuy()
  {
    return _accountNumberBuy;
  }

  std::string& Configuration::getAccountNumberSell()
  {
    return _accountNumberSell;
  }

  const std::string& Configuration::getDataFeedVenueId() const
  {
    return _dataFeedVenueId;
  }

  const std::string& Configuration::getOrderVenueId() const
  {
    return _orderVenueId;
  }

  int Configuration::getTExitMultiplier() const
  {
    return _texitMultiplier;
  }

  int Configuration::getOpenStrategyAllowance() const
  {
    return _openStrategyAllowance;
  }

  bool Configuration::shouldReattemptPrimary() const
  {
    return _shouldReattemptPrimary;
  }

  std::string& Configuration::getDBName()
  {
    return _dbName;
  }

  const std::string& Configuration::getCentralDbAddress() const
  {
    return _centralDbAddress;
  }

  int Configuration::getCentralDbPort() const
  {
    return _centralDbPort;
  }

  const std::string& Configuration::getCentralDbUser() const
  {
    return _centralDbUser;
  }

  const std::string& Configuration::getCentralDbPassword() const
  {
    return _centralDbPassword;
  }

  const std::string& Configuration::getCentralDbName() const
  {
    return _centralDbName;
  }

  int& Configuration::getMaxAttempts()
  {
    return _maxAttempts;
  }

  const int& Configuration::getTExitInterval()
  {
    return _tExitInterval;
  }

  const int& Configuration::getLogRotateHours()
  {
    return _logRotateHours;
  }

  const int& Configuration::getIntervalMillForPersist()
  {
    return _intervalMillForPersist;
  }

  const int& Configuration::getPersistFileNum()
  {
    return _persistFileNum;
  }

  const std::string& Configuration::getBackendInstanceFileName() const
  {
    return _backendInstanceFileName;
  }

  const std::string& Configuration::getIbGatewayHost() const {
    return _ibGatewayHost;
  }

  int Configuration::getIbGatewayPort() const {
    return _ibGatewayPort;
  }

  boost::local_time::time_zone_ptr Configuration::getTZNewYork()
  {
    return _tzNewYork;
  }

  void Configuration::rolloutBackupHandler(const char* filename, std::size_t size) {
    std::lock_guard<std::mutex> l(_mutex);
    std::stringstream newNameStream;
    newNameStream << filename << "." << _curFileIndex;
    std::string newName(newNameStream.str().c_str());
    std::stringstream compressCmdStream;
    compressCmdStream << "tar zcvf " << newName << ".tar.gz " << newName << " --remove-files";

    if (size > 0)
    {
      std::cout << "Rolling out [" << filename << "] as [" << newName << "] and compress because it reached [" << size << " bytes]" << std::endl;
    }
    else
    {
      std::cout << "Rolling out [" << filename << "] as [" << newName << "] and compress because time is up" << std::endl;
    }
    // BACK IT UP


    try
    {
      boost::filesystem::rename(filename, newName);
      std::string compressCmd = compressCmdStream.str();
      std::thread logCompressThread([compressCmd]() {
        system(compressCmd.c_str());
      });

      logCompressThread.detach();
    }
    catch (std::exception& e )
    {
      std::cout << "Logging file [" << filename << "] cannot be archived because " << e.what()<< std::endl;
    }
    catch (...)
    {
      std::cout << "Logging file [" << filename << "] cannot be archived!" << std::endl;
      return;
    }
    _curFileIndex++;
  }

  void Configuration::loadConfig()
  {
    // Load configuration from file
    el::Configurations conf("./config/logger.conf");
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Helpers::installPreRollOutCallback(Configuration::rolloutBackupHandler);
    el::Loggers::reconfigureAllLoggers(conf);
    _curFileIndex = 0;

    try
    {
      bpo::options_description config("Configuration");
      config.add_options()

      ("COMMON.LogDir",
       bpo::value<std::string>(&_logDir)->default_value("./Log/"),
       "Log root directory")

      ("COMMON.ConfigFile",
       bpo::value<std::string>(&_configFile)->default_value("FIXConfig.conf"),
       "FIX Config")

      ("COMMON.ConfigFileOP",
       bpo::value<std::string>(&_configFileOP)->default_value("FIXOPConfig.conf"),
       "FIX OP Config")

      ("COMMON.IBConfigFile",
       bpo::value<std::string>(&_ibConfigFile)->default_value("IBConfig.conf"),
       "Interactive Brokers Config")

      ("COMMON.StartClOprderID",
       bpo::value<long>(&_startClOrdrId)->default_value(1),
       "Client Order Id")

      ("COMMON.LogLevel",
       bpo::value<int>(&_LogLevel)->default_value(3),
       "Log Level")

      ("COMMON.FXCMFeedLogLevel",
       bpo::value<int>(&_fxcmFeedLogLevel)->default_value(0),
       "FXCM Feed Log Level")

      ("COMMON.FXCMOPLogLevel",
       bpo::value<int>(&_fxcmOPLogLevel)->default_value(0),
       "FXCM OP Log Level")

      ("COMMON.Port",
       bpo::value<int>(&_serverPort)->default_value(12345),
       "Server Port Number")

      ("COMMON.AccountNumberBuy",
       bpo::value<std::string>(&_accountNumberBuy)->default_value("1000662639"),
       "Account Number Buy")

      ("COMMON.AccountNumberSell",
       bpo::value<std::string>(&_accountNumberSell)->default_value("1000662639"),
       "Account Number Sell")

      ("COMMON.DataFeedVenueId",
       bpo::value<std::string>(&_dataFeedVenueId)->default_value("FXCM"),
       "Data Feed Venue Identifier")

      ("COMMON.OrderVenueId",
       bpo::value<std::string>(&_orderVenueId)->default_value("FXCM"),
       "Order Venue Identifier")

      ("COMMON.TExitMultipler",
       bpo::value<int>(&_texitMultiplier)->default_value(1),
       "TExit Multiplier")

      ("COMMON.OpenStrategyAllowance",
       bpo::value<int>(&_openStrategyAllowance)->default_value(-1),
       "Open Strategy Allowance")

      ("COMMON.ShouldReattemptPrimary",
       bpo::value<bool>(&_shouldReattemptPrimary)->default_value(false),
       "Flag to reattempt primary order on cancelation")

      ("COMMON.DB",
       bpo::value<std::string>(&_dbName)->default_value("./ordersdb/orderdb"),
       "DB File Name")

      ("COMMON.CentralDbAddress",
        bpo::value<std::string>(&_centralDbAddress)->default_value("127.0.0.1"),
        "Central DB Address")

      ("COMMON.CentralDbPort",
        bpo::value<int>(&_centralDbPort)->default_value(3306),
        "Central DB Port")

      ("COMMON.CentralDbUser",
        bpo::value<std::string>(&_centralDbUser)->default_value("root"),
        "Central DB User")

      ("COMMON.CentralDbPassword",
        bpo::value<std::string>(&_centralDbPassword)->default_value("q1w2e3"),
        "Central DB Password")

      ("COMMON.CentralDbName",
        bpo::value<std::string>(&_centralDbName)->default_value("AlgoEngine"),
        "Central DB Name")

      ("COMMON.MaxAttempts",
       bpo::value<int>(&_maxAttempts)->default_value(3),
       "Max IOC Attempts")

      ("COMMON.TExitInterval",
       bpo::value<int>(&_tExitInterval)->default_value(10000),
       "T-Exit Interval")

      ("COMMON.LogRotateHours",
       bpo::value<int>(&_logRotateHours)->default_value(24),
       "Log Rotate Hours")

      ("COMMON.IntervalMillForPersist",
        bpo::value<int>(&_intervalMillForPersist)->default_value(10),
        "The interval for each persist")

      ("COMMON.PersistFileNum",
        bpo::value<int>(&_persistFileNum)->default_value(20),
        "Number of persisted file")

      ("COMMON.BackendInstanceFileName",
        bpo::value<std::string>(&_backendInstanceFileName)->default_value("backend.id"),
        "Backend instance identifier file")
      ;

      bpo::options_description configFileOptions;
      configFileOptions.add(config);
      freopen("./logs/stdout", "a", stdout);
      freopen("./logs/stderr", "a", stderr);

      bpo::variables_map vm;
      std::ifstream ifs(ServerConstant::ConfigFile.c_str());

      if (ifs)
      {
        bpo::store(bpo::parse_config_file(ifs, configFileOptions, true), vm);
        bpo::notify(vm);
      }
      else
      {
        LOG(ERROR) << "File open Error: " << ServerConstant::ConfigFile;
        return;
      }

      // Read IB Config
      bpo::options_description ibConfig("IB Configuration");
      ibConfig.add_options()

      ("COMMON.IbApiGatewayHost",
        bpo::value<std::string>(&_ibGatewayHost)->default_value("127.0.0.1"),
        "IB API Gateway host address")

      ("COMMON.IbApiGatewayPort",
        bpo::value<int>(&_ibGatewayPort)->default_value(7497),
        "IB API Gateway host address")
      ;

      bpo::options_description ibConfigFileOptions;
      ibConfigFileOptions.add(ibConfig);

      ifs.close();
      ifs.open(_ibConfigFile);

      if (ifs)
      {
        bpo::store(bpo::parse_config_file(ifs, ibConfigFileOptions, true), vm);
        bpo::notify(vm);
      }
      else
      {
        LOG(ERROR) << "File open Error: " << _ibConfigFile << std::endl;
        return;
      }

    }
    catch (boost::program_options::error &e)
    {
      std::cerr << "Bad Options: " << e.what() << std::endl;
      return;
    }
    catch (std::exception& e)
    {
      std::cerr << " Unknown Exception: " << e.what() << std::endl;
      return;
    }
    catch (...)
    {
      std::cerr << __FUNCTION__ << " : " << __LINE__ << "Unknown Exception: " << std::endl;
      return;
    }
    // load the time zone database which comes with boost, we can modify it if needed
    try {
      boost::local_time::tz_database tzDatabase;
      tzDatabase.load_from_file("./config/date_time_zonespec.csv");
      _tzNewYork = tzDatabase.time_zone_from_region("America/New_York");
      LOG(INFO) << "Using time zone database file at ./config/date_time_zonespec.csv";
    }
    catch (...)
    {
      std::string nyc("EST-5EDT,M3.2.0,M11.1.0");
      _tzNewYork = boost::local_time::time_zone_ptr(new boost::local_time::posix_time_zone(nyc));
      LOG(INFO) << "Cannot find time zone database file at ./config/date_time_zonespec.csv. Falling back to NYC timezone only";
    }
    std::thread logRotatorThread([this]() {
      const std::chrono::seconds wakeUpDelta = std::chrono::hours(this->getLogRotateHours());

      LOG(INFO) << "getLogRotateHours: " << std::to_string(this->getLogRotateHours());
      auto nextWakeUp = std::chrono::system_clock::now() + wakeUpDelta;

      while (true) {
        std::this_thread::sleep_until(nextWakeUp);
        nextWakeUp += wakeUpDelta;
        LOG(INFO) << "About to rotate log file!";
        auto L = el::Loggers::getLogger("default");
        if (L == nullptr)
        {
          LOG(ERROR) << "Oops, it is not called default!";
        }
        else
        {
          std::string filename = L->typedConfigurations()->filename(el::Level::Global);
          L->reconfigure();
          rolloutBackupHandler(filename.c_str());
          _curFileIndex = 0;
        }
      }

    });

    logRotatorThread.detach();
  }

  void Configuration::print()
  {
    LOG(INFO) << "============ Config Settings ========";
    LOG(INFO) << "Log Directory [" << _logDir << "]";
    LOG(INFO) << "FXCM Feed Config File [" << _configFile << "]";
    LOG(INFO) << "FXCM OP Config File [" << _configFileOP << "]";
    LOG(INFO) << "FXCM OP Start Order Id [" << _startClOrdrId << "]";
    LOG(INFO) << "FXCM OP Log Level      [" << _fxcmOPLogLevel << "]";
    LOG(INFO) << "FXCM Feed Log Level    [" << _fxcmFeedLogLevel << "]";
    LOG(INFO) << "Server Port Number     [" << _serverPort << "]";
    LOG(INFO) << "FXCM Account Number (Buy)   [" << _accountNumberBuy << "]";
    LOG(INFO) << "FXCM Account Number (Sell)   [" << _accountNumberSell << "]";
    LOG(INFO) << "DB Name   [" << _dbName << "]";
    LOG(INFO) << "Max Attempts   [" << _maxAttempts << "]";
    LOG(INFO) << "T-Exit interval   [" << _tExitInterval << "]";
    LOG(INFO) << "Log Rotate Hours   [" << _logRotateHours << "]";
    LOG(INFO) << "Backend Instance Identifier file name   [" << _backendInstanceFileName << "]";
    LOG(INFO) << "Data Feed Venue Id   [" << _dataFeedVenueId << "]";
    LOG(INFO) << "Order Venue Id   [" << _orderVenueId << "]";
    LOG(INFO) << "TExit Multiplier [" << _texitMultiplier << "]";
    LOG(INFO) << "OpenStrategyAllowance [" << _openStrategyAllowance << "]";
    LOG(INFO) << "================================";
  }
}
