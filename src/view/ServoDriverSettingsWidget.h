#pragma once
#include <qpushbutton.h>
#include <qtableview.h>

#include <QComboBox>
#include <QTableView>
#include <QWidget>

class ServoDriverSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ServoDriverSettingsWidget(QWidget* parent = nullptr);

    void updateSlaveIDCombobox(const size_t& slaveCount);
    QComboBox* servoDriverTypeComboBox;
    QComboBox* servoDriverIdComboBox;
    QTableView* servoDriverSettingsView;

    QPushButton* writeButton;
    QPushButton* readButton;
};