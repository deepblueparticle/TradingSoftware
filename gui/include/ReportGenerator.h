#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QThread>
#include <QString>
#include <string>
#include <map>

namespace Algo
{
class ReportGenerator : public QThread
{
public:
    ReportGenerator(const QString &date);
    Q_OBJECT
    void run();

private:
    void createClosedStrategyReport(const QString &applicationPath);
    void createOpenPositionReport(const QString &applicationPath);
    void createFilledOrderReport(const QString &applicationPath);
    void createEventReport(const QString &applicationPath);

    std::string checkFilledOrderStatus(std::map<QString, double> &quantityMap,
                                       QString &clientOrderId, double &sentQty, double &recvQty);
    double getSlippage(std::string &orderType, int &side, double &pointCPrice, double &orderPrice,
                                        double &fillPrice, double &tickSize);
    double applyTickSize(double price, double ticksize);

    QString _startDate;

signals:
    void resultReady(const QString &message);
};
}
#endif // REPORTGENERATOR_H
