/*
 * =====================================================================================
 *
 *       Filename:  UniqueIdGenerator.H
 *
 *    Description:  
 *
 *        Created:  09/18/2016 11:57:20 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef UniqueIdGenerator_H
#define UniqueIdGenerator_H

#include <thread>
#include <mutex>
#include <ctime>

namespace Algo
{
  class UniqueIdGenerator
  {
    public:
      static UniqueIdGenerator& getInstance();
      long generateUniqueOrderId();

    private:
      UniqueIdGenerator();
      std::mutex uniqueOrderIdMutex;
  };
}
#endif
