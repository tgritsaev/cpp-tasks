#pragma once

#include <cstring>
#include <string>
#include <iostream>

class StringView {
public:
    StringView(const std::string& str, size_t start = 0, size_t size = std::string::npos) {
        if (start > str.size()) {
            start = str.size();
        }
        if (size > str.size() - start) {
            size = str.size() - start;
        }
        start_ = str.data() + start;
        size_ = size;
    }

    StringView(const char* start) : start_(start), size_(std::strlen(start)) {
    }

    StringView(const char* start, size_t size) : start_(start), size_(size) {
    }

    char operator[](size_t index) const {
        return start_[index];
    }

    size_t Size() const {
        return size_;
    }

private:
    const char* start_;
    size_t size_;
};
