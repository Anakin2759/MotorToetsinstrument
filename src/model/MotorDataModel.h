#pragma once
#include <QStandardItem>
#include <QStandardItemModel>
#include <vector>

#include "common.h"

class MotorDataModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit MotorDataModel(QObject* parent = nullptr);

    void updateData();
    inline std::vector<MotorViewData>& getMotorViewData()
    {
        return m_data;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    const std::vector<MotorViewData>& getMotorData()
    {
        return m_data;
    }
signals:
    void speedChanged(int speed);

private:
    std::vector<MotorViewData> m_data;
};