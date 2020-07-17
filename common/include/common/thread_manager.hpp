#pragma once

#include "common/log.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <stdexcept>
#include <vector>

namespace common
{
typedef uint32_t TaskId;

template <typename ThreadState>
class ThreadManager
{
    struct Job;
    typedef std::function<ThreadState (void)> ThreadStateInitFunction;
    typedef std::function<void (ThreadState&)> ThreadStateCleanupFunction;

    typedef std::function<void (ThreadState&, Job&)> TaskFunction;

    struct Job
    {
        TaskId id;
        TaskFunction func;
        void* args;
    };

    struct Task
    {
        TaskId id;
        TaskFunction func;
    };

    bool m_cancel = false;
    uint64_t m_sleep = 200;

    std::mutex m_jobs_mutex;

    std::map<TaskId, Task> m_tasks;
    std::queue<Job> m_jobs;
    std::vector<std::thread> m_threads;

    bool get_job(Job& job) {
        std::lock_guard<std::mutex> lock(m_jobs_mutex);

        if (m_jobs.empty()) {
            return false;
        }

        job = m_jobs.front();
        m_jobs.pop();

        return true;
    }

    //  Thread main function
    void thread_main(ThreadState state, ThreadStateCleanupFunction cleanup_state) {
        //  Loop until threads are canceled
        while (!m_cancel) {
            Job job{};

            //  Sleep until a job is available
            if (!get_job(job)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(m_sleep));
                continue;
            }

            //  Execute actual task function with state and job arguments
            job.func(state, job);
        }

        //  Clean-up thread state
        if (cleanup_state) {
            cleanup_state(state);
        }
    }

public:
    ThreadManager() = default;

    ~ThreadManager() {
        cancel();
    }

    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

    template <typename T>
    void add_task(const TaskId task_id, std::function<void (ThreadState&, T&)> task_func) {
        if (!task_func) {
            std::runtime_error("Task function cannot be null.");
        }

        if (m_tasks.find(task_id) != m_tasks.end()) {
            std::runtime_error("A task exists with the same ID.");
        }

        Task task{};
        task.id = task_id;

        //  This lambda converts the (void*) job arguments to the task
        //  argments type, then deletes the job arguments when finished.
        task.func = [task_func](ThreadState& state, Job& job) {
            //  Cast job arguments to task specific arguments
            T* task_args = static_cast<T*>(job.args);
            if (task_args == nullptr) {
                throw std::runtime_error("Failed to cast task arguments.");
            }

            //  Call actual task function
            task_func(state, *task_args);

            //  Delete job arguments
            delete task_args;
        };

        m_tasks[task_id] = task;
    }

    template <typename T>
    void add_job(const TaskId task_id, const T& task_args) {
        if (m_tasks.find(task_id) == m_tasks.end()) {
            throw std::runtime_error("Task does not exist.");
        }

        Job job{};
        job.id = task_id;
        job.args = new T(task_args);
        job.func = m_tasks.at(task_id).func;

        std::lock_guard<std::mutex> lock(m_jobs_mutex);
        m_jobs.push(job);
    }

    void cancel() {
        m_cancel = true;
        for (std::thread& thread : m_threads) {
            thread.join();
        }

        m_cancel = false;
        m_threads.clear();
    }

    void start_threads(
        ThreadStateInitFunction init_state,
        ThreadStateCleanupFunction cleanup_state,
        uint32_t thread_count = 0
    ) {
        if (!m_threads.empty()) {
            throw std::runtime_error("Threads are already active.");
        }

        //  If thread count is not specified use hardware thread count
        if (thread_count == 0) {
            thread_count = std::max<uint32_t>(
                1,
                std::thread::hardware_concurrency()
            );
        }

        //  Create threads
        for (auto n = 0; n < thread_count; ++n) {
            ThreadState state;
            if (init_state) {
                state = init_state();
            }

            m_threads.emplace_back(&ThreadManager::thread_main, this, state, cleanup_state);
        }

        common::log_debug("Started %d threads.", thread_count);
    }
};
}
