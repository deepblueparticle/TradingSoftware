/*
 * =====================================================================================
 *
 *       Filename:  DatabaseConnection.H
 *
 *    Description:
 *
 *        Created:  01/02/2017 01:35:59 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef DatabaseConnection_H
#define DatabaseConnection_H

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <shared/easylogging++.h>
#include <boost/lockfree/spsc_queue.hpp>
#include <iostream>

namespace Algo
{
  class DatabaseConnection
  {
  public:
    bool startConnection(const std::string& user,
                         const std::string& passwd,
                         const std::string& db,
                         const std::string& host = "",
                         unsigned int port = 0);

    bool startTransaction();
    bool commitTransaction();

    template<typename Object>
    bool persistInOneTransaction(Object& object_)
    {
      return startTransaction() && persistObject(object_) &&
             commitTransaction();
    }

    template<typename Object>
    bool persistObject(Object& object_)
    {
      assert(!_transaction.finalized());

      try
      {
        _db->persist(object_);
      }
      catch(const odb::exception& e)
      {
         LOG(ERROR) << "DB error: " << e.what ();
         return false;
      }
      return true;
    }

    template<typename Object>
    bool updateObject(Object& object_)
    {
      assert(!_transaction.finalized());

      try
      {
        _db->update(object_);
      }
      catch(const odb::exception& e)
      {
         LOG(ERROR) << "DB error: " << e.what ();
         return false;
      }
      return true;
    }

    template<typename Queue>
    void persistQueue(Queue& object_)
    {
      try
      {
        odb::core::transaction t(_db->begin());
        typename Queue::value_type value;
        while(object_.template pop(value))
        {
           _db->persist(value);
           delete value;
        }
        t.commit();
      }
      catch(const odb::exception& e)
      {
         LOG(ERROR) << "DB error: " << e.what ();
      }
    }

    template<typename Output>
    std::shared_ptr<Output> loadObject(unsigned long id_)
    {
      try
      {
        odb::core::transaction t(_db->begin());
        std::shared_ptr<Output> output(_db->load<Output>(id_));
        t.commit();
        return output;
      }
      catch(const odb::exception& e)
      {
         LOG(ERROR) << "DB error: " << e.what ();;
      }
      return nullptr;
    }

    template<typename Output>
    std::vector<Output> queryAll()
    {
      try
      {
        odb::core::transaction t(_db->begin());
        odb::result<Output> data = _db->query<Output>();
        std::vector<Output> vec;
        for (typename odb::result<Output>::iterator i (data.begin ()); i != data.end (); ++i)
        {
          vec.push_back(*i);
        }
        t.commit();
        return vec;
      }
      catch(const odb::exception& e)
      {
         LOG(ERROR) << "DB error: " << e.what ();
      }
    }

  private:
    odb::core::database      *_db;
    odb::core::transaction    _transaction;
  };
}
#endif
