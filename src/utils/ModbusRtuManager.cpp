#include "Logger.h"
#include "ModbusRtuManager.h"
/**
 * @brief Construct a new ModbusRTUManager object.
 *
 * @param device The serial device to use.
 * @param baud The baud rate to use.
 * @param parity The parity to use.
 * @param data_bit The number of data bits to use.
 * @param stop_bit The number of stop bits to use.
 * @param slaveId The slave ID to use.
 *
 * @throws None.
 */
ModbusRTUManager::ModbusRTUManager(const std::string &device, int baud, char parity, int data_bit, int stop_bit)
    : m_device(device), m_baud(baud), m_parity(parity), m_data_bit(data_bit), m_stop_bit(stop_bit)

{
}

ModbusRTUManager::~ModbusRTUManager()
{
    disconnect();
}

/**
 * @brief Connect to the ModbusRTU device.
 *
 * This function tries to connect to the ModbusRTU device specified by the
 * `m_device`, `m_baud`, `m_parity`, `m_data_bit`, and `m_stop_bit`
 * parameters. If the connection is successful, the function sets the
 * `m_isConnected` flag to true and returns true. If there is an error during
 * the connection, the function throws a `std::runtime_error` exception with
 * an appropriate error message.
 *
 * @return True if the connection is successful.
 * @return False if there was an error during the connection.
 * @throws std::runtime_error If the libmodbus context could not be created or
 * the connection failed.
 */
bool ModbusRTUManager::connect()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // 创建 Modbus context
    m_modbusContext = modbus_new_rtu(m_device.c_str(), m_baud, m_parity, m_data_bit, m_stop_bit);
    if (m_modbusContext == nullptr)
    {
        LOG_ERROR("Unable to create the libmodbus context");
        return false;
    }

    // 尝试连接
    if (modbus_connect(m_modbusContext) == -1)
    {
        LOG_ERROR("Connection failed: {}", modbus_strerror(errno));
        modbus_free(m_modbusContext);
        m_modbusContext = nullptr;
        return false;
    }
    m_isConnected = true;
    LOG_INFO("Modbus connection established on port: {}", m_device);
    return true;
}

/**
 * @brief Disconnects from the ModbusRTU device.
 *
 * This function disconnects from the ModbusRTU device if it is currently
 * connected. If the device is not connected, the function does nothing.
 *
 * @throws None.
 */
void ModbusRTUManager::disconnect()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // 先检查是否有上下文
    if (m_modbusContext != nullptr)
    {
        if (m_isConnected)
        {
            // 断开连接
            modbus_close(m_modbusContext);
            m_isConnected = false;
        }
        // 释放上下文
        modbus_free(m_modbusContext);
        m_modbusContext = nullptr;
        LOG_TRACE("Disconnected from ModbusRTU device");
    }
}

bool ModbusRTUManager::isConnected() const
{
    return m_isConnected;
}

bool ModbusRTUManager::readHoldingRegisters_03(int start_addr, int number_of_registers, std::vector<uint16_t> &dest)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    dest.resize(number_of_registers);
    int rc = modbus_read_registers(m_modbusContext, start_addr, number_of_registers, dest.data());
    if (rc == -1)
    {
        LOG_ERROR("Read holding registers failed: {}", modbus_strerror(errno));
        return false;
    }
    return true;
}

bool ModbusRTUManager::readInputRegisters_04(int start_addr, int number_of_registers, std::vector<uint16_t> &dest)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    dest.resize(number_of_registers);
    int rc = modbus_read_input_registers(m_modbusContext, start_addr, number_of_registers, dest.data());
    if (rc == -1)
    {
        LOG_ERROR("Read input registers failed:{} ", modbus_strerror(errno));
        return false;
    }
    return true;
}

bool ModbusRTUManager::writeSingleRegister_06(int reg_addr, uint16_t value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    int rc = modbus_write_register(m_modbusContext, reg_addr, value);
    if (rc == -1)
    {
        LOG_ERROR("Write single register failed: {}", modbus_strerror(errno));
        return false;
    }
    return true;
}

bool ModbusRTUManager::writeMultipleRegisters_10(int start_addr, const std::vector<uint16_t> &values)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    int rc = modbus_write_registers(m_modbusContext, start_addr, values.size(), values.data());
    if (rc == -1)
    {
        LOG_ERROR("Write multiple registers failed: {}", modbus_strerror(errno));
        return false;
    }
    return true;
}

bool ModbusRTUManager::readCoils_01(int start_addr, int number_of_coils, std::vector<uint8_t> &dest)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    dest.resize(number_of_coils);
    int rc = modbus_read_bits(m_modbusContext, start_addr, number_of_coils, dest.data());
    if (rc == -1)
    {
        LOG_ERROR("Read coils failed: {}", modbus_strerror(errno));
        return false;
    }
    return true;
}

bool ModbusRTUManager::readDiscreteInputs_02(int start_addr, int number_of_inputs, std::vector<uint8_t> &dest)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    dest.resize(number_of_inputs);
    int rc = modbus_read_input_bits(m_modbusContext, start_addr, number_of_inputs, dest.data());
    if (rc == -1)
    {
        LOG_ERROR("Read discrete inputs failed: {}", modbus_strerror(errno));
        return false;
    }
    return true;
}

/**
 * @brief Write a single coil value to the ModbusRTU device.
 *
 * @param coil_addr The address of the coil to write to.
 * @param status The new status of the coil. True for on, false for off.
 * @return True if the write was successful.
 * @return False if there was an error.
 */
bool ModbusRTUManager::writeSingleCoil_05(int coil_addr, bool status)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    int rc = modbus_write_bit(m_modbusContext, coil_addr, status ? 1 : 0);
    if (rc == -1)
    {
        LOG_ERROR("Write single coil failed: {}", modbus_strerror(errno));
        return false;
    }
    return true;
}

/**
 * @brief Write multiple coil values to the ModbusRTU device.
 *
 * @param start_addr The address of the first coil to write to.
 * @param values The vector of coil values to write.
 * @return True if the write was successful.
 * @return False if there was an error.
 */
bool ModbusRTUManager::writeMultipleCoils_0f(int start_addr, const std::vector<uint8_t> &values)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isConnected() || m_modbusContext == nullptr)
        return false;

    int rc = modbus_write_bits(m_modbusContext, start_addr, values.size(), values.data());
    if (rc == -1)
    {
        LOG_ERROR("Write multiple coils failed: {}", modbus_strerror(errno));
        return false;
    }
    return true;
}

void ModbusRTUManager::setTimeout(uint32_t sec, uint32_t usec)
{
    modbus_set_byte_timeout(m_modbusContext, sec, usec);
    modbus_set_response_timeout(m_modbusContext, sec, usec);
}
