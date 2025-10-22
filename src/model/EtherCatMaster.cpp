#include <ecrt.h>

#include <cstddef>
#include <cstring> // For strerror

#include "CreateServoDriver.h"
#include "EtherCatMaster.h"
#include "utils/Logger.h"
#include <chrono>

// 构造函数，初始化成员变量
EtherCatMaster::EtherCatMaster(const size_t &index) : m_master(nullptr), m_index(0)
{
    // 你可以在此处做一些必要的初始化
    m_index = index;

    // 创建EtherCAT主站
    m_master = ecrt_request_master(m_index); // 请求 EtherCAT 主站

    if (m_master == nullptr)
    {
        LOG_ERROR("初始化主站{}失败: ", m_index);
        return;
    }

    // 获取主站的信息
    if (ecrt_master(m_master, &m_info))
    {
        LOG_ERROR("获取主站{}信息失败", m_index);
        return;
    }
    m_isActivated = false;
    LOG_TRACE("EtherCAT 主站{} 初始化.", m_index);

    auto DefaultEvent = [this]() {
        if (m_isActivated)
        {
            ecrt_master_receive(m_master);
            ecrt_domain_process(m_output_domain);
            ecrt_domain_process(m_input_domain);

            readMotorData();

            ecrt_domain_queue(m_output_domain);
            ecrt_domain_queue(m_input_domain);
            ecrt_master_send(m_master);
        }
    };
    loop.setDefaultEvent(DefaultEvent);
}

EtherCatMaster::~EtherCatMaster()
{
    if (m_master != nullptr)
    {
        ecrt_release_master(m_master);
        m_master = nullptr;
        LOG_TRACE("EtherCAT 主站{}释放 .", m_index);
    }
}

bool EtherCatMaster::updateMasterInfo() noexcept
{
    if (m_master == nullptr)
    {
        return false;
    }

    // 获取主站的信息
    if (ecrt_master(m_master, &m_info))
    {
        LOG_ERROR("获取主站{}信息失败", m_index);
        return false;
    }
    LOG_TRACE("EtherCAT 主站{} 信息更新.\n从站数量 {}\n网络状态 {}\n", m_index, m_info.slave_count,
              (m_info.link_up ? "在线" : "离线"));
    return true;
}

bool EtherCatMaster::updateSlaveInfoList() noexcept
{
    if (m_master == nullptr)
    {
        LOG_TRACE("主站未初始化");
        return false;
    }
    if (m_info.slave_count == 0)
    {
        return false;
    }
    m_slave_info_list.clear();
    // 获取每个从站的信息
    for (size_t i = 0; i < m_info.slave_count; i++)
    {
        ec_slave_info_t slave_info;
        auto errorCode = ecrt_master_get_slave(m_master, i, &slave_info);
        if (errorCode)
        {
            LOG_ERROR("获取从站{}信息失败,错误码{}", i, errorCode);
            continue;
        }
        m_slave_info_list.push_back(slave_info);
    }

    return true;
}

bool EtherCatMaster::updateSlaveConfigList() noexcept
{
    if (m_master == nullptr)
    {
        LOG_TRACE("主站未初始化");
        return false;
    }

    // 获取从站数量
    size_t slave_count = m_slave_info_list.size();

    if (slave_count == 0)
    {
        LOG_ERROR("没有从站连接到主站{}.", m_index);
        return false;
    }
    m_slave_list.clear();
    // 获取每个从站的信息
    for (size_t i = 0; i < slave_count; i++)
    {
        ec_slave_config_t *slave_config = getSlaveConfig(i);
        if (slave_config == nullptr)
        {
            LOG_ERROR("获取从站{}配置失败.", i);
            return false;
        }
        m_slave_list.emplace_back(CreateServoDriver(*this, i));
        isServoChecked.push_back(1);
    }

    return true;
}

bool EtherCatMaster::createDomain() noexcept
{
    if (m_slave_info_list.empty())
    {
        return false;
    }
    m_input_domain = ecrt_master_create_domain(m_master);
    m_output_domain = ecrt_master_create_domain(m_master);
    if (m_input_domain == nullptr || m_output_domain == nullptr)
    {
        LOG_TRACE("创建域失败");
        return false;
    }
    LOG_TRACE("创建域成功");
    return true;
}
bool EtherCatMaster::slaveConfigPDOs() noexcept
{
    if (m_slave_list.empty())
    {
        LOG_TRACE("没有从站连接到主站{}.", m_index);
        return false;
    }

    for (auto &slave : m_slave_list)
    {
        slave->configPDOs();
    }
    return true;
}

