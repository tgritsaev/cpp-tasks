#include <tuple>
#include <memory>
#include <cstddef>

template <typename T, typename Deleter = std::default_delete<T>>
struct UniquePtr {
private:
    std::tuple<T*, Deleter> Tuple;

public:
    UniquePtr() noexcept {
        std::get<0>(Tuple) = nullptr;
    }
    UniquePtr(T* other) noexcept {
        std::get<0>(Tuple) = other;
    }
    UniquePtr(T* other, Deleter del) noexcept {
        Tuple = {other, del};
    }
    /* UniquePtr(T* other, Deleter del) noexcept {
        Tuple = {other, del};
    } */

    const Deleter& get_deleter() const {
        return std::get<1>(Tuple);
    }
    Deleter& get_deleter() {
        return std::get<1>(Tuple);
    }

    UniquePtr(UniquePtr&& other) {  // NOLINT
        std::get<0>(Tuple) = std::get<0>(other.Tuple);
        other.release();
    }
    UniquePtr& operator=(const std::nullptr_t& other) noexcept {
        std::get<0>(Tuple) = other;
        return *this;
    }
    UniquePtr& operator=(UniquePtr&& other) noexcept {  // OK
        reset(other.release());
        return *this;
    }
    ~UniquePtr() {
        std::get<1>(Tuple)(std::get<0>(Tuple));
        std::get<0>(Tuple) = nullptr;
    }
    T& operator*() const {  // OK
        return *std::get<0>(Tuple);
    }
    T* operator->() const {  // OK
        return std::get<0>(Tuple);
    }
    T* release() {
        T* result = nullptr;
        std::swap(result, std::get<0>(Tuple));
        return result;
    }
    void reset(T* other) noexcept {  // OK
        T* old_ptr = std::get<0>(Tuple);
        std::get<0>(Tuple) = other;
        if (old_ptr) {
            std::get<1>(Tuple)(old_ptr);
        }
    }
    void swap(UniquePtr& other) {  // ok
        std::swap(std::get<0>(Tuple), std::get<0>(other.Tuple));
    }
    T* get() const {  // OK
        return std::get<0>(Tuple);
    }
    explicit operator bool() const {  // OK
        return (std::get<0>(Tuple));
    }

    UniquePtr& operator=(const UniquePtr&) const = delete;  // OK
    UniquePtr(const UniquePtr&) = delete;  // OK
};

