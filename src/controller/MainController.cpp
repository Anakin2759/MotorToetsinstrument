#include <qlogging.h>

#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <cstddef>
#include <filesystem>
#include <memory>

#include "MainController.h"
#include "utils/ExcelWriter.h"
#include "utils/JsonParser.h"
#include "utils/Logger.h"
#include "utils/ThreadPool.h"

const static std::map<std::string, std::string> power_map = {
    {"009", "90W"},   {"010", "100W"},  {"020", "200W"},  {"040", "400W"},   {"075", "750W"},   {"085", "850W"},
    {"100", "1000W"}, {"130", "1300W"}, {"200", "2000W"}, {"10D", "10000W"}, {"10E", "100000W"}};

const static std::map<char, std::string> motorTypeMap = {
    {'A', "伺服马达"}, {'B', "伺服驱动器"}, {'C', "伺服马达插件"},   {'D', "调位马达"},
    {'E', "变频马达"}, {'F', "步进马达"},   {'G', "步进马达驱动器"}, {'H', "调速器"},
    {'J', "直线电机"}, {'T', "调速马达"},   {'K', "普通马达"},       {'L', "无框力矩电机"}};

static std::map<char, std::string> vendor_map = {{'H', "华成"},     {'C', "汇川"},   {'F', "富士"}, {'S', "松下"},
                                                 {'T', "精研"},     {'D', "台达"},   {'E', "禾川"}, {'R', "儒竟"},
                                                 {'M', "多摩川"},   {'Z', "中大"},   {'B', "大银"}, {'A', "其他"},
                                                 {'K', "科尔摩根"}, {'Q', "盛泰奇"}, {'G', "泰格"}, {'X', "行动元"}};

inline uint64_t getCurrentUnixTimestamp()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

void populateTable(QStandardItemModel *model, const QVector<QMap<QString, QString>> &pnConfig)
{
    // 设置表头
    QStringList headers = {"轴", "索引", "子索引", "名称", "含义", "单位", "注备", "用户输入"};
    model->setHorizontalHeaderLabels(headers);

    int row = 0;

    // 遍历 pnConfig 这个 QVector
    for (const auto &config : pnConfig)
    {
        model->setItem(row, 0, new QStandardItem(config.value("axis", "Unknown")));
        model->setItem(row, 1, new QStandardItem(config.value("index", "Unknown")));
        model->setItem(row, 2, new QStandardItem(config.value("subindex", "Unknown")));
        model->setItem(row, 3, new QStandardItem(config.value("name", "Unknown")));
        model->setItem(row, 4, new QStandardItem(config.value("meaning", "Unknown")));
        model->setItem(row, 5, new QStandardItem(config.value("units", "Unknown")));
        model->setItem(row, 6, new QStandardItem(config.value("comments", "Unknown")));

        // 最后一列用于用户输入，设置为可编辑
        auto *inputItem = new QStandardItem();
        inputItem->setEditable(true); // 允许编辑
        model->setItem(row, 7, inputItem);
        ++row;
    }

    // 禁用除最后一列外的编辑
    for (int i = 0; i < model->rowCount(); ++i)
    {
        for (int j = 0; j < model->columnCount() - 1; ++j)
        {
            QStandardItem *item = model->item(i, j);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 设置为不可编辑
        }
    }
}

