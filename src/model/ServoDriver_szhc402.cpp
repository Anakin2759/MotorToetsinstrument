#include <ecrt.h>

#include <cstdint>
#include <vector>

#include "ServoDriver_szhc402.h"
#include "utils/Logger.h"

ServoDriver_szhc402::ServoDriver_szhc402(EtherCatMaster &master, const size_t &index)
    : m_master(master), m_index(index), m_workState(WorkState::INIT), m_axisNumber(2),
      m_slaveConfig(master.getSlaveConfig(index)), vendor_id(master.getSlaveInfoList()[index].vendor_id),
      product_code(master.getSlaveInfoList()[index].product_code), m_AxisList(),
      m_motorData(), m_offsetList{{}, {}}, slave_pdo_entries_rx_1{{0x6040, 0x00, 16}, {0x607A, 0x00, 32},
                                                                  {0x60E0, 0x00, 16}, {0x60E1, 0x00, 16},
                                                                  {0x6060, 0x00, 8},  {0x6098, 0x00, 8}},
      slave_pdo_entries_rx_2{{0x6840, 0x00, 16}, {0x687A, 0x00, 32}, {0x68E0, 0x00, 16},
                             {0x68E1, 0x00, 16}, {0x6860, 0x00, 8},  {0x6898, 0x00, 8}},
      slave_pdo_entries_tx_1{{0x6041, 0x00, 16}, {0x603F, 0x00, 16}, {0x6064, 0x00, 32}, {0x60F4, 0x00, 32},
                             {0x606C, 0x00, 32}, {0x6077, 0x00, 16}, {0x6061, 0x00, 8}},
      slave_pdo_entries_tx_2{{0x6841, 0x00, 16}, {0x683F, 0x00, 16}, {0x6864, 0x00, 32}, {0x68F4, 0x00, 32},
                             {0x686C, 0x00, 32}, {0x6877, 0x00, 16}, {0x6861, 0x00, 8}},
      slave_pdos{{0x1600, (unsigned int)slave_pdo_entries_rx_1.size(), slave_pdo_entries_rx_1.data()},
                 {0x1610, (unsigned int)slave_pdo_entries_rx_2.size(), slave_pdo_entries_rx_2.data()},
                 {0x1a00, (unsigned int)slave_pdo_entries_tx_1.size(), slave_pdo_entries_tx_1.data()},
                 {0x1a10, (unsigned int)slave_pdo_entries_tx_2.size(), slave_pdo_entries_tx_2.data()}},
      syncs_info{{0, EC_DIR_OUTPUT, 0, nullptr, EC_WD_DISABLE},
                 {1, EC_DIR_INPUT, 0, nullptr, EC_WD_DISABLE},
                 {2, EC_DIR_OUTPUT, 2, slave_pdos.data() + 0, EC_WD_ENABLE},
                 {3, EC_DIR_INPUT, 2, slave_pdos.data() + 2, EC_WD_DISABLE},
                 {0xff}}
{
    for (size_t i = 0; i < 2; i++)
    {
        m_AxisList.emplace_back(new Axis(i));
        m_motorData.emplace_back(std::make_unique<MotorData>());
    }
}

std::vector<ec_pdo_entry_reg_t> ServoDriver_szhc402::getInputDomainRegs()
{
    std::vector<ec_pdo_entry_reg_t> regs;
    uint16_t position = m_index;

    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Status_Word1), 0, &m_offsetList[0].status_word,
                                      nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Position_Actual_Value_Abs1), 0,
                                      &m_offsetList[0].actual_position, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Error_Code1), 0, &m_offsetList[0].errcode,
                                      nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Torque_Actual_Value1), 0,
                                      &m_offsetList[0].actual_torque, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Mode_Of_Operation_Display1), 0,
                                      &m_offsetList[0].modes_of_operation_display, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Velocity_Actual_Value1), 0,
                                      &m_offsetList[0].actual_velocity, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Following_Error_Actual_Value1), 0,
                                      &m_offsetList[0].following_error_actual_value, nullptr});

    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Status_Word2), 0, &m_offsetList[1].status_word,
                                      nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Position_Actual_Value_Abs2), 0,
                                      &m_offsetList[1].actual_position, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Error_Code2), 0, &m_offsetList[1].errcode,
                                      nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Torque_Actual_Value2), 0,
                                      &m_offsetList[1].actual_torque, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Mode_Of_Operation_Display2), 0,
                                      &m_offsetList[1].modes_of_operation_display, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Velocity_Actual_Value2), 0,
                                      &m_offsetList[1].actual_velocity, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Following_Error_Actual_Value2), 0,
                                      &m_offsetList[1].following_error_actual_value, nullptr});
    return regs;
}

