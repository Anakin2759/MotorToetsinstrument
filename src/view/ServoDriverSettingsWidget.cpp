#include <qboxlayout.h>
#include <qheaderview.h>
#include <qtableview.h>
#include <qwidget.h>

#include "ServoDriverSettingsWidget.h"


ServoDriverSettingsWidget::ServoDriverSettingsWidget(QWidget* parent) : QWidget(parent)
{
    QHBoxLayout* layout     = new QHBoxLayout(this);
    QWidget* buttonWidget   = new QWidget();
    servoDriverSettingsView = new QTableView();
    servoDriverSettingsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    servoDriverTypeComboBox = new QComboBox();
    servoDriverIdComboBox   = new QComboBox();
    writeButton             = new QPushButton("写入");
    readButton              = new QPushButton("读取");
    buttonWidget->setLayout(new QVBoxLayout());
    buttonWidget->layout()->addWidget(servoDriverTypeComboBox);
    buttonWidget->layout()->addWidget(servoDriverIdComboBox);
    buttonWidget->layout()->addWidget(writeButton);
    buttonWidget->layout()->addWidget(readButton);
    layout->addWidget(servoDriverSettingsView);
    layout->addWidget(buttonWidget);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);
}

void ServoDriverSettingsWidget::updateSlaveIDCombobox(const size_t& slaveCount)
{
    servoDriverIdComboBox->clear();
    for (size_t i = 0; i < slaveCount; i++)
    {
        servoDriverIdComboBox->addItem(QString("从站%1").arg(i));
    }
}