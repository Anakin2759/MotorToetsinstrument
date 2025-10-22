#pragma once

#include <QMap>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>
class SettingFileManager
{
public:
    SettingFileManager(const QString &filePath) : settings(filePath, QSettings::IniFormat) {}

    // 获取 错误码 数据
    QMap<QString, QString> getErrorCodeConfig()
    {
        QMap<QString, QString> errorCodeConfig;

        // 读取 ErrorCodes 配置
        settings.beginGroup("ErrorCodes");
        QStringList keys = settings.childKeys();

        for (const QString &key : keys)
        {
            // key 为错误码，value 为描述
            errorCodeConfig.insert(key, settings.value(key).toString());
        }
        settings.endGroup();

        return errorCodeConfig;
    }

    // 获取 PN/axisN 数据
    QVector<QMap<QString, QString>> getPNConfig()
    {
        QVector<QMap<QString, QString>> pnConfig;
        pnConfig.reserve(settings.childGroups().size());

        QStringList groups = settings.childGroups();

        for (const QString &group : groups)
        {
            if (group.startsWith("PNE") || group.startsWith("PNF"))
            {
                settings.beginGroup(group);
                QMap<QString, QString> singleConfig;

                for (const QString &key : settings.childKeys())
                {
                    // qDebug() << "Key:" << key << "Value:" << settings.value(key).toString();
                    singleConfig.insert(key, settings.value(key).toString());
                }

                pnConfig.push_back(singleConfig);
                settings.endGroup();
            }
        }

        qDebug() << "PNConfig size:" << pnConfig.size(); // 打印最终大小
        return pnConfig;
    }

    // 设置单个键值
    void setValue(const QString &group, const QString &key, const QVariant &value)
    {
        settings.beginGroup(group);
        settings.setValue(key, value);
        settings.endGroup();
    }

    // 保存一组键值
    void saveGroup(const QString &group, const std::map<std::string, std::string> &data)
    {
        settings.beginGroup(group);
        for (const auto &[key, value] : data)
        {
            settings.setValue(QString::fromStdString(key), QString::fromStdString(value));
        }
        settings.endGroup();
    }

private:
    QSettings settings;
};