MainController::MainController(MainWindow &window, QObject *parent)
    : QObject(parent), window(window), sqlrw(new SQLRW("test.db"))
{
    motorDataModel = new MotorDataModel();
    window.functionTabWidget->motorDataView->setModel(motorDataModel);

    runTimer = new QTimer(this);

    motorDataModel->updateData();

    settingFileManager = new SettingFileManager("setting/szhc402.ini");
    auto pnsettings = settingFileManager->getPNConfig();
    errorCodeConfig = settingFileManager->getErrorCodeConfig();

    model = new QStandardItemModel();
    populateTable(model, pnsettings);
    window.functionTabWidget->servoDriverSettingsWidget->servoDriverSettingsView->setModel(model);

    initEventloop();

    QObject::connect(window.controlButtonBar->masterComboBox, &QComboBox::currentIndexChanged,
                     [this](int index) -> void { m_master_index = index; });
    QObject::connect(window.controlButtonBar->initButton, &QPushButton::clicked, [this, &window]() {
        LOG_TRACE("初始化");
        if (master != nullptr)
        {
            delete master;
        }
        master = new EtherCatMaster(m_master_index);
        master->updateMasterInfo();

        if (master->updateSlaveInfoList())
        {
            LOG_TRACE("更新从站信息成功");
        }
        else
        {
            LOG_ERROR("更新从站信息失败");
        }

        window.functionTabWidget->servoDriverListWidget->updateServoDriverList(master->getSlaveInfoList());
        window.functionTabWidget->servoDriverSettingsWidget->updateSlaveIDCombobox(master->getSlaveInfoList().size());
        window.functionTabWidget->snCodeWritter->updateUI(master->getSlaveInfoList().size());
    });
    QObject::connect(window.controlButtonBar->activeButton, &QPushButton::clicked, [this]() {
        auto func = [this]() { this->changeState(); };
        ThreadPool::getInstance().enqueue(func);
    });
    QObject::connect(window.controlButtonBar->enableButton, &QPushButton::clicked, [this]() {
        LOG_TRACE("使能");

        master->get_enable_flag().store(true);
    });
    QObject::connect(window.controlButtonBar->runButton, &QPushButton::clicked, [this]() {
        LOG_TRACE("运行");
        runLoop->start();
        sqlRwLoop->start();
        runTimer->setInterval(testData.testTime.toMinutes() * 60 * 1000);
        remainingTime = testData.testTime;
        runTimer->start();
        runTimerLoop->start();
        temperatureScanLoop->start();
    });
    QObject::connect(window.controlButtonBar->stopButton, &QPushButton::clicked, [this]() {
        LOG_TRACE("停止");
        stop();
    });
    QObject::connect(window.controlButtonBar->disableButton, &QPushButton::clicked, [this]() {
        LOG_TRACE("去使能");
        master->get_enable_flag() = false;
    });
    QObject::connect(window.controlButtonBar->createReportButton, &QPushButton::clicked, [this]() {
        LOG_TRACE("产生报告");
        writeToExcel();
    });

    QObject::connect(window.controlButtonBar->clearLogButton, &QPushButton::clicked,
                     [&window]() { window.loggerWidget->clear(); });

    QObject::connect(window.functionTabWidget->snCodeWritter->savebutton, &QPushButton::clicked, [this, &window]() {
        LOG_TRACE("写入SN码");
        for (size_t i = 0; i < window.functionTabWidget->snCodeWritter->editList.size(); i++)
        {
            auto &item = window.functionTabWidget->snCodeWritter->editList[i];
            if (item->edit->text().isEmpty())
            {
                LOG_ERROR("SN码为空");
                continue;
            }

            auto slave = master->getSlaveList()[i / 2];
            auto axisNumber = slave->getIndex();

            JsonParser parser;

            slave->getSnData()[i % axisNumber] = parser(item->getSnCode());
            if (slave->getSnData()[i].find("PN") != slave->getSnData()[i].end())
            {
                auto &pn = slave->getSnData()[i % axisNumber]["PN"];

                std::string power = power_map.at(pn.substr(4, 3));

                slave->getMotorData()[i % axisNumber]->motor_type = motorTypeMap.at(pn[3]);
                slave->getMotorData()[i % axisNumber]->vendor = vendor_map.at(pn[7]);

                slave->getMotorData()[i % axisNumber]->power = power;
            }
            if (slave->getSnData()[i % axisNumber].find("FIRM") != slave->getSnData()[i].end())
            {
                slave->getMotorData()[i % axisNumber]->firm = slave->getSnData()[i]["FIRM"];
            }
            if (slave->getSnData()[i % axisNumber].find("SN") != slave->getSnData()[i].end())
            {
                slave->getMotorData()[i % axisNumber]->sn = slave->getSnData()[i]["SN"];
            }
        }
    });
    QObject::connect(window.functionTabWidget->servoDriverSettingsWidget->readButton, &QPushButton::clicked,
                     [this, &window]() {
                         LOG_TRACE("读取配置");
                         readServoSettings();
                     });

    QObject::connect(window.functionTabWidget->servoDriverSettingsWidget->writeButton, &QPushButton::clicked,
                     [this, &window]() {
                         LOG_TRACE("写入配置");
                         writeServoSettings();
                     });

    QObject::connect(
        window.functionTabWidget->testDataSettingsWidget->saveButton, &QPushButton::clicked, [this, &window]() {
            testData.testername =
                window.functionTabWidget->testDataSettingsWidget->testerNameEdit->text().toStdString();
            testData.testerID = window.functionTabWidget->testDataSettingsWidget->testerIdEdit->text().toStdString();
            testData.orderID =
                window.functionTabWidget->testDataSettingsWidget->testOrderNumberEdit->text().toStdString();

            bool ok;
            int minutes = window.functionTabWidget->testDataSettingsWidget->testTimeEdit->text().toInt(&ok);
            if (ok)
            {
                testData.testTime.setMinutes(minutes);
            }
            else
            {
                LOG_ERROR("无效的时间输入");
            }
            testData.defaultSpeed =
                window.functionTabWidget->testDataSettingsWidget->testDefaultSpeedEdit->text().toInt(&ok);
            if (!ok)
            {
                LOG_ERROR("无效的速度输入");
            }
        });
    QObject::connect(runTimer, &QTimer::timeout, [this]() { stop(); });

    QObject::connect(window.functionTabWidget->temperatureScanSettingWidget,
                     &TemperatureScanSettingWidget::startTemperatureScan,
                     [this](size_t startStationNumber, size_t endStationNumber, std::string com, int baudrate,
                            int dataBits, int stopBits, char parity) {
                         if (startStationNumber == std::numeric_limits<size_t>::max() ||
                             endStationNumber == std::numeric_limits<size_t>::max())
                         {
                             LOG_ERROR("无效的站号");
                             return;
                         }
                         temperatureScanStartStationNumber = startStationNumber;
                         temperatureScanEndStationNumber = endStationNumber;
                         modbus = new ModbusRTUManager(com, baudrate, parity, dataBits, stopBits);
                         modbus->connect();

                         temperatureScanFlag = true;
                         LOG_TRACE("连接温度扫描仪");
                     });
    QObject::connect(window.functionTabWidget->temperatureScanSettingWidget,
                     &TemperatureScanSettingWidget::stopTemperatureScan, [this]() {
                         delete modbus;
                         temperatureScanFlag = false;
                         LOG_TRACE("断开温度扫描仪连接");
                     });
}

