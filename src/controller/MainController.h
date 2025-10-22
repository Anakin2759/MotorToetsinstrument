#pragma once

#include <qlocale.h>

#include <QTime>
#include <string>

#include "model/CreateServoDriver.h"
#include "model/EtherCatMaster.h"
#include "model/MotorDataModel.h"
#include "model/SdoHandler.h"
#include "model/common.h"
#include "utils/EventLoop.h"
#include "utils/Logger.h"
#include "utils/ModbusRtuManager.h"
#include "utils/SQLRW.h"
#include "utils/SettingFileManager.h"
#include "view/MainWindow.h"

struct ContorlTime
{
    size_t day = 0;
    size_t hour = 2;
    size_t minute = 0;
    size_t second = 0;

    ContorlTime() = default;
    ContorlTime(const ContorlTime &other)
    {
        day = other.day;
        hour = other.hour;
        minute = other.minute;
        second = other.second;
    }
    ContorlTime &operator=(const ContorlTime &other)
    {
        if (this != &other)
        {
            day = other.day;
            hour = other.hour;
            minute = other.minute;
            second = other.second;
        }
        return *this;
    }
    void decreaseOneSecond()
    {
        if (second > 0)
        {
            --second;
        }
        else
        {
            second = 59;
            if (minute > 0)
            {
                --minute;
            }
            else
            {
                minute = 59;
                if (hour > 0)
                {
                    --hour;
                }
                else
                {
                    hour = 23;
                    if (day > 0)
                    {
                        --day;
                    }
                }
            }
        }
    }
    void setMinutes(size_t minutes)
    {
        day = minutes / (24 * 60);
        hour = (minutes % (24 * 60)) / 60;
        minute = minutes % 60;
        second = 0;
    }
    size_t toMinutes() const { return day * 24 * 60 + hour * 60 + minute; }

    std::string toString()
    {
        return QString("%1天%2小时%3分钟%4秒").arg(day).arg(hour).arg(minute).arg(second).toStdString();
    }
};

struct TestData
{
    int32_t defaultSpeed = std::numeric_limits<int>::max();
    std::string testername;
    std::string testerID;
    std::string orderID;
    ContorlTime testTime;
};

class MainController : public QObject
{
    Q_OBJECT
public:
    MainController(MainWindow &window, QObject *parent = nullptr);
    ~MainController();
    void initEventloop();
    void changeState();

    void writeSnCode();

    void updateMotorViewData();
    void writeToExcel();
    void stop();

    void writeServoSettings();
    void readServoSettings();

private:
    MainWindow &window;
    MotorDataModel *motorDataModel;
    EtherCatMaster *master = nullptr;
    std::vector<IServoDriver *> servoDrivers;
    size_t m_master_index = 0;

    EventLoop *updateMotorDataLoop;
    EventLoop *runLoop;
    EventLoop *stopLoop;
    EventLoop *sqlRwLoop;

    SQLRW *sqlrw;
    SettingFileManager *settingFileManager;

    bool enable_running_flag = false;
    QStandardItemModel *model;
    SdoHandler sdoHandler;
    TestData testData;
    QMap<QString, QString> errorCodeConfig;
    QTimer *runTimer;
    ContorlTime remainingTime;
    EventLoop *runTimerLoop;
    EventLoop *temperatureScanLoop;
    std::atomic_bool temperatureScanFlag = false;
    ModbusRTUManager *modbus = nullptr;
    size_t temperatureScanStartStationNumber = 0;
    size_t temperatureScanEndStationNumber = 0;
    
    EventLoop *TaskFlowLoop;
};
