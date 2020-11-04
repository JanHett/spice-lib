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
 * \brief Convolves `img` horizontally with `filter_h` and vertically with
 * `filter_v`
 * 
 * \tparam T 
 * \tparam Channels 
 * \tparam Filter_Channels 
 * \param img 
 * \param filter 
 * \return image<T, Channels> 
 */
// template<typename T, size_t Channels, size_t Filter_Channels = Channels>
// image<T, Channels> separable(image<T, Channels> img,
//     std::vector<T> filter_h[Filter_Channels],
//     std::vector<T> filter_v[Filter_Channels])
// {
//     static_assert(Filter_Channels == Channels || Filter_Channels == 1,
//         "The filter must either have a single channel or the same number as " +
//         "the filtered image");

    
// }

/**
 * \brief Convolves `img` with `filter` by separating `filter` into two vectors
 * `Fv` and `Fh` such that `filter == mul(Fv, Fh)`
 * 
 * \tparam T 
 * \tparam Channels 
 * \tparam Filter_Channels 
 * \param img 
 * \param filter 
 * \return image<T, Channels> 
 */
// template<typename T, size_t Channels, size_t Filter_Channels = Channels>
// image<T, Channels> separable(image<T, Channels> img,
//     image<T, Filter_Channels> filter)
// {
//     static_assert(Filter_Channels == Channels || Filter_Channels == 1,
//         "The filter must either have a single channel or the same number as " +
//         "the filtered image");
// }

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
