#pragma once

#include <type_traits>
#include <cstddef>
#include <utility>

template <class Type, size_t Id, bool = std::is_empty_v<Type> && !std::is_final_v<Type>>
class ComperessedPairOneElement {
public:
    ComperessedPairOneElement() : value_() {
    }
    ComperessedPairOneElement(const Type& value) : value_(value) {
    }
    ComperessedPairOneElement(Type&& value) : value_(std::move(value)) {
    }
    Type& Get() {
        return value_;
    }
    const Type& Get() const {
        return value_;
    }

private:
    Type value_;
};

template <class Type, size_t Id>
struct ComperessedPairOneElement<Type, Id, true> : Type {
public:
    ComperessedPairOneElement() : Type() {
    }
    ComperessedPairOneElement(const Type& value) : Type(value) {
    }
    ComperessedPairOneElement(Type&& value) : Type(std::move(value)) {
    }
    Type& Get() {
        return *this;
    }
    const Type& Get() const {
        return *this;
    }
};

template <typename FirstType, typename SecondType>
class CompressedPair : public ComperessedPairOneElement<FirstType, 0>,
                       public ComperessedPairOneElement<SecondType, 1> {
public:
    CompressedPair()
        : ComperessedPairOneElement<FirstType, 0>(), ComperessedPairOneElement<SecondType, 1>() {
    }
    CompressedPair(const FirstType& first, const SecondType& second)
        : ComperessedPairOneElement<FirstType, 0>(first),
          ComperessedPairOneElement<SecondType, 1>(second) {
    }
    CompressedPair(FirstType&& first, const SecondType& second)
        : ComperessedPairOneElement<FirstType, 0>(std::move(first)),
          ComperessedPairOneElement<SecondType, 1>(second) {
    }
    CompressedPair(const FirstType& first, SecondType&& second)
        : ComperessedPairOneElement<FirstType, 0>(first),
          ComperessedPairOneElement<SecondType, 1>(std::move(second)) {
    }
    CompressedPair(FirstType&& first, SecondType&& second)
        : ComperessedPairOneElement<FirstType, 0>(std::move(first)),
          ComperessedPairOneElement<SecondType, 1>(std::move(second)) {
    }
    FirstType& GetFirst() {
        return ComperessedPairOneElement<FirstType, 0>::Get();
    }
    const FirstType& GetFirst() const {
        return ComperessedPairOneElement<FirstType, 0>::Get();
    }
    SecondType& GetSecond() {
        return ComperessedPairOneElement<SecondType, 1>::Get();
    };
    const SecondType& GetSecond() const {
        return ComperessedPairOneElement<SecondType, 1>::Get();
    };
};