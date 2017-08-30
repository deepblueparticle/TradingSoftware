/*
 * =====================================================================================
 *
 *       Filename:  UniqueIdGenerator.C
 *
 *    Description:  
 *
 *        Created:  09/20/2016 11:49:15 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <util/UniqueIdGenerator.H>
#include <constant/ServerConstant.H>

namespace Algo
{
  UniqueIdGenerator& UniqueIdGenerator::getInstance()
  {
    static UniqueIdGenerator uniqueIdGenerator;
    return uniqueIdGenerator;
  }

  long UniqueIdGenerator::generateUniqueOrderId()
  {
    // PASSED param is ignored, but kept there so not to change signature
    static int orderIdCounter = 1;
    long orderId = 0;
    {
      std::lock_guard<std::mutex> lock(uniqueOrderIdMutex);
      orderId = std::time(nullptr) % ServerConstant::digitsForSeconds + orderIdCounter * ServerConstant::digitsForSeconds;
      orderIdCounter++;
      if (orderIdCounter == ServerConstant::maxOrdersPerSecond) {
        orderIdCounter = 0;
      }
    }
    return orderId;
  }

  UniqueIdGenerator::UniqueIdGenerator()
  {
  }
}
