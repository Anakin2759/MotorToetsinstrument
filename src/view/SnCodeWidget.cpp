#include <qboxlayout.h>
#include <qlabel.h>

#include <QLineEdit>
#include <string>

#include "SnCodeWidget.h"

SnCodeWidget::SnCodeWidget()
{
    // 初始化UI
    QHBoxLayout* layout = new QHBoxLayout(this);
    label               = new QLabel;
    edit                = new QLineEdit;
    layout->addWidget(label);
    layout->addWidget(edit);
    layout->setStretch(0, 1);
    layout->setStretch(1, 7);
    setFixedHeight(50);
}

std::string SnCodeWidget::getSnCode() const
{
    return edit->text().toStdString();
}

void SnCodeWidget::setSnCodeNumber(const size_t& snCodeNumber)
{
    label->setText(QString("电机%1 SN码").arg(snCodeNumber));
}