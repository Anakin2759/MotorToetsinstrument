#pragma once
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
class ControlButtonBar : public QWidget
{
    Q_OBJECT
public:
    explicit ControlButtonBar(QWidget* parent = nullptr);
    QComboBox* masterComboBox;
    QPushButton* initButton;
    QPushButton* activeButton;

    QPushButton* enableButton;
    QPushButton* runButton;

    QPushButton* stopButton;
    QPushButton* disableButton;
    QPushButton* createReportButton;
    QPushButton* clearLogButton;
    QPushButton* clearServoAlarmButton;
};
