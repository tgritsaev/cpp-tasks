#pragma once

#include <memory>
#include <string>
#include <type_traits>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int value) : value_(value) {
    }

    int GetValue() const {
        return value_;
    }

    int value_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& symbol) : symbol_(symbol) {
    }

    const std::string& GetName() const {
        return symbol_;
    }

    std::string symbol_;
};

class Cell : public Object {
public:
    Cell() {
    }

    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    std::shared_ptr<Object> first_{nullptr};
    std::shared_ptr<Object> second_{nullptr};
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return (std::dynamic_pointer_cast<T>(obj) ? true : false);
}
