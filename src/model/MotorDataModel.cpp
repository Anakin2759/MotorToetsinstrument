#include <cstddef>

#include "MotorDataModel.h"

MotorDataModel::MotorDataModel(QObject* parent) : QStandardItemModel(parent)
{
}

void MotorDataModel::updateData()
{
    auto& data = m_data;
    if (rowCount() != data.size())
    {
        clear();
        setHorizontalHeaderLabels({
            "伺服地址",
            "电机ID",
            "电机速度",
            "电机位置",
            "电机转矩",
            "电机负载率",
            "电机温度",
            "电机状态",
            "报警码",
            "功率",
            "品牌",
            "SN码",
            "运行时间",
            "剩余时间",
            "目标速度",
        });
        setRowCount(data.size());
        for (size_t i = 0; i < data.size(); i++)
        {
            setItem(i, 0, new QStandardItem(QString::number(data[i].servoAddress)));
            setItem(i, 1, new QStandardItem(QString::number(data[i].index)));
            setItem(i, 2, new QStandardItem(QString::number(data[i].velocity)));
            setItem(i, 3, new QStandardItem(QString::number(data[i].position)));
            setItem(i, 4, new QStandardItem(QString::number(data[i].torque)));
            setItem(i, 5, new QStandardItem(QString::number(data[i].loadrate)));
            setItem(i, 6, new QStandardItem(QString::number(data[i].temperature)));

            setItem(i, 7, new QStandardItem(data[i].status));
            setItem(i, 8, new QStandardItem(data[i].errorCode));
            setItem(i, 9, new QStandardItem(data[i].power));
            setItem(i, 10, new QStandardItem(data[i].vendor));
            setItem(i, 11, new QStandardItem(data[i].sn));
            setItem(i, 12, new QStandardItem(data[i].runTime));
            setItem(i, 13, new QStandardItem(data[i].remainTime));
            setItem(i, 14, new QStandardItem(QString::number(data[i].targetVelocity)));
        }
    }
    else
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            item(i, 0)->setText(QString::number(data[i].servoAddress));
            item(i, 1)->setText(QString::number(data[i].index));
            item(i, 2)->setText(QString::number(data[i].velocity));
            item(i, 3)->setText(QString::number(data[i].position));

            item(i, 4)->setText(QString::number(data[i].torque));
            item(i, 5)->setText(QString::number(data[i].loadrate));
            item(i, 6)->setText(QString::number(data[i].temperature));

            item(i, 7)->setText(data[i].status);
            item(i, 8)->setText(data[i].errorCode);
            item(i, 9)->setText(data[i].power);
            item(i, 10)->setText(data[i].vendor);
            item(i, 11)->setText(data[i].sn);
            item(i, 12)->setText(data[i].runTime);
            item(i, 13)->setText(data[i].remainTime);
            item(i, 14)->setText(QString::number(data[i].targetVelocity));
        }
    }
}

Qt::ItemFlags MotorDataModel::flags(const QModelIndex& index) const
{
    auto defaultFlags = QStandardItemModel::flags(index);
    if (index.column() == columnCount() - 1)
    {
        return defaultFlags | Qt::ItemIsEditable;
    }
    return defaultFlags & ~Qt::ItemIsEditable;
}

bool MotorDataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::EditRole)
    {
        bool ok;
        int newValue = value.toInt(&ok);
        if (ok)
        {
            m_data[index.row()].targetVelocity = newValue;

            updateData();
            emit dataChanged(index, index); // 通知视图数据已更新
            return true;
        }
    }
    return false;
}
