/*
 * =====================================================================================
 *
 *       Filename:  SpinLockHelper.H
 *
 *    Description:  
 *
 *        Created:  09/22/2016 08:24:24 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef SpinLockHelper_H
#define SpinLockHelper_H

#include <util/SpinLock.H>

namespace Algo
{
  class SpinLockHelper
  {
    public:
      SpinLockHelper(SpinLock *spinLock);
      ~SpinLockHelper();

      SpinLockHelper() = delete;
      SpinLockHelper(const SpinLockHelper&) = delete;
      SpinLockHelper(SpinLockHelper&&) = delete;
      SpinLockHelper& operator=(const SpinLockHelper&) = delete;
      SpinLockHelper& operator=(SpinLockHelper&&) = delete;

    private:
      // Prevent heap allocation
      void *operator new(size_t);
      void *operator new[](size_t);
      void  operator delete(void *);
      void  operator delete[](void*);

      SpinLock *_spinLock;
  };
}

#endif
