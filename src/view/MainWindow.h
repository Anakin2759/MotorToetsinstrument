#pragma once

#include <QMainWindow>
#include <QString>
#include <QWidget>

#include "ControlButtonBar.h"
#include "FunctionTabWidget.h"
#include "LoggerWidget.h"

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ControlButtonBar* controlButtonBar;
    LoggerWidget* loggerWidget;
    FunctionTabWidget* functionTabWidget;

private:
};
