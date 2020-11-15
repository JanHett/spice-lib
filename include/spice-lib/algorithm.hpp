/**
 * \file algorithm.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief 
 * \version 0.1
 * \date 2020-11-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SPICE_ALGORITHM
#define SPICE_ALGORITHM

#include <type_traits>

namespace spice {

/**
 * \brief Returns the lesser of the given values
 * 
 * \note This implementation is branchless, but returns a new value instead of a
 * reference and only works on floating point and signed integer types.
 * 
 * \tparam T 
 * \param a 
 * \param b 
 * \return T 
 */
template<typename T, std::enable_if_t<std::is_signed_v<T>, int> = 0>
T min (T const & a, T const & b) {
    // 0 if b < a, 1 otherwise
    bool a_smaller_b = a < b;
    return a_smaller_b * a + !a_smaller_b * b;
}

/**
 * \brief Returns the greater of the given values
 * 
 * \note This implementation is branchless, but returns a new value instead of a
 * reference and only works on floating point and signed integer types.
 * 
 * \tparam T 
 * \param a 
 * \param b 
 * \return T 
 */
template<typename T, std::enable_if_t<std::is_signed_v<T>, int> = 0>
T max (T const & a, T const & b) {
    // 0 if a < b, 1 otherwise
    bool a_larger_b = b < a;
    return a_larger_b * a + !a_larger_b * b;
}

/**
 * \brief If v compares less than lo, returns lo; otherwise if hi compares less
 * than v, returns hi; otherwise returns v.
 * 
 * \note This implementation is branchless, but returns a new value instead of a
 * reference and only works on floating point and signed integer types.
 * 
 * \tparam T 
 * \param a 
 * \param b 
 * \return T 
 */
template<typename T, std::enable_if_t<std::is_signed_v<T>, int> = 0>
T clamp (T const & val, T const & lo, T const & hi) {
    return spice::max(spice::min(val, hi), lo);
}

}

#endif // SPICE_ALGORITHM