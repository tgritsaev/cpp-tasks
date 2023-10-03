#pragma once

#include <type_traits>
#include "algo_impl.h"

/// Use one of the following:
/// AlgoRes ComputeIntegral(data);
/// AlgoRes ComputeUnsignedIntegral(data);
/// AlgoRes ComputeFloat(data);
/// AlgoRes ComputeGeneral(data);

template <class T>
AlgoRes Run(T data) {
    if (std::is_same_v<T, unsigned>) {
        return ComputeUnsignedIntegral(data);
    } else if (std::is_same_v<T, signed>) {
        return ComputeIntegral(data);
    } else if (std::is_same_v<T, Int128> || std::is_same_v<T, Int256>) {
        return ComputeGeneral(data);
    } else {
        return ComputeFloat(data);
    }
}
