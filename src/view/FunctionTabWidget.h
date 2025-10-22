#pragma once

#include <QTabWidget>

#include "MotorDataView.h"
#include "ServoDriverListWidget.h"
#include "ServoDriverSettingsWidget.h"
#include "SnCodeWritter.h"
#include "TaskFlowDesignerView.h"
#include "TemperatureScanSettingWidget.h"
#include "TestDataSettingsWidget.h"
class FunctionTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit FunctionTabWidget(QWidget *parent = nullptr);
    ServoDriverListWidget *servoDriverListWidget;
    TestDataSettingsWidget *testDataSettingsWidget;
    ServoDriverSettingsWidget *servoDriverSettingsWidget;
    MotorDataView *motorDataView;
    SnCodeWritter *snCodeWritter;
    TemperatureScanSettingWidget *temperatureScanSettingWidget;
    TaskFlowDesignerView *taskFlowDesignerView;
};