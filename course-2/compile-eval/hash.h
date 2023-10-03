#pragma once
#include <iostream>
#include <string>

constexpr int hash2(const char* s, int pos, long long st, int p, int mod) {
    return (s[pos] == '\0' ? 0
                           : (static_cast<long long>(s[pos] * st) +
                              hash2(s, pos + 1, (static_cast<long long>(st) * p) % mod, p, mod))) %
           mod;
}

constexpr int hash(const char* s, int p, int mod) {
    return (*s == 0 ? 0 : hash2(s, 0, 1, p, mod));
}
