#pragma once

#include <ecrt.h>

#include <cstring>
#include <string>
#include <type_traits> // for std::is_same_v
#include <vector>

#include "utils/Logger.h"

class SdoHandler
{
public:
    SdoHandler()
    {
    }

    // 模板操作符
    template <typename T>
    bool operator()(ec_master_t *master, uint16_t slaveIndex, uint32_t index, uint8_t subIndex, T &value,
                    bool isUpload);

private:
    ec_master_t *master;
    // 模板的上传和下载函数
    template <typename T>
    bool Upload(uint16_t slaveIndex, uint32_t index, uint8_t subIndex, T &value);

    template <typename T>
    bool Download(uint16_t slaveIndex, uint32_t index, uint8_t subIndex, T &value);
};

template <typename T>
bool SdoHandler::operator()(ec_master_t *master, uint16_t slaveIndex, uint32_t index, uint8_t subIndex, T &value,
                            bool isUpload)
{
    if (master == nullptr)
    {
        return false;
    }
    this->master = master;
    if (isUpload)
    {
        return Upload(slaveIndex, index, subIndex, value);
    }
    else
    {
        return Download(slaveIndex, index, subIndex, value);
    }
}

// 模板的上传函数实现
template <typename T>
bool SdoHandler::Upload(uint16_t slaveIndex, uint32_t index, uint8_t subIndex, T &value)
{
    uint32_t abort_code;

    if constexpr (std::is_same_v<T, std::string>)
    {
        uint8_t *target = new uint8_t[256]; // 为字符串分配缓冲区
        size_t result_size;

        if (ecrt_master_sdo_upload(master, slaveIndex, index, subIndex, target, 256, &result_size, &abort_code) < 0)
        {
            LOG_ERROR("sdo upload fault");
            delete[] target;
            return false;
        }

        value.assign(reinterpret_cast<char *>(target), result_size); // 将数据转换为字符串
        delete[] target;
    }
    else
    {
        uint8_t *target = new uint8_t[sizeof(T)];
        size_t result_size;

        if (ecrt_master_sdo_upload(master, slaveIndex, index, subIndex, target, sizeof(T), &result_size, &abort_code) <
            0)
        {
            LOG_ERROR("sdo upload fault");
            delete[] target;
            return false;
        }

        if (result_size > sizeof(T))
        {
            LOG_ERROR("上传的数据大小超过了目标类型的大小");
            delete[] target;
            return false;
        }

        std::memcpy(&value, target, result_size); // 将数据复制到目标类型
        delete[] target;
    }

    return true;
}

// 模板的下载函数实现
template <typename T>
bool SdoHandler::Download(uint16_t slaveIndex, uint32_t index, uint8_t subIndex, T &value)
{
    uint32_t abort_code;

    if constexpr (std::is_same_v<T, std::string>)
    {
        size_t data_size = value.size();
        std::vector<uint8_t> data_buffer(data_size);

        std::memcpy(data_buffer.data(), value.data(), data_size);

        if (ecrt_master_sdo_download(master, slaveIndex, index, subIndex, data_buffer.data(), data_size, &abort_code) <
            0)
        {
            LOG_ERROR("sdo download fault");
            return false;
        }
    }
    else
    {
        if (ecrt_master_sdo_download(master, slaveIndex, index, subIndex, reinterpret_cast<uint8_t *>(&value),
                                     sizeof(T), &abort_code) < 0)
        {
            LOG_ERROR("sdo download fault");
            return false;
        }
    }

    return true;
}
