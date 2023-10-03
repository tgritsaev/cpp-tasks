#include "is_prime.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>

// bool IsPrime(uint64_t x) {
//     if (x <= 1) {
//         return false;
//     }
//     uint64_t root = sqrt(x);
//     auto bound = std::min(root + 6, x);
//     for (auto i = 2ull; i < bound; ++i) {
//         if (x % i == 0) {
//             return false;
//         }
//     }
//     return true;
// }

bool HasDivisorsInHalfInterview(std::atomic<bool>& is_prime, uint64_t x, uint64_t left,
                                uint64_t right) {
    for (auto i = left; i < right; ++i) {
        if (x % i == 0) {

            is_prime = false;
        }
        if (is_prime == false) {
            return false;
        }
    }
    return true;
}

bool IsPrime(uint64_t x) {
    std::atomic<bool> is_prime = true;
    if (x <= 1) {
        return false;
    }
    uint64_t root = sqrt(x);
    auto bound = std::min(root + 6, x);

    // assert(std::thread::hardware_concurrency() > 1);
    size_t concurrency = std::thread::hardware_concurrency() - 1;
    uint64_t step = std::max((bound - 1ull) / concurrency, 1ull);

    std::vector<std::thread> workers;

    for (auto i = 2ull; i < bound; i += step) {
        workers.emplace_back(HasDivisorsInHalfInterview, std::ref(is_prime), x, i, i + step);
    }
    for (auto& t : workers) {
        t.join();
    }
    return is_prime;
}
