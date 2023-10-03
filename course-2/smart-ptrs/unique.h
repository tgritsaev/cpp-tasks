#pragma once

#include <cstddef>  // std::nullptr_t
#include <iostream>

#include "compressed_pair.h"

template <typename T>
class DefaultDeleter {
   public:
    void operator()(T* value) { delete value; }
};

template <typename T>
class DefaultDeleter<T[]> {
   public:
    void operator()(T* value) { delete[] value; }
};

// Primary template
template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
   public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : value_(ptr) {}
    UniquePtr(T* ptr, Deleter deleter) : value_(std::forward<T*>(ptr), std::forward<Deleter>(deleter)) {}

    UniquePtr(UniquePtr& ptr) = delete;

    UniquePtr(UniquePtr&& other) noexcept {
        if (other.value_.GetFirst() != value_.GetFirst()) {
            value_.GetFirst() = other.value_.GetFirst();
            value_.GetSecond() = std::forward<Deleter>(other.value_.GetSecond());
            other.value_.GetFirst() = nullptr;
        }
    }

    template <typename Fp, typename Fd = DefaultDeleter<Fp>>
    UniquePtr(UniquePtr<Fp, Fd>&& other) {
        value_.GetFirst() = other.value_.GetFirst();
        // value_.GetSecond() = std::forward<Fd>(other.value_.GetSecond());
        other.value_.GetFirst() = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (other.value_.GetFirst() != value_.GetFirst()) {
            (*this).~UniquePtr();
            value_.GetFirst() = other.value_.GetFirst();
            value_.GetSecond() = std::forward<Deleter>(other.value_.GetSecond());
            other.value_.GetFirst() = nullptr;
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        (*this).~UniquePtr();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (value_.GetFirst()) {
            value_.GetSecond()(value_.GetFirst());
            value_.GetFirst() = nullptr;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* value = value_.GetFirst();
        value_.GetFirst() = nullptr;
        return value;
    }
    void Reset(T* ptr = nullptr) {
        T* old_ptr = value_.GetFirst();
        value_.GetFirst() = ptr;
        if (old_ptr) {
            value_.GetSecond()(old_ptr);
        }
    }
    void Swap(UniquePtr& other) { std::swap(value_.GetFirst(), other.value_.GetFirst()); }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const { return value_.GetFirst(); }
    Deleter& GetDeleter() { return value_.GetSecond(); }
    const Deleter& GetDeleter() const { return value_.GetSecond(); }

    explicit operator bool() const { return (value_.GetFirst() ? true : false); }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const { return *value_.GetFirst(); }

    T* operator->() const { return value_.GetFirst(); }

   private:
    template <typename Fp, typename Fd>
    friend class UniquePtr;

    CompressedPair<T*, Deleter> value_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
   public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : value_(ptr) {}
    explicit UniquePtr(T* ptr, Deleter deleter) : value_(std::forward<T*>(ptr), std::forward<Deleter>(deleter)) {}

    UniquePtr(UniquePtr& ptr) = delete;

    explicit UniquePtr(UniquePtr&& other) noexcept {
        if (other.value_.GetFirst() != value_.GetFirst()) {
            value_.GetFirst() = other.value_.GetFirst();
            value_.GetSecond() = std::forward<Deleter>(other.value_.GetSecond());
            other.value_.GetFirst() = nullptr;
        }
    }

    template <typename Fp, typename Fd = DefaultDeleter<Fp>>
    UniquePtr(UniquePtr<Fp, Fd>&& other) {
        value_.GetFirst() = other.value_.GetFirst();
        // value_.GetSecond() = std::forward<Fd>(other.value_.GetSecond());
        other.value_.GetFirst() = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (other.value_.GetFirst() != value_.GetFirst()) {
            (*this).~UniquePtr();
            value_.GetFirst() = other.value_.GetFirst();
            value_.GetSecond() = std::forward<Deleter>(other.value_.GetSecond());
            other.value_.GetFirst() = nullptr;
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        (*this).~UniquePtr();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (value_.GetFirst()) {
            value_.GetSecond()(value_.GetFirst());
            value_.GetFirst() = nullptr;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* value = value_.GetFirst();
        value_.GetFirst() = nullptr;
        return value;
    }
    void Reset(T* ptr = nullptr) {
        T* old_ptr = value_.GetFirst();
        value_.GetFirst() = ptr;
        if (old_ptr) {
            value_.GetSecond()(old_ptr);
        }
    }
    void Swap(UniquePtr& other) { std::swap(value_.GetFirst(), other.value_.GetFirst()); }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const { return value_.GetFirst(); }
    Deleter& GetDeleter() { return value_.GetSecond(); }
    const Deleter& GetDeleter() const { return value_.GetSecond(); }

    explicit operator bool() const { return (value_.GetFirst() ? true : false); }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    T& operator[](size_t i) { return *(value_.GetFirst() + i); }

    const T operator[](size_t i) const { return (value_.GetFirst() + i); }

   private:
    template <typename Fp, typename Fd>
    friend class UniquePtr;

    CompressedPair<T*, Deleter> value_;
};
