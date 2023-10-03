#include <memory>
#include <cstddef>
#include <utility>

class Vector {
private:
    int* a;
    size_t sz, cp;

    static int* Allocate(size_t n) {
        return static_cast<int*>(operator new(n * sizeof(int)));
    }
    static void Deallocate(int* buf) {
        operator delete(buf);
    }

    static void Constructor(void * buf) {
        new (buf) (int)();  // NOLINT
    }
    static void Constructor(void * buf, const int& elem) {
        new (buf) (int)(elem);  // NOLINT
    }
    /*static void Constructor(void * buf, int&& elem) {
        new (buf) int(std::move(elem));
    }
    static void Destroy(int * buf) {
        buf->~T();
    }*/

public:
    int& operator[](size_t i) {
        return a[i];
    }
    const int& operator[](size_t i) const {
        return a[i];
    }
    size_t size() const {
        return sz;
    }
    void push_back(const int& elem) {
        if (sz == cp) {
            if (sz == 0)
                Reserve(1);
            else
                Reserve(2 * sz);
        }
        a[sz] = int(elem);  // NOLINT
        ++sz;
    }
    void pop_back() {  // don't forget destroy
        std::destroy_at(a + sz - 1);
        --sz;
    }
    Vector& operator=(const Vector& other) {
        // Deallocate(a);
        cp = sz = 0;

        Reserve(other.sz);
        for (size_t i = 0; i != other.sz; ++i) {
            a[i] = (other[i]);  // NOLINT
        }
        cp = sz = other.sz;
        return *this;
    }

    explicit Vector(size_t n = 0) {
        a = Allocate(n);
        for (size_t i = 0; i != n; ++i) {
            Constructor(a + i);
        }
        sz = n;
        cp = n;
    }
    explicit Vector(const Vector& other) {
        a = Allocate(other.sz);
        for (size_t i = 0; i != other.sz; ++i) {
            Constructor(a + i, other[i]);
        }
        sz = cp = other.sz;
    }
    ~Vector() {  // don't forget Destroy
        for (size_t i = 0; i != cp; ++i) {
            // delete (a + i);
        }
        Deallocate(a);
    }
    void Reserve(size_t n) {  // don't forget Destroy
        if (n > cp) {
            auto b = Allocate(n);
            for (size_t i = 0; i != sz; ++i) {
                Constructor(b + i, a[i]);  // std::move
                // delete (a + i);
            }
            Deallocate(a);
            a = b;
            cp = n;
        }
    }
};