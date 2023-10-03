#pragma once

#include <functional>
#include <utility>

template <class F, class... Args>
constexpr auto BindFront(F&& f, Args&&... args) {
    return [ wrapped_f = std::forward<F>(f),
             ... wrapped_args = std::forward<Args>(args) ]<typename... T>(T && ... args) mutable {
        return wrapped_f(std::forward<decltype(wrapped_args)>(wrapped_args)...,
                         std::forward<T>(args)...);
    };
}