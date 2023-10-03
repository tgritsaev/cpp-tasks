#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};

class Semaphore {
public:
    Semaphore(int count) : count_(count) {
    }

    void Leave() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty() == false) {
            ++count_;
            queue_.front()->notify_one();
            queue_.pop();
        }
    }

    template <class Func>
    void Enter(Func callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (count_ == 0) {
            auto condition{std::shared_ptr<std::condition_variable>()};
            condition->wait(lock);
            queue_.push(condition);
        }
        --count_;
        callback(count_);
    }

    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::queue<std::shared_ptr<std::condition_variable>> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_ = 0;
};