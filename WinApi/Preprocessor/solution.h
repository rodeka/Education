#ifndef SOLUTION_H
#define SOLUTION_H

#include <type_traits>

#define IS_BASE_OF(A, B) std::is_base_of<B, A>::value

template <typename A, typename B1, typename B2, typename B3, typename B4>
struct ToCode {
    static constexpr int value =
        (IS_BASE_OF(A, B1) ? 1 << 0 : 0) |
        (IS_BASE_OF(A, B2) ? 1 << 1 : 0) |
        (IS_BASE_OF(A, B3) ? 1 << 2 : 0) |
        (IS_BASE_OF(A, B4) ? 1 << 3 : 0);
};

#define TO_CODE(A, B1, B2, B3, B4) ToCode<A, B1, B2, B3, B4>::value

#endif