#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QWidget>

#include "ServoDriverWidget.h"

ServoDriverWidget::ServoDriverWidget(QWidget* parent) : QWidget(parent)
{
    m_IndexLabel      = new QLabel;
    m_NameLabel       = new QLabel;
    m_driverInfoLabel = new QLabel;
    m_checkBox        = new QCheckBox;
    m_checkBox->setCheckable(true);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(m_IndexLabel);
    layout->addWidget(m_NameLabel);
    layout->addWidget(m_driverInfoLabel);
    layout->addWidget(m_checkBox);
    setLayout(layout);

    setFixedHeight(80);
}

void ServoDriverWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect r = rect();
    r.setBottom(r.bottom() - 1);
    r.setRight(r.right() - 1);
    painter.drawRect(r);
    QWidget::paintEvent(event);
}