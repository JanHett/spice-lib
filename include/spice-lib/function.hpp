/**
 * \file function.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief Contains implementations of commonly used mathematical functions
 * \version 0.1
 * \date 2020-10-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SPICE_FUNCTIONS
#define SPICE_FUNCTIONS

#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>

namespace spice {

namespace function {

/**
 * \brief Square root of 2 * PI
 * 
 * \todo Make this constexpr as soon as `std::sqrt` is constexpr
 */
const auto sqrt_2_pi = std::sqrt(2 * M_PI);

/**
 * \brief Calculates the gaussian function at the given point
 * 
 * \param std_deviation 
 * \param x 
 * \param y 
 * \return float 
 */
float gaussian(float std_deviation, float x, float y = 0) {
    return (1 / (sqrt_2_pi * std_deviation)) * std::pow(
        M_E,
        -((x * x + y * y) / (2 * std_deviation * std_deviation)));
};

/**
 * \brief Calculate the values of the unary function `fn` over the interval
 * `[begin, end)`
 * 
 * If the steps do not divide this interval evenly, an additional sample is
 * added at the end.
 * 
 * \tparam T_arg type of arguments to `fn`
 * \tparam T_vals type of values of `fn`
 * \tparam Fn unary function type
 * \param fn 
 * \param begin 
 * \param end 
 * \param step 
 * \return std::vector<T> a vector of length `(end - begin) / step`
 */
template<typename T_arg, typename T_vals, typename Fn>
std::vector<T_vals> evaluate_unary(Fn fn, T_arg begin, T_arg end, T_arg step) {
    size_t length = std::ceil((end - begin) / step);

    std::vector<T_vals> fn_vals(length);
    for (size_t i = 0; i < length; ++i) {
        fn_vals[i] = fn(begin + i * step);
    }

    return fn_vals;
}

/**
 * \brief Calculate the values of the binary function `fn` over the area
 * `[(begin_x, begin_y), (end_x, end_y))`
 * 
 * If the steps do not divide this area evenly, an additional column and/or row
 * is added at the end.
 * 
 * \tparam T_arg type of arguments to `fn`
 * \tparam T_vals type of values of `fn`
 * \tparam Fn binary function type
 * \param fn 
 * \param begin_x 
 * \param end_x 
 * \param step_x 
 * \param begin_y 
 * \param end_y 
 * \param step_y 
 * \return std::vector<T> a vector of length `(end - begin) / step`
 */
template<typename T_arg, typename T_vals, typename Fn>
std::vector<T_vals> evaluate_binary(Fn fn,
    T_arg begin_x, T_arg end_x, T_arg step_x,
    T_arg begin_y, T_arg end_y, T_arg step_y)
{
    size_t width = std::ceil((end_x - begin_x) / step_x);
    size_t height = std::ceil((end_y - begin_y) / step_y);

    std::vector<T_vals> fn_vals(width * height);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            fn_vals[y * width + x] = fn(begin_x + x * step_x,
                begin_y + y * step_y);
        }
    }

    return fn_vals;
}

}

}

#endif // SPICE_FUNCTIONS