std::vector<ec_pdo_entry_reg_t> ServoDriver_szhc402::getOutputDomainRegs()
{
    std::vector<ec_pdo_entry_reg_t> regs;
    uint16_t position = m_index;

    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Control_Word1), 0,
                                      &m_offsetList[0].control_word, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Target_Position1), 0,
                                      &m_offsetList[0].target_position, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Mode_Of_Operation1), 0,
                                      &m_offsetList[0].modes_of_operation, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Positive_Torque_Limit1), 0,
                                      &m_offsetList[0].positive_torque_limit_value, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Negative_Torque_Limit1), 0,
                                      &m_offsetList[0].negative_torque_limit_value, nullptr});

    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Control_Word2), 0,
                                      &m_offsetList[1].control_word, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Target_Position2), 0,
                                      &m_offsetList[1].target_position, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Mode_Of_Operation2), 0,
                                      &m_offsetList[1].modes_of_operation, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Positive_Torque_Limit2), 0,
                                      &m_offsetList[1].positive_torque_limit_value, nullptr});
    regs.push_back(ec_pdo_entry_reg_t{0, position, vendor_id, product_code,
                                      (uint16_t)((uint16_t)ServoAddress::Negative_Torque_Limit2), 0,
                                      &m_offsetList[1].negative_torque_limit_value, nullptr});
    return regs;
}

bool ServoDriver_szhc402::configPDOs()
{
    if (m_slaveConfig == nullptr)
    {
        return false;
    }

    if (ecrt_slave_config_pdos(m_slaveConfig, EC_END, syncs_info.data()) != 0)
    {
        LOG_ERROR("从站{}配置pdo失败", m_index);
        return false;
    }

    LOG_TRACE("从站{}配置pdo成功", m_index);
    return true;
}

void ServoDriver_szhc402::configSDOs()
{
    if (product_code == 0x402 && vendor_id == 0x92b)
    {
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2e00, 0x04, 1);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2f00, 0x04, 1);

        ecrt_slave_config_sdo32(m_slaveConfig, 0x2e00, 0x2e, 10000);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2e00, 0x2f, 0);

        ecrt_slave_config_sdo32(m_slaveConfig, 0x2f00, 0x2e, 10000);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2f00, 0x2f, 0);

        ecrt_slave_config_sdo32(m_slaveConfig, 0x2e00, 0x33, 4);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2f00, 0x33, 4);

        ecrt_slave_config_sdo32(m_slaveConfig, 0x2e00, 0x5f, 4661);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2f00, 0x5f, 4661);

        ecrt_slave_config_sdo32(m_slaveConfig, 0x2e00, 0x2a, 100);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2e00, 0x2b, 100);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2f00, 0x2a, 100);
        ecrt_slave_config_sdo32(m_slaveConfig, 0x2f00, 0x2b, 100);
    }
}

void ServoDriver_szhc402::readMotorData()
{
    auto &input_domain_pd = m_master.getInputDomainPD();
    for (size_t i = 0; i < 2; ++i)
    {
        m_motorData[i]->position = EC_READ_S32(input_domain_pd + m_offsetList[i].actual_position);
        m_motorData[i]->velocity = EC_READ_S32(input_domain_pd + m_offsetList[i].actual_velocity);
        m_motorData[i]->torque = EC_READ_S16(input_domain_pd + m_offsetList[i].actual_torque);
        m_motorData[i]->status_word = EC_READ_U16(input_domain_pd + m_offsetList[i].status_word);
        m_motorData[i]->error_code = EC_READ_U16(input_domain_pd + m_offsetList[i].errcode);

        m_motorData[i]->load_rate = m_AxisList[i]->getLoadRate(m_motorData[i]->torque);
    }
}

void ServoDriver_szhc402::writeMotorData()
{
    auto &out_domain_pd = m_master.getOutputDomainPD();
    for (size_t i = 0; i < 2; i++)
    {
        EC_WRITE_U16(out_domain_pd + m_offsetList[i].control_word, m_motorData[i]->control_word.load());
        EC_WRITE_S32(out_domain_pd + m_offsetList[i].target_position, m_motorData[i]->target_position.load());
    }
}

ec_slave_config_state_t *ServoDriver_szhc402::CheckState()
{
    if (auto errorCode = ecrt_slave_config_state(m_slaveConfig, &m_state))
    {
        LOG_ERROR("从站{}状态检测失败: {}", m_index, errorCode);
        return nullptr;
    }
    switch (m_state.al_state)
    {
    case 1:
        m_workState = WorkState::INIT;
        break;
    case 2:
        m_workState = WorkState::PREOP;
        break;

    case 4:
        m_workState = WorkState::SAFEOP;
        break;
    case 8:
        m_workState = WorkState::OP;
        break;
    default:
        m_workState = WorkState::INIT;
        break;
    }
    m_motorData[0]->online = m_state.online;
    m_motorData[1]->online = m_state.online;
    return &m_state;
}

