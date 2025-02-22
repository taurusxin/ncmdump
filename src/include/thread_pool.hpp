#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <memory>

namespace cxxpool
{

class ThreadPool
{
public:
    ThreadPool():
        thread_num_(std::thread::hardware_concurrency() * 2),
        is_running_(true)
    {
        work_threads_ = std::make_unique<std::thread[]>(thread_num_);
        for (unsigned int i = 0; i < thread_num_; ++i)
        {
            work_threads_[i] = std::thread([&](){
                while (true)
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [&](){ return !is_running_ || !queue_.empty(); });
                    if (!is_running_ && queue_.empty())
                        return;
                    else if (queue_.empty())
                        continue;

                    std::function<void()> func = std::move(queue_.front());
                    queue_.pop();
                    lock.unlock();
                    std::invoke(func);
                }
            });
        }
    }

    ~ThreadPool() noexcept
    {
        if (is_running_)
            join();
    }

    template<class Func, class... Args>
    void submit(Func&& func, Args&&... args)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(std::bind(func, std::forward<Args>(args)...));
        lock.unlock();
        cv_.notify_one();
    }

    void join()
    {
        is_running_ = false;
        cv_.notify_all();

        for (unsigned int i = 0; i < thread_num_; ++i)
        {
            if (work_threads_[i].joinable())
                work_threads_[i].join();
        }
    }

    bool joinable()
    {
        return is_running_;
    }

private:
    const unsigned int                  thread_num_;
    std::unique_ptr<std::thread[]>      work_threads_;
    std::mutex                          mutex_;
    std::atomic<bool>                   is_running_;
    std::condition_variable             cv_;
    std::queue<std::function<void()>>   queue_;
};

} // namespace cxxpool
