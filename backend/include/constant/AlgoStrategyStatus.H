/*
 * =====================================================================================
 *
 *       Filename:  AlgoStrategyStatus.H
 *
 *    Description:  This holds all backend service constants
 *
 *        Created:  05/01/2017 06:40:00 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef AlgoStrategyStatus_H
#define AlgoStrategyStatus_H

namespace Algo
{
  enum AlgoStrategyStatus
  {
     PENDING=1,
     WAITING,
     OPEN,
     STOPPED,
     SARTED,
     RUNNING,
     UNKNOWN
  };
}

#endif
