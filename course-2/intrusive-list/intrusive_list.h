#pragma once

#include <algorithm>
#include <iostream>

class ListHook {
public:
    ListHook() : previous_{nullptr}, next_{nullptr} {
    }

    bool IsLinked() const {
        return (previous_ || next_);
    }

    void Unlink() {
        if (previous_ && next_) {
            previous_->next_ = next_;
            next_->previous_ = previous_;
        }
        previous_ = nullptr;
        next_ = nullptr;
    }

    // Must unlink element from list
    ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook&) = delete;

private:
    template <class T>
    friend class List;

    // that helper function might be useful
    void LinkBefore(ListHook* other) {

        auto x = other->previous_;
        if (other->previous_) {
            other->previous_->next_ = this;
        }
        if (other) {
            other->previous_ = this;
        }

        previous_ = x;
        next_ = other;
    }

    // void LinkAfter(ListHook* other) {
    //     is_linked_ = true;
    //     next_ = other;
    // }

    ListHook* previous_{nullptr};
    ListHook* next_{nullptr};
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        Iterator(ListHook* element) : element_(element) {
        }

        Iterator& operator++() {
            element_ = element_->next_;
            return *this;
        }
        Iterator operator++(int) {
            element_ = element_->next_;
            return Iterator(element_->next_);
        }

        T& operator*() const {
            return *static_cast<T*>(element_);
        }
        T* operator->() const {
            return static_cast<T*>(element_);
        }

        bool operator!=(const Iterator& other) const {
            return (element_ != other.element_);
        }
        bool operator==(const Iterator& other) const {
            return element_ == other.element_;
        }

        ListHook* element_;
    };

    List() {
        begin_ = new ListHook();
        end_ = new ListHook();
        begin_->LinkBefore(end_);
    }

    List(const List&) = delete;
    List(List&& other) : begin_(other.begin_), end_(other.end_) {
        other.begin_ = new ListHook();
        other.end_ = new ListHook();
        other.begin_->LinkBefore(other.end_);
    }

    // must unlink all elements from list
    ~List() {
        while (!IsEmpty()) {
            PopBack();
        }
        delete begin_;
        delete end_;
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        if (begin_) {
            delete begin_;
        }
        if (end_) {
            delete end_;
        }
        begin_ = other.begin_;
        end_ = other.end_;
        return *this;
    }

    bool IsEmpty() const {
        return begin_->next_ == end_;
    }
    // that method is allowed to be O(n)
    size_t Size() const {
        size_t size = 0;
        for (Iterator iterator = Begin(); iterator != End(); ++iterator) {
            ++size;
        }
        return size;
    }

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem) {
        elem->LinkBefore(end_);
    }
    void PushFront(T* elem) {
        elem->LinkBefore(begin_->next_);
    }

    T& Front() {
        return *static_cast<T*>(begin_->next_);
    }
    const T& Front() const {
        return *static_cast<T*>(begin_->next_);
    }

    T& Back() {
        return *static_cast<T*>(end_->previous_);
    }
    const T& Back() const {
        return *static_cast<T*>(end_->previous_);
    }

    void PopBack() {
        end_->previous_->Unlink();
    }
    void PopFront() {
        begin_->next_->Unlink();
    }

    Iterator Begin() const {
        return Iterator(begin_->next_);
    }
    Iterator End() const {
        return Iterator(end_);
    }

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element) {
        return Iterator(element);
    }

private:
    ListHook* begin_;
    ListHook* end_;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}
