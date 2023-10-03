#pragma once

#include <utility>
#include <stddef.h>

using std::size_t;

template <size_t N>
constexpr auto Fib() {
    if constexpr (N < 2) {
        return [] { return N; };
    } else {
        return [] { return Fib<N - 1>()() + Fib<N - 2>()(); };
    }
}

template <size_t N>
constexpr auto Fact() {
    if constexpr (N == 0) {
        return [] { return 1; };
    } else {
        return [] { return Fact<N - 1>()() * N; };
    }
}

template <size_t A, size_t B>
constexpr auto GCD() {
    if constexpr (B == 0) {
        return [] { return A; };
    } else {
        return [] { return GCD<B, A % B>()(); };
    }
}

template <size_t A, size_t H = 2>
constexpr auto Prime() -> bool (*)() {
    if constexpr (A <= 1 || (A % H == 0 && A != H)) {
        return [] { return false; };
    } else if constexpr (A == 2 || A < H * H) {
        return [] { return true; };
    } else if constexpr (H < A) {
        return [] { return Prime<A, H + 1>()(); };
    }
}
