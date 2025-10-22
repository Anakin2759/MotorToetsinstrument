#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include <vector>

#include "SnCodeWidget.h"

class SnCodeWritter : public QWidget
{
    Q_OBJECT
public:
    explicit SnCodeWritter(QWidget* parent = nullptr);
    ~SnCodeWritter();
    void updateUI(const size_t& number = 0);
    QVBoxLayout* scrollLayout;
    QPushButton* savebutton;
    std::vector<std::unique_ptr<SnCodeWidget>> editList;
};