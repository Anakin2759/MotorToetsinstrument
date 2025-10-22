#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <thread>

#include "LockFreeQueue.h"
#include "Logger.h"
#include "ThreadPool.h"

class EventLoop
{
public:
    EventLoop(std::size_t interval_ms = 1) : running(false), ms(interval_ms)
    {
        defaultEvent = [this]() {};
    }

    ~EventLoop()
    {
        stop(); // 确保在析构时停止事件循环
        // 等待可能未完成的任务安全退出
        std::unique_lock<std::mutex> lock(mtx);
        cv.notify_all(); // 通知所有阻塞线程
    }
    // 启动事件循环
    void start()
    {
        bool expected = false;
        if (running.compare_exchange_strong(expected, true))
        {
            events.clear(); // 清空事件队列
            // 将事件循环任务添加到线程池中管理
            ThreadPool::getInstance().enqueue([this]() { loop(); });
        }
    }

    // 停止事件循环
    void stop()
    {
        bool expected = true;
        if (running.compare_exchange_strong(expected, false))
        {
            cv.notify_one(); // 通知事件循环线程退出
        }
    }

    // 添加事件到事件队列
    template <typename Func> void add(Func &&func)
    {
        if (!events.enqueue(std::forward<Func>(func)))
        {
            LOG_ERROR("事件队列已满，无法添加新事件");
        }
        cv.notify_one(); // 通知事件循环线程有新事件
    }

    // 设置空事件的默认行为
    void setDefaultEvent(std::function<void()> func) { defaultEvent = std::move(func); }

private:
    // 事件循环函数
    void loop()
    {
        auto next_run_time = std::chrono::steady_clock::now(); // 初始时间点

        while (running)
        {
            std::function<void()> event;

            // 尝试从事件队列中获取事件
            auto result = events.dequeue();

            // 如果队列为空，使用空事件或默认行为
            if (!result)
            {
                if (defaultEvent)
                {
                    event = defaultEvent;
                }
                else
                {
                    event = []() { std::this_thread::sleep_for(std::chrono::microseconds(1000)); };
                }
            }
            else
            {
                // 成功从队列中取出事件
                event = std::move(result.value());
            }

            // 执行事件
            event();

            // 计算下次执行时间，确保每次间隔ms毫秒
            next_run_time += std::chrono::milliseconds(ms);
            std::this_thread::sleep_until(next_run_time);

            // 由于空事件可能会导致空队列，检查条件变量
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::milliseconds(ms), [this] { return !running || !events.empty(); });
        }
    }

    std::atomic<bool> running;                   // 运行状态标志
    LockFreeQueue<std::function<void()>> events; // 事件队列（锁自由队列）
    std::mutex mtx;                              // 用于事件等待的互斥量
    std::condition_variable cv;                  // 条件变量，用于通知事件循环线程
    std::size_t ms;                              // 事件执行的间隔时间
    std::function<void()> defaultEvent;          // 空事件的默认行为
};
