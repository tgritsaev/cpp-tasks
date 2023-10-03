#pragma once

constexpr int next_prime(int x) {
    if (x == 1) {
        ++x;
    }
    int y = x;
    bool is_prime = false;
    while (is_prime == false) {
        is_prime = true;
        for (int d = 2; d * d <= y; ++d) {
            if (y % d == 0) {
                is_prime = false;
            }
        }
        if (is_prime == false) {
            ++y;
        }
    }
    return y;
}
