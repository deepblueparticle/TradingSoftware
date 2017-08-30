#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSignalMapper>
#include <QPushButton>
#include <QMap>
#include <QFile>
#include <QTimer>
#include <shared/order.h>
#include <memory>
#include <fstream>
#include <QSharedPointer>
#include <QtSql/QSqlQuery>
#include <QCalendarWidget>

#include "newalgodialog.h"
#include "algomodel1.h"
#include "heartbeat.h"
#include "GUIWorkerThread.h"
#include "ReportGenerator.h"

namespace Ui {
class MainWindow;
}
namespace Algo {

const int MAXDATABUFFER = 100000;
const int MAX_BUF_SIZE = 2048;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void update_connection_frame(QLabel *label, bool status);
    void update_row_color(int row, bool status);
    void start_relogin();
    void insert_row_from_db(QSqlQuery query);

    long insert_row(std::shared_ptr<algomodel1> val);
    void insert_row_from_sheet(std::shared_ptr<algomodel1> val);

private slots:
    void start_stop_slot();

    void load_algo_model1();
    void slotClicked(const QDate& date);
    void generateReport();
    void timeout_update();
    void strategy_window_update();
    void alert_window_update();
    void loadFromDb(const QSqlQuery &query);
    void updateStrategyThirdRow(QSqlQuery query);
    void updateStrategyStatus(QSqlQuery query);
    void updateAlertWindow(QSqlQuery query);
    void strategyRowClicked(int x, int y);
    void reportDone(const QString &message);

signals:
    void algo_item_clicked(QTreeWidgetItem *item, int column);


public slots:
    void process_data(std::string data);
    void update_server_disconnected();

private:
    Ui::MainWindow *ui;
    QString         m_lastSelectedTreeItem;
    int             m_lastStartegyId;
    QSignalMapper   m_buttonSignalMapper;
    QMap<QPushButton*, QPair<int, int> > m_buttonMap;
    size_t _pendingDataSize;
    char _buffer[MAXDATABUFFER];
    char _pendingBuffer[MAXDATABUFFER];
    bool            m_connectionstate;
    HeartbeatThread *m_heartbeatthread;
    QString          m_resyncOrderFile;

    void process_buffer(const char *buffer, qint32 bytesAvailable);
    void handle_read_header(size_t bytes_transferred);
    void process_handler(const char *buffer);
    void add_instruments(std::string instrument);
    void setStrategyText(long rowId, Algo::StrategyStatus status, bool createNew);

    void update_algoin_tree_widget(AlgoModel1 val);
    bool check_status();
    void start_gui_update_timer(QTimer *timer);
    void stop_gui_update_timer(QTimer  *timer);
    
	QTimer  *m_timer;
    int      m_counter;
    bool     m_isSyncDone;
    long     m_lastOrderIndex;
    long     m_lastAlertIndex;
    long     m_lastStrategyIndex;
    void non_editable(size_t row, size_t index, QString str);
    std::map <long, AlgoModel1> m_mStregyIdAlgoMap;
    QSharedPointer<GUIWorkerThread> _guiInitThread;

    QTimer  *_strategy_window_update_timer;
    QSharedPointer<GUIWorkerThread> _strategyWindowThread;
    QString _lastStrategyWindowUpdateTime;
    bool    _showUpdateStatus;

    QTimer  *_alert_window_update_timer;
    QSharedPointer<GUIWorkerThread> _alertWindowThread;
    QSharedPointer<ReportGenerator> _reportGeneratorThread;
    QString _lastAlertWindowUpdateTime;
    QString _lastId;

    std::map<long, std::pair<long, StrategyStatus>> _strategyList;
    std::map<long, std::pair<long, std::shared_ptr<algomodel1>>> _iId2StrategyMap;
    QSharedPointer<QCalendarWidget> _calendar;
};
}
#endif // MAINWINDOW_H
