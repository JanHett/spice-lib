/**
 * \file convolution.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief Contains convolution functions
 * \version 0.1
 * \date 2020-10-23
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef SPICE_CONVOLUTION
#define SPICE_CONVOLUTION

#include <type_traits>
#include <vector>

#include <fftw3.h>

#include <spatial_convolution_halide.h>

#include "image.hpp"

namespace spice {

namespace convolve {

/**
 * \brief Convolves `img` with `filter`
 * 
 * \tparam T 
 * \tparam Channels 
 * \tparam Filter_Channels 
 * \param img 
 * \param filter 
 * \return image<T, Channels> 
 */
template<typename T, size_t Channels, size_t Filter_Channels = Channels>
image<T, Channels> spatial(image<T, Channels> img,
    image<T, Filter_Channels> filter)
{
    static_assert(Filter_Channels == Channels || Filter_Channels == 1,
        "The filter must either have a single channel or the same number as the filtered image");

    image<T, Channels> output(img.width(), img.height());

    Halide::Runtime::Buffer<float> in_buf =
        Halide::Runtime::Buffer<float>(
            img.data(),
            img.width(),
            img.height(),
            img.channels());

    Halide::Runtime::Buffer<float> filter_buf =
        Halide::Runtime::Buffer<float>(
            filter.data(),
            filter.width(),
            filter.height(),
            filter.channels());

    Halide::Runtime::Buffer<float> out_buf =
        Halide::Runtime::Buffer<float>(
            output.data(),
            output.width(),
            output.height(),
            output.channels());

    auto success = spatial_convolution_halide(in_buf, filter_buf, out_buf);

    if (success != 0)
        std::cout << "Failed to apply adaptive threshold\n";

    return output;
}

/**
 * \brief Convolves `img` horizontally with `filter_h` and vertically with
 * `filter_v`
 * 
 * `filter_h` is assumed to be of height 1 and `filter_v` is assumed to be of
 * width 1.
 * 
 * \tparam T 
 * \tparam Channels 
 * \tparam Filter_Channels 
 * \param img 
 * \param filter 
 * \return image<T, Channels> 
 */
template<typename T, size_t Channels, size_t Filter_Channels = Channels>
image<T, Channels> separable(image<T, Channels> img,
    image<T, Filter_Channels> filter_h,
    image<T, Filter_Channels> filter_v)
{
    static_assert(Filter_Channels == Channels || Filter_Channels == 1,
        "The filter must either have a single channel or the same number as the filtered image");

    return spatial(spatial(img, filter_h), filter_v);
}

/**
 * \brief Convolves `img` with `filter` by separating `filter` into two vectors
 * `Fv` and `Fh` such that `filter == mul(Fv, Fh)`
 * 
 * This function assumes that the passed kernel is separable, i.e. has rank 1
 * 
 * \tparam T 
 * \tparam Channels 
 * \tparam Filter_Channels 
 * \param img 
 * \param filter 
 * \return image<T, Channels> 
 */
template<typename T, size_t Channels, size_t Filter_Channels = Channels>
image<T, Channels> separable(image<T, Channels> img,
    image<T, Filter_Channels> filter)
{
    static_assert(Filter_Channels == Channels || Filter_Channels == 1,
        "The filter must either have a single channel or the same number as the filtered image");

    // possible improvement:
    // check if filter Matrix has rank 1 and throw error if not the case?
    // That's relatively expensive and should be optional if implemented

    image<T, Filter_Channels> filter_h (filter.width(), 1);
    image<T, Filter_Channels> filter_v (1, filter.height());

    for (size_t c = 0; c < Filter_Channels; ++c) {
        // find any non-zero element `E` in the kernel
        size_t x, y;
        float e;
        for (y = 0; y < filter.height(); ++y) {
            for (x = 0; x < filter.width(); ++x) {
                if (filter(x, y, c) != 0) {
                    e = filter(x, y, c);
                    goto found_e;
                }
            }
        }
        found_e:
        // let the vertical kernel be the column of e and the horizontal one the
        // row divided by `e`
        for (size_t k_x = 0; k_x < filter.width(); ++k_x) {
            filter_h(k_x, y, c) = filter(k_x, y, c) / e;
        }
        for (size_t k_y = 0; k_y < filter.height(); ++k_y) {
            filter_v(x, k_y, c) = filter(x, k_y, c);
        }

        return separable(img, filter_h, filter_v);
    }
}

/**
 * \brief Convolves `img` with `filter` by multiplying the two in frequency
 * space
 * 
 * \tparam T 
 * \tparam Channels 
 * \tparam Filter_Channels 
 * \param img 
 * \param filter 
 * \return image<T, Channels> 
 */
// template<typename T, size_t Channels, size_t Filter_Channels = Channels>
// image<T, Channels> fft_based(image<T, Channels> img,
//     image<T, Filter_Channels> filter)
// {
//     static_assert(Filter_Channels == Channels || Filter_Channels == 1,
//         "The filter must either have a single channel or the same number as " +
//         "the filtered image");

//     fftw_complex * img_out = fftw_malloc(sizeof(fftw_complex) * img.width() * img.height());
//     fftw_plan p = fftw_plan_dft_r2c_2d(img.width(), img.height(),
//         img.data(), img_out,
//         FFTW_ESTIMATE);

//     image<T, Channels>

//     fftw_free(img_out);

//     // extend img to have dimensions L1 * L2, i.e. img.width + filter.width - 1 * img.height + filter.height - 1
//     // take filter to be centered at 0, repeat with period L1 * L2 and oad the rest woth zeros
//     // take DFT of both
//     // multiply
//     // take inverse dft of result
//     // chop off filter.width - 1/filter.height - 1
// }

}

}

#endif // SPICE_CONVOLUTION
