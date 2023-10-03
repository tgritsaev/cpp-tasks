#include <type_traits>
#include <iterator>
#include <cstddef>
#include <utility>

template <typename U, typename V>
concept IsSame = std::is_same_v<U, V>;

template <typename U, typename V>
concept IsNotSame = !std::is_same_v<U, V>;

template <class P, class T>
concept Predicate = requires(P p, T t) {
    { p(t) } -> IsSame<bool>;
};

template <class T>
concept NonVoidSquareBrackets = requires(T t) {
    { t[static_cast<size_t>(0)] } -> IsNotSame<void>;
};

template <typename It>
concept IsRandomAccess = std::is_same_v<std::random_access_iterator_tag,
                                        typename std::iterator_traits<It>::iterator_category>;

template <typename T>
concept RandomAccess = requires(T t) {
    { t.begin() } -> IsRandomAccess;
    { t.end() } -> IsRandomAccess;
};

template <typename T>
concept Indexable = (NonVoidSquareBrackets<T> || RandomAccess<T>);