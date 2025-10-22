#include <QLabel>
#include <QVBoxLayout>

#include "MainWindow.h"
#include "utils/Logger.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      controlButtonBar(new ControlButtonBar()),
      loggerWidget(new LoggerWidget()),
      functionTabWidget(new FunctionTabWidget())
{
    resize(1920, 1080);
    QWidget* centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(controlButtonBar);
    mainLayout->addWidget(functionTabWidget);
    mainLayout->addWidget(loggerWidget);

    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 7);
    mainLayout->setStretch(2, 2);

    setupSpdlog(loggerWidget);
}
