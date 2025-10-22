#include "TaskFlowWidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
TaskFlowWidget_Run::TaskFlowWidget_Run(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    QLabel *label = new QLabel("运行任务");

    QLabel *velocityLabel = new QLabel("速度:");
    velocityEdit = new QLineEdit;

    layout->addWidget(label);
    layout->addWidget(velocityLabel);
    layout->addWidget(velocityEdit);
    layout->setStretch(0, 1);
    layout->setStretch(1, 1);
    layout->setStretch(2, 1);
}

TaskFlowWidget_Stop::TaskFlowWidget_Stop(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    QLabel *label = new QLabel("停止任务");
    layout->addWidget(label);
    layout->addStretch(); // 使用 addStretch() 添加动态伸缩空间

    layout->setStretch(0, 1);
    layout->setStretch(1, 2);
}

TaskFlowWidget_Wait::TaskFlowWidget_Wait(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    QLabel *label = new QLabel("等待任务");

    QLabel *timeLabel = new QLabel("等待时间:");
    timeEdit = new QLineEdit;
    layout->addWidget(label);
    layout->addWidget(timeLabel);
    layout->addWidget(timeEdit);

    layout->setStretch(0, 1);
    layout->setStretch(1, 1);
    layout->setStretch(2, 1);
}