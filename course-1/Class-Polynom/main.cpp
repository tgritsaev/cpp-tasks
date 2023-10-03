#include <iostream>
#include <algorithm>
#include <vector>

template <typename T>
class Polynomial {
private:
    std::vector<T> a;

public:
    Polynomial<T>(std::vector<T> b): a(b) {}
    Polynomial<T>(): a({}) {}
    Polynomial<T>(T b): a({b}) {}
    template<typename Iter>
    Polynomial<T>(Iter start, Iter end) {
        for (; start != end; ++start) {
            this->a.push_back(*start);
        }
    }

    bool operator==(const Polynomial<T>& b) const {
        // return a == b.a;
        if (Degree() != b.Degree())
            return false;
        for (int i = 0; i <= Degree(); ++i) {
            if (a[i] != b.a[i])
                return false;
        }
        return true;
    }
    bool operator!=(const Polynomial<T>& b) const {
        // return a != b.a;
        if (Degree() == b.Degree()) {
            for (int i = 0; i <= Degree(); ++i) {
                if (a[i] != b.a[i])
                    return true;
            }
            return false;
        }
        return true;
    }
    Polynomial<T> operator+(const Polynomial<T>& b) const {
        std::vector<T> X(std::max(b.a.size(), a.size()), T(0));
        Polynomial<T> c(X);
        for (size_t i = 0; i < std::max(b.a.size(), a.size()); ++i) {
            if (i < b.a.size())
                c.a[i] += b.a[i];
            if (i < a.size())
                c.a[i] += a[i];
        }
        return c;
    }
    Polynomial<T> operator-(const Polynomial<T>& b) const {
        std::vector<T> X(std::max(b.a.size(), a.size()), T(0));
        Polynomial<T> c(X);
        for (size_t i = 0; i < std::max(b.a.size(), a.size()); ++i) {
            if (i < b.a.size())
                c.a[i] -= b.a[i];
            if (i < a.size())
                c.a[i] += a[i];
        }
        return c;
    }
    Polynomial<T> operator*(const Polynomial<T>& b) const {
        std::vector<T> X(b.a.size() + a.size(), T(0));
        Polynomial<T> c(X);
        for (size_t i = 0; i < a.size(); ++i) {
            for (size_t j = 0; j < b.a.size(); ++j) {
                c.a[i + j] += a[i] * b.a[j];
            }
        }
        while (c.a.size() && c.a.back() == T(0)) {
            c.a.pop_back();
        }
        return c;
    }
    Polynomial<T>& operator+=(const Polynomial<T>& b) {
        if (b.a.size() > a.size())
            a.resize(b.a.size());
        for (size_t i = 0; i < b.a.size(); ++i) {
            a[i] += b[i];
        }
        return *this;
    }
    Polynomial<T>& operator-=(const Polynomial<T>& b) {
        if (b.a.size() > a.size())
            a.resize(b.a.size());
        for (size_t i = 0; i < b.a.size(); ++i) {
            a[i] -= b[i];
        }
        return *this;
    }
    Polynomial<T>& operator*=(const Polynomial<T>& b) {
        *this = *this * b;
        return *this;
    }

    T operator[](size_t x) const {
        if (x >= a.size())
            return 0;
        return a[x];
    }
    T operator()(T x) const {
        T ans = 0, val = x;
        for (size_t i = 0; i < a.size(); ++i) {
            if (i) {
                ans = ans + val * a[i];
                val = val * x;
            } else {
                ans = a[i];
            }
        }
        return ans;
    }

    Polynomial<T> operator&(const Polynomial<T>& oth) const {
        Polynomial<T> f(T(0)), g(T(1));
        for (size_t i = 0; i < a.size(); ++i) {
            f += (a[i] * g);
            g *= oth;
        }
        return f;
    }

    Polynomial<T> operator/(const Polynomial<T>& oth) const {
        Polynomial<T> A(a), ans(0);
        while (A.Degree() >= oth.Degree()) {
            T base = A[A.Degree()] / oth[oth.Degree()];
            std::vector<T> arr(A.Degree() - oth.Degree() + 1, T(0));
            arr.back() = T(1);
            Polynomial<T> val(arr);
            ans += base * val;
            A -= base * (val * oth);
        }
        return ans;
    }

    Polynomial<T> operator%(const Polynomial<T>& oth) const {
        Polynomial<T> A(a);
        while (A.Degree() >= oth.Degree()) {
            T base = A[A.Degree()] / oth[oth.Degree()];
            std::vector<T> arr(A.Degree() - oth.Degree() + 1, T(0));
            arr.back() = T(1);
            Polynomial<T> val(arr);
            A -= base * (val * oth);
        }
        return A;
    }

    Polynomial<T> operator,(const Polynomial<T>& oth) const {
        Polynomial<T> A(a), B(oth.a), C(0);
        if (A.Degree() < B.Degree())
            std::swap(A, B);
        while (B != C) {
            Polynomial<T> c = A % B;
            A = B;
            B = c;
        }
        A = A / A[A.Degree()];
        return A;
    }

    int Degree() const {
        if (a.size() == 0)
            return -1;
        auto it = this->a.end() - 1;
        while (it != a.begin()) {
            if (*it != T(0))
                break;
            --it;
        }
        if (*it == T(0))
            --it;
        return (it - a.begin());
    }
    auto begin() const {
        return a.cbegin();
    }
    auto end() const {
        auto it = a.cbegin();
        return it + (*this).Degree() + 1;
    }
};

template<typename T>
Polynomial<T> operator+(const T x, const Polynomial<T>& b) {
    Polynomial<T> a(x);
    return a + b;
}
template<typename T>
Polynomial<T> operator-(const T x, const Polynomial<T>& b) {
    Polynomial<T> a(x);
    return a - b;
}
template<typename T>
Polynomial<T> operator*(const T x, const Polynomial<T>& b) {
    Polynomial<T> a(x);
    return a * b;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Polynomial<T>& a) {
    bool printed = false;
    for (int i = a.Degree(); i >= 0; --i) {
        if (a[i] == T(0)) continue;
        printed = true;
        if (i) {
            if (a[i] == T(-1)) {
                out << '-';
            } else if (a[i] == T(1)) {
                if (i != a.Degree())
                    out << '+';
            } else if (a[i] > T(0)) {
                if (i != a.Degree())
                    out << '+';
                out << a[i] << "*";
            } else {
                out << a[i] << "*";
            }
            if (i > 1) {
                out << "x^" << i;
            } else {
                out << "x";
            }
        } else {
            if (a[i] > T(0) && i != a.Degree())
                out << '+';
            out << a[i];
        }
    }
    if (!printed) {
        out << 0;
    }
    return out;
}

