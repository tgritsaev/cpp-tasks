#pragma once

#include <utility>
#include <tuple>

template <class F>
constexpr auto Curry(F&& f) {
    return [wrapped_f = std::forward<F>(f)]<typename... T>(T && ... args) mutable {
        return wrapped_f(std::make_tuple(std::forward<T>(args)...));
    };
}

template <class F>
constexpr auto Uncurry(F&& f) {
    return [wrapped_f = std::forward<F>(f)]<typename... T>(std::tuple<T...> && args) mutable {
        return std::apply(wrapped_f, args);
    };
}
