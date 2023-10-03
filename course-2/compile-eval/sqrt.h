
// #define int long long int

template <bool, class T, class F>
struct If {};

template <class T, class F>
struct If<true, T, F> {
    static const int value = T::value;
};

template <class T, class F>
struct If<false, T, F> {
    static const int value = F::value;
};

template <int V>
struct Value {
    static const int value = V;
};

template <int N, int L = 0, int R = 31623 + 10>
struct Sqrt {
    static const int M = (L + R) / 2;
    static const int value =
        If<L + 1 == R, Value<R>, If<(M * M < N), Sqrt<N, M, R>, Sqrt<N, L, M> > >::value;
};
