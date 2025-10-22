#pragma once
#include "TaskFlowWidget.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <vector>
class TaskFlowDesignerView : public QWidget
{
    Q_OBJECT
public:
    TaskFlowDesignerView(QWidget *parent = nullptr);
    ~TaskFlowDesignerView();

private:
    QVBoxLayout *scrollLayout;
    std::vector<QWidget *> taskFlowWidgetList{};
    QPushButton *addRunTaskButton;
    QPushButton *addStopTaskButton;
    QPushButton *addWaitTaskButton;
    QPushButton *saveButton;
    QPushButton *clearButton;
    QPushButton *startButton;
    QPushButton *endButton;
};