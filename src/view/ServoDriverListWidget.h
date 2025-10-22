#pragma once

#include <ecrt.h>

#include <QVBoxLayout>
#include <QWidget>

#include "ServoDriverWidget.h"

class ServoDriverListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ServoDriverListWidget(QWidget* parent = nullptr);
    void updateServoDriverList(const std::vector<ec_slave_info_t>& slaveInfoList);
    QVBoxLayout* scrollLayout;
    std::vector<std::unique_ptr<ServoDriverWidget>> ServoDriverList;
};