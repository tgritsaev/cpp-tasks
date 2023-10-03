#pragma once

// Paste here your implementation of compressed_pair from seminar 2 to use in UniquePtr
#include <cstddef>
#include <type_traits>
#include <utility>

template <class Type, size_t Id, bool = std::is_empty_v<Type> && !std::is_final_v<Type>>
class ComperessedPairOneElement {
   public:
    ComperessedPairOneElement() : value_() {}
    // ComperessedPairOneElement(const Type& value) : value_(value) {
    // }
    template <typename FType>
    ComperessedPairOneElement(FType&& value) : value_(std::forward<FType>(value)) {}
    Type& Get() { return value_; }
    const Type& Get() const { return value_; }

   private:
    Type value_;
};

template <class Type, size_t Id>
struct ComperessedPairOneElement<Type, Id, true> : Type {
   public:
    ComperessedPairOneElement() : Type() {}

    template <typename FType>
    ComperessedPairOneElement(FType&& value) : Type(std::forward<FType>(value)) {}
    Type& Get() { return *this; }
    const Type& Get() const { return *this; }
};

template <typename FirstType, typename SecondType>
class CompressedPair : public ComperessedPairOneElement<FirstType, 0>, public ComperessedPairOneElement<SecondType, 1> {
   public:
    CompressedPair() : ComperessedPairOneElement<FirstType, 0>(), ComperessedPairOneElement<SecondType, 1>() {}
    CompressedPair(const FirstType& first) : ComperessedPairOneElement<FirstType, 0>(first) {}

    template <typename FType, typename SType>
    CompressedPair(FType&& first, SType&& second)
        : ComperessedPairOneElement<FType, 0>(std::forward<FType>(first)),
          ComperessedPairOneElement<SType, 1>(std::forward<SType>(second)) {}

    FirstType& GetFirst() { return ComperessedPairOneElement<FirstType, 0>::Get(); }
    const FirstType& GetFirst() const { return ComperessedPairOneElement<FirstType, 0>::Get(); }
    SecondType& GetSecond() { return ComperessedPairOneElement<SecondType, 1>::Get(); };
    const SecondType& GetSecond() const { return ComperessedPairOneElement<SecondType, 1>::Get(); };
};