MainController::~MainController()
{
    delete motorDataModel;
    delete updateMotorDataLoop;
    delete runLoop;
    delete sqlrw;
    if (master != nullptr)
    {
        delete master;
    }
}

void MainController::changeState()
{
    if (window.controlButtonBar->activeButton->text() == "切换实时状态")
    {
        LOG_TRACE("切换实时状态");
        if (master == nullptr)
        {
            LOG_ERROR("主站未初始化");
            return;
        }

        if (master->updateSlaveConfigList() == false)
        {
            LOG_ERROR("更新从站配置失败");
            return;
        }

        if (master->createDomain() == false)
        {
            LOG_ERROR("创建域失败");
            return;
        }

        if (master->slaveConfigPDOs() == false)
        {
            LOG_ERROR("配置PDOs失败");
            return;
        }

        if (master->registersPdoEntriesforDomain() == false)
        {
            LOG_ERROR("注册PDO条目失败");
            return;
        }

        if (master->slaveConfigSDOs() == false)
        {
            LOG_ERROR("配置SDOs失败");
            return;
        }

        if (master->active() == false)
        {
            LOG_ERROR("激活失败");
            return;
        }

        if (master->getDomainsProcessData() == false)
        {
            LOG_ERROR("获取域数据失败");
            return;
        }

        if (master->isActivated() == true)
        {
            window.controlButtonBar->activeButton->setText("取消实时状态");
        }
        master->rtLoopStart();

        updateMotorDataLoop->start();
        sqlrw->deleteAllMotorData();
        sqlrw->deleteAllTemperatureData();
    }
    else
    {
        LOG_TRACE("取消实时状态");
        motorDataModel->getMotorViewData().clear();
        stop();
        updateMotorDataLoop->stop();

        master->deactivate();
        if (master->isActivated() == false)
        {
            window.controlButtonBar->activeButton->setText("切换实时状态");
        }
        master->rtLoopStop();
    }
}