bool EtherCatMaster::registersPdoEntriesforDomain() noexcept
{
    m_input_domain_regs.clear();
    m_output_domain_regs.clear();

    for (auto &slave : m_slave_list)
    {
        std::vector<ec_pdo_entry_reg_t> regs1 = slave->getInputDomainRegs();
        std::vector<ec_pdo_entry_reg_t> regs2 = slave->getOutputDomainRegs();
        for (auto &item : regs1)
        {
            m_input_domain_regs.push_back(item);
        }
        for (auto &item : regs2)
        {
            m_output_domain_regs.push_back(item);
        }
    }
    m_input_domain_regs.push_back(ec_pdo_entry_reg_t{});
    m_output_domain_regs.push_back(ec_pdo_entry_reg_t{});

    if (ecrt_domain_reg_pdo_entry_list(m_output_domain, m_output_domain_regs.data()) != 0)
    {
        LOG_ERROR("Output PDO entry registration failed!");
        return false;
    }
    LOG_TRACE("配置从站 PDO输出 条目成功");
    if (ecrt_domain_reg_pdo_entry_list(m_input_domain, m_input_domain_regs.data()) != 0)
    {
        LOG_ERROR("Input PDO entry registration failed!");
        return false;
    }
    LOG_TRACE("配置从站 PDO输入 条目成功");
    return true;
}

bool EtherCatMaster::slaveConfigSDOs() noexcept
{
    if (m_slave_list.empty())
    {
        return false;
    }
    for (auto &slave : m_slave_list)
    {
        slave->configSDOs();
    }
    LOG_TRACE("从站配置SDO条目成功");
    return true;
}

bool EtherCatMaster::active() noexcept
{
    if (m_master == nullptr)
    {
        return false;
    }
    auto errorCode = ecrt_master_activate(m_master);
    if (errorCode)
    {
        LOG_ERROR("激活主站{}失败: {}", m_index, strerror(errorCode));
        return false;
    }
    m_isActivated = true;

    LOG_TRACE("激活主站{}成功", m_index);
    return true;
};

bool EtherCatMaster::getDomainsProcessData() noexcept
{
    if (m_master == nullptr)
    {
        return false;
    }

    m_input_domain_pd = ecrt_domain_data(m_input_domain);
    if (m_input_domain_pd == nullptr)
    {
        LOG_ERROR("Process input domain failed");
        return false;
    }
    m_output_domain_pd = ecrt_domain_data(m_output_domain);
    if (m_output_domain_pd == nullptr)
    {
        LOG_ERROR("Process output domain failed");
        return false;
    }
    LOG_TRACE("获取域数据指针成功");
    return true;
}

bool EtherCatMaster::deactivate() noexcept
{
    if (m_master == nullptr)
    {
        return false;
    }
    auto errorCode = ecrt_master_deactivate(m_master);
    if (errorCode)
    {
        LOG_ERROR("取消激活主站{}失败: {}", m_index, strerror(errorCode));
        return false;
    }
    m_isActivated = false;
    return true;
};

ec_slave_config_t *EtherCatMaster::getSlaveConfig(const size_t &index)
{
    if (m_master == nullptr)
    {
        return nullptr;
    }
    return ecrt_master_slave_config(m_master, 0, m_slave_info_list[index].position, m_slave_info_list[index].vendor_id,
                                    m_slave_info_list[index].product_code);
}

void EtherCatMaster::checkDomainState()
{
    auto errorCode = ecrt_domain_state(m_input_domain, &m_input_domain_state);
    if (errorCode)
    {
        LOG_ERROR("检测输入域状态失败: {}", strerror(errorCode));
        return;
    }
    errorCode = ecrt_domain_state(m_output_domain, &m_output_domain_state);
    if (errorCode)
    {
        LOG_ERROR("检测输出域状态失败: {}", strerror(errorCode));
        return;
    }
    std::vector<std::string> wcState = {"没有交换任何已注册的过程数据", "一部分已注册的过程数据已交换",
                                        "所有已注册的过程数据都已交换"};
    if (sendNumber % 500 == 0)
        LOG_TRACE("\n输入域状态: {},\n输出域状态: {}", wcState[m_input_domain_state.wc_state],
                  wcState[m_output_domain_state.wc_state]);
}

void EtherCatMaster::checkMasterState()
{
    auto errorCode = ecrt_master_state(m_master, &m_master_state);
    if (errorCode)
    {
        LOG_ERROR("检测主站状态失败: {}", strerror(errorCode));
        return;
    }

    LOG_TRACE("主站状态: {}", alState[m_master_state.al_states]);
}

void EtherCatMaster::checkSlaveState()
{
    for (auto slave : m_slave_list)
    {
        auto slave_state = slave->CheckState();
        if (slave_state == nullptr)
        {
            return;
        }
        if (sendNumber % 500 == 0)
            LOG_TRACE("从站{}状态: {}", slave->getIndex(), alState[slave_state->al_state]);
    }
}

