#ifndef LOGGER_H
#define LOGGER_H

#include <fmt/core.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

#include <QMutex>
#include <QObject>
#include <QPlainTextEdit>

class QTextEditSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
    explicit QTextEditSink(QPlainTextEdit* textEdit) : m_textEdit(textEdit)
    {
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        // 使用 spdlog 的格式化功能生成完整的日志消息
        spdlog::memory_buf_t formatted;
        base_sink<std::mutex>::formatter_->format(msg, formatted);
        QString logMessage = QString::fromStdString(fmt::to_string(formatted));

        // 锁定互斥锁以保护多线程访问
        QMutexLocker locker(&m_mutex);

        // 将日志消息提交到主线程
        QMetaObject::invokeMethod(m_textEdit,
                                  [this, logMessage]()
                                  {
                                      // 在主线程中更新 UI
                                      m_textEdit->appendPlainText(logMessage);

                                      // 如果行数超过 1000 行，清除日志
                                      if (m_textEdit->document()->lineCount() > 1000)
                                      {
                                          m_textEdit->clear();
                                      }
                                  });
    }

    void flush_() override
    {
        // 可选实现：处理刷新逻辑
    }

private:
    QPlainTextEdit* m_textEdit;
    QMutex m_mutex; // 保护多线程访问
};

inline void setupSpdlog(QPlainTextEdit* textEdit)
{
    auto sink   = std::make_shared<QTextEditSink>(textEdit);
    auto logger = std::make_shared<spdlog::logger>("qt_logger", sink);
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    spdlog::set_default_logger(logger);
}

#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

#endif // LOGGER_H