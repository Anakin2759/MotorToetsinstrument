#pragma once
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <QWidget>
class TestDataSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestDataSettingsWidget(QWidget* parent = nullptr);
    QLineEdit* testerNameEdit;
    QLineEdit* testerIdEdit;
    QLineEdit* testTimeEdit;
    QLineEdit* testOrderNumberEdit;
    QLineEdit* testDefaultSpeedEdit;
    QPushButton* saveButton;
};