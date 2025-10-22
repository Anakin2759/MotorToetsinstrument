#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "view/ControlButtonBar.h"
ControlButtonBar::ControlButtonBar(QWidget* parent) : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    masterComboBox = new QComboBox();
    masterComboBox->addItem("主站0");
    masterComboBox->addItem("主站1");

    QLabel* masterLabel = new QLabel("主站选择");
    initButton          = new QPushButton(tr("初始化"));

    activeButton          = new QPushButton(tr("切换实时状态"));
    enableButton          = new QPushButton(tr("使能"));
    runButton             = new QPushButton(tr("运行"));
    stopButton            = new QPushButton(tr("停止"));
    disableButton         = new QPushButton(tr("去使能"));
    createReportButton    = new QPushButton(tr("产生报告"));
    clearLogButton        = new QPushButton(tr("清空日志"));
    clearServoAlarmButton = new QPushButton(tr("清除报警"));

    layout->addWidget(masterLabel);
    layout->addWidget(masterComboBox);
    layout->addWidget(initButton);
    layout->addWidget(activeButton);
    layout->addWidget(enableButton);
    layout->addWidget(runButton);
    layout->addWidget(stopButton);
    layout->addWidget(disableButton);
    layout->addWidget(createReportButton);
    layout->addWidget(clearLogButton);
    layout->addWidget(clearServoAlarmButton);
}