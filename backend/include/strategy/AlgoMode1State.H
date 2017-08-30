/*
 * =====================================================================================
 *
 *       Filename:  AlgoMode1State.H
 *
 *    Description:  
 *
 *        Created:  09/22/2016 07:02:53 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef AlgoMode1State_H
#define AlgoMode1State_H

namespace Algo
{
  enum AlgoMode1State 
  {
    AlgoMode1State_BEGIN,
    AlgoMode1State_POINT_A,
    AlgoMode1State_POINT_B,
    AlgoMode1State_REATTEMPT,
    AlgoMode1State_STOPPED,
    AlgoMode1State_ORDER_PENDING,
    AlgoMode1State_REATTEMPT_SECONDARY
  };
}

#endif