void ServoDriver_szhc402::cycle_task()
{
    auto &sendNumber = m_master.getSendNumber();
    readMotorData();
    for (size_t i = 0; i < 2; i++)
    {
        if (m_master.get_enable_flag().load() == true)
        {
            switch (sys_working_statatus)
            {
            case SysWorkingStatus::POWERONMODE:
                handlePowerOnMode(i);
                break;
            case SysWorkingStatus::SAFEMODE:
                handleSafeMode(i);
                break;
            case SysWorkingStatus::OPMODE:
                handleOpMode(i);
                break;
            case SysWorkingStatus::RUNMODE:
                handleRunMode(i);
                break;

            case SysWorkingStatus::LINKDOWNMODE:
                break;
            default:
                LOG_TRACE("未知状态");
                break;
            }
        }
        else
        {
            m_motorData[i]->control_word = 0;
        }
    }
    writeMotorData();
}
void ServoDriver_szhc402::handleLinkDownMode(size_t i)
{
    auto &SendNumber = m_master.getSendNumber();
    if (SendNumber > 100)
    {
        if (m_state.online == 0)
        {
            if (SendNumber % 500 == 0)
                LOG_ERROR("主站连接断开");
            sys_working_statatus = SysWorkingStatus::LINKDOWNMODE;
        }
        else
        {
            sys_working_statatus = SysWorkingStatus::POWERONMODE;
        }
    }
}

void ServoDriver_szhc402::handlePowerOnMode(size_t i)
{
    auto &SendNumber = m_master.getSendNumber();
    if (SendNumber > 300)
    {
        if (m_state.online == 0)
        {
            if (SendNumber % 500 == 0)
                LOG_ERROR("从站连接断开");
            sys_working_statatus = SysWorkingStatus::LINKDOWNMODE;
        }
        else
        {
            sys_working_statatus = SysWorkingStatus::SAFEMODE;
        }
    }
}

void ServoDriver_szhc402::handleSafeMode(size_t i)
{
    auto &sendNumber = m_master.getSendNumber();
    if (sendNumber % 500 == 0)
    {
        LOG_TRACE("安全模式");
    }
    if (sendNumber > 200)

    {
        //     if (m_motorData[i]->status_word & 0x0008 == 1)
        //     {
        //         m_motorData[i]->control_word = 0x80;

        //         return;
        //     }
        // else
        if (m_state.al_state == 8)
        {
            sys_working_statatus = SysWorkingStatus::OPMODE;
        }
    }
}
void ServoDriver_szhc402::handleOpMode(size_t i)
{
    auto &sendNumber = m_master.getSendNumber();
    switch ((m_motorData[i]->status_word.load() & 0x006f))
    {
    case 0x0000:
    case 0x0020:
        if (sendNumber % 500 == 0)
            LOG_TRACE("初始化未完成");
        break;
    case 0x0040:
    case 0x0060:
        m_motorData[i]->control_word = 0x06;
        if (sendNumber % 500 == 0)
            LOG_TRACE("初始化完成，切换至主电路off模式");
        break;
    case 0x0021:

        m_motorData[i]->control_word = 0x07;
        if (sendNumber % 500 == 0)
            LOG_TRACE("主电路off模式切换至伺服准备模式");
        break;
    case 0x0023:
        m_motorData[i]->control_word = 0x0F;
        if (sendNumber % 500 == 0)
            LOG_TRACE("伺服准备模式切换至伺服使能模式");
        break;
    case 0x0027:
        if (sendNumber % 500 == 0)
        {
            LOG_TRACE("伺服使能状态 发送次数{}", sendNumber);
        }
        sys_working_statatus = SysWorkingStatus::RUNMODE;

        break;
    case 0x0007:

        LOG_TRACE("即停止");
        break;
    case 0x000F:
    case 0x002F:

        LOG_TRACE("异常判断");
        break;
    case 0x0008:
    case 0x0028:
        if (sendNumber % 500 == 0)
        {
            LOG_TRACE("异常/报警状态");
            LOG_TRACE("错误码:" + std::to_string(m_motorData[i]->error_code.load()));
        }
        m_motorData[i]->control_word = 0x80;
        sys_working_statatus = SysWorkingStatus::POWERONMODE;
        break;
    default:
        if (sendNumber % 500 == 0)
            LOG_TRACE("未知状态:{}", m_motorData[i]->status_word.load());
        break;
    }
}

void ServoDriver_szhc402::handleRunMode(size_t i)
{
    if (m_motorData[i]->status_word & 0x0008)
    {
        sys_working_statatus = SysWorkingStatus::SAFEMODE;
        return;
    }

    auto &sendNumber = m_master.getSendNumber();

    if (sendNumber % 500 == 0)
    {
        LOG_TRACE("运行模式");
    }
    int32_t velocity = m_AxisList[i]->getCalculatedVelocity();

    m_motorData[i]->target_position = m_motorData[i]->position.load() + velocity;
    auto out_domain_pd = m_master.getOutputDomainPD();
    EC_WRITE_S32(out_domain_pd + m_offsetList[i].target_position, m_motorData[i]->target_position.load());
}