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

#ifndef SPICE_FUNCTION
#define SPICE_FUNCTION

#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>

namespace spice {

/**
 * \brief Contains implementations of commonly used mathematical functions
 * 
 */
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
 * The implemented function is a simplified gaussian function suitable for use
 * as an impulse response for convolution:
 * 
 * \f[
 * g(x, y) = \frac{1}{2\pi\sigma^2} \cdot e^{\frac{x^2}{2\sigma^2}}
 * \f]
 * 
 * where \f$\sigma\f$ is the standard deviation and \f$e\f$ is Euler's constant.
 * 
 * \param std_deviation 
 * \param x 
 * \return float 
 */
inline float gaussian(float std_deviation, float x) {
    return (1 / (sqrt_2_pi * std_deviation)) * std::pow(
        M_E,
        -((x * x) / (2 * std_deviation * std_deviation)));
};

/**
 * \brief Calculates the gaussian function at the given point
 * 
 * The implemented function is a simplified gaussian function suitable for use
 * as an impulse response for convolution:
 * 
 * \f[
 * g(x, y) = \frac{1}{2\pi\sigma^2} \cdot e^{\frac{x^2 + y^2}{2\sigma^2}}
 * \f]
 * 
 * where \f$\sigma\f$ is the standard deviation and \f$e\f$ is Euler's constant.
 * 
 * \param std_deviation 
 * \param x 
 * \param y 
 * \return float 
 */
inline float gaussian(float std_deviation, float x, float y) {
    return (1 / (2 * M_PI * std_deviation * std_deviation)) * std::pow(
        M_E,
        -((x * x + y * y) / (2 * std_deviation * std_deviation)));
};

/**
 * \brief Calculate the values of the unary function `fn` over the interval
 * \f$[begin, end)\f$
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
 * \param fn_vals a buffer of length `std::ceil((end - begin) / step)` to
 * write the result to
 */
template<typename T_arg, typename T_vals, typename Fn>
void evaluate_unary(Fn fn, T_arg begin, T_arg end, T_arg step,
    T_vals * const fn_vals)
{
    size_t length = std::ceil((end - begin) / step);

    for (size_t i = 0; i < length; ++i) {
        fn_vals[i] = fn(begin + i * step);
    }
}

/**
 * \brief Calculate the values of the unary function `fn` over the interval
 * \f$[begin, end)\f$
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
 * \return std::vector<T> a vector of length `std::ceil((end - begin) / step)`
 */
template<typename T_arg, typename T_vals, typename Fn>
std::vector<T_vals> evaluate_unary(Fn fn, T_arg begin, T_arg end, T_arg step) {
    size_t length = std::ceil((end - begin) / step);
    std::vector<T_vals> fn_vals(length);
    evaluate_unary(fn, begin, end, step, fn_vals.data());
    return fn_vals;
}

/**
 * \brief Calculate the values of the binary function `fn` over the area
 * \f$[(begin\_x, begin\_y), (end\_x, end\_y))\f$
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
 * \param fn_vals a buffer of length `std::ceil((end_x - begin_x) / step_x) *
 * std::ceil((end_y - begin_y) / step_y)` to write the result to
 */
template<typename T_arg, typename T_vals, typename Fn>
void evaluate_binary(Fn fn,
    T_arg begin_x, T_arg end_x, T_arg step_x,
    T_arg begin_y, T_arg end_y, T_arg step_y,
    T_vals * const fn_vals)
{
    size_t width = std::ceil((end_x - begin_x) / step_x);
    size_t height = std::ceil((end_y - begin_y) / step_y);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            fn_vals[y * width + x] = fn(begin_x + x * step_x,
                begin_y + y * step_y);
        }
    }
}

/**
 * \brief Calculate the values of the binary function `fn` over the area
 * \f$[(begin\_x, begin\_y), (end\_x, end\_y))\f$
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
 * \return std::vector<T> a vector of length `std::ceil((end_x - begin_x) /
 * step_x) * std::ceil((end_y - begin_y) / step_y)`
 */
template<typename T_arg, typename T_vals, typename Fn>
std::vector<T_vals> evaluate_binary(Fn fn,
    T_arg begin_x, T_arg end_x, T_arg step_x,
    T_arg begin_y, T_arg end_y, T_arg step_y)
{
    size_t width = std::ceil((end_x - begin_x) / step_x);
    size_t height = std::ceil((end_y - begin_y) / step_y);

    std::vector<T_vals> fn_vals(width * height);
    evaluate_binary(fn,
        begin_x, end_x, step_x,
        begin_y, end_y, step_y,
        fn_vals.data());

    return fn_vals;
}

}

}

#endif // SPICE_FUNCTION