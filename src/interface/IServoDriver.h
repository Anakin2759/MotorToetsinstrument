#pragma once
#include <ecrt.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
enum class WorkState
{
    INIT,
    PREOP,
    SAFEOP,
    OP,
    LINK_DOWN
};

struct MotorData
{
    std::atomic<uint16_t> control_word                = 0;
    std::atomic<int32_t> position                     = 0;
    std::atomic<int32_t> velocity                     = 0;
    std::atomic<uint16_t> status_word                 = 0;
    std::atomic<uint8_t> mode                         = 0;
    std::atomic<int16_t> torque                       = 0;
    std::atomic<uint16_t> positive_torque_limit_value = 0;
    std::atomic<uint16_t> negative_torque_limit_value = 0;
    std::atomic<double> load_rate                     = 0;
    std::atomic<uint16_t> error_code                  = 0;
    std::atomic<double> temperature                   = 0;
    std::atomic<bool> online                          = false;
    std::atomic<int32_t> target_position              = 0;
    std::atomic<int32_t> target_speed                 = 0;

    std::string sn = "000";

    std::string vendor     = "未知";
    std::string power      = "0W";
    std::string motor_type = "未知";
    std::string firm       = "未知";
};
struct dev_info_t
{
    uint32_t vendor_id;
    uint32_t product_code;
};

class IServoDriver
{
public:
    virtual const size_t& get_axis_number()                       = 0;
    virtual const uint32_t& getVendorId() const                   = 0;
    virtual const uint32_t& getProductCode() const                = 0;
    virtual const size_t& getIndex() const                        = 0;
    virtual std::vector<ec_pdo_entry_reg_t> getInputDomainRegs()  = 0;
    virtual std::vector<ec_pdo_entry_reg_t> getOutputDomainRegs() = 0;
    virtual bool configPDOs()                                     = 0;
    virtual void configSDOs()                                     = 0;
    virtual ec_slave_config_state_t* CheckState()                 = 0;
    virtual void readMotorData()                                  = 0;
    virtual void writeMotorData()                                 = 0;

    virtual const std::vector<std::unique_ptr<MotorData>>& getMotorData() const = 0;

    virtual const WorkState& getWorkState() const                                  = 0;
    virtual void setTargetVelocity(size_t index, const int32_t& velocity)          = 0;
    virtual void cycle_task()                                                      = 0;
    virtual std::vector<std::unordered_map<std::string, std::string>>& getSnData() = 0;
};
