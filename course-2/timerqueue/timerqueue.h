#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <set>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

    void Add(const T& element, TimePoint time) {
        std::lock_guard<std::mutex> lock(mutex_);
        set_.insert({time, element});
        cv_.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (set_.empty() == true) {
            cv_.wait(lock);
        }
        auto elem = set_.begin()->second;
        cv_.wait_until(lock, set_.begin()->first);
        set_.erase(set_.begin());
        return elem;
    }

private:
    std::condition_variable cv_;
    std::multiset<std::pair<TimePoint, T>> set_;
    std::mutex mutex_;
};