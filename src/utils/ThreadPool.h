#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "LockFreeQueue.h"
class ThreadPool
{
public:
    static ThreadPool& getInstance(size_t thread_count = std::thread::hardware_concurrency(), size_t queue_size = 100)
    {
        static ThreadPool instance(thread_count, queue_size);
        return instance;
    }

    // 禁止拷贝和赋值
    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename F, typename... Args>
    bool enqueue(F&& func, Args&&... args)
    {
        auto task = std::bind(std::forward<F>(func), std::forward<Args>(args)...);

        if (!tasks.enqueue(std::move(task)))
        {
            return false; // 队列已满
        }

        cv.notify_one();
        return true;
    }

    void stop()
    {
        {
            std::unique_lock<std::mutex> lock(cv_mutex);
            stop_flag = true;
        }

        cv.notify_all();

        for (std::thread& worker : workers)
        {
            if (worker.joinable())
                worker.join();
        }

        workers.clear();
    }

    ~ThreadPool()
    {
        stop();
    }

private:
    LockFreeQueue<std::function<void()>> tasks;
    std::vector<std::thread> workers;

    std::mutex cv_mutex;
    std::condition_variable cv;
    std::atomic<bool> stop_flag;

    ThreadPool(size_t thread_count, size_t queue_size) : tasks(queue_size), stop_flag(false)
    {
        for (size_t i = 0; i < thread_count; ++i)
        {
            workers.emplace_back(
                [this]
                {
                    while (true)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->cv_mutex);
                            this->cv.wait(lock, [this] { return this->stop_flag || !this->tasks.empty(); });

                            if (this->stop_flag && this->tasks.empty())
                            {
                                return;
                            }

                            if (auto opt_task = this->tasks.dequeue())
                            {
                                task = std::move(*opt_task);
                            }
                            else
                            {
                                continue;
                            }
                        }

                        task();
                    }
                });
        }
    }
};
