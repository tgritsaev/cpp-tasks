#pragma once

#include <functional>
#include <boost/context/continuation.hpp>
#include <iostream>

namespace ctx = boost::context;

class Coroutine;

thread_local Coroutine* current_coroutine = nullptr;

class Coroutine {
public:
    explicit Coroutine(std::function<void()> f) {
        current_coroutine = this;
        first_ = ctx::callcc([this, f](ctx::continuation&& sink) {
            second_ = std::move(sink);
            f();
            return std::move(second_);
        });
    }

    ~Coroutine() {
        current_coroutine = nullptr;
    }

    void resume() {
        if (can_resume_ == false) {
            throw std::runtime_error("can_resume is false");
        }
        can_resume_ = false;
        first_ = first_.resume();
    }

    void suspend() {
        can_resume_ = true;
        second_ = second_.resume();
    }

private:
    bool can_resume_ = false;
    ctx::continuation first_;
    ctx::continuation second_;
};

void suspend() {
    if (current_coroutine == nullptr) {
        throw std::runtime_error("nullptr");
    }
    current_coroutine->suspend();
}
