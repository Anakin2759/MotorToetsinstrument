#include <ecrt.h>

#include <QScrollArea>
#include <memory>

#include "ServoDriverListWidget.h"

ServoDriverListWidget::ServoDriverListWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout     = new QVBoxLayout;
    QScrollArea *scrollArea = new QScrollArea;
    QWidget *scrollContent  = new QWidget; // 用于滚动区域的内容
    scrollLayout            = new QVBoxLayout;

    scrollLayout->setSpacing(30); // 设置组件间隔为10像素

    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true); // 确保滚动区域可调整大小
    layout->addWidget(scrollArea);
    setLayout(layout);
}

void ServoDriverListWidget::updateServoDriverList(const std::vector<ec_slave_info_t> &slaveInfoList)
{
    ServoDriverList.clear();

    QLayoutItem *item = scrollLayout->itemAt(0);

    // 检查该项是否是 QSpacerItem（即伸缩空间）
    if (QSpacerItem *spacer = dynamic_cast<QSpacerItem *>(item))
    {
        // 如果是伸缩空间，则移除它
        scrollLayout->removeItem(item);
        delete spacer; // 删除该伸缩空间对象
    }
    for (size_t i = 0; i < slaveInfoList.size(); i++)
    {
        ServoDriverList.emplace_back(std::make_unique<ServoDriverWidget>());

        auto &index        = slaveInfoList[i].position;
        auto slave_name    = QString(slaveInfoList[i].name);
        auto &vendor_id    = slaveInfoList[i].vendor_id;
        auto &product_code = slaveInfoList[i].product_code;

        QString name = QString("从站%1").arg(index);
        QString type = QString(slave_name);
        QString info = QString("Vendor ID：%1 Product Code：%2").arg(vendor_id).arg(product_code);

        ServoDriverList[i]->setServoDriverInfo(name, type, info);
        scrollLayout->addWidget(ServoDriverList[i].get());
    }
    scrollLayout->addStretch();
}
