#pragma once

template <class T>
struct Pipeline {
    Pipeline(T f) : f(f) {
    }

    constexpr auto operator|(auto f) const {
        auto right = [*this, f]<typename F>(F&& arg) {
            auto x = this->f(arg);
            return f(x);
        };
        return Pipeline<decltype(right)>(right);
    }

    template <class F>
    constexpr auto operator()(F&& arg) const {
        return f(arg);
    }

    T f{};
};

struct EmptyPlHelper {
    constexpr auto operator|(auto f) const {
        return Pipeline(f);
    }
};

constexpr auto kEmptyPl{EmptyPlHelper()};