#include <qgridlayout.h>
#include <qlabel.h>

#include "TestDataSettingsWidget.h"

TestDataSettingsWidget::TestDataSettingsWidget(QWidget* parent) : QWidget(parent)
{
    QGridLayout* layout           = new QGridLayout(this);
    QLabel* testerNameLabel       = new QLabel("测试人员姓名");
    QLabel* testerIdLabel         = new QLabel("测试人员编号");
    QLabel* testTimeLabel         = new QLabel("测试时间");
    QLabel* testOrderNumberLabel  = new QLabel("测试订单号");
    QLabel* testDefaultSpeedLabel = new QLabel("测试默认速度");
    testerNameEdit                = new QLineEdit();
    testerIdEdit                  = new QLineEdit();
    testTimeEdit                  = new QLineEdit();
    testOrderNumberEdit           = new QLineEdit();
    testDefaultSpeedEdit          = new QLineEdit();


    layout->addWidget(testerNameLabel, 0, 0);
    layout->addWidget(testerNameEdit, 0, 1);
    layout->addWidget(testerIdLabel, 1, 0);
    layout->addWidget(testerIdEdit, 1, 1);
    layout->addWidget(testTimeLabel, 2, 0);
    layout->addWidget(testTimeEdit, 2, 1);
    layout->addWidget(testOrderNumberLabel, 3, 0);
    layout->addWidget(testOrderNumberEdit, 3, 1);
    layout->addWidget(testDefaultSpeedLabel, 4, 0);
    layout->addWidget(testDefaultSpeedEdit, 4, 1);

    saveButton = new QPushButton("保存");
    //在按钮两端添加弹簧

    layout->addWidget(saveButton, 5, 0, 1, 2);
}