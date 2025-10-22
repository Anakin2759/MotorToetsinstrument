#include "TaskFlowDesignerView.h"
#include <QScrollArea>
TaskFlowDesignerView::TaskFlowDesignerView(QWidget *parent)
    : QWidget(parent), addRunTaskButton(new QPushButton()), addWaitTaskButton(new QPushButton()),
      saveButton(new QPushButton()), clearButton(new QPushButton()), startButton(new QPushButton()),
      endButton(new QPushButton())
{
    QHBoxLayout *layout = new QHBoxLayout(this); // 创建一个水平 layout
    QVBoxLayout *buttonAreaLayout = new QVBoxLayout;

    addRunTaskButton->setText("添加运行任务");
    addWaitTaskButton->setText("添加等待任务");
    saveButton->setText("保存");
    clearButton->setText("清空");
    startButton->setText("开始");
    endButton->setText("结束");

    buttonAreaLayout->addWidget(addRunTaskButton);
    buttonAreaLayout->addWidget(addWaitTaskButton);
    buttonAreaLayout->addWidget(saveButton);
    buttonAreaLayout->addWidget(clearButton);
    buttonAreaLayout->addWidget(startButton);
    buttonAreaLayout->addWidget(endButton);

    QScrollArea *scrollArea = new QScrollArea;
    QWidget *scrollContent = new QWidget; // 用于滚动区域的内容
    scrollLayout = new QVBoxLayout;

    scrollLayout->setSpacing(30); // 设置组件间隔为10像素

    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true); // 确保滚动区域可调整大小

    //    layout->add

    // connect(addRunTaskButton, &QPushButton::clicked, this, [this]() {
    //     QWidget *taskFlowWidget = new TaskFlowWidget_Run();
    //     taskFlowWidgetList.push_back(taskFlowWidget);
    //     scrollLayout->addWidget(taskFlowWidget);
    // });
    // connect(addStopTaskButton, &QPushButton::clicked, this, [this]() {
    //     QWidget *taskFlowWidget = new TaskFlowWidget_Stop();
    //     taskFlowWidgetList.push_back(taskFlowWidget);
    //     scrollLayout->addWidget(taskFlowWidget);
    // });
    // connect(addWaitTaskButton, &QPushButton::clicked, this, [this]() {
    //     QWidget *taskFlowWidget = new TaskFlowWidget_Wait();
    //     taskFlowWidgetList.push_back(taskFlowWidget);
    //     scrollLayout->addWidget(taskFlowWidget);
    // });

    // connect(saveButton, &QPushButton::clicked, [this]() {});
    // connect(clearButton, &QPushButton::clicked, [this]() { taskFlowWidgetList.clear(); });
    // connect(startButton, &QPushButton::clicked, [this]() {});
    // connect(endButton, &QPushButton::clicked, [this]() {});
}

TaskFlowDesignerView::~TaskFlowDesignerView() {}
