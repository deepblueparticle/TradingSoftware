/*
 * =====================================================================================
 *
 *       Filename:  SpinLock.H
 *
 *    Description:  
 *
 *        Created:  09/18/2016 03:18:01 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef SpinLock_H
#define SpinLock_H

#include <thread>
#include <atomic>

namespace Algo
{
  class SpinLock
  {
    public:
      SpinLock();
      void lock();
      void unlock();

    private:
      std::atomic_flag locked = ATOMIC_FLAG_INIT;
  };
}

#endif
