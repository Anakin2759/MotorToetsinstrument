#include <sqlite3.h>

#include "SQLRW.h"

// 构造函数，初始化数据库并创建表
SQLRW::SQLRW(const std::string &db_name)
{
    db_ = new SQLiteDB(db_name);
    createMotorTable();
    createTemperatureTable();
}
SQLRW::~SQLRW()
{
    delete db_;
}
// 插入 motor_data 表中的数据
void SQLRW::insertMotorData(int slave_id, int motor_id, double speed, double torque, double load_rate,
                            uint64_t timestamp)
{
    std::string sql_motor =
        "INSERT INTO motor_data (slave_id, motor_id, speed, torque, load_rate, timestamp) "
        "VALUES (" +
        std::to_string(slave_id) + ", " + std::to_string(motor_id) + ", " + std::to_string(speed) + ", " +
        std::to_string(torque) + ", " + std::to_string(load_rate) + ", " + std::to_string(timestamp) +
        ");"; // 移除单引号

    db_->execute(sql_motor);
}

// 插入 temperature_data 表中的数据
void SQLRW::insertTemperatureData(int slave_id, int motor_id, double temperature, uint64_t timestamp)
{
    std::string sql_temperature =
        "INSERT INTO temperature_data (slave_id, motor_id, temperature, timestamp) "
        "VALUES (" +
        std::to_string(slave_id) + ", " + std::to_string(motor_id) + ", " + std::to_string(temperature) + ", " +
        std::to_string(timestamp) + ");";
    db_->execute(sql_temperature);
}

// 清空 motor_data 表
void SQLRW::deleteAllMotorData()
{
    std::string sql = "DELETE FROM motor_data;";
    db_->execute(sql);
}

// 清空 temperature_data 表
void SQLRW::deleteAllTemperatureData()
{
    std::string sql = "DELETE FROM temperature_data;";
    db_->execute(sql);
}

// 计算某个属性的最大值、最小值、平均值
std::optional<std::tuple<double, double, double>> SQLRW::getStatistics(const std::string &field, int slave_id,
                                                                       int motor_id)
{
    // 根据属性名确定表名
    std::string table;
    if (field == "speed" || field == "load_rate" || field == "torque")
    {
        table = "motor_data";
    }
    else if (field == "temperature")
    {
        table = "temperature_data";
    }
    else
    {
        // 如果字段名不匹配已知的属性，返回空的 optional
        return std::nullopt;
    }

    // 构建 SQL 查询语句
    std::string sql = "SELECT MAX(" + field + "), MIN(" + field + "), AVG(" + field + ") FROM " + table +
                      " WHERE slave_id = " + std::to_string(slave_id) + " AND motor_id = " + std::to_string(motor_id) +
                      ";";

    std::optional<std::tuple<double, double, double>> result;

    db_->query(sql,
               [&](sqlite3_stmt *stmt)
               {
                   double max = sqlite3_column_double(stmt, 0);
                   double min = sqlite3_column_double(stmt, 1);
                   double avg = sqlite3_column_double(stmt, 2);

                   result = std::make_tuple(max, min, avg);
               });

    return result;
}

// 创建 motor_data 表
void SQLRW::createMotorTable()
{
    std::string sql =
        "CREATE TABLE IF NOT EXISTS motor_data ("
        "slave_id INTEGER NOT NULL, "
        "motor_id INTEGER NOT NULL, "
        "speed REAL, "
        "torque REAL, "
        "load_rate REAL, "
        "timestamp INTEGER NOT NULL, "
        "PRIMARY KEY(slave_id, motor_id, timestamp));";
    db_->execute(sql);
}

// 创建 temperature_data 表
void SQLRW::createTemperatureTable()
{
    std::string sql =
        "CREATE TABLE IF NOT EXISTS temperature_data ("
        "slave_id INTEGER NOT NULL, "
        "motor_id INTEGER NOT NULL, "
        "temperature REAL, "
        "timestamp INTEGER NOT NULL, "
        "PRIMARY KEY(slave_id, motor_id, timestamp));";
    db_->execute(sql);
}
