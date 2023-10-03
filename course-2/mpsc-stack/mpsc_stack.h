#pragma once

#include <iostream>
#include <atomic>
#include <optional>
#include <stdexcept>
#include <utility>

template <class T>
class MPSCStack {
public:
    // Push adds one element to stack top.
    //
    // Safe to call from multiple threads.
    void Push(const T& value) {
        Node* new_head = new Node(value, head_);
        while (head_.compare_exchange_weak(new_head->prev, new_head) == false) {
        }
    }

    // Pop removes top element from the stack.
    //
    // Not safe to call concurrently.
    std::optional<T> Pop() {
        if (head_ == nullptr) {
            return std::nullopt;
        }
        Node* prev_head = head_;
        while (head_.compare_exchange_weak(prev_head, prev_head->prev) == false) {
        }
        T value = prev_head->value;
        delete prev_head;
        return value;
    }

    // DequeuedAll Pop's all elements from the stack and calls cb() for each.
    //
    // Not safe to call concurrently with Pop()
    template <class TFn>
    void DequeueAll(const TFn& cb) {
        std::optional<T> a;
        while ((a = Pop()) != std::nullopt) {
            // std::cout << "call" << std::endl;
            cb(*a);
        }
    }

    ~MPSCStack() {
        auto fake_func = [](T& el) { (void)el; };
        DequeueAll(fake_func);
    }

private:
    struct Node {
        Node(T v, Node* n) : value(v), prev(n) {
        }

        T value;
        Node* prev;
    };

    std::atomic<Node*> head_{nullptr};
};
