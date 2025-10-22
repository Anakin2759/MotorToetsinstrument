#pragma once
#include <ecrt.h>

#include <cstddef>
#include <vector>

#include "Axis.h"
#include "EtherCatMaster.h"
#include "interface/IServoDriver.h"
enum class SysWorkingStatus
{
    SAFEMODE,
    OPMODE,
    RUNMODE,
    POWERONMODE,
    LINKDOWNMODE
};
constexpr int Scalefactor = 22;
enum class ServoAddress
{
    Error_Code1 = 0x603F, // 错误码（重复，即一个错误码对应多个报警名称，不建议使用） UNSIGNED16  RO  Y
    Control_Word1 = 0x6040, // 控制字 UNSIGNED16 RW Y
    Status_Word1  = 0x6041, // 状态字 UNSIGNED16 RO  Y

    Mode_Of_Operation1 = 0x6060, // 运行模式 INTEGER8 RW  Y   8:周期性同步位置模式； 9：周期性同步速度模式；
    Mode_Of_Operation_Display1 = 0x6061, // 运行模式读取 INTEGER8 RO  Y

    Position_Actual_Value_Increment1 = 0x6063, // 位置反馈增量值 INTEGER32 RO  Y
    Position_Actual_Value_Abs1       = 0x6064, // 绝对位置反馈 INTEGER32 RO  Y
    Velocity_Actual_Value1           = 0x606C, // 实际速度 INTEGER32 RO  Y 单位：r/min 精度：0
    Target_Torque1                   = 0x6071, // 目标转矩  INTEGER16 RW Y 单位：0.1%额定转矩；P4.14
    Max_Torque_Limit1                = 0x6072, // 最大转矩限制  UNSIGNED16 RW Y
    Torque_Actual_Value1             = 0x6077, // 实际转矩 INTEGER16 RO  Y 精度：0.1%； 单位：额定转矩
    Current_Actual_Value1            = 0x6078, // 实际输出电流 INTEGER16 RO Y 精度：0.1； 单位：mA；
    DC_Link_Circuit_Voltage1         = 0x6079, // 母线电压 UNSIGNED32 RO N
    Target_Position1                 = 0x607A, // 目标位置 INTEGER32 RW  Y
    Max_Profile_Velocity1            = 0x607F, // 最大转速限制  UNSIGNED32 RW  Y
    HOMING_MODE1                     = 0X6098, // 回原点模式 USINT8 RW Y
    Positive_Torque_Limit1           = 0x60E0, // 正向力矩限幅 UNSIGNED16 RW  Y
    Negative_Torque_Limit1           = 0x60E1, // 反向力矩限幅 UNSIGNED16 RW  Y
    Following_Error_Actual_Value1    = 0x60F4, // 位置指令偏差 INTEGER32 RO  Y
    Target_Velocity1                 = 0x60FF, // 目标速度 INTEGER32 RW Y （P4.13总线速度指令）

    // 轴2的PDO 0x68××
    Error_Code2 = 0x683F, // 错误码（重复，即一个错误码对应多个报警名称，不建议使用） UNSIGNED16  RO  Y
    Control_Word2 = 0x6840, // 控制字 UNSIGNED16 RW Y
    Status_Word2  = 0x6841, // 状态字 UNSIGNED16 RO  Y

    Mode_Of_Operation2 = 0x6860, // 运行模式 INTEGER8 RW  Y   8:周期性同步位置模式； 9：周期性同步速度模式；
    Mode_Of_Operation_Display2 = 0x6861, // 运行模式读取 INTEGER8 RO  Y

    Position_Actual_Value_Increment2 = 0x6863, // 位置反馈增量值 INTEGER32 RO  Y
    Position_Actual_Value_Abs2       = 0x6864, // 绝对位置反馈 INTEGER32 RO  Y
    Velocity_Actual_Value2           = 0x686C, // 实际速度 INTEGER32 RO  Y 单位：r/min 精度：0
    Target_Torque2                   = 0x6871, // 目标转矩  INTEGER16 RW Y 单位：0.1%额定转矩；P4.14
    Max_Torque_Limit2                = 0x6872, // 最大转矩限制  UNSIGNED16 RW Y
    Torque_Actual_Value2             = 0x6877, // 实际转矩 INTEGER16 RO  Y 精度：0.1%； 单位：额定转矩
    Current_Actual_Value2            = 0x6878, // 实际输出电流 INTEGER16 RO Y 精度：0.1； 单位：mA；
    DC_Link_Circuit_Voltage2         = 0x6879, // 母线电压 UNSIGNED32 RO N
    Target_Position2                 = 0x687A, // 目标位置 INTEGER32 RW  Y
    Max_Profile_Velocity2            = 0x687F, // 最大转速限制  UNSIGNED32 RW  Y
    HOMING_MODE2                     = 0X6898, // 回原点模式 USINT8 RW Y
    Positive_Torque_Limit2           = 0x68E0, // 正向力矩限幅 UNSIGNED16 RW  Y
    Negative_Torque_Limit2           = 0x68E1, // 反向力矩限幅 UNSIGNED16 RW  Y
    Following_Error_Actual_Value2    = 0x68F4, // 位置指令偏差 INTEGER32 RO  Y

