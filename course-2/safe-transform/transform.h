#pragma once
#include <list>

template <typename It, typename P, typename F,
          typename T = typename std::iterator_traits<It>::value_type>
void TransformIf(It begin, It end, P predicat, F functor) {
    std::list<T> values;
    auto reset = [&]() {
        auto iterator = begin;
        for (auto value : values) {
            *iterator = value;
            ++iterator;
        }
    };
    for (auto iterator = begin; iterator != end; ++iterator) {
        try {
            values.push_back(*iterator);
        } catch (...) {
        }
        try {
            if (predicat(*iterator)) {
                try {
                    functor(*iterator);
                } catch (...) {
                    throw;
                }
            }
        } catch (...) {
            reset();
            throw;
        }
    }
}
