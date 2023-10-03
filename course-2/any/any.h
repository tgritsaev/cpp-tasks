#pragma once
#include <iostream>
#include <memory>

class Any {
public:
    class InnerBase {
    public:
        virtual ~InnerBase() = default;

        virtual InnerBase* Copy() = 0;
    };

    template <typename T>
    class Inner : public InnerBase {
    public:
        Inner(const T& val) : val_(val) {
        }
        // Inner(T&& val) : val_(std::move(val)) {
        // }
        InnerBase* Copy() override {
            return new Inner<T>(val_);
        }
        T val_;
    };

    Any() {
    }

    template <class T>
    Any(const T& value) : ptr_(new Inner<T>(value)) {
    }

    template <class T>
    Any& operator=(const T& value) {
        std::cout << "in copy\n";
        Any inplace_element(value);
        Swap(inplace_element);
        return *this;
    }

    Any(const Any& rhs) {
        ptr_.reset(rhs.ptr_->Copy());
    }

    Any& operator=(const Any& rhs) {
        Any inplace_element(rhs);
        Swap(inplace_element);
        return *this;
    }

    ~Any() {
    }

    bool Empty() const {
        return (ptr_ == nullptr);
    }

    void Clear() {
        ptr_.reset();
    }

    void Swap(Any& rhs) {
        std::swap(ptr_, rhs.ptr_);
    }

    template <class T>
    const T& GetValue() const {
        // try {
        return (dynamic_cast<Inner<T>&>(*ptr_)).val_;
        // } catch (...) {
        //     throw std::bad_cast();
        // }
    }

private:
    std::unique_ptr<InnerBase> ptr_;
};
