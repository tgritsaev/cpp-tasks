#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <type_traits>

// template <class RandomAccessIterator, class T, class Func>
// T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func
// func) {
//     auto cur_value(initial_value);
//     while (first != last) {
//         cur_value = func(cur_value, *first++);
//     }
//     return cur_value;
// }

template <class RandomAccessIterator, class T, class Func>
void MakeFunc(std::vector<T>& results, size_t results_index, RandomAccessIterator first,
              RandomAccessIterator last, T initial_value, Func func) {
    for (auto it = first; it != last; ++it) {
        initial_value = func(initial_value, *it);
    }
    results[results_index] = initial_value;
}

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    if (std::is_same_v<T, bool> == true) {
        return (Reduce(first, last, (initial_value == true ? 1 : 0), func) == 1 ? true : false);
    }

    std::vector<T> results;
    size_t size = last - first;

    size_t concurrency = std::thread::hardware_concurrency();
    size_t step = std::max(size / (concurrency - 1), static_cast<size_t>(1));

    std::vector<std::thread> jobs;
    results.resize(size / step + 1);
    size_t index = 0;
    for (auto it = first; it < last; it += step, ++index) {
        jobs.emplace_back(MakeFunc<RandomAccessIterator, T, Func>, std::ref(results), index, it,
                          std::min((it + step), last), initial_value, func);
    }
    for (auto& job : jobs) {
        job.join();
    }

    T result = initial_value;
    for (size_t i = 0; i < index; ++i) {
        result = func(results[i], result);
    }
    return result;
}
