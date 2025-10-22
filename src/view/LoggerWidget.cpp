#include "LoggerWidget.h"
LoggerWidget::LoggerWidget(QWidget* parent) : QPlainTextEdit(parent)
{
    setReadOnly(true);
    setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    setMaximumBlockCount(1000);
}