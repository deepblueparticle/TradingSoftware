/*
 * =====================================================================================
 *
 *       Filename:  EventType.H
 *
 *    Description:  This holds all event types
 *
 *        Created:  05/01/2017 06:40:00 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef shared_EventType_H
#define shared_EventType_H

namespace Algo
{
enum EventType
{
  PointA=1,
  PointB,
  PointC,
  Ack,
  Filled,
  Active,
  Canceled,
  Rejected,
  RejectedSecondaryLimitModify,
  CancelPoint1,
  CancelPoint2,
  TExit,
  TExitAck,
  StrategyChangedPendingToRunning,
  StrategyTerminatedDueToStopTime,
  ErrorSendingIOC,
  CancelTickA,
  PointABeforeStart,
  MaxReattemptReached,
  ErrorSendingSecondaryLimit,
  BracketOrder,
  StrategyTerminatedFromGui,
  CancelTickB,
  KilledMaxOpen
};
} // namespace Algo

#endif
