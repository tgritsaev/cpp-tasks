#pragma once
#include <iostream>
#include <vector>

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    T result = init;
    for (; first != last; ++first) {
        result = func(result, *first);
    }
    return result;
}

struct Sum {
    int operator()(int a, int b) {
        return a + b;
    }
};

struct Prod {
    int operator()(int a, int b) {
        return a * b;
    }
};

struct Concat {
    std::vector<int> operator()(const std::vector<int>& a, const std::vector<int>& b) {
        std::vector<int> result;
        auto add = [&result](const std::vector<int>& a) {
            for (const auto& el : a) {
                result.push_back(el);
            }
        };
        add(a);
        add(b);
        return result;
    }
};

class Length {
public:
    Length(int* observed_size) {
        std::cout << observed_size << " " << size_ << '\n';
        size_ = observed_size;
        std::cout << observed_size << " " << size_ << '\n';
    }

    template <typename T>
    T operator()(const T& a, const T& /*b*/) {
        ++*size_;
        std::cout << size_ << '\n';
        return a;
    }

private:
    int* size_;
};
