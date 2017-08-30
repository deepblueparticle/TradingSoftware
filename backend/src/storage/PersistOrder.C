/*
 * =====================================================================================
 *
 *       Filename:  PersistOrder.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 12:20:03 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/order.h>
#include <shared/easylogging++.h>
#include <storage/OrderStore.H>
#include <storage/PersistOrder.H>

namespace Algo
{
  PersistOrder& PersistOrder::getInstance()
  {
    static PersistOrder persistOrder;
    return persistOrder;
  }

  bool PersistOrder::initDB(const std::string &dbpath) 
  {
    _dbName = dbpath;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, _dbName, &_db);
    return status.ok();
  }

  void PersistOrder::loadOrders() 
  {
    try {
      leveldb::Iterator *it = _db->NewIterator(leveldb::ReadOptions());
      LOG(INFO) << "Loading Orders from DB.";
      for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        std::string val = it->value().ToString();
        LOG(INFO) << "ClOrderId " << key << " Order " << val;
        Order *order = new Order();

        if (Order::fromString(val, *order))
        {
          OrderStatus orderStatus = order->getOrderStatus();
          if (orderStatus == OrderStatus_CONFIRMED || orderStatus == OrderStatus_PENDING ||
            orderStatus == OrderStatus_PARTIALLY_FILLED || orderStatus == OrderStatus_REPLACED)
          {
            OrderStore::getInstance().addOrderIntoClOrderIdToOrder(key, order);
            LOG(INFO) << "Order loaded: ClOrderId " << key;

          }
        }
        else
        {
          LOG(INFO) << "Order cannot be loaded: ClOrderId " << key;
        }
      }
    }
    catch (std::exception e)
    {
      LOG(INFO) << "Could not load Orders from DB. " << e.what();
    }
    catch (...) {
      LOG(INFO) << "Could not load Orders from DB.";
    }
  }

  leveldb::Iterator* PersistOrder::getDBIter() 
  {
    return _db->NewIterator(leveldb::ReadOptions());
  }

  bool PersistOrder::storeOrders(const std::string &clorderId, std::string order) 
  {
    return _db->Put(leveldb::WriteOptions(), clorderId, order).ok();
  }

  void PersistOrder::close() 
  {
    delete _db;
  }

  PersistOrder::PersistOrder()
  {
  }
}