void MainController::initEventloop()
{
    updateMotorDataLoop = new EventLoop(500);

    auto func = [this]() {
        auto &datas = motorDataModel->getMotorViewData();
        if (datas.size() == 0)
        {
            for (auto &slave : master->getSlaveList())
            {
                for (auto i = 0; i < slave->get_axis_number(); ++i)
                {
                    MotorViewData data{};
                    data.servoAddress = slave->getIndex();
                    data.index = i;
                    data.velocity = slave->getMotorData()[i]->velocity;
                    data.position = slave->getMotorData()[i]->position;
                    data.torque = slave->getMotorData()[i]->torque;
                    data.loadrate = slave->getMotorData()[i]->load_rate;
                    data.temperature = slave->getMotorData()[i]->temperature;
                    data.sn = QString::fromStdString(slave->getMotorData()[i]->sn);
                    data.status = "未知";
                    if (testData.defaultSpeed != std::numeric_limits<int>::max())
                    {
                        data.targetVelocity = testData.defaultSpeed;
                    }
                    datas.push_back(data);

                    slave->getMotorData()[i]->target_speed = data.targetVelocity;
                }
            }
            LOG_TRACE("初始化数据显示");
        }
        else
        {
            for (auto &slave : master->getSlaveList())
            {
                for (auto i = 0; i < slave->get_axis_number(); ++i)
                {
                    auto index = slave->getIndex() * slave->get_axis_number() + i;
                    datas[index].servoAddress = slave->getIndex();
                    datas[index].index = i;
                    datas[index].velocity = slave->getMotorData()[i]->velocity;
                    datas[index].position = slave->getMotorData()[i]->position;
                    datas[index].torque = slave->getMotorData()[i]->torque;
                    datas[index].loadrate = slave->getMotorData()[i]->load_rate;
                    datas[index].temperature = slave->getMotorData()[i]->temperature;
                    if (slave->getMotorData()[i]->online.load() == 0)
                    {
                        datas[index].status = "离线";
                    }
                    else if (slave->getMotorData()[i]->error_code.load() != 0)
                    {
                        datas[index].status = "报警";
                    }
                    else
                    {
                        datas[index].status = "正常";
                    }

                    datas[index].errorCode = errorCodeConfig[QString::number(slave->getMotorData()[i]->error_code)];
                    datas[index].runTime = QString::fromStdString(testData.testTime.toString());
                    datas[index].remainTime = QString::fromStdString(remainingTime.toString());

                    slave->getMotorData()[i]->target_speed = datas[index].targetVelocity;

                    datas[index].sn = QString::fromStdString(slave->getMotorData()[i]->sn);
                    datas[index].power = QString::fromStdString(slave->getMotorData()[i]->power);
                    datas[index].vendor = QString::fromStdString(slave->getMotorData()[i]->vendor);
                }
            }
        }

        motorDataModel->updateData();
    };

    updateMotorDataLoop->setDefaultEvent(func);

    runLoop = new EventLoop(1);
    auto func1 = [this]() { master->run(); };
    runLoop->setDefaultEvent(func1);

    stopLoop = new EventLoop(1);
    auto func2 = [this]() { master->stop(); };
    stopLoop->setDefaultEvent(func2);

    sqlRwLoop = new EventLoop(1000);
    auto func3 = [this]() {
        auto timestamp = getCurrentUnixTimestamp();
        for (auto &slave : master->getSlaveList())
        {
            for (auto i = 0; i < slave->get_axis_number(); ++i)
            {
                sqlrw->insertMotorData(slave->getIndex(), i, slave->getMotorData()[i]->velocity,
                                       slave->getMotorData()[i]->torque, slave->getMotorData()[i]->load_rate,
                                       timestamp);
                sqlrw->insertTemperatureData(slave->getIndex(), i, slave->getMotorData()[i]->temperature, timestamp);
            }
        }
    };
    sqlRwLoop->setDefaultEvent(func3);

    runTimerLoop = new EventLoop(1000);
    auto func4 = [this]() { remainingTime.decreaseOneSecond(); };
    runTimerLoop->setDefaultEvent(func4);

    temperatureScanLoop = new EventLoop(500);
    auto temperatureScanFunc = [this]() {
        if (temperatureScanFlag == false)
        {
            return;
        }
        int start_id = temperatureScanStartStationNumber;
        std::vector<uint16_t> values;
        for (auto &slave : master->getSlaveList())
        {
            for (auto i = 0; i < slave->get_axis_number(); ++i)
            {

                modbus->setSlaveId(start_id++);

                modbus->readHoldingRegisters_03(0, 1, values);
                auto data = (double)values[0] / 10.0;
                slave->getMotorData()[i]->temperature = data;
            }
        }
    };
    temperatureScanLoop->setDefaultEvent(temperatureScanFunc);
}

void MainController::stop()
{
    auto func = [this]() {
        runLoop->stop();
        stopLoop->start();
        usleep(2000 * 1000);
        stopLoop->stop();
        sqlRwLoop->stop();
        QMetaObject::invokeMethod(runTimer, "stop", Qt::QueuedConnection);

        runTimerLoop->stop();
        temperatureScanLoop->stop();
    };
    ThreadPool::getInstance().enqueue(func);
}

