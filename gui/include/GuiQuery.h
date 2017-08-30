#ifndef GUIQUERY_H
#define GUIQUERY_H

#include <QString>
#include <vector>

namespace Algo
{
  QString getStartupQuery();
  QString getStrategyThirdRowUpdateQuery(const QString &lastGuiUpdateTime);
  QString getStrategyStatusQuery(const std::vector<int> &strategyList);
  QString getAlertWindowUpdateQuery(const QString &lastGuiUpdateTime, const QString &lastId);
  QString getAllFilledOrdersFromAGivenDayQuery(const QString &date);
  QString getOpenPositionsFromAGivenDayQuery(const QString &date);
  QString getAllClosedStrategiesFromAGivenDayQuery(const QString &date);
  QString getAllFilledOrdersForAGivenStrategy(const QString &strategyId);
  QString getTimeStampClause(const QString &lastGuiUpdateTime, const QString &prefix);
  QString getTimeStampClause(const QString &lastGuiUpdateTime, const QString &prefix, const QString &lastId);
  QString getStrategyString(const std::vector<int> &strategyList);
}
#endif // GUIQUERY_H
