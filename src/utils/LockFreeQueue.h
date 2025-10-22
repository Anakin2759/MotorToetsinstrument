#pragma once

#include <atomic>
#include <optional> // 用于返回更明确的队列状态
#include <vector>
template <typename T>
class LockFreeQueue
{
public:
    explicit LockFreeQueue(size_t size = 50) : buffer(size), head(0), tail(0)
    {
    }

    bool enqueue(T item)
    {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail    = increment(current_tail);

        if (next_tail == head.load(std::memory_order_acquire))
        {
            return false; // 队列已满
        }

        buffer[current_tail] = std::move(item);
        tail.store(next_tail, std::memory_order_release);
        return true;
    }

    std::optional<T> dequeue()
    {
        size_t current_head = head.load(std::memory_order_relaxed);

        if (current_head == tail.load(std::memory_order_acquire))
        {
            return std::nullopt; // 队列为空
        }

        T item = std::move(buffer[current_head]);
        head.store(increment(current_head), std::memory_order_release);
        return item;
    }

    void clear()
    {
        head.store(0, std::memory_order_relaxed);
        tail.store(0, std::memory_order_relaxed);
    }

    bool empty() const
    {
        return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
    }

    bool full() const
    {
        return increment(tail.load(std::memory_order_acquire)) == head.load(std::memory_order_acquire);
    }

private:
    size_t increment(size_t index) const noexcept
    {
        return (index + 1) % buffer.size();
    }

    std::vector<T> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};