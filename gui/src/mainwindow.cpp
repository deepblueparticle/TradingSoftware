#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GuiQuery.h"

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QModelIndex>
#include <QFileDialog>
#include <QDateEdit>
#include <QTimeZone>
#include <QByteArray>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QStandardItemModel>
#include <QCalendarWidget>
#include <algorithm>


#include "tcpclient.h"
#include <shared/commands.h>
#include <shared/EventType.H>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/map.hpp>
#include <stdlib.h>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "symboltickmap.h"
#include <shared/order.h>
#include <shared/strategiesreader.h>
#include <shared/pnl.h>


using namespace std::placeholders;

namespace  Algo {

const QString connected("<P><b><i><FONT color='green' FONT SIZE = 4>CONNECTED</i></b></P></br>");
const QString connectedb("<P><b><i><FONT color='white' FONT SIZE = 4>CONNECTED</i></b></P></br>");
const QString disConnected("<P><b><i><FONT color='red' FONT SIZE = 4>DISCONNECTED</i></b></P></br>");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_lastSelectedTreeItem(""),
    m_lastStartegyId(0),
    _pendingDataSize(0),
    m_connectionstate(true),
    ui(new Ui::MainWindow),
    m_isSyncDone(false),
    _showUpdateStatus(false)
{
    memset(_buffer, 0, MAXDATABUFFER);
    memset(_pendingBuffer, 0, MAXDATABUFFER);
    ui->setupUi(this);
    //ui->treeWidgetAlgo->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableWidgetMainWindow->horizontalHeader()->setSectionsMovable(true);
    ui->tableWidgetMainWindow->verticalHeader()->setSectionsMovable(true);

    //ui->tableWidgetMainWindow->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetMainWindow->resizeColumnsToContents();
    ui->feedConnStatus->setText(disConnected);
    ui->orderConnStatus->setText(disConnected);
    ui->backendConnStatus->setText(connected);

    connect( &(TcpClient::getInstance()),
             SIGNAL(dataReceived(std::string)),
             this,
             SLOT(process_data(std::string)) );

    connect(&(TcpClient::getInstance()),
            SIGNAL(socketDiconnected()),
            this,
            SLOT(update_server_disconnected()));

    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load_algo_model1()));
    connect(ui->actionGenerate_Report, SIGNAL(triggered()), this, SLOT(generateReport()));
    if (Settings::isReadOnly()) {
        ui->actionLoad->setVisible(false);
    }

    QMainWindow::showFullScreen();
    ui->tableWidgetMainWindow->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->tableWidgetMainWindow->resizeColumnsToContents();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout_update()));
    m_timer->start(500);

    qDebug() << "GUI start-time: " << QTime().currentTime();
    //Get all Running|Waiting|Open strategies on startup
    qRegisterMetaType<QSqlQuery>("sqlquery");
    _guiInitThread = QSharedPointer<GUIWorkerThread>(new GUIWorkerThread(getStartupQuery()));
    connect(_guiInitThread.data(), SIGNAL(resultReady(QSqlQuery)), this, SLOT(loadFromDb(QSqlQuery)));
    _guiInitThread->start();

    //Timer for strategy window update
    _strategy_window_update_timer = new QTimer(this);
    connect(_strategy_window_update_timer, SIGNAL(timeout()), this, SLOT(strategy_window_update()));
    _lastStrategyWindowUpdateTime = "";

    //Timer for alert window update
    _alert_window_update_timer = new QTimer(this);
    connect(_alert_window_update_timer, SIGNAL(timeout()), this, SLOT(alert_window_update()));
    _lastAlertWindowUpdateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    _lastId = "";

    connect(ui->tableWidgetMainWindow, SIGNAL(cellClicked(int,int)), this, SLOT(strategyRowClicked(int,int)));

    //Show calendar to pick up date while generating reports
    _calendar = QSharedPointer<QCalendarWidget>(new QCalendarWidget);
    _calendar->setWindowTitle("Select a date");
    connect(_calendar.data(),SIGNAL(clicked(const QDate)),this,SLOT(slotClicked(const QDate)));
}

