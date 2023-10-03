#pragma once
#include <array>
#include <tuple>
#include <optional>
#include <iostream>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
public:
    constexpr ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        for (size_t i = 0; i < sz_; ++i) {
            if (a_[i].first == key) {
                return a_[i].second;
            }
        }
        a_[sz_].first = key;
        return a_[sz_++].second;
    }

    constexpr const V& operator[](const K& key) const {
        for (size_t i = 0; i < sz_; ++i) {
            if (a_[i].first == key) {
                return a_[i].second;
            }
        }
    }

    constexpr bool Erase(const K& key) {
        for (size_t i = 0; i < sz_; ++i) {
            if ((&std::get<0>(a_))[i].first == key) {
                for (size_t j = i; j < sz_; ++j) {
                    (&std::get<0>(a_))[j] = (&std::get<0>(a_))[j + 1];
                }
                --sz_;
                return true;
            }
        }
        return false;
    }

    constexpr bool Find(const K& key) const {
        for (size_t i = 0; i < sz_; ++i) {
            if (a_[i].first == key) {
                return true;
            }
        }
        return false;
    }

    constexpr size_t Size() const {
        return sz_;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        return a_[pos];
    }

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        return a_[pos];
    }

    constexpr void Swap(size_t pos) {
        std::swap(a_[pos], a_[pos + 1]);
    }

private:
    static constexpr size_t kN = 1000;

    size_t sz_ = 0;
    std::array<std::pair<K, V>, kN> a_;
};
