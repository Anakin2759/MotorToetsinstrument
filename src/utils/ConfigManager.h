#pragma once

#include <QSettings>
#include <QString>
#include <iostream>
#include <map>
#include <string>

class ConfigManager
{
public:
    explicit ConfigManager(const QString& filePath) : settings(filePath, QSettings::IniFormat)
    {
    }

    // 设置属性
    void setValue(const std::string& group, const std::string& key, const std::string& value)
    {
        data[group][key] = value;
    }

    // 获取属性
    std::string getValue(const std::string& group, const std::string& key, const std::string& defaultValue = "") const
    {
        auto groupIt = data.find(group);
        if (groupIt != data.end())
        {
            auto keyIt = groupIt->second.find(key);
            if (keyIt != groupIt->second.end())
            {
                return keyIt->second;
            }
        }
        return defaultValue;
    }

    // 序列化：将数据写入 ini 文件
    void serializeToIni()
    {
        for (const auto& group : data)
        {
            settings.beginGroup(QString::fromStdString(group.first));
            for (const auto& keyValue : group.second)
            {
                settings.setValue(QString::fromStdString(keyValue.first), QString::fromStdString(keyValue.second));
            }
            settings.endGroup();
        }
        settings.sync(); // 确保写入到磁盘
    }

    // 反序列化：从 ini 文件读取数据
    void deserializeFromIni()
    {
        data.clear(); // 清除现有数据

        QStringList groups = settings.childGroups();
        for (const QString& group : groups)
        {
            settings.beginGroup(group);
            QStringList keys = settings.childKeys();
            for (const QString& key : keys)
            {
                data[group.toStdString()][key.toStdString()] = settings.value(key).toString().toStdString();
            }
            settings.endGroup();
        }
    }

    // 打印数据（用于测试）
    void printData() const
    {
        for (const auto& group : data)
        {
            std::cout << "[" << group.first << "]\n";
            for (const auto& keyValue : group.second)
            {
                std::cout << keyValue.first << "=" << keyValue.second << "\n";
            }
        }
    }

private:
    std::map<std::string, std::map<std::string, std::string>> data; // 存储配置的多级 map
    QSettings settings;                                             // QSettings 实例
};
