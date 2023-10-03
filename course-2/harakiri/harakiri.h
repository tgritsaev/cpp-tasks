#pragma once

#include <string>
#include <iostream>

// Should not allow reuse and yell under sanitizers.
// Fix the interface and implementation.
// AwesomeCallback should add "awesomeness".

class OneTimeCallback {
public:
    virtual ~OneTimeCallback() {
    }
    virtual std::string operator()() & = delete;
    virtual std::string operator()() const&& = 0;
};

// Implement ctor, operator(), maybe something else...
class AwesomeCallback : public OneTimeCallback {
public:
    AwesomeCallback(std::string str) : str_(std::move(str) + "awesomeness") {
        std::cout << "!!" << str_ << '\n';
    }

    virtual std::string operator()() const&& override {
        std::string ans = std::move(str_);
        delete this;
        return ans;
    }

    std::string str_;
};
