#include <QVBoxLayout>

#include "FunctionTabWidget.h"

FunctionTabWidget::FunctionTabWidget(QWidget *parent)
    : QTabWidget(parent), servoDriverListWidget(new ServoDriverListWidget()),
      testDataSettingsWidget(new TestDataSettingsWidget()), servoDriverSettingsWidget(new ServoDriverSettingsWidget()),
      motorDataView(new MotorDataView()), snCodeWritter(new SnCodeWritter()),
      temperatureScanSettingWidget(new TemperatureScanSettingWidget()), taskFlowDesignerView(new TaskFlowDesignerView())
{
    // setMovable(true);
    addTab(servoDriverListWidget, "伺服驱动列表");
    addTab(servoDriverSettingsWidget, "伺服驱动参数设置");
    addTab(testDataSettingsWidget, "测试数据设置");

    addTab(motorDataView, "电机数据监控");
    addTab(snCodeWritter, "SN码写入");
    addTab(temperatureScanSettingWidget, "温度扫描设置");
    addTab(taskFlowDesignerView, "任务流程设计器");
}
