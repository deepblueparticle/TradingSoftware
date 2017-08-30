/*
 * =====================================================================================
 *
 *       Filename:  SpinLock.C
 *
 *    Description:  
 *
 *        Created:  09/21/2016 06:48:29 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <util/SpinLock.H>

namespace Algo
{
  SpinLock::SpinLock()
  {
  }

  void SpinLock::lock() 
  {
    while (locked.test_and_set(std::memory_order_acquire)) { ; }
  }

  void SpinLock::unlock() 
  {
    locked.clear(std::memory_order_release);
  }
}
