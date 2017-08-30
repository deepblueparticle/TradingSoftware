/*
 * =====================================================================================
 *
 *       Filename:  PersistOrder.H
 *
 *    Description:  
 *
 *        Created:  09/18/2016 04:19:20 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef PersistOrder_H
#define PersistOrder_H

#include <leveldb/db.h>

namespace Algo
{
  class PersistOrder {
    public:
      static PersistOrder &getInstance();
      bool initDB(const std::string &dbpath);
      void loadOrders();
      leveldb::Iterator *getDBIter();
      bool storeOrders(const std::string &clorderId, std::string order);
      void close();

    private:
      PersistOrder();
      PersistOrder(const PersistOrder &) = delete;
      PersistOrder &operator=(const PersistOrder &) = delete;

      std::string _dbName;
      leveldb::DB *_db;
  };
}

#endif
