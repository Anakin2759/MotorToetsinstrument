#pragma once
#include <qlineedit.h>

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
class TemperatureScanSettingWidget : public QWidget
{
    Q_OBJECT
public:
    TemperatureScanSettingWidget(QWidget* parent = nullptr);
    ~TemperatureScanSettingWidget();
signals:
    void startTemperatureScan(size_t startStationNumber, size_t endStationNumber, std::string com, int baudrate,
                              int dataBits, int stopBits, char parity);
    void stopTemperatureScan();

private:
    QLineEdit* startStationNumberEdit;
    QLineEdit* endStationNumberEdit;
    QComboBox* comComboBox;
    QLineEdit* baudrateEdit;
    QLineEdit* dataBitsEdit;
    QLineEdit* stopBitsEdit;
    QLineEdit* parityEdit;
    QPushButton* connectButton;
    QPushButton* disconnectButton;
};