    Target_Velocity2 = 0x68FF, // 目标速度 INTEGER32 RW Y （P4.13总线速度指令）
};

union ServoControlWord
{
    uint16_t value;
    struct
    {
        uint16_t switchOn : 1;
        uint16_t enableVoltage : 1;
        uint16_t quickStop : 1;
        uint16_t enableOperation : 1;
        uint16_t operationModeSpecific : 3;
        uint16_t faultReset : 1;
        uint16_t halt : 1;
        uint16_t operationModeSpecific2 : 1;
        uint16_t reserved : 6;
    } bit;
};

union ServoStatusWord
{
    uint16_t value;
    struct
    {
        uint16_t readyToSwitchOn : 1;
        uint16_t switchedOn : 1;
        uint16_t operationEnable : 1;
        uint16_t fault : 1;
        uint16_t voltageEnabled : 1;
        uint16_t quickStop : 1;
        uint16_t switchOnDisabled : 1;
        uint16_t warning : 1;
        uint16_t reserved1 : 1;
        uint16_t remote : 1;
        uint16_t operationModeSpecific : 1;
        uint16_t internalLimitActive : 1;
        uint16_t operationModeSpecific2 : 2;
        uint16_t reserved2 : 2;
    } bit;
};

struct Offset
{
    unsigned int control_word                = 0; /*控制字*/
    unsigned int target_position             = 0; /*目标位置*/
    unsigned int modes_of_operation          = 0; /*模式*/
    unsigned int positive_torque_limit_value = 0;
    unsigned int negative_torque_limit_value = 0;

    unsigned int status_word                  = 0; /*状态字*/
    unsigned int errcode                      = 0; /*错误码*/
    unsigned int actual_velocity              = 0; /*实际速度*/
    unsigned int actual_position              = 0; /*实际位置*/
    unsigned int actual_torque                = 0; /*实际力矩*/
    unsigned int modes_of_operation_display   = 0; /*运行模式显示*/
    unsigned int following_error_actual_value = 0;
};

class ServoDriver_szhc402 : public IServoDriver
{
public:
    ServoDriver_szhc402(EtherCatMaster& master, const size_t& index);
    ~ServoDriver_szhc402();

    inline const size_t& get_axis_number() override
    {
        return m_axisNumber;
    };

    inline const uint32_t& getVendorId() const override
    {
        return vendor_id;
    }
    inline const uint32_t& getProductCode() const override
    {
        return product_code;
    }
    inline const size_t& getIndex() const override
    {
        return m_index;
    }
    inline const std::vector<std::unique_ptr<MotorData>>& getMotorData() const override
    {
        return m_motorData;
    }
    const WorkState& getWorkState() const override
    {
        return m_workState;
    }
    void setTargetVelocity(size_t index, const int32_t& velocity) override
    {
        m_AxisList[index]->setTargetSpeed(velocity * Scalefactor);
    }

    std::vector<ec_pdo_entry_reg_t> getInputDomainRegs() override;
    std::vector<ec_pdo_entry_reg_t> getOutputDomainRegs() override;
    ec_slave_config_state_t* CheckState() override;

    bool configPDOs() override;
    void configSDOs() override;

    void readMotorData() override;
    void writeMotorData() override;

    void cycle_task() override;
    std::vector<std::unordered_map<std::string, std::string>>& getSnData() override
    {
        return snData;
    };

private:
    void handlePowerOnMode(size_t i);
    void handleOpMode(size_t i);
    void handleRunMode(size_t i);
    void handleSafeMode(size_t i);
    void handleLinkDownMode(size_t i);
    EtherCatMaster& m_master;

    size_t m_axisNumber;
    ec_slave_config_t* m_slaveConfig;
    size_t m_index;

    ec_slave_config_t* m_slaveConfig1;
    std::vector<Axis*> m_AxisList;
    ec_slave_config_state_t m_state{};

    ServoStatusWord m_statusWord1;
    ServoControlWord m_controlWord1;
    ServoStatusWord m_statusWord2;
    ServoControlWord m_controlWord2;

    uint32_t vendor_id;
    uint32_t product_code;
    std::vector<Offset> m_offsetList;

    std::vector<ec_pdo_entry_info_t> slave_pdo_entries_rx_1;
    std::vector<ec_pdo_entry_info_t> slave_pdo_entries_rx_2;
    std::vector<ec_pdo_entry_info_t> slave_pdo_entries_tx_1;
    std::vector<ec_pdo_entry_info_t> slave_pdo_entries_tx_2;
    std::vector<ec_pdo_info_t> slave_pdos;
    std::vector<ec_sync_info_t> syncs_info;

    WorkState m_workState;
    std::vector<std::unique_ptr<MotorData>> m_motorData;
    bool isConnected{true};
    SysWorkingStatus sys_working_statatus = SysWorkingStatus::SAFEMODE;
    std::vector<std::unordered_map<std::string, std::string>> snData;
};
