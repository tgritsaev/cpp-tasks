#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <mutex>
#include <atomic>

// Atomically do the following:
//    if (*value == expected_value) {
//        sleep_on_address(value)
//    }
void FutexWait(int *value, int expected_value) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expected_value, nullptr, nullptr, 0);
}

// Wakeup 'count' threads sleeping on address of value(-1 wakes all)
void FutexWake(int *value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

class Mutex {
public:
    void Lock() {
        auto cmpxchg = [](std::atomic<int> &val, int expected, int desired) {
            val.compare_exchange_weak(expected, desired);
            return expected;
        };

        int c;
        if ((c = cmpxchg(val_, 0, 1)) != 0) {
            do {
                if (c == 2 || cmpxchg(val_, 1, 2) != 0) {
                    FutexWait(reinterpret_cast<int *>(&val_), 2);
                }
            } while ((c = cmpxchg(val_, 0, 2)) != 0);
        }
    }

    void Unlock() {
        if (val_.fetch_sub(1) != 1) {
            val_ = 0;
            FutexWake(reinterpret_cast<int *>(&val_), 1);
        }
    }

private:
    std::atomic<int> val_{0};
};
