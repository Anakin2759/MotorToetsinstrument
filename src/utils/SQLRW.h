#ifndef SQLRW_H
#define SQLRW_H
#include <atomic>
#include <chrono>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "Logger.h"
#include "SQLiteDB.h"
#include "ThreadPool.h"
inline std::string getCurrentTimestamp()
{
    // 获取当前时间点
    auto now             = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // 格式化时间为字符串
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
};
class SQLRW
{
public:
    // 构造函数，初始化数据库并创建表
    explicit SQLRW(const std::string &db_name);
    ~SQLRW();
    // 插入 motor_data 表和 temperature_data 表中的数据
    void insertMotorData(int slave_id, int motor_id, double speed, double torque, double load_rate, uint64_t timestamp);
    void insertTemperatureData(int slave_id, int motor_id, double temperature, uint64_t timestamp);

    // 清空 motor_data 表
    void deleteAllData();
    void deleteAllMotorData();

    // 清空 temperature_data 表
    void deleteAllTemperatureData();

    // 计算某个属性的最大值、最小值、平均值
    std::optional<std::tuple<double, double, double>> getStatistics(const std::string &field, int slave_id,
                                                                    int motor_id);

private:
    SQLiteDB *db_;

    // 创建 motor_data 表
    void createMotorTable();

    // 创建 temperature_data 表
    void createTemperatureTable();
};

#endif // SQLRW_H
