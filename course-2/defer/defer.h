#pragma once
#include <algorithm>
#include <iostream>

template <typename Callback>
class CallbackStorage {
public:
    explicit CallbackStorage(Callback callback) {
        ::new (GetCallbackBuffer()) Callback(std::move(callback));
    }

    void* GetCallbackBuffer() {
        return static_cast<void*>(callback_buffer_);
    }

    Callback& GetCallback() {
        return *reinterpret_cast<Callback*>(GetCallbackBuffer());
    }

    ~CallbackStorage() {
        if (!called_incomplete_detructor_) {
            reinterpret_cast<Callback*>(GetCallbackBuffer())->~Callback();
        }
    }

    void IncompleteDestructor() {
        if (!called_incomplete_detructor_) {
            reinterpret_cast<Callback*>(GetCallbackBuffer())->~Callback();
            called_incomplete_detructor_ = true;
        }
    }

private:
    alignas(Callback) char callback_buffer_[sizeof(Callback)];
    bool called_incomplete_detructor_{false};
};

template <typename Callback>
class Defer final {
public:
    Defer(Callback&& callback) : callback_storage_(std::forward<Callback>(callback)) {
    }

    void Invoke() {
        std::move(callback_storage_.GetCallback())();
        is_deleted_ = true;
    }

    void Cancel() {
        is_deleted_ = true;
        callback_storage_.IncompleteDestructor();
    }

    ~Defer() {
        if (!is_deleted_) {
            std::forward<Callback>(callback_storage_.GetCallback())();
        }
    }

private:
    CallbackStorage<Callback> callback_storage_;
    bool is_deleted_{false};
};
