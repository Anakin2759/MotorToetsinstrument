#include <functional>
#include <memory>
#include <sqlite3.h>
#include <stdexcept>
#include <string>

class SQLiteDB
{
public:
    // 构造函数
    explicit SQLiteDB(const std::string &db_name)
    {

        // 使用 SQLITE_OPEN_CREATE 选项，如果文件不存在则创建它
        if (sqlite3_open_v2(db_name.c_str(), &db_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK)
        {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));
        }
    }

    // 析构函数
    ~SQLiteDB()
    {
        if (db_)
        {
            sqlite3_close(db_);
        }
    }

    // 执行不带返回值的SQL语句
    void execute(const std::string &sql)
    {
        char *errmsg = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK)
        {
            std::string error_msg = "SQL error: ";
            error_msg += errmsg;
            sqlite3_free(errmsg);
            throw std::runtime_error(error_msg);
        }
    }

    // 执行带返回值的SQL查询
    template <typename Callback> void query(const std::string &sql, Callback callback)
    {
        sqlite3_stmt *stmt = prepareStatement(sql);

        int rc;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            callback(stmt);
        }

        if (rc != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to execute query: " + std::string(sqlite3_errmsg(db_)));
        }

        sqlite3_finalize(stmt);
    }

private:
    // 预处理 SQL 语句
    sqlite3_stmt *prepareStatement(const std::string &sql)
    {
        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db_)));
        }
        return stmt;
    }

    sqlite3 *db_ = nullptr;
};
