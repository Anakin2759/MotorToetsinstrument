#pragma once

#include <QPlainTextEdit>

class LoggerWidget : public QPlainTextEdit
{
    Q_OBJECT
public:
    LoggerWidget(QWidget* parent = nullptr);
};