#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <memory>

struct State {
    size_t ref_count;
    std::vector<std::shared_ptr<std::string>> array;
};

class COWVector {
public:
    COWVector() {
        state_ = new State();
        ++state_->ref_count;
    }
    ~COWVector() {
        --state_->ref_count;
        if (state_->ref_count == 0) {
            delete state_;
        }
    }

    COWVector(const COWVector& other) {
        state_ = other.state_;
        ++state_->ref_count;
    }
    COWVector& operator=(const COWVector& other) {
        if (state_) {
            --state_->ref_count;
            if (state_->ref_count == 0) {
                delete state_;
            }
        }
        state_ = other.state_;
        ++state_->ref_count;
        return *this;
    }

    size_t Size() const {
        return state_->array.size();
    }

    void Resize(size_t size) {
        for (size_t index = 0; index < size; ++index) {
            state_->array.emplace_back(std::make_shared<std::string>(""));
        }
        std::cout << "resized good\n";
    }

    const std::string& Get(size_t at) {
        if (state_->array[at]) {
            return *state_->array[at];
        }
        return "";
    }
    const std::string& Back() {
        if (state_->array.back()) {
            return *state_->array.back();
        }
        return "";
    }

    void PushBack(const std::string& value) {
        --state_->ref_count;
        std::vector<std::shared_ptr<std::string>> new_array(state_->array);
        if (state_->ref_count == 0) {
            delete state_;
        }
        state_ = new State();
        state_->ref_count = 1;
        for (size_t index = 0; index < new_array.size(); ++index) {
            if (new_array[index]) {
                state_->array.emplace_back(new std::string(*new_array[index]));
            } else {
                state_->array.emplace_back(nullptr);
            }
        }
        state_->array.emplace_back(new std::string(value));
    }

    void Set(size_t at, const std::string& value) {
        if (state_->ref_count > 1) {
            --state_->ref_count;
            std::vector<std::shared_ptr<std::string>> new_array(state_->array);
            if (state_->ref_count == 0) {
                delete state_;
            }
            state_ = new State();
            state_->ref_count = 1;
            for (size_t index = 0; index < new_array.size(); ++index) {
                if (index != at && new_array[index]) {
                    state_->array.emplace_back(new std::string(*new_array[index]));
                } else {
                    state_->array.emplace_back(nullptr);
                }
            }
            auto pos = &state_->array[at];
            std::cout << "?1 " << pos << '\n';
            std::cout << state_->array.size() << '\n';
            if (state_->array[at]) {
                std::cout << "1.1\n";
                *state_->array[at] = value;
            } else {
                std::cout << "1.2\n";
                state_->array[at] = std::make_shared<std::string>(std::string(value));
            }
            auto pos2 = &state_->array[at];
            std::cout << "?2 " << pos2 << '\n';
        } else {
            std::cout << "ref_count=1\n" << state_->array[at] << " " << *state_->array[at] << '\n';
            *state_->array[at] = value;
            std::cout << state_->array[at] << " " << *state_->array[at] << '\n';
        }
    }

private:
    State* state_;
};