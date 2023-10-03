#pragma once
#include <algorithm>
#include <array>

template <size_t N>
constexpr int determinant(const std::array<std::array<int, N>, N>& a) {
    int ans = 0;
    for (size_t internal = 0; internal < N; internal++) {
        std::array<std::array<int, N - 1>, N - 1> cut_a{};
        for (size_t i = 1; i < N; i++) {
            for (size_t j = 0; j < N; j++) {
                auto& link = (&std::get<0>(cut_a))[i - 1];
                if (j < internal) {
                    (&std::get<0>(link))[j] = a[i][j];
                } else if (j > internal) {
                    (&std::get<0>(link))[j - 1] = a[i][j];
                }
            }
        }
        ans += a[0][internal] * (internal % 2 == 0 ? 1 : -1) * determinant<N - 1>(cut_a);
    }
    return ans;
}

template <>
constexpr int determinant<1>(const std::array<std::array<int, 1>, 1>& a) {
    return a[0][0];
}