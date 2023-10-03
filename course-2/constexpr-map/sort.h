#pragma once

#include <constexpr_map.h>

#include <type_traits>
#include <iostream>
#include <string>

template <class K, class V, int S>
constexpr auto Sort(ConstexprMap<K, V, S> map) {
    size_t N = map.Size();
    ConstexprMap<K, V> new_map;
    for (size_t i = 0; i < N; ++i) {
        auto [k, v] = map.GetByIndex(i);
        new_map[k] = v;
    }
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j + 1 < N; ++j) {
            if constexpr (std::is_integral_v<K> == false) {
                if (new_map.GetByIndex(j + 1).first < new_map.GetByIndex(j).first) {
                    new_map.Swap(j);
                }
            } else {
                if (new_map.GetByIndex(j).first < new_map.GetByIndex(j + 1).first) {
                    new_map.Swap(j);
                }
            }
        }
    }
    return new_map;
}