void MainWindow::slotClicked(const QDate& date)
{
    _calendar->hide();
    QMessageBox::information(NULL,"Date Selected: ",date.toString("yyyy-MM-dd"));
    _reportGeneratorThread = QSharedPointer<ReportGenerator>(
                new ReportGenerator(date.toString("yyyy-MM-dd")));
    connect(_reportGeneratorThread.data(), SIGNAL(resultReady(const QString&)), this, SLOT(reportDone(const QString&)));
    _reportGeneratorThread->start();
}

void MainWindow::reportDone(const QString &message)
{
    QMessageBox::information(NULL, "Attention", message);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFromDb(const QSqlQuery &query)
{
    insert_row_from_db(query);
    qDebug() << "GUI start end-time: " << QTime().currentTime();
    start_gui_update_timer(_strategy_window_update_timer);
    start_gui_update_timer(_alert_window_update_timer);

    m_heartbeatthread = new HeartbeatThread;
    connect(m_heartbeatthread,
            SIGNAL(readyHeartbeatMessage()),
            &(TcpClient::getInstance()),
            SLOT(readreadyHeartbeatMessage()));
    m_heartbeatthread->start();
}

void MainWindow::start_gui_update_timer(QTimer *timer)
{
    timer->start(500);
}

void MainWindow::stop_gui_update_timer(QTimer *timer)
{
    timer->stop();
}

void MainWindow::strategy_window_update()
{
    stop_gui_update_timer(_strategy_window_update_timer);
    if (_strategyList.size()) {
        std::vector<int> keys;
        for(std::map<long, std::pair<long, StrategyStatus>>::iterator iter = _strategyList.begin();
            iter != _strategyList.end();++iter)
        {
            if (iter->second.second != StrategyStatus_STOPPED) {
                keys.push_back(iter->first);
            }
        }
        if (_showUpdateStatus) {
            _strategyWindowThread = QSharedPointer<GUIWorkerThread>(
                        new GUIWorkerThread(
                            getStrategyStatusQuery(keys)));
            connect(_strategyWindowThread.data(), SIGNAL(resultReady(QSqlQuery)), this, SLOT(updateStrategyStatus(QSqlQuery)));
        }
        else {
            _strategyWindowThread = QSharedPointer<GUIWorkerThread>(
                        new GUIWorkerThread(
                            getStrategyThirdRowUpdateQuery(_lastStrategyWindowUpdateTime)));
            connect(_strategyWindowThread.data(), SIGNAL(resultReady(QSqlQuery)), this, SLOT(updateStrategyThirdRow(QSqlQuery)));
        }
        _strategyWindowThread->start();
    }
    else {
        start_gui_update_timer(_strategy_window_update_timer);
    }
}

void MainWindow::alert_window_update()
{
    stop_gui_update_timer(_alert_window_update_timer);
    if (_strategyList.size()) {
        _alertWindowThread = QSharedPointer<GUIWorkerThread>(
                    new GUIWorkerThread(
                        getAlertWindowUpdateQuery(_lastAlertWindowUpdateTime, _lastId)));
        connect(_alertWindowThread.data(), SIGNAL(resultReady(QSqlQuery)), this, SLOT(updateAlertWindow(QSqlQuery)));
        _alertWindowThread->start();
    }
    else {
        start_gui_update_timer(_alert_window_update_timer);
    }
}

void MainWindow::updateAlertWindow(QSqlQuery query)
{
    int lastId = -1;
    while (query.next()) {
        /*qDebug() << "Status " << query.value(0).toInt() <<
                    "Strategy Id " << query.value(1).toLongLong() <<
                    "Upload Id " << query.value(2).toLongLong() <<
                    "Upload Name " << query.value(3).toString() <<
                    "Time " << query.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") <<
                    "Message " << query.value(5).toString();*/
        //this will give the present number of rows available.
        size_t insertRow = ui->tableWidgetAlert->rowCount();

        //insert the row at the bottom of the table widget - using.
        ui->tableWidgetAlert->insertRow(insertRow);

        //After a new row is inserted we can add the table widget items as required.
        QTableWidgetItem *strategyitem = new QTableWidgetItem(QString::number(query.value(3).toLongLong()));
        ui->tableWidgetAlert->setItem(insertRow,0, strategyitem);

        QTableWidgetItem *internalIdItem = new QTableWidgetItem(query.value(4).toString());
        ui->tableWidgetAlert->setItem(insertRow,1, internalIdItem);

        QTableWidgetItem *timeStamp = new QTableWidgetItem(query.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
        ui->tableWidgetAlert->setItem(insertRow,2, timeStamp);

        QTableWidgetItem *reason = new QTableWidgetItem(query.value(6).toString());
        ui->tableWidgetAlert->setItem(insertRow,3, reason);

        long strategyId = query.value(1).toLongLong();
        StrategyStatus status = (StrategyStatus)query.value(1).toInt();

        _lastAlertWindowUpdateTime = query.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        lastId = std::max(lastId, query.value(0).toInt());
    }
    if (lastId != -1)
        _lastId = QString::number(lastId);
    start_gui_update_timer(_alert_window_update_timer);
}

void MainWindow::updateStrategyThirdRow(QSqlQuery query)
{
    while (query.next()) {
        int strategyId = query.value(0).toInt();
        int eventType = query.value(2).toInt();
        QString eventTimestamp = query.value(3).toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString orderType = query.value(4).toString();
        if (_strategyList.find(strategyId) != _strategyList.end())
        {
            int strategyRow = _strategyList[strategyId].first;

            switch(eventType) {
            case EventType::PointC:
                ui->tableWidgetMainWindow->item(strategyRow + 2,10)->setText(eventTimestamp);
                break;
            case EventType::PointA:
                ui->tableWidgetMainWindow->item(strategyRow + 2,11)->setText(eventTimestamp);
                break;
            case EventType::PointB:
                ui->tableWidgetMainWindow->item(strategyRow + 2,12)->setText(eventTimestamp);
                break;
            case EventType::CancelPoint1:
                ui->tableWidgetMainWindow->item(strategyRow + 2,13)->setText(eventTimestamp);
                break;
            case EventType::CancelPoint2:
                ui->tableWidgetMainWindow->item(strategyRow + 2,14)->setText(eventTimestamp);
                break;
            case EventType::Filled:
                if (orderType == "Secondary_Limit")
                    ui->tableWidgetMainWindow->item(strategyRow + 2,16)->setText(eventTimestamp);
                else if (orderType == "Secondary_Stop")
                    ui->tableWidgetMainWindow->item(strategyRow + 2,17)->setText(eventTimestamp);
                else if (orderType == "Primary")
                    ui->tableWidgetMainWindow->item(strategyRow + 2,15)->setText(eventTimestamp);
                break;
            }
        }
        _lastStrategyWindowUpdateTime = eventTimestamp; // Sql rows will always be sorted.
    }
    _showUpdateStatus = true;
    start_gui_update_timer(_strategy_window_update_timer);
}

void MainWindow::updateStrategyStatus(QSqlQuery query)
{
    while (query.next()) {
        long strategyId = query.value(0).toInt();
        StrategyStatus status = (StrategyStatus)query.value(1).toInt();
        std::map<long, std::pair<long, StrategyStatus>>::iterator iter = _strategyList.find(strategyId);

        if (iter != _strategyList.end() && iter->second.second != status) {
            setStrategyText(iter->second.first, status, false);
            iter->second.second = status;
            /*if (status == StrategyStatus_STOPPED) {
                int strategyRow = _strategyList[strategyId].first;
                _strategyList.erase(iter);
            }*/
        }
    }
    start_gui_update_timer(_strategy_window_update_timer);
    _showUpdateStatus = false;
}

void MainWindow::timeout_update()
{
        if(ui->orderConnStatus->text()==connected)
            ui->orderConnStatus->setText(connectedb);

        else if(ui->orderConnStatus->text()==connectedb)
            ui->orderConnStatus->setText(connected);
        else
            ui->orderConnStatus->setText(disConnected);

        if(ui->feedConnStatus->text()==connected)
            ui->feedConnStatus->setText(connectedb);

        else if(ui->feedConnStatus->text()==connectedb)
            ui->feedConnStatus->setText(connected);
        else
            ui->feedConnStatus->setText(disConnected);

        if(ui->backendConnStatus->text()==connected)
            ui->backendConnStatus->setText(connectedb);

        else if(ui->backendConnStatus->text()==connectedb)
            ui->backendConnStatus->setText(connected);
        else
            ui->backendConnStatus->setText(disConnected);
}

//void MainWindow::updateConnectionFrame(QFrame *frame, bool status)
void MainWindow::update_connection_frame(QLabel *label, bool status)

{
    if(status)
    {
        label->setText(connected);
    }
    else
    {
        label->setText(disConnected);
    }
}

void MainWindow::update_row_color(int row, bool status)
{
    QColor changeColor;
    if(status)
       changeColor = QColor(255,255,102);
    else
       changeColor = QColor(255,255,255);

    for (int i=0; i<ui->tableWidgetMainWindow->columnCount(); ++i)
    {
        ui->tableWidgetMainWindow->item(row, i)->setBackground(changeColor);
    }
}

void MainWindow::insert_row_from_db(QSqlQuery query)
{
    long maxInternalId = -1;
    while (query.next()) {
        QSqlRecord name = query.record();
        std::shared_ptr<algomodel1> am(new algomodel1());

        //First row data
        am->m_upname = query.value(name.indexOf("UploadName")).toString().toStdString();
        am->m_algoType =(AlgoType) query.value(name.indexOf("AlgoType")).toInt();
        am->m_strategyId = query.value(name.indexOf("StrategyId")).toLongLong();
        am->dataSymbol = query.value(name.indexOf("Instrument")).toString().toStdString();
        am->m_trailingPercentage = query.value(name.indexOf("TrailingPercentage")).toDouble();
        am->m_status = (StrategyStatus)query.value(name.indexOf("StrategyStatus")).toInt();
        am->m_identifier = query.value(name.indexOf("UploadId")).toString().toStdString();
        am->m_prevClose = query.value(name.indexOf("PreviousClose")).toDouble();
        am->m_baseVal = query.value(name.indexOf("BaseValue")).toDouble();
        am->m_entryPrice = query.value(name.indexOf("PointCPercentage")).toDouble();
        am->m_pointAPercentage = query.value(name.indexOf("PointAPercentage")).toDouble();
        am->m_pointBPercentage = query.value(name.indexOf("PointBPercentage")).toDouble();
        am->m_cp1Percentage = query.value(name.indexOf("CP1Percentage")).toDouble();
        am->m_cp2Percentage = query.value(name.indexOf("CP2Percentage")).toDouble();
        am->m_parentOrderPrice = query.value(name.indexOf("ParentOrderPrice")).toDouble();
        am->m_lossPercentage = query.value(name.indexOf("LossPercentage")).toDouble();
        am->m_profitPercentage = query.value(name.indexOf("ProfitPercentage")).toDouble();
        am->m_mode = (query.value(name.indexOf("Side")).toString() == "BUY"?OrderMode_BUY:OrderMode_SELL);
        am->m_qty = query.value(name.indexOf("Quantity")).toLongLong();
        am->m_tolerance = query.value(name.indexOf("Tolerance")).toDouble();
        am->m_startDate = query.value(name.indexOf("StartDate")).toString().toStdString();
        am->m_startTime = query.value(name.indexOf("StartTime")).toString().toStdString();
        am->m_endDate = query.value(name.indexOf("EndDate")).toString().toStdString();
        am->m_endTime = query.value(name.indexOf("EndTime")).toString().toStdString();
        am->m_cancelTicks = query.value(name.indexOf("CancelTicks")).toLongLong();
        am->m_internalId = query.value(name.indexOf("InternalId")).toLongLong();
        am->m_tickSize = query.value(name.indexOf("TickSize")).toDouble();
        am->m_secondaryOrderExitDate = query.value(name.indexOf("TExitDate")).toString().toStdString();
        am->m_secondaryOrderExitTime = query.value(name.indexOf("TExitTime")).toString().toStdString();

        //second row data
        am->m_trailing = query.value(name.indexOf("Trailing")).toDouble();
        am->m_entryPriceCalculated = query.value(name.indexOf("PointCPrice")).toDouble();
        am->m_pointACalculated = query.value(name.indexOf("PointAPrice")).toDouble();
        am->m_pointBCalculated = query.value(name.indexOf("PointBPrice")).toDouble();
        am->m_cp1Calculated = query.value(name.indexOf("CP1Price")).toDouble();
        am->m_cp2Calculated = query.value(name.indexOf("CP2Price")).toDouble();
        am->m_lossCalculated = query.value(name.indexOf("Loss")).toDouble();
        am->m_profitCalculated = query.value(name.indexOf("Profit")).toDouble();
        am->m_lossLimitPrice = query.value(name.indexOf("LossLimitPrice")).toDouble();

        _strategyList[am->m_strategyId] = std::make_pair(insert_row(am), am->m_status);
        maxInternalId = std::max(maxInternalId, am->m_internalId);
    }
    UpdateId(maxInternalId);
    ui->tableWidgetMainWindow->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetMainWindow->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->tableWidgetMainWindow->resizeColumnsToContents();
}

void MainWindow::setStrategyText(long rowId, Algo::StrategyStatus status, bool createNew)
{
    QString buttonText;
    QColor buttonColor;

    if(status == StrategyStatus_PENDING)
    {
        buttonText= QString("Pending");
        buttonColor=QColor::fromRgb(20, 252, 104);
    }
    else if(status == StrategyStatus_WAITING)
    {
        buttonText= QString("Waiting");
        buttonColor=QColor::fromRgb(200, 200, 0);
    }
    else if(status == StrategyStatus_RUNNING)
    {
        buttonText= QString("Running");
        buttonColor=QColor::fromRgb(0, 191, 255);
    }
    else if(status == StrategyStatus_SARTED)
    {
        buttonText = QString("Started");
        buttonColor=QColor::fromRgb(0, 191, 255);
    }
    else if(status == StrategyStatus_STOPPED)
    {
        buttonText=QString("Stopped");
        buttonColor=QColor::fromRgb(255, 0, 0);
    }
    else if(status == StrategyStatus_START)
    {
        buttonText=QString("Start");
        buttonColor=QColor::fromRgb(0, 76, 153);
    }
    else if(status == StrategyStatus_OPEN)
    {
        buttonText=QString("Open");
        buttonColor=QColor::fromRgb(0, 255, 100);
    }
    else
    {
        buttonText = QString("Unknown");
        buttonColor=QColor::fromRgb(100, 100, 200);
    }

    QTableWidgetItem *statusButton = NULL;
    if (createNew) {
        statusButton = new QTableWidgetItem(buttonText);
        statusButton->setBackgroundColor(buttonColor);
        statusButton->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetMainWindow->setItem(rowId, 5, statusButton);
    }
    else {
        statusButton = ui->tableWidgetMainWindow->item(rowId, 5);
        statusButton->setText(buttonText);
        statusButton->setBackgroundColor(buttonColor);
        statusButton->setTextAlignment(Qt::AlignCenter);
    }
}


long MainWindow::insert_row(std::shared_ptr<algomodel1> val)
{
    //this will give the present number of rows available.
    size_t insertRow = ui->tableWidgetMainWindow->rowCount();

    //insert the row at the bottom of the table widget - using.
    ui->tableWidgetMainWindow->insertRow(insertRow);

    non_editable(insertRow, 0, val->m_upname.c_str());
    non_editable(insertRow, 1, QString::number((int)val->m_algoType));
    non_editable(insertRow, 2, QString::number(val->m_strategyId));
    non_editable(insertRow, 3, val->dataSymbol.c_str());
    non_editable(insertRow, 4, QString::number(val->m_trailingPercentage));

    setStrategyText(insertRow, val->m_status, true);

    non_editable(insertRow, 6, val->m_identifier.c_str());
    non_editable(insertRow, 8, QString::number(val->m_prevClose));
    non_editable(insertRow, 9, QString::number(val->m_baseVal));
    non_editable(insertRow, 10, QString::number(val->m_entryPrice));
    non_editable(insertRow, 11, QString::number(val->m_pointAPercentage));
    non_editable(insertRow, 12, QString::number(val->m_pointBPercentage));
    non_editable(insertRow, 13, QString::number(val->m_cp1Percentage));
    non_editable(insertRow, 14, QString::number(val->m_cp2Percentage));
    non_editable(insertRow, 15, QString::number(val->m_parentOrderPrice));
    non_editable(insertRow, 16, QString::number(val->m_profitPercentage));
    non_editable(insertRow, 17, QString::number(val->m_lossPercentage));
    non_editable(insertRow, 18, QString::number(val->m_lossLimitPrice));

    if(val->m_mode == OrderMode_BUY)
        non_editable(insertRow, 19, QString("BUY"));
    else
        non_editable(insertRow, 19, QString("SELL"));
    non_editable(insertRow, 20, QString::number(val->m_qty));
    non_editable(insertRow, 21, QString::number(val->m_tolerance));
    non_editable(insertRow, 22, val->m_startDate.c_str());
    non_editable(insertRow, 23, val->m_startTime.c_str());
    non_editable(insertRow, 24, val->m_endDate.c_str());
    non_editable(insertRow, 25, val->m_endTime.c_str());
    non_editable(insertRow, 26, QString::number(val->m_cancelTicks));
    non_editable(insertRow, 27, QString::number(val->m_internalId));
    non_editable(insertRow, 28, QString::number(val->m_tickSize));
    non_editable(insertRow, 29, val->m_secondaryOrderExitDate.c_str());
    non_editable(insertRow, 30, val->m_secondaryOrderExitTime.c_str());

    // Populate second row
    ++insertRow;
    ui->tableWidgetMainWindow->insertRow(insertRow);
    non_editable(insertRow, 4, QString::number(val->m_trailing));
    non_editable(insertRow, 10, QString::number(val->m_entryPriceCalculated));
    non_editable(insertRow, 11, QString::number(val->m_pointACalculated));
    non_editable(insertRow, 12, QString::number(val->m_pointBCalculated));
    non_editable(insertRow, 13, QString::number(val->m_cp1Calculated));
    non_editable(insertRow, 14, QString::number(val->m_cp2Calculated));
    non_editable(insertRow, 15, QString::number(val->m_parentOrderPrice));
    non_editable(insertRow, 16, QString::number(val->m_profitCalculated));
    non_editable(insertRow, 17, QString::number(val->m_lossCalculated));
    non_editable(insertRow, 18, QString::number(val->m_lossLimitPrice));

    // Create third row. Populatation will happen later
    ++insertRow;
    ui->tableWidgetMainWindow->insertRow(insertRow);
    non_editable(insertRow, 10, "");
    non_editable(insertRow, 11, "");
    non_editable(insertRow, 12, "");
    non_editable(insertRow, 13, "");
    non_editable(insertRow, 14, "");
    non_editable(insertRow, 15, "");
    non_editable(insertRow, 16, "");
    non_editable(insertRow, 17, "");

    return insertRow - 2;
}

void MainWindow::strategyRowClicked(int x, int y)
{
    if ((x % 3) == 0 && (y == 5))
    {
        QTableWidgetItem *statusButton = ui->tableWidgetMainWindow->item(x, 5);
        statusButton->setSelected(false);
        long internalId = ui->tableWidgetMainWindow->item(x, 27)->text().toLong();
        if (_iId2StrategyMap.find(internalId) != _iId2StrategyMap.end()) {
            if (Settings::isReadOnly())
            {
                QMessageBox::information(NULL, "Attention", "This operation is not allowed in ReadOnly GUI");
            }
            else
            {
                std::shared_ptr<algomodel1> algo = _iId2StrategyMap[internalId].second;
                setStrategyText(x, StrategyStatus_PENDING, false);
                AlgoModel1 am;
                algo->toAlgoModel1(&am);
                am.SetStatus(StrategyStatus_PENDING);
                algo->m_status = StrategyStatus_PENDING;
                am.Dump();
                char buffer[MAX_BUF];
                int size = am.Serialize(buffer);
                TcpClient::getInstance().writeToServer(buffer, size);
            }
        }
        else {
            long strategyId = ui->tableWidgetMainWindow->item(x, 2)->text().toLong();
            std::map<long, std::pair<long, StrategyStatus>>::iterator iter = _strategyList.find(strategyId);
            if (iter != _strategyList.end()) {
                StrategyStatus status = iter->second.second;
                if (status != StrategyStatus_STOPPED) {
                    if (Settings::isReadOnly())
                    {
                        QMessageBox::information(NULL, "Attention", "This operation is not allowed in ReadOnly GUI");
                    }
                    else
                    {
                        AlgoModelStopStrategy stopStrategy;
                        stopStrategy.setStrategyId(strategyId);
                        char buffer[MAX_BUF];
                        int size = stopStrategy.serialize(buffer);
                        TcpClient::getInstance().writeToServer(buffer, size);
                    }
                }
            }
        }
    }
}

void MainWindow::start_stop_slot()
{
    if(!check_status())
        return;
    QWidget *button = QApplication::focusWidget();
    if(button == NULL)
    {
        return;
    }
    int row = ui->tableWidgetMainWindow->rowAt(button->pos().y());
    int column = ui->tableWidgetMainWindow->columnAt(button->pos().x());
    if (column == 5)
    {
        QPushButton *pButton = qobject_cast<QPushButton*>(button);

        if(pButton == nullptr)
            return;

        if(pButton->text() == "Start" )
        {
            long internalId = ui->tableWidgetMainWindow->item(row, 28)->text().toLong();
            std::shared_ptr<algomodel1> algo = algomodelStrategy1::getInstance().getStrategy(internalId);
            AlgoModel1 am;
            algo->toAlgoModel1(&am);
            am.SetStatus(StrategyStatus_PENDING);
            am.Dump();
            char buffer[MAX_BUF];
            int size = am.Serialize(buffer);

            pButton->setText("Pending");
            //ui->tableWidgetMainWindow->setCellWidget(rowcol.value().first, 3, button);
            TcpClient::getInstance().writeToServer(buffer, size);
        }
        else if(pButton->text() != "Stopped")
        {
            long internalId = ui->tableWidgetMainWindow->item(row, 28)->text().toLong();
            std::shared_ptr<algomodel1> algo = algomodelStrategy1::getInstance().getStrategy(internalId);
            AlgoModel1 am;
            algo->toAlgoModel1(&am);
            am.Dump();
            am.SetStatus(StrategyStatus_STOPPED);
            char buffer[MAX_BUF];
            int size = am.Serialize(buffer);
            pButton->setText("Stopped");
            TcpClient::getInstance().writeToServer(buffer, size);
            pButton->setAutoFillBackground(true);
            pButton->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(255, 255, 255)");
        }
    }
}

bool MainWindow::check_status()
{
    if(!m_connectionstate)
    {
        QMessageBox::warning(
                    this,
                    tr("Oreder Recnnect"),
                    tr("GUI is not connected to back-end.") );
        return false;
    }
    return true;
}

int getPrecision(double number)
{
    int precision = 0;
    while (std::fabs(number - round(number)) > std::numeric_limits<double>::epsilon())
    {
        number *= 10;
        ++precision;
    }
    return precision;
}

void MainWindow::insert_row_from_sheet(std::shared_ptr<algomodel1> val)
{
    long rowId = insert_row(val);
    _iId2StrategyMap[val->m_internalId] = std::make_pair(rowId, val);
}

void MainWindow::generateReport()
{
    _calendar->show();
}

void MainWindow::load_algo_model1()
{
    if(!check_status())

    {
        return;
    }
    QStringList strategyFiles = QFileDialog::getOpenFileNames(
                this,
                tr("Load File"),
                QDir::currentPath(),
                tr("Excel files (*.xlsx);;All Files (*)")
                );
    QStringList::const_iterator constIterator = strategyFiles.constBegin();
    QStringList::const_iterator constIteratorEnd = strategyFiles.constEnd();

    for ( ; constIterator != constIteratorEnd; ++constIterator)
    {
        const QString& loadPath = *constIterator;
        InsertAlgo insertAlgo = std::bind(&MainWindow::insert_row_from_sheet, this, _1);
        int strategyAdded;
        QFileInfo finfo(loadPath);
        std::string filename = finfo.fileName().toStdString();
        filename = filename.substr(0, filename.find_first_of('.'));

        if (UPLOAD_FILE_NAME_SIZE < filename.length())
        {
            QMessageBox::critical(
                    this,
                    tr("File name too long"),
                    tr("Could not add strategies, because file name "
                       "is longer than %1 symbols")
                        .arg(UPLOAD_FILE_NAME_SIZE));
            continue;
        }

        getStrategiesFromExcel(loadPath.toStdString(), insertAlgo, strategyAdded, filename);

        if(!strategyAdded)
            QMessageBox::warning(
                    this,
                    tr("Load strategy"),
                    tr("Could not add strategy please check excel"));

    }
    ui->tableWidgetMainWindow->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->tableWidgetMainWindow->resizeColumnsToContents();
}

void MainWindow::non_editable(size_t row, size_t index, QString str)
{
    QTableWidgetItem *strategyitem = new QTableWidgetItem(str);
    ui->tableWidgetMainWindow->setItem(row,index, strategyitem);
}

void MainWindow::update_server_disconnected()
{
    qDebug() << "Backend disconnected";
    update_connection_frame(ui->feedConnStatus, false);
    update_connection_frame(ui->orderConnStatus, false);
    update_connection_frame(ui->backendConnStatus, false);
    m_connectionstate = false;
    m_heartbeatthread->terminate();
}

void MainWindow::process_data(std::string data)
{
    size_t bytes_transferred = data.size();
    char buffer[MAXDATABUFFER];

    if(_pendingDataSize > 0)
    {
        memcpy(buffer, _pendingBuffer, _pendingDataSize);
        memcpy(buffer+_pendingDataSize, data.c_str(), bytes_transferred);
        bytes_transferred += _pendingDataSize;
        process_buffer(buffer, bytes_transferred);
    }
    else
    {
        memcpy(buffer, data.c_str(), bytes_transferred);
        process_buffer(buffer, bytes_transferred);
    }
}

void MainWindow::process_buffer(const char *buffer, qint32 bytesAvailable)
{
    uint16_t tmp1 = 0, tmp2 = 0;
    uint16_t packetSize = 0;
    uint16_t initialSize = bytesAvailable;
    uint16_t currentBuffPos =  0;
    while(bytesAvailable>1)
    {
        DESERIALIZE_16(tmp1, tmp2, packetSize = tmp2, buffer, currentBuffPos);

        if(bytesAvailable && bytesAvailable < packetSize+2)
        {
            memcpy (_pendingBuffer, buffer+currentBuffPos-2, bytesAvailable);
            _pendingDataSize = bytesAvailable;// - (currentBuffPos-2);
            break;
        }
        process_handler(buffer+currentBuffPos-2);
        currentBuffPos = currentBuffPos + packetSize;
        bytesAvailable = initialSize - currentBuffPos;
    }
    if(bytesAvailable <= 0)
    {
        memset(_pendingBuffer, 0, MAXDATABUFFER);
        _pendingDataSize = 0;
    }
}

void MainWindow::handle_read_header( size_t bytes_transferred )
{
    char buffer[MAXDATABUFFER] ={'0'};
    if(_pendingDataSize > 0)
    {
        memcpy(buffer, _pendingBuffer, _pendingDataSize);
        memcpy(buffer+_pendingDataSize, _buffer, bytes_transferred);
        bytes_transferred += _pendingDataSize;
        process_buffer(buffer, bytes_transferred);
    }
    else
    {
        memcpy(buffer, _buffer, bytes_transferred);
        process_buffer(buffer, bytes_transferred);
    }
}

void MainWindow::process_handler(const char* buffer)
{
    // First Category of response, then confirmation
    uint8_t commandCategory;
    uchar tmpChar = 0;
    uint32_t offset = 2;

    // De-serialize command category
    DESERIALIZE_8(tmpChar, commandCategory=tmpChar, buffer, offset);
    switch (commandCategory)
    {
    case CommandCategory::CommandCategory_CONNECTION:
    {
        qDebug() << " Received CommandCategory_CONNECTION " ;
        Heartbeat h(buffer+offset);
        if(!h.GetFeedConnection())
        {
            update_connection_frame(ui->feedConnStatus, false);
        }
        else
        {
            update_connection_frame(ui->feedConnStatus, true);
        }
        if(!h.GetOPConnection())
        {
            update_connection_frame(ui->orderConnStatus, false);
        }
        else
        {
            update_connection_frame(ui->orderConnStatus, true);
        }
        update_connection_frame(ui->backendConnStatus, true);
        break;
    }
    case CommandCategory::CommandCategory_STRATEGY_ID:
    {
        qDebug() << " Received CommandCategory_Strategy_Id";
        AlgoModelStrategyId algoModelStrategyId(buffer+offset);
        std::map<long, std::pair<long, std::shared_ptr<algomodel1>>>::iterator iter = _iId2StrategyMap.find(algoModelStrategyId.getInternalId());
        int rowId = iter->second.first;
        _strategyList[algoModelStrategyId.getStrategyId()] = std::make_pair(rowId, iter->second.second->m_status);
        QTableWidgetItem *item = ui->tableWidgetMainWindow->item(rowId,2);
        item->setText(QString::number(algoModelStrategyId.getStrategyId()));
        _iId2StrategyMap.erase(iter);
        break;
    }
    default:
        qDebug() << "Unhandled Command from the server [" <<(int) commandCategory <<"]";
        break;
    }
  }
}

