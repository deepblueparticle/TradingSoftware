#include "newalgodialog.h"
#include "ui_newalgodialog.h"
#include <shared/client_utils.h>
#include <QDebug>
#include <QMessageBox>
#include "symboltickmap.h"

#include "algomodel1.h"

namespace  Algo {

NewAlgoDialog::NewAlgoDialog(QString str, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAlgoDialog)
{
    ui->setupUi(this);
    this->setStyleSheet("");
    //QLayout *myLayout = this->layout();
    //myLayout->setSizeConstraint(QLayout::SetFixedSize);
    ui->lineEditInstrumentName->setText(str);
    float tick;
    int precission;
    SymolTickMap::getInstance().get(str.toStdString(), tick, m_lot, precission);

    ui->lineEditTickSize->setText(QString::number(tick, 'f', precission));
    QPalette qp;
    qp.setColor(QPalette::Background, QColor(0, 255, 0));
    setPalette(qp);

    QRegExp positiveDouble("[+]?\\d*\\.?\\d+");
    QRegExpValidator *v = new QRegExpValidator(positiveDouble);

    QRegExp positivePercentage("^[0-9]+(\.[0-9]{2})?$");
    QRegExpValidator *percentV = new QRegExpValidator(positivePercentage);

    QRegExp time("^[0-2]?[0-9]:[0-5]?[0-9]:[0-5]?[0-9]$");
    QRegExpValidator *vt = new QRegExpValidator(time);

    ui->lineEditBaseVal->setValidator(v);
    ui->lineEdiLoss->setValidator(percentV);
    ui->lineEditCP1->setValidator(percentV);
    ui->lineEditCp2->setValidator(percentV);
    ui->lineEditEntryPrice->setValidator(v);
    ui->lineEditPointA->setValidator(percentV);
    ui->lineEditPointB->setValidator(percentV);
    ui->lineEditPrevClose->setValidator(v);
    ui->lineEditProfit->setValidator(percentV);
    ui->lineEditTolerance->setValidator(v);
    ui->lineEditEndTime->setValidator(vt);
    ui->lineEditStartTime->setValidator(vt);

    ui->lineEdiLoss->setText(QString("35"));
    ui->lineEditCP1->setText(QString("39"));
    ui->lineEditCp2->setText(QString("39"));
    ui->lineEditEntryPrice->setText(QString("1"));
    ui->lineEditPointA->setText(QString("25"));
    ui->lineEditPointB->setText(QString("6"));
    ui->lineEditProfit->setText(QString("95"));
    ui->lineEditTolerance->setText(QString("5"));
    ui->lineEditQty->setText(QString("1000"));
    ui->lineEditMaxQty->setText(QString("1000"));
    ui->lineEditEndTime->setText(QString("00:00:00"));
    ui->lineEditStartTime->setText(QString("00:00:00"));
    ui->lineEditCancelTicks->setText(QString("20"));
}

NewAlgoDialog::~NewAlgoDialog()
{
    delete ui;
}

QString NewAlgoDialog::instrumentName() const
{
    return m_instrumentName;
}

void NewAlgoDialog::setInstrumentName(const QString &instrumentName)
{
    m_instrumentName = instrumentName;
}
QString NewAlgoDialog::tickSize() const
{
    return m_tickSize;
}

void NewAlgoDialog::setTickSize(const QString &tickSize)
{
    m_tickSize = tickSize;
}



void NewAlgoDialog::on_pushButtonAdd_clicked()
{
    if(ui->lineEditTickSize->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Tick Size cannot be empty.") );

        return;
    }
    if(ui->lineEditPrevClose->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Prev Close cannot be empty.") );

        return;
    }
    if(ui->lineEditBaseVal->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Base Value cannot be empty.") );
        return;
    }
    if(ui->lineEditEntryPrice->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Entry Price cannot be empty.") );
        return;
    }
    if(ui->lineEditPointA->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Point A cannot be empty.") );
        return;
    }
    if(ui->lineEditPointB->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Point B cannot be empty.") );
        return;
    }
    if(ui->lineEditCP1->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("CP 1 cannot be empty.") );
        return;
    }
    if(ui->lineEditCp2->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("CP2 cannot be empty.") );
        return;
    }
    if(ui->lineEditProfit->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Profit cannot be empty.") );
        return;
    }
    if(ui->lineEdiLoss->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Loss cannot be empty.") );
        return;
    }
    if(ui->lineEditTolerance->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Tolerance cannot be empty.") );
        return;
    }
    if(ui->lineEditStartTime->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Start Time cannot be empty.") );
        return;
    }
    if(ui->lineEditEndTime->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("End Time cannot be empty.") );
        return;
    }

    if(ui->lineEditQty->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Qty cannot be empty.") );
        return;
    }

    if(ui->lineEditMaxQty->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Max Qty cannot be empty.") );
        return;
    }

    if(ui->lineEditCancelTicks->text().isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Field Empty"),
                    tr("Cancel Ticks cannot be empty.") );
        return;
    }

    m_instrumentName = ui->lineEditInstrumentName->text();
    m_tickSize = ui->lineEditTickSize->text();

    m_mode = ui->comboBox->currentText();
    m_prevClose = ui->lineEditPrevClose->text();
    m_baseVal = ui->lineEditBaseVal->text();
    m_entryPrice = ui->lineEditEntryPrice->text();

    m_pointA = ui->lineEditPointA->text();
    m_pointB = ui->lineEditPointB->text();
    m_cp1 = ui->lineEditCP1->text();
    m_cp2 = ui->lineEditCp2->text();

    m_profit = ui->lineEditProfit->text();
    m_loss = ui->lineEdiLoss->text();
    m_tolerance = ui->lineEditTolerance->text();
    m_startTime = ui->lineEditStartTime->text();
    m_endTime = ui->lineEditEndTime->text();

    m_qty = QString::number(ui->lineEditQty->text().toLongLong()*m_lot);
    m_maxqty = QString::number(ui->lineEditMaxQty->text().toLongLong()*m_lot);

    m_cancelTicks = ui->lineEditCancelTicks->text().toLongLong();
    qDebug() << "m_cancelTicks = [" << m_cancelTicks << "]";
    if(m_maxqty<m_qty)
    {
        QMessageBox::warning(
                    this,
                    tr("Wrong Qty"),
                    tr("Quantity cannot be greater than maxqty") );
        return;
    }


    if(!verifyTime(m_startTime))
    {
        QMessageBox::warning(
                    this,
                    tr("Wrong time format"),
                    tr("Please enetr time as HH:MM:SS between 00:00:00 and 24:00:00") );
        return;
    }

//    if(!verifyTime(m_endTime))
//    {
//        QMessageBox::warning(
//                    this,
//                    tr("Wrong time format"),
//                    tr("Please enetr time as HH:MM:SS between 00:00:00 and 24:00:00") );
//        return;
//    }

    this->accept();
}


QString NewAlgoDialog::endTime() const
{
    return m_endTime;
}

void NewAlgoDialog::setEndTime(const QString &endTime)
{
    m_endTime = endTime;
}

QString NewAlgoDialog::startTime() const
{
    return m_startTime;
}

void NewAlgoDialog::setStartTime(const QString &startTime)
{
    m_startTime = startTime;
}

QString NewAlgoDialog::tolerance() const
{
    return m_tolerance;
}

void NewAlgoDialog::setTolerance(const QString &tolerance)
{
    m_tolerance = tolerance;
}

QString NewAlgoDialog::loss() const
{
    return m_loss;
}

void NewAlgoDialog::setLoss(const QString &loss)
{
    m_loss = loss;
}

QString NewAlgoDialog::qty() const
{
    return m_qty;
}

void NewAlgoDialog::setQty(const QString &qty)
{
    m_qty = qty;
}
QString NewAlgoDialog::maxqty() const
{
    return m_maxqty;
}

void NewAlgoDialog::setMaxqty(const QString &qty)
{
    m_maxqty = qty;
}


QString NewAlgoDialog::profit() const
{
    return m_profit;
}

void NewAlgoDialog::setProfit(const QString &profit)
{
    m_profit = profit;
}

QString NewAlgoDialog::cp2() const
{
    return m_cp2;
}

void NewAlgoDialog::setCp2(const QString &cp2)
{
    m_cp2 = cp2;
}

QString NewAlgoDialog::cp1() const
{
    return m_cp1;
}

void NewAlgoDialog::setCp1(const QString &cp1)
{
    m_cp1 = cp1;
}

QString NewAlgoDialog::pointB() const
{
    return m_pointB;
}

void NewAlgoDialog::setPointB(const QString &pointB)
{
    m_pointB = pointB;
}

QString NewAlgoDialog::pointA() const
{
    return m_pointA;
}

void NewAlgoDialog::setPointA(const QString &pointA)
{
    m_pointA = pointA;
}

QString NewAlgoDialog::entryPrice() const
{
    return m_entryPrice;
}

void NewAlgoDialog::setEntryPrice(const QString &entryPrice)
{
    m_entryPrice = entryPrice;
}

QString NewAlgoDialog::baseVal() const
{
    return m_baseVal;
}

void NewAlgoDialog::setBaseVal(const QString &baseVal)
{
    m_baseVal = baseVal;
}

QString NewAlgoDialog::prevClose() const
{
    return m_prevClose;
}

void NewAlgoDialog::setPrevClose(const QString &prevClose)
{
    m_prevClose = prevClose;
}

QString NewAlgoDialog::mode() const
{
    return m_mode;
}

void NewAlgoDialog::setMode(const QString &mode)
{
    m_mode = mode;
}


void NewAlgoDialog::on_comboBox_currentIndexChanged(const QString &str)
{
    qDebug() << "Combobox [" << str << "]";
    if(str == "BUY")
    {
        qDebug() << "Combobox [BUY]";
        QPalette qp;
        qp.setColor(QPalette::Background, QColor(0, 255, 0));
        this->setPalette(qp);
    }
    else
    {
        qDebug() << "Combobox [SELL]";
        QPalette qp;
        qp.setColor(QPalette::Background, QColor(255, 0, 0));
        this->setPalette(qp);
    }
}

void NewAlgoDialog::on_pushButtonClearAll_clicked()
{
    ui->lineEditBaseVal->clear();
    ui->lineEdiLoss->clear();
    ui->lineEditCP1->clear();
    ui->lineEditCp2->clear();
    ui->lineEditEndTime->clear();
    ui->lineEditEntryPrice->clear();
    ui->lineEditPointA->clear();
    ui->lineEditPointB->clear();
    ui->lineEditPrevClose->clear();
    ui->lineEditProfit->clear();
    ui->lineEditStartTime->clear();
    ui->lineEditProfit->clear();
    ui->lineEditTolerance->clear();
    ui->lineEditCancelTicks->clear();
}
long long NewAlgoDialog::cancelTicks() const
{
    return m_cancelTicks;
}

void NewAlgoDialog::setCancelTicks(long long cancelTicks)
{
    m_cancelTicks = cancelTicks;
}

long long NewAlgoDialog::trailing() const
{
    return m_trailing;
}

void NewAlgoDialog::setTrailing(long long trailing)
{
    m_trailing = trailing;
}

}
