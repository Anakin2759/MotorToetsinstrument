#pragma once
#include <QTableView>

class MotorDataView : public QTableView
{
    Q_OBJECT
public:
    explicit MotorDataView(QWidget* parent = nullptr);
};