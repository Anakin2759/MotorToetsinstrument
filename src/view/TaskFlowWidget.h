#pragma once

#include <QLineEdit>
#include <QWidget>
enum class TaskFlowType
{
    run,
    stop,
    wait,
};
class TaskFlowWidget_Run : public QWidget
{
    Q_OBJECT
public:
    TaskFlowWidget_Run(QWidget *parent = nullptr);

    QLineEdit *velocityEdit;
};

class TaskFlowWidget_Stop : public QWidget
{
    Q_OBJECT
public:
    TaskFlowWidget_Stop(QWidget *parent = nullptr);
};
class TaskFlowWidget_Wait : public QWidget
{
    Q_OBJECT
public:
    TaskFlowWidget_Wait(QWidget *parent = nullptr);

    QLineEdit *timeEdit;
};
