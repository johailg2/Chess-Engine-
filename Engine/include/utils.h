#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

inline int popcountll(uint64_t x) {
    #ifdef _MSC_VER
    return __popcnt64(x);
    #else
    return __builtin_popcountll(x);
    #endif
}

#endif // UTILS_H
