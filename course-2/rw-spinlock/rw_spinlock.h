#pragma once

#include <atomic>

struct RWSpinLock {
    void LockRead() {
        int cp = flag;
        while ((flag & 1) == 1 || flag.compare_exchange_weak(cp, cp + 2) == false) {
            cp = flag;
        }
    }

    void UnlockRead() {
        flag.fetch_sub(2);
    }

    void LockWrite() {
        int cp = flag.load();
        while (cp != 0 || flag.compare_exchange_weak(cp, 1) == false) {
            cp = 0;
        }
    }

    void UnlockWrite() {
        flag.fetch_sub(1);
    }

    std::atomic<int> flag{0};
};
