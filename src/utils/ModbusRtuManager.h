#pragma once

#include <modbus/modbus-rtu.h>

#include <mutex>
#include <string>
#include <vector>

class ModbusRTUManager
{
public:
    ModbusRTUManager(const std::string &device, int baud, char parity, int data_bit, int stop_bit);
    ~ModbusRTUManager();
    inline void setSlaveId(int slaveId)
    {
        modbus_set_slave(m_modbusContext, slaveId);
    };
    bool connect();
    void disconnect();

    bool isConnected() const;

    // Modbus function codes
    bool readHoldingRegisters_03(int start_addr, int number_of_registers, std::vector<uint16_t> &dest);
    bool readInputRegisters_04(int start_addr, int number_of_registers, std::vector<uint16_t> &dest);
    bool writeSingleRegister_06(int reg_addr, uint16_t value);
    bool writeMultipleRegisters_10(int start_addr, const std::vector<uint16_t> &values);
    bool readCoils_01(int start_addr, int number_of_coils, std::vector<uint8_t> &dest);
    bool readDiscreteInputs_02(int start_addr, int number_of_inputs, std::vector<uint8_t> &dest);
    bool writeSingleCoil_05(int coil_addr, bool status);
    bool writeMultipleCoils_0f(int start_addr, const std::vector<uint8_t> &values);

    void setTimeout(uint32_t sec, uint32_t usec);

private:
    modbus_t *m_modbusContext = nullptr;
    std::string m_device;
    int m_baud;
    char m_parity;
    int m_data_bit;
    int m_stop_bit;
    bool m_isConnected = false;

    mutable std::mutex m_mutex; // 保护线程安全
};
