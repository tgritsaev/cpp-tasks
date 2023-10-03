#pragma once

#include <mutex>
#include <queue>
#include <atomic>
#include <iostream>
#include <string>
#include <thread>

template <class T>
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(int size) {
        max_size_ = size;
        for (size_t i = 0; i < static_cast<size_t>(size); ++i) {
            queue_[i].gen = i;
        }
    }

    ~MPMCBoundedQueue() {
        delete[] queue_;
    }

    bool Enqueue(const T& value) {
        // std::cout << ind << " " << expected << " " << end_ << std::endl;
        while (true) {
            size_t copy_end = end_.load();
            size_t ind = copy_end % max_size_;
            // std::string str = std::to_string(queue_[ind].gen.load()) + " " +
            //                   std::to_string(copy_end) + " " + std::to_string(end_) + "\n";
            // std::cout << str;
            if (queue_[ind].gen.load() == copy_end &&
                end_.compare_exchange_weak(copy_end, copy_end + 1)) {
                queue_[ind].value = value;
                queue_[ind].gen = queue_[ind].gen + 1;
                return true;
            } else {
                if (copy_end == start_ + max_size_) {
                    return false;
                } else {
                    // std::string str = "yielding, start_:" + std::to_string(start_) +
                    //                   " copy_end:" + std::to_string(copy_end) + '\n';
                    // std::cout << str;
                    std::this_thread::yield();
                }
            }
        }
    }

    bool Dequeue(T& data) {
        // std::cout << ind << " " << expected << " " << end_ << std::endl;
        while (true) {
            size_t copy_start = start_.load();
            size_t ind = copy_start % max_size_;
            if (queue_[ind].gen.load() == copy_start + 1 &&
                start_.compare_exchange_weak(copy_start, copy_start + 1)) {
                data = std::move(queue_[ind].value);
                queue_[ind].gen = queue_[ind].gen + max_size_ - 1;
                return true;
            } else {
                if (copy_start == end_) {
                    return false;
                } else {
                    // std::string str = "yielding, end_:" + std::to_string(end_) +
                    //                   " copy_start:" + std::to_string(copy_start) + '\n';
                    // std::cout << str;
                    std::this_thread::yield();
                }
            }
        }
    }

private:
    struct Node {
        T value;
        std::atomic<size_t> gen;
    };

    inline size_t StepBack() {
        return max_size_ - 1;
    }

    std::atomic<size_t> start_{0};
    std::atomic<size_t> end_{0};

    static const size_t kMaxN = (1024 * 1024 * 10);
    Node* queue_ = new Node[kMaxN];

    std::atomic<size_t> max_size_{1};
};
