/*
 * =====================================================================================
 *
 *       Filename:  AlgoEngine.C
 *
 *    Description:
 *
 *        Created:  09/30/2016 12:20:41 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <thread>
#include <config/Configuration.H>
#include <shared/easylogging++.h>
#include <shared/defines.h>
#include <constant/ServerGlobals.H>

#include <interactive_brokers/MarketDataHandler.H>
#include <nanex/MarketDataHandler.H>
#include <interactive_brokers/OrderHandler.H>
#include <interactive_brokers/ContractsFactory.H>

#include <storage/PersistOrder.H>
#include <storage/StrategyStore.H>
#include <fix/FixOPHandler.H>
#include <fix/FixMessageHandler.H>
#include <connection/ServerConnection.H>
#include <connection/ConnectionManager.H>
#include <symbol/InstrumentMap.H>
#include <symbol/InstrumentMap.H>
#include <connection/DatabaseConnection.H>
#include <constant/ServerGlobals.H>
#include <constant/DBOperation.H>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <boost/serialization/assume_abstract.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem.hpp>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <storage/RunningStrategyStore.H>
#include <storage/StoppedStrategyStore.H>
#include <algorithm>

#include <db/Instrument_odb.H>
#include <db/Backend_odb.H>

int runServer();
std::string getLastStoredStrategyFileName(const std::string& path_ = "./persist/");
void restoreFromArchive(const std::string& archiveName_, Algo::StrategyStore& strategyStoreHandler_);
void persistToArchive(const std::string& archiveName_, const Algo::StrategyStore& strategyStoreHandler_);
bool checkDBName(const std::string& levelDBName_);
bool initBackendInstance(Algo::DatabaseConnection &dbConnection_, std::string &error_);
bool loadInstruments(Algo::DatabaseConnection &dbConnection_, std::string &error_);
void registerHandlers();

template<class T>
int persistElementFromQueueAndPop(Algo::DatabaseConnection& db, T& queue)
{
  typename T::value_type value;
  if (queue.template pop(value))
  {
    Algo::DBOperation operation = value.first;

    switch(operation)
    {
      case Algo::DBOperation::Insert:
        db.persistObject(*(value.second));
        return 1;
      case Algo::DBOperation::Update:
        db.updateObject(*(value.second));
        return 1;
    }
  }
  return 0;
}

int main(int argc, char** argv)
{
  return runServer();
}

int runServer()
{
  Algo::Configuration* config = Algo::Configuration::getInstance();
  config->loadConfig();
  LOG(INFO) << "Log Level " << (int)config->getLogLevel();

  config->print();

  const std::string& levelDBName = config->getDBName();
  if (!checkDBName(levelDBName))
  {
    return EXIT_FAILURE;
  }

  if (Algo::PersistOrder::getInstance().initDB(levelDBName))
  {
    Algo::PersistOrder::getInstance().loadOrders();
    LOG(INFO) << "LevelDb is a success " << config->getDBName();
  }
  else
  {
    LOG(INFO) << "LevelDb is a failure " << config->getDBName();
    return EXIT_FAILURE;
  }

  Algo::DatabaseConnection db;
  bool dbConnected = db.startConnection(
    config->getCentralDbUser(),
    config->getCentralDbPassword(),
    config->getCentralDbName(),
    config->getCentralDbAddress(),
    config->getCentralDbPort()
  );
  if (!dbConnected) {
    return EXIT_FAILURE;
  }

  // initialize backend instance id
  std::string error = "";
  if(!initBackendInstance(db, error)) {
    LOG(ERROR) << "Failed to initialize backend instance. Error=[" << error << "] Existing...";
    return EXIT_FAILURE;
  }

  registerHandlers();

  // load symbols now from db
  if(!loadInstruments(db, error)) {
    LOG(ERROR) << "Failed to load instruments from db. Error=[" << error << "] Existing...";
    return EXIT_FAILURE;
  }

  Algo::StrategyStore strategyStoreHandler;
  std::string archiveName = getLastStoredStrategyFileName();
  try
  {
    if (!archiveName.empty() && boost::filesystem::exists(archiveName))
    {
      restoreFromArchive(archiveName, strategyStoreHandler);
      LOG(INFO) << "Number of running strategies after recovery: " << strategyStoreHandler.getNumberOfRunningStrategies();
    }
    else
    {
      LOG(INFO) << "Nothing to recover. Looks like it is fresh start of backend";
    }
    LOG(INFO) << "No of open positions after recovery are following.";
    for (std::map<std::string, int>::iterator iter = Algo::openStrategyAllowanceMap.begin();
            iter != Algo::openStrategyAllowanceMap.end(); ++iter)
    {
      LOG(INFO) << "[" << iter->first << "," << iter->second << "]";
    }
  }
  catch (std::exception)
  {
    //if the file we get from getLastStoredStrategyFileName has issues, it's probably a result of crashing, so we just jump to
    //next persist file, by renaming this one and rerunning getLastStoredStrategyFileName
    std::string oldArchievName = archiveName;
    boost::replace_all(archiveName, ".dat", ".bak");
    boost::filesystem::rename(oldArchievName, archiveName);
    archiveName = getLastStoredStrategyFileName();
    restoreFromArchive(archiveName, strategyStoreHandler);
  }
  strategyStoreHandler.setMaxAttempts(config->getMaxAttempts());
  std::thread strategyThread(&Algo::StrategyStore::working, std::ref(strategyStoreHandler));

  Algo::FixMessageHandler& fixMsgHandler = Algo::FixMessageHandler::getInstance();
  Algo::FixOPHandler& opHandler = Algo::FixOPHandler::getInstance();

  Algo::Nanex::MarketDataHandler& nanexMsgHandler = Algo::Nanex::MarketDataHandler::getInstance();

  Algo::Ib::MarketDataHandler& ibMsgHandler = Algo::Ib::MarketDataHandler::getInstance();
  Algo::Ib::OrderHandler& ibOrderHandler = Algo::Ib::OrderHandler::getInstance();

  if ("FXCM" == Algo::Db::Backend::getInstance().dataFeedVenueId()) {
    fixMsgHandler.start(config->getConfigFile(), config->getFXCMFeedLogLevel());
  } else if ("IB" == Algo::Db::Backend::getInstance().dataFeedVenueId()) {
    bool connected = ibMsgHandler.connect(config->getIbGatewayHost(),
                                          config->getIbGatewayPort());
    if (!connected) {
      return EXIT_FAILURE;
    }

    std::thread ibMarketDataThread([&]() {
      while(Algo::gIsRunning) { ibMsgHandler.processMessages(); }
    });
    ibMarketDataThread.detach();
  } else if ("NANEX" == Algo::Db::Backend::getInstance().dataFeedVenueId()) {
    std::thread nanexMarketDataThread([&]() {
      nanexMsgHandler.processMessages();
    });
    nanexMarketDataThread.detach();
  }

  if ("FXCM" == Algo::Db::Backend::getInstance().orderVenueId()) {
    opHandler.setBuyAccount(config->getAccountNumberBuy().c_str());
    opHandler.setSellAccount(config->getAccountNumberSell().c_str());
    opHandler.start(config->getConfigFileOP(), config->getFXCMOPLogLevel());
  } else if ("IB" == Algo::Db::Backend::getInstance().orderVenueId()) {
    bool connected = ibOrderHandler.connect(config->getIbGatewayHost(),
                                            config->getIbGatewayPort());
    if (!connected) {
      return EXIT_FAILURE;
    }

    std::thread ibOrderThread([&](){
      while(Algo::gIsRunning) { ibOrderHandler.processMessages(); }
    });
    ibOrderThread.detach();
  }

  Algo::ServerConnection serverConnection(config->getServerPort());
  std::thread serverThread(serverConnection);
  //db.persist(Algo::internalEventsQueue);

  auto now = std::time(nullptr);
  auto nowtm = *std::localtime(&now);
  std::ostringstream oss;
  oss << std::put_time(&nowtm, "%d_%m_%Y_%H_%M_%S");
  auto persistDateTimeStr = oss.str();
  const int & intervalMillForPersist = config->getIntervalMillForPersist();
  const int & persistFileNum = config->getPersistFileNum();
  LOG(INFO) << "The interval for each persist is " << intervalMillForPersist << " millseconds and number of persisted file is " << persistFileNum;

  std::thread serializeThread([&persistDateTimeStr, &intervalMillForPersist, &strategyStoreHandler, &persistFileNum]() {
    int index = 0;
    while (Algo::gIsRunning)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(intervalMillForPersist));
      if (strategyStoreHandler.getNumberOfRunningStrategies() != 0)
      {
        std::string outputFile = "./persist/strategy_" + persistDateTimeStr + "_" + std::to_string(index) + ".dat";
        index = ((index + 1) % persistFileNum);
        persistToArchive(outputFile, strategyStoreHandler);//should this do a copy?
      }
    }
  });
  serializeThread.detach();

  const unsigned elementsPerTransaction = 100;
  const unsigned commitInterval = 100; // milliseconds
  unsigned elementsCnt = 0;
  std::chrono::time_point<std::chrono::system_clock> lastCommitTime;
  lastCommitTime = std::chrono::system_clock::now();
  while (Algo::gIsRunning)
  {
    bool status;
    int elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now() - lastCommitTime)
                          .count();
    if (elementsPerTransaction == elementsCnt || elapsedTime > commitInterval) {
      status = db.commitTransaction();
      if (!status) continue;
      elementsCnt = 0;
      lastCommitTime = std::chrono::system_clock::now();
    }

    if (0 == elementsCnt)
    {
      status = db.startTransaction();
      if (!status) continue;
    }

    Algo::Elements e;
    while (Algo::gSpscStrategyServerMsgQueue.pop(e))
    {
      LOG(DEBUG) << "Sending to client " << e._clientName ;
      std::lock_guard<std::mutex> l(Algo::ConnectionManager::getInstance().getMutex());
      std::string client(e._clientName);
      if (Algo::ConnectionManager::getInstance().isConnected(client))
      {
        Algo::ConnectionManager::getInstance().getConnection(client)->write(e._elements, e._size);
      }
    }

    elementsCnt += persistElementFromQueueAndPop(db, Algo::gStrategiesQueue);
    elementsCnt += persistElementFromQueueAndPop(db, Algo::gOrdersQueue);
    elementsCnt +=
        persistElementFromQueueAndPop(db, Algo::gInternalEventsQueue);
    elementsCnt += persistElementFromQueueAndPop(db, Algo::gRejectEventsQueue);
    elementsCnt +=
        persistElementFromQueueAndPop(db, Algo::gIncomingMessageEventsQueue);

    if (0 == elementsCnt)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  LOG(INFO) << "Running strategies finally: " << strategyStoreHandler.getNumberOfRunningStrategies();
  LOG(INFO) << "Closing fix connections";
  //Stop all threads.
  if ("FXCM" == Algo::Db::Backend::getInstance().orderVenueId()) {
    fixMsgHandler.stop();
    opHandler.stop();
  } else if ("IB" == Algo::Db::Backend::getInstance().orderVenueId()) {}

  LOG(INFO) << "Waiting for strategy and server thread....";
  strategyThread.join();
  LOG(INFO) << "Waiting for server thread....";
  serverThread.join();
  LOG(INFO) << "Shuting down....";

  std::string outputFile = "./persist/strategy_" + persistDateTimeStr +"_close.dat";
  if (strategyStoreHandler.getNumberOfRunningStrategies() != 0)
  {
    persistToArchive(outputFile, strategyStoreHandler);//should this do a copy?
  }
  return EXIT_SUCCESS;
}

void restoreFromArchive(const std::string& archiveName_, Algo::StrategyStore& strategyStoreHandler_)
{
  std::ifstream ifs(archiveName_);
  boost::archive::binary_iarchive ia(ifs);
  long currentStrategyId;
  long firstStrategyId;
  ia >> strategyStoreHandler_;
  //ia >> Algo::RunningStrategyStore::getInstance();
  //ia >> Algo::StoppedStrategyStore::getInstance();

  ia >> currentStrategyId;
  ia >> firstStrategyId;
  initCurrentStrategyId(currentStrategyId);
  setFirstStrategyId(firstStrategyId);
  LOG(INFO) << "Restoring From Archive. CurrentStrategyId: " << getCurrentStrategyId() <<
    " FirstStrategyId: " << getFirstStrategyId();

  strategyStoreHandler_.recoverFromFile();
}
void persistToArchive(const std::string& archiveName_, const Algo::StrategyStore& strategyStoreHandler_)
{
  std::ofstream ofs(archiveName_, std::ios::out | std::ios::trunc);
  try
  {
    boost::archive::binary_oarchive oa(ofs);
    // Profiler p{ "serialize" };
    oa << strategyStoreHandler_;
    //oa << Algo::RunningStrategyStore::getInstance();
    //oa << Algo::StoppedStrategyStore::getInstance();
    long currentStrategyId = getCurrentStrategyId();
    long firstStrategyId = getFirstStrategyId();
    oa << currentStrategyId;
    oa << firstStrategyId;
    ofs.close();
  }
  catch (...)
  {
    try
    {
      ofs.close();
      namespace fs = boost::filesystem;
      fs::path dir(archiveName_); //remove stale file
      if (fs::exists(dir))
      {
        boost::filesystem::remove(dir);
      }
    }
    catch (...)
    {

    }

    LOG(DEBUG) << "persistToArchive failed!";
    return;
  }
}

std::string getLastStoredStrategyFileName(const std::string& path_)
{
  namespace fs = boost::filesystem;
  fs::path dir(path_);
  fs::directory_iterator endIter;

  typedef std::map<std::time_t, fs::path> ResultSet;
  ResultSet resultSet;
  if ((!fs::exists(dir)) || (!fs::is_directory(dir)))
  {
    LOG(INFO) << "Directory " << path_ << " does not exist creating it.";
    fs::create_directory(dir);
    return "";
  }
  std::string latestFileStr{ "" };
  using SizeWithFile = std::tuple<std::time_t, double, std::string>; // time, size, path
  std::vector<SizeWithFile> sizeWithFileNameVector;
  for (fs::directory_iterator dirIter(dir); dirIter != endIter; ++dirIter)
  {

    if (fs::is_regular_file(dirIter->status()))
    {
      const fs::path& currentPath = dirIter->path();
      std::string currentPathStr = currentPath.string();
      std::time_t fileTime = fs::last_write_time(currentPath);
      int fileSize = fs::file_size(currentPath);
      if (currentPathStr.find(".dat") != std::string::npos)
      {
        sizeWithFileNameVector.push_back(std::make_tuple(fileTime, fileSize, currentPathStr));
      }
    }
  }
  std::sort(sizeWithFileNameVector.begin(), sizeWithFileNameVector.end(), [](SizeWithFile a, SizeWithFile b) {
    return std::get<0>(a) < std::get<0>(b);
  });

  int sizeWithFileNameVectorSize = sizeWithFileNameVector.size();
  if (sizeWithFileNameVectorSize == 0)
  {
    return "";
  }
  const SizeWithFile& latestFile = sizeWithFileNameVector[sizeWithFileNameVectorSize - 1];
  latestFileStr = std::get<2>(latestFile);
  if (sizeWithFileNameVectorSize >= 2)
  {
    const SizeWithFile& secondLatestFile = sizeWithFileNameVector[sizeWithFileNameVectorSize - 2];

    //If the persisted file is not created on close, it must be caused by crash, and we are recovering. We do not use the latest in the case.
    if (latestFileStr.find("_close") == std::string::npos)
    {
      latestFileStr = std::get<2>(secondLatestFile);
    }
  }
  if (!latestFileStr.empty())
  {
    LOG(INFO) << "Latest persistested file is:  " << latestFileStr << ". Using it for restoring state.";
  }
  return latestFileStr;
}

bool checkDBName(const std::string& levelDBName_)
{
  if (levelDBName_.empty())
  {
    LOG(INFO) << "Cannot get LevelDb db name. Exiting";
    abort();
  }
  boost::filesystem::path dbPath(levelDBName_);
  boost::filesystem::path dbDir = dbPath.parent_path();
  if (!boost::filesystem::exists(dbDir))
  {
    LOG(INFO) << "LevelDb db dir: " << dbDir.string() << " does not exist. Creating one.";
    boost::system::error_code ec;
    if (!boost::filesystem::create_directory(dbDir, ec))
    {
      std::string reason("Cannot create LevelDb db dir: " + dbDir.string() + ". Exiting. Reason:\n");
      int error = ec.value();
      switch (error)
      {
      case boost::system::errc::success:
        reason += "error code is success, wired.";
        break;
      case boost::system::errc::permission_denied:
        reason += "permission denied to create dir here.";
        break;
      default:
        reason += "error code: " + std::to_string(error);
        break;
      }
      LOG(INFO) << reason;
      return false;
    }
  }
  return true;
}

bool initBackendInstance(Algo::DatabaseConnection &dbConnection_, std::string &error_)
{
  Algo::Configuration *configuration = Algo::Configuration::getInstance();
  std::string backendInstanceFileName = configuration->getBackendInstanceFileName();
  std::ifstream file(backendInstanceFileName, std::ios::binary);
  Algo::Db::Backend &backend = Algo::Db::Backend::getInstance();

  //check if file exists.
  if (file.good()) {
    try {
       unsigned long id;
       file >> id;
       std::shared_ptr<Algo::Db::Backend> backendObject = dbConnection_.loadObject<Algo::Db::Backend>(id);

       if (backendObject == nullptr) {
          error_ = "Failed to load backend object from db. Backend id=" + std::to_string(id);
          return false;
       }
       backend = *backendObject;
       LOG(INFO) << "Retrieved Backend id. Backend data=[" << backend.getValues() << "]";
       file.close();
       if ( (backend.buyAccount()  != configuration->getAccountNumberBuy())        ||
            (backend.sellAccount() != configuration->getAccountNumberSell())       ||
            (backend.dataFeedVenueId()     != configuration->getDataFeedVenueId()) || 
            (backend.orderVenueId()     != configuration->getOrderVenueId()) ) {
          error_ = "Data mismacth from configuration settings. Configuration has changed from last run. Please check.";
          return false;
       }
    }
    catch(std::exception &e) {
      file.close();
      LOG(ERROR) << "Failed to deserialize backend data. Error: " << e.what();
      error_ = "Could not deserialize backend data from " + backendInstanceFileName;
      return false;
    }
  }
  // file does not exist. Make an entry in db and save it in file
  else {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    backend.setUuid(boost::uuids::to_string(uuid));
    backend.setBuyAccount(configuration->getAccountNumberBuy());
    backend.setSellAccount(configuration->getAccountNumberSell());
    backend.setDataFeedVenueId(configuration->getDataFeedVenueId());
    backend.setOrderVenueId(configuration->getOrderVenueId());

    bool success = dbConnection_.persistInOneTransaction(backend);
    if (!success)
    {
       error_ = "Failed to save instance id in db. Backend instance data=[" + backend.getValues() + "]";
       return false;
    }

    LOG(INFO) << "Successfully saved backend data in db. Backend data=[" << backend.getValues() << "]";
    std::ofstream ofs(backendInstanceFileName, std::ios::binary);
    try {
       ofs << backend.id();
       LOG(INFO) << "Successfully serialized Backend id. Backend data=[" << backend.getValues() << "]";
       ofs.close();
    }
    catch(std::exception &e) {
      ofs.close();
      LOG(ERROR) << "Failed to serialize backend data. Error: " << e.what();
      error_ = "Could not serialize backend data from";
      return false;
    }
  }
  return true;
}

bool loadInstruments(Algo::DatabaseConnection &dbConnection_, std::string &error_)
{
  std::vector<Algo::Db::Instrument> result = dbConnection_.queryAll<Algo::Db::Instrument>();

  for (auto it = result.begin(); it != result.end(); ++it)
  {
     LOG(INFO) << "Instrument " << it->id() << " " << *it;
     Algo::InstrumentMap::getInstance().pushSymbol(it->id(), *it);
  }
  return true;
}

void registerHandlers()
{
  // Market data handlers
  Algo::MarketDataHandlerMap& mdhm = Algo::MarketDataHandlerMap::getInstance();
  mdhm.put("NANEX", &Algo::Nanex::MarketDataHandler::getInstance());
  mdhm.put("IB", &Algo::Ib::MarketDataHandler::getInstance());
  mdhm.put("FXCM", &Algo::FixMessageHandler::getInstance());

  // Order handlers
  Algo::OrderHandlerMap& ohm = Algo::OrderHandlerMap::getInstance();
  ohm.put("IB", &Algo::Ib::OrderHandler::getInstance());
  ohm.put("FXCM", &Algo::FixOPHandler::getInstance());
}