void MainController::writeToExcel()
{
    try
    {
        // 确保父目录存在
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm *local_time = std::localtime(&now_time);

        // 格式化文件夹名称为 YY_MM_DD_HH
        std::ostringstream folderName;
        folderName << std::put_time(local_time, "%y_%m_%d_%H");

        // 定义文件夹路径
        std::filesystem::path repoPath = std::filesystem::current_path() / "repo";
        std::filesystem::path dirPath = repoPath / folderName.str();

        // 创建 repo 文件夹（如果不存在）
        std::filesystem::create_directory(repoPath);

        // 创建时间格式的文件夹
        if (std::filesystem::create_directory(dirPath))
        {
            LOG_TRACE("文件夹 创建成功！");
        }
        else
        {
            LOG_TRACE("文件夹 创建失败！");
        }
        for (auto &slave : master->getSlaveList())
        {
            if (slave->getSnData().empty())
            {
                continue;
            }
            for (size_t i = 0; i < slave->get_axis_number(); i++)
            {
                std::string xlsx_file_name = dirPath.string() + "/" + slave->getSnData()[i]["SN"] + ".xlsx";
                uint8_t current_row = 0;
                std::unique_ptr<ExcelWriter> m_excelWriter = std::make_unique<ExcelWriter>(xlsx_file_name);
                m_excelWriter->createWorkSheet("Report");

                m_excelWriter->mergeCells(current_row++, 0, 0, 13, "伺服电机老化测试报告");
                m_excelWriter->writeCell(current_row, 0, "检测员 工号:");
                m_excelWriter->writeCell(current_row, 1, testData.testerID);
                m_excelWriter->writeCell(current_row, 2, "检测员 姓名:");
                m_excelWriter->writeCell(current_row, 3, testData.testername);
                m_excelWriter->writeCell(current_row, 4, "时间:");

                auto t = std::time(nullptr);
                auto tm = *std::localtime(&t);
                std::ostringstream oss;
                oss << std::put_time(&tm, "%Y:%m:%d:%H:%M");
                std::string currentTime = oss.str();
                m_excelWriter->writeCell(current_row, 5, currentTime);

                m_excelWriter->writeCell(current_row, 6, "检测时长:");

                m_excelWriter->writeCell(current_row, 7, testData.testTime.toString());
                m_excelWriter->writeCell(current_row, 8, "检测间隔:");
                m_excelWriter->writeCell(current_row++, 9, std::to_string(master->getSendInterval()));
                m_excelWriter->writeCell(++current_row, 0, "订单号:");
                m_excelWriter->writeCell(current_row, 1, testData.orderID);

                current_row++;

                m_excelWriter->writeCell(current_row, 0, "设备编号");

                m_excelWriter->writeCell(current_row, 1, "电机SN码");
                m_excelWriter->writeCell(current_row++, 2, slave->getMotorData()[i]->sn);
                m_excelWriter->writeCell(current_row, 0, "测试结果");
                m_excelWriter->writeCell(current_row, 1, "合格");

                current_row++;
                m_excelWriter->writeCell(current_row, 0, "程序运行次数:");
                m_excelWriter->writeCell(current_row++, 1, "1");
                m_excelWriter->writeCell(current_row, 0, "程序运行时间:");
                m_excelWriter->writeCell(current_row++, 1, "60");

                // EMDA040H1JD3

                m_excelWriter->writeCell(current_row, 0, "电机品牌");
                m_excelWriter->writeCell(current_row++, 1, slave->getMotorData()[i]->vendor);

                m_excelWriter->writeCell(current_row, 0, "电机型号");

                m_excelWriter->writeCell(current_row++, 1, slave->getMotorData()[i]->firm);

                m_excelWriter->writeCell(current_row, 0, "电机类型");

                m_excelWriter->writeCell(current_row++, 1, slave->getMotorData()[i]->motor_type);

                m_excelWriter->writeCell(current_row++, 0, "额定功率");

                m_excelWriter->writeCell(current_row, 1, slave->getMotorData()[i]->power);

                current_row++;

                auto write_data = [this, &i, &slave](ExcelWriter *m_excelWriter, uint8_t &current_row,
                                                     std::string name) {
                    auto data = sqlrw->getStatistics(name, slave->getIndex(), i);
                    if (data.has_value())
                    {
                        LOG_TRACE("有数据输出");
                        auto [max, min, avg] = data.value();
                        LOG_TRACE("max:{} min:{} avg:{}", max, min, avg);

                        m_excelWriter->writeCell(current_row, 0, "最大值：");
                        m_excelWriter->writeCell(current_row, 1, std::to_string(max));
                        m_excelWriter->writeCell(current_row, 2, "最小值");
                        m_excelWriter->writeCell(current_row, 3, std::to_string(min));
                        m_excelWriter->writeCell(current_row, 4, "平均值");
                        m_excelWriter->writeCell(current_row++, 5, std::to_string(avg));
                    }
                    else
                    {
                        LOG_ERROR("没有数据输出");
                        m_excelWriter->writeCell(current_row, 0, "最大值：");
                        m_excelWriter->writeCell(current_row, 1, std::to_string(-1));
                        m_excelWriter->writeCell(current_row, 2, "最小值");
                        m_excelWriter->writeCell(current_row, 3, std::to_string(-1));
                        m_excelWriter->writeCell(current_row, 4, "平均值");
                        m_excelWriter->writeCell(current_row++, 5, std::to_string(-1));
                    }
                };
                auto data = sqlrw->getStatistics("speed", 0, 0);
                auto [max, min, avg] = data.value();
                LOG_TRACE("11111111max:{} min:{} avg:{}", max, min, avg);

                current_row++;
                m_excelWriter->writeCell(current_row++, 0, "电机表壳温度");
                write_data(m_excelWriter.get(), current_row, "temperature");

                current_row++;
                m_excelWriter->writeCell(current_row++, 0, "轴负载率");
                write_data(m_excelWriter.get(), current_row, "load_rate");

                current_row++;
                m_excelWriter->writeCell(current_row++, 0, "轴扭矩");
                write_data(m_excelWriter.get(), current_row, "torque");

                current_row++;
                m_excelWriter->writeCell(current_row++, 0, "电机速度");
                write_data(m_excelWriter.get(), current_row, "speed");

                m_excelWriter->save();
            }
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        LOG_ERROR(std::string("创建文件夹时出错: ") + e.what());
    }
}

int convertToInt(const QString &str1)
{
    std::string str = str1.toStdString();
    int base = 10;

    try
    {
        if (str.find("0x") == 0 || str.find("0X") == 0)
        {
            base = 16;
        }
        else if (str.find("0b") == 0 || str.find("0B") == 0)
        {
            base = 2;
            return std::stoi(str.substr(2), nullptr, base);
        }
        else if (str[0] == '0' && str.length() > 1)
        {
            base = 8;
        }
        return std::stoi(str, nullptr, base);
    }
    catch (const std::invalid_argument &e)
    {
        LOG_ERROR("无效的输入字符串: " + std::string(e.what()));
        return -1;
    }
    catch (const std::out_of_range &e)
    {
        LOG_ERROR("输入字符串超出范围: " + std::string(e.what()));
        return -1;
    }
}
void MainController::writeServoSettings()
{
    if (window.functionTabWidget->servoDriverSettingsWidget->servoDriverIdComboBox->count() == 0)
    {
        return;
    }
    uint16_t slaveIndex = window.functionTabWidget->servoDriverSettingsWidget->servoDriverIdComboBox->currentIndex();
    for (size_t row = 0; row < model->rowCount(); row++)
    {
        uint32_t index = convertToInt(model->item(row, 1)->text());
        uint8_t subIndex = convertToInt(model->item(row, 2)->text());
        if (model->item(row, 7)->text() == "")
        {
            continue;
        }
        int32_t value = model->item(row, 7)->text().toInt();

        sdoHandler(master->getMaster(), slaveIndex, index, subIndex, value, 0);
    }
}

void MainController::readServoSettings()
{
    if (window.functionTabWidget->servoDriverSettingsWidget->servoDriverIdComboBox->count() == 0)
    {
        return;
    }
    uint16_t slaveIndex = window.functionTabWidget->servoDriverSettingsWidget->servoDriverIdComboBox->currentIndex();
    for (size_t row = 0; row < model->rowCount(); row++)
    {
        uint32_t index = convertToInt(model->item(row, 1)->text());
        uint8_t subIndex = convertToInt(model->item(row, 2)->text());
        int32_t value = -1;
        qDebug() << "index" << index << "| subIndex" << subIndex;
        auto item = model->item(row, 7);

        sdoHandler(master->getMaster(), slaveIndex, index, subIndex, value, 1);

        model->item(row, 7)->setText(QString::number(value));
    }
}