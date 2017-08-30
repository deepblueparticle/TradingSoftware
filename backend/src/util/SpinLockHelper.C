/*
 * =====================================================================================
 *
 *       Filename:  SpinLockHelper.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 08:27:45 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <util/SpinLockHelper.H>

namespace Algo
{
  SpinLockHelper::SpinLockHelper(SpinLock *spinLock) : _spinLock(spinLock)
  {
    _spinLock->lock();
  }

  SpinLockHelper::~SpinLockHelper()
  {
    _spinLock->unlock();
  }
}
