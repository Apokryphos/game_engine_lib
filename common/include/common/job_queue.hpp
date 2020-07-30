#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace common
{
template <typename T>
class JobQueue
{
public:
    bool m_cancel {false};
    mutable std::mutex m_cancel_mutex;
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
    std::condition_variable m_condition;

public:
    void cancel() {
        {
            std::lock_guard<std::mutex> lock(m_cancel_mutex);
            m_cancel = true;
        }
        m_condition.notify_all();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    bool is_canceled() const {
        std::lock_guard<std::mutex> lock(m_cancel_mutex);
        return m_cancel;
    }

    void push(T& value) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(value);
        }
        m_condition.notify_one();
    }

    //  Waits until a job is ready or the queue is canceled.
    //  Returns true if the value was popped from the queue
    //  Returns false if the queue was canceled.
    bool wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(
            lock,
            [this] {
                return !m_queue.empty() || is_canceled();
            }
        );

        if (m_cancel) {
            return false;
        }

        value = m_queue.front();
        m_queue.pop();

        return true;
    }
};
}
