#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <cstddef>
#include <string>
class SnCodeWidget : public QWidget
{
    Q_OBJECT
public:
    SnCodeWidget();
    ~SnCodeWidget() = default;

    std::string getSnCode() const;
    void setSnCodeNumber(const size_t& snCodeNumber);
    QLabel* label;
    QLineEdit* edit;
};