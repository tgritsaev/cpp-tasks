#pragma once
#define DEFINE2(a, b) a##b
#define DEFINE1(a, b) DEFINE2(a, b)
#define defer SimpleDefer DEFINE1(a, __COUNTER__) = [&]()

template <typename T>
struct SimpleDefer {
    T func;

    constexpr SimpleDefer(T&& create_func) : func(create_func) {
    }

    constexpr ~SimpleDefer() noexcept {
        func();
    }
};