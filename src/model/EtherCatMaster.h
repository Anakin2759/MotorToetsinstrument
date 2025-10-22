#pragma once
#include <ecrt.h>

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "interface/IServoDriver.h"
#include "utils/EventLoop.h"
class EtherCatMaster
{
public:
    explicit EtherCatMaster(const size_t &index = 0);
    ~EtherCatMaster();

    bool updateMasterInfo() noexcept;
    bool updateSlaveInfoList() noexcept;
    bool updateSlaveConfigList() noexcept;
    bool createDomain() noexcept;
    bool slaveConfigPDOs() noexcept;
    bool registersPdoEntriesforDomain() noexcept;
    bool slaveConfigSDOs() noexcept;
    bool active() noexcept;
    bool getDomainsProcessData() noexcept;
    bool deactivate() noexcept;

    void readMotorData() noexcept;
    void writeMotorData() noexcept;

    ec_master_t *getMaster() { return m_master; }

    const std::vector<ec_slave_info_t> &getSlaveInfoList() const { return m_slave_info_list; }

    bool enableServo();

    void clearServoAlarm();

    void run();
    void stop();

    void reserve() { direction = -direction; };

    inline bool isActivated() const { return m_isActivated; };

    void getIndex(){};
    ec_slave_config_t *getSlaveConfig(const size_t &index);

    void rtLoopStart() { loop.start(); }
    void rtLoopStop() { loop.stop(); }
    void rtLoopAdd(std::function<void()> func) { loop.add(func); }
    void checkMasterState();
    void checkDomainState();
    void checkSlaveState();
    size_t &getSendNumber() { return sendNumber; }
    uint8_t *&getInputDomainPD();
    uint8_t *&getOutputDomainPD();
    const std::vector<IServoDriver *> &getSlaveList() { return m_slave_list; }
    std::atomic_bool &get_enable_flag() { return enable_flag; }
    std::atomic_bool &get_clear_alarm_flag() { return clear_alarm_flag; }
    std::atomic_int &get_direction() { return direction; }

    const size_t &getReversalInterval() const { return reversalInterval; }
    void setReversalInterval(const size_t &reversalInterval) { this->reversalInterval = reversalInterval; }
    const size_t &getSendInterval() const { return sendInterval; }
    void setSendInterval(const size_t &sendInterval) { this->sendInterval = sendInterval; }

    void runWithoutChangeDirection(int32_t velocity);

    void taskFlow_start();
    void taskFlow_motor_run(int32_t velocity);
    void taskFlow_motor_stop();
    void taskFlow_sleep(uint32_t sleepTime);
    void taskFlow_end();

private:
    ec_master_t *m_master;
    ec_master_info_t m_info{};
    std::vector<ec_slave_info_t> m_slave_info_list;

    size_t m_index = 0;
    bool m_isActivated = false;
    std::vector<IServoDriver *> m_slave_list;
    ec_domain_t *m_input_domain;
    ec_domain_t *m_output_domain;
    uint8_t *m_input_domain_pd = nullptr;
    uint8_t *m_output_domain_pd = nullptr;

    std::vector<ec_pdo_entry_reg_t> m_input_domain_regs;
    std::vector<ec_pdo_entry_reg_t> m_output_domain_regs;
    EventLoop loop;
    EventLoop *taskFlowLoop;

    ec_master_state_t m_master_state{};
    ec_domain_state_t m_input_domain_state{};
    ec_domain_state_t m_output_domain_state{};

    std::map<int, std::string> alState = {{1, "初始化"}, {2, "PREOP"}, {4, "SAFEOP"}, {8, "OP"}};
    size_t sendNumber = 1;
    size_t reversalInterval = 10 * 60 * 1000;
    size_t sendInterval = 1;
    std::atomic_int direction = 1;

    std::vector<uint8_t> isServoChecked;

    std::atomic_bool clear_alarm_flag{false};
    std::atomic_bool enable_flag{false};

    std::vector<std::function<void()>> events{};
};