bool EtherCatMaster::enableServo() { enable_flag = true; }

void EtherCatMaster::readMotorData() noexcept
{
    for (auto &slave : m_slave_list)
    {
        slave->readMotorData();
    }
}

void EtherCatMaster::writeMotorData() noexcept
{
    for (auto &slave : m_slave_list)
    {
        slave->writeMotorData();
    }
}

void EtherCatMaster::clearServoAlarm() { clear_alarm_flag = true; }

void EtherCatMaster::run()
{
    if (sendNumber % (reversalInterval / sendInterval) == 0)
    {
        direction = -1 * direction;
    }

    for (auto slave : m_slave_list)
    {
        for (auto i = 0; i < slave->get_axis_number(); ++i)
        {
            slave->setTargetVelocity(i, (slave->getMotorData()[i]->target_speed.load() * direction));
        }
    }

    auto func = [this]() {
        ecrt_master_receive(m_master);
        ecrt_domain_process(m_input_domain);
        ecrt_domain_process(m_output_domain);

        checkDomainState();
        checkSlaveState();
        for (auto &slave : m_slave_list)
        {
            slave->cycle_task();
        }
        ecrt_domain_queue(m_output_domain);
        ecrt_domain_queue(m_input_domain);
        ecrt_master_send(m_master);
        sendNumber++;

        if (sendNumber == std::numeric_limits<size_t>::max())
        {
            sendNumber = 1;
        }
    };
    loop.add(func);
}

void EtherCatMaster::stop()
{
    for (auto slave : m_slave_list)
    {
        for (auto i = 0; i < slave->get_axis_number(); ++i)
        {
            slave->setTargetVelocity(i, 0);
        }
    }

    auto func = [this]() {
        ecrt_master_receive(m_master);
        ecrt_domain_process(m_input_domain);
        ecrt_domain_process(m_output_domain);

        checkDomainState();
        checkSlaveState();
        for (auto &slave : m_slave_list)
        {
            slave->cycle_task();
        }
        ecrt_domain_queue(m_output_domain);
        ecrt_domain_queue(m_input_domain);
        ecrt_master_send(m_master);
        sendNumber++;

        if (sendNumber == std::numeric_limits<size_t>::max())
        {
            sendNumber = 1;
        }
    };
    loop.add(func);
}

uint8_t *&EtherCatMaster::getInputDomainPD() { return m_input_domain_pd; }
uint8_t *&EtherCatMaster::getOutputDomainPD() { return m_output_domain_pd; }

void EtherCatMaster::runWithoutChangeDirection(int32_t velocity)
{
    for (auto slave : m_slave_list)
    {
        for (auto i = 0; i < slave->get_axis_number(); ++i)
        {
            slave->setTargetVelocity(i, slave->getMotorData()[i]->target_speed.load());
        }
    }

    auto func = [this]() {
        ecrt_master_receive(m_master);
        ecrt_domain_process(m_input_domain);
        ecrt_domain_process(m_output_domain);

        checkDomainState();
        checkSlaveState();
        for (auto &slave : m_slave_list)
        {
            slave->cycle_task();
        }
        ecrt_domain_queue(m_output_domain);
        ecrt_domain_queue(m_input_domain);
        ecrt_master_send(m_master);
        sendNumber++;

        if (sendNumber == std::numeric_limits<size_t>::max())
        {
            sendNumber = 1;
        }
    };
    loop.add(func);
}

void EtherCatMaster::taskFlow_motor_run(int32_t velocity)
{

    taskFlowLoop->setDefaultEvent([this, velocity]() { runWithoutChangeDirection(velocity); });
}

void EtherCatMaster::taskFlow_motor_stop()
{
    auto func = [this]() {
        auto func = [this]() {
            ecrt_master_receive(m_master);
            ecrt_domain_process(m_input_domain);
            ecrt_domain_process(m_output_domain);

            checkDomainState();
            checkSlaveState();

            ecrt_domain_queue(m_output_domain);
            ecrt_domain_queue(m_input_domain);
            ecrt_master_send(m_master);
            sendNumber++;

            if (sendNumber == std::numeric_limits<size_t>::max())
            {
                sendNumber = 1;
            }
        };
        loop.add(func);
    };
    taskFlowLoop->setDefaultEvent(func);
}

void EtherCatMaster::taskFlow_start()
{
    taskFlowLoop = new EventLoop(1);

    taskFlowLoop->start();
}

void EtherCatMaster::taskFlow_end()
{
    taskFlowLoop->stop();
    delete taskFlowLoop;
}

void EtherCatMaster::taskFlow_sleep(uint32_t sleepTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}