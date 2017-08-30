#ifndef NEWALGODIALOG_H
#define NEWALGODIALOG_H

#include <QDialog>
#include <QString>

#include "ui_logindialog.h"

namespace Ui {
class NewAlgoDialog;
}
namespace  Algo {

class NewAlgoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewAlgoDialog(QString, QWidget *parent = 0);
    ~NewAlgoDialog();

    QString instrumentName() const;
    void setInstrumentName(const QString &instrumentName);

    QString tickSize() const;
    void setTickSize(const QString &tickSize);

    QString mode() const;
    void setMode(const QString &mode);

    QString prevClose() const;
    void setPrevClose(const QString &prevClose);

    QString baseVal() const;
    void setBaseVal(const QString &baseVal);

    QString entryPrice() const;
    void setEntryPrice(const QString &entryPrice);

    QString pointA() const;
    void setPointA(const QString &pointA);

    QString pointB() const;
    void setPointB(const QString &pointB);

    QString cp1() const;
    void setCp1(const QString &cp1);

    QString cp2() const;
    void setCp2(const QString &cp2);

    QString profit() const;
    void setProfit(const QString &profit);

    QString loss() const;
    void setLoss(const QString &loss);

    QString tolerance() const;
    void setTolerance(const QString &tolerance);

    QString startTime() const;
    void setStartTime(const QString &startTime);

    QString endTime() const;
    void setEndTime(const QString &endTime);

    QString qty() const;
    void setQty(const QString &qty);

    QString maxqty() const;
    void setMaxqty(const QString &maxqty);

    long long cancelTicks() const;
    void setCancelTicks(long long cancelTicks);

    long long trailing() const;
    void setTrailing(long long trailing);

private slots:
    void on_pushButtonAdd_clicked();

    //void changeColor(const QString &str);
    void on_comboBox_currentIndexChanged(const QString &str);

    void on_pushButtonClearAll_clicked();

private:
    QString m_instrumentName;
    QString m_tickSize;
    QString m_mode;
    QString m_prevClose;
    QString m_baseVal;
    QString m_entryPrice;
    QString m_pointA;
    QString m_pointB;
    QString m_cp1;
    QString m_cp2;
    QString m_profit;
    QString m_loss;
    QString m_tolerance;
    QString m_startTime;
    QString m_endTime;
    QString m_qty;
    QString m_maxqty;
    long long m_cancelTicks;
    long long m_trailing;
    long long m_lot;
    Ui::NewAlgoDialog *ui;
};
}
#endif // NEWALGODIALOG_H
