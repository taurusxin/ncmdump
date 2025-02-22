#pragma once

#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <atomic>
#include <string>
#include <functional>

namespace cxxlog
{

class Logger
{
public:
    Logger():
        is_running_(true)
    {
        thread_ = std::thread([&]() {
            while (true)
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [&](){ return !is_running_ || !msg_queue_.empty(); });
                if (!is_running_ && msg_queue_.empty())
                    return;
                else if (msg_queue_.empty())
                    continue;
                std::function<void()> func = std::move(msg_queue_.front());
                msg_queue_.pop();
                lock.unlock();
                std::invoke(func);
            }
        });
    }

    ~Logger()
    {
        if (is_running_)
            join();
    }

    template<typename... Args>
    void cerr(Args... args)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        msg_queue_.push(std::bind([](auto... args) {
            std::cerr << generateTimeFormatString();
            ((std::cerr << args), ...);
            std::cerr << std::endl;
            }, std::move(args)...));
        lock.unlock();
        cv_.notify_all();
    }

    template<typename... Args>
    void cout(Args... args)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        msg_queue_.push(std::bind([](auto... args) {
            std::cout << generateTimeFormatString();
            ((std::cout << args), ...);
            std::cout << std::endl;
            }, std::move(args)...));
        lock.unlock();
        cv_.notify_all();
    }

    void join()
    {
        is_running_ = false;
        cv_.notify_all();

        if (thread_.joinable())
            thread_.join();
    }

    bool joinable()
    {
        return is_running_;
    }

protected:
    static std::string generateTimeFormatString()
    {
        auto t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "[%H:%M:%S]");
        return ss.str();
    }

private:
    std::condition_variable             cv_;
    std::mutex                          mutex_;
    std::atomic<bool>                   is_running_;
    std::queue<std::function<void()>>   msg_queue_;
    std::thread                         thread_;
};

} // namespace cxxlog
