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
#include <complex>
#include <numeric>  // for debugging - remove when done

#include <fftw3.h>

#include <spatial_convolution_halide.h>
#include <multiply_complex_buffers_halide.h>

#include "image.hpp"
#include "algorithm.hpp"
#include "print.hpp"

namespace spice {

namespace convolve {

// Have we successfully initialised fftw threads yet?
// TODO: move this to a more appropriate place
// TODO: make this work
// int fftw_threads_status = 0;

/**
 * \brief Convolves `img` with `filter`
 * 
 * Keep in mind that straight up spatial convolution is expensive. If the filter
 * is separable, i.e. the kernel matrix has a rank of 1, consider using
 * `convolution::separable` instead.
 * 
 * This function exists mainly as a generalised implementation to be used in the
 * imlpementations of `convolution::separable`.
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
    // Check if filter Matrix has rank 1 and throw error if not the case?
    // That's relatively expensive and should be optional if implemented.

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
 * \param nthreads Maximum number of threads to use
 * \return image<T, Channels> 
 *
 * \todo: use fftw_plan_many_dft?
 */
template<typename T, size_t Channels, size_t Filter_Channels = Channels>
image<T, Channels> dft_based(image<T, Channels> img,
    image<T, Filter_Channels> filter,
    size_t n_threads = 1)
{
    static_assert(Filter_Channels == Channels || Filter_Channels == 1,
        "The filter must either have a single channel or the same number as the filtered image");

    using scalar_t = float;

    // set up fftw threads - TODO: make this work
    // if (!fftw_threads_status) {
    //     fftw_threads_status = fftw_init_threads();
    // }
    // fftw_plan_with_nthreads(n_threads);

    // set up buffers and plan for `img`
    auto padded_w = (img.width() + filter.width() - 1) * 2;
    auto padded_h = (img.height() + filter.height() - 1) * 2;

    auto frq_buffer_width = (padded_w / 2 + 1);
    auto frq_buffer_height = padded_h;
    auto frq_buffer_size = frq_buffer_width * frq_buffer_height;
    auto spatial_buffer_size = padded_h * padded_w;

    scalar_t * img_spatial = reinterpret_cast<scalar_t*>(fftwf_malloc(sizeof(scalar_t) *
        spatial_buffer_size));
    std::complex<scalar_t> * img_frequency = reinterpret_cast<std::complex<scalar_t>*>(
        fftwf_malloc(sizeof(std::complex<scalar_t>) *
        frq_buffer_size));

    // forward dft
    fftwf_plan p_img_fwd = fftwf_plan_dft_r2c_2d(padded_h, padded_w,
        img_spatial,
        reinterpret_cast<fftwf_complex*>(img_frequency),
        FFTW_ESTIMATE);

    // inverse dft
    fftwf_plan p_img_inv = fftwf_plan_dft_c2r_2d(padded_h, padded_w,
        reinterpret_cast<fftwf_complex*>(img_frequency),
        img_spatial,
        FFTW_ESTIMATE);
        
    // set up buffers and plan for filter
    scalar_t * filter_spatial = reinterpret_cast<scalar_t*>(fftwf_malloc(sizeof(scalar_t) *
        spatial_buffer_size));
    std::complex<scalar_t> * filter_frequency = reinterpret_cast<std::complex<scalar_t>*>(
        fftwf_malloc(sizeof(std::complex<scalar_t>) *
        frq_buffer_size));

    fftwf_plan p_filter_fwd = fftwf_plan_dft_r2c_2d(padded_h, padded_w,
        filter_spatial,
        reinterpret_cast<fftwf_complex*>(filter_frequency),
        FFTW_ESTIMATE);

    // set up image for result
    image<T, Channels> result(img.width(), img.height());

    // compute dft, multiplication and inverse dft channel-wise
    for (size_t c = 0; c < img.channels(); ++c) {
        // set up the filter if necessary
        if (Filter_Channels == Channels || c == 0) {
            // calculate offset to make filter centred in padded image
            auto offset_x = padded_w / 2 - filter.width() / 2;
            auto offset_y = padded_h / 2 - filter.height() / 2;

            for (size_t y = 0; y < padded_h; ++y) {
                for (size_t x = 0; x < padded_w; ++x) {
                    // using offsets to center the filter in padded_w X padded_h
                    if (x >= offset_x && x < offset_x + filter.width() &&
                        y >= offset_y && y < offset_y + filter.height()) {
                        filter_spatial[y * padded_w + x]
                            = filter(x - offset_x, y - offset_y, c);
                    } else {
                        // all other values are set to 0
                        filter_spatial[y * padded_w + x] = scalar_t{};
                    }
                }
            }

            // FOR DEBUGGING
            // image<T, 1> filter_spatial_buffer(filter_spatial, padded_w,
            //     padded_h);
            // print::image(filter_spatial_buffer, 20);
            // write_image("../data/testing/filter_buffer.jpg", filter_spatial_buffer);
            // std::cout << "gaussian sum: " << std::reduce(
            //     filter_spatial, filter_spatial + spatial_buffer_size) << "\n";

            // apply dft to filter_spatial
            fftwf_execute(p_filter_fwd);

            // FOR DEBUGGING
        //     std::vector<T> filter_frq(frq_buffer_size);
        //     std::transform(filter_frequency,
        //         filter_frequency + frq_buffer_size,
        //         filter_frq.begin(),
        //         [](auto v){ return std::abs(v); });
        //     image<T, 1> filter_frq_buffer(filter_frq.data(), frq_buffer_width,
        //         frq_buffer_height);
        //     print::image(filter_frq_buffer, 20);
        //     write_image("../data/testing/filter_frq.jpg", filter_frq_buffer);
        }

        // copy channel from `img` to img_spatial buffer
        size_t offset_left = (filter.width() - 1) / 2;
        size_t offset_top = (filter.height() - 1) / 2;
        for (int y = 0; y < padded_h; ++y) {
            for (int x = 0; x < padded_w; ++x) {
                // TODO: comparisons in clamp are slowing this down - avoid them
                img_spatial[y * padded_w + x] =
                    img(spice::clamp<int>(x - offset_left, 0, img.width() - 1),
                        spice::clamp<int>(y - offset_top, 0, img.height() - 1),
                        c);
            }
        }

        // FOR DEBUGGING
        // image<T, 1> image_spatial_buffer(img_spatial, padded_w,
        //     padded_h);
        // write_image(
        //     (std::string("../data/testing/img_padded_") +
        //         std::to_string(c) + ".jpg").c_str(),
        //     image_spatial_buffer);
        // print::image(image_spatial_buffer, 20);

        // apply dft to img_spatial
        fftwf_execute(p_img_fwd);

        // FOR DEBUGGING
        // std::vector<T> img_frq(frq_buffer_size);
        // std::transform(img_frequency,
        //     img_frequency + frq_buffer_size,
        //     img_frq.begin(),
        //     [](auto v){ return std::abs(v) / 100; });
        // image<T, 1> img_frq_buffer(img_frq.data(), frq_buffer_width,
        //     frq_buffer_height);
        // // print::image(img_frq_buffer, 20);
        // write_image(
        //     (std::string("../data/testing/img_frq_") +
        //         std::to_string(c) + ".jpg").c_str(),
        //     img_frq_buffer);

        // FOR DEBUGGING
        // fftwf_execute(p_img_inv);
        // std::vector<T> img_spatial_buffer_re_inverted(spatial_buffer_size);
        // std::transform(img_spatial,
        //     img_spatial + spatial_buffer_size,
        //     img_spatial_buffer_re_inverted.begin(),
        //     [&](auto v){ return v / (spatial_buffer_size); });
        // image<T, 1> image_spatial_buffer_re_inverted(
        //     img_spatial_buffer_re_inverted.data(), padded_w, padded_h);
        // // print::image(image_spatial_buffer_re_inverted, 20);
        // write_image(
        //     (std::string("../data/testing/img_re_inverted_") +
        //         std::to_string(c) + ".jpg").c_str(),
        //     image_spatial_buffer_re_inverted);

        // multiply img_frequency with filter_frequency
        // for (size_t i = 0; i < frq_buffer_size; ++i) {
        //     img_frequency[i] *= filter_frequency[i];
        // }
        auto img_frq_buf = Halide::Runtime::Buffer<T>(
            reinterpret_cast<float*>(img_frequency),
            // innermost dimension is fixed size 2: the real and imaginary
            // parts
            2, frq_buffer_size);

        auto filter_frq_buf = Halide::Runtime::Buffer<const T>(
            reinterpret_cast<float*>(filter_frequency),
            // innermost dimension is fixed size 2: the real and imaginary
            // parts
            2, frq_buffer_size);
        multiply_complex_buffers_halide(img_frq_buf, filter_frq_buf,
            img_frq_buf);


        // FOR DEBUGGING
        // std::vector<T> img_frq_mult(frq_buffer_size);
        // std::transform(img_frequency,
        //     img_frequency + frq_buffer_size,
        //     img_frq_mult.begin(),
        //     [](auto v){ return std::abs(v) / 100; });
        // image<T, 1> img_frq_mult_buffer(img_frq_mult.data(), frq_buffer_width,
        //     frq_buffer_height);
        // print::image(img_frq_mult_buffer, 20);
        // write_image(
        //     (std::string("../data/testing/img_frq_mult_") +
        //         std::to_string(c) + ".jpg").c_str(),
        //     img_frq_mult_buffer);

        // compute inverse dft of img_spatial
        fftwf_execute(p_img_inv);

        // FOR DEBUGGING
        // std::vector<T> img_spatial_buffer_final_inverted(spatial_buffer_size);
        // std::transform(img_spatial,
        //     img_spatial + spatial_buffer_size,
        //     img_spatial_buffer_final_inverted.begin(),
        //     [&](auto v){ return v / (spatial_buffer_size); });
        // image<T, 1> image_spatial_buffer_final_inverted(
        //     img_spatial_buffer_final_inverted.data(), padded_w, padded_h);
        // // print::image(image_spatial_buffer_final_inverted, 20);
        // write_image(
        //     (std::string("../data/testing/img_final_inverted_") +
        //         std::to_string(c) + ".jpg").c_str(),
        //     image_spatial_buffer_final_inverted);

        // copy channel from img_spatial to result
        auto offset_x = padded_w / 2 + offset_left;
        auto offset_y = padded_h / 2 + offset_top;
        for (size_t y = 0; y < img.height(); ++y) {
            for (size_t x = 0; x < img.width(); ++x) {
                result(x, y, c) = img_spatial[
                        // offset to get the result from lower-right quadrant
                        (offset_y + y) * padded_w + x + offset_x
                    ] / spatial_buffer_size;
            }
        }
    }

    // free all our plans and temp buffers
    fftwf_destroy_plan(p_img_fwd);
    fftwf_destroy_plan(p_filter_fwd);
    fftwf_free(img_spatial);
    fftwf_free(img_frequency);
    fftwf_free(filter_spatial);
    fftwf_free(filter_frequency);

    // extend img to have dimensions L1 * L2,
    // i.e. img.width + filter.width - 1 * img.height + filter.height - 1
    // take filter to be centered at 0, repeat with period L1 * L2 and pad the rest with zeros
    // take DFT of both
    // multiply
    // take inverse dft of result
    // chop off filter.width - 1/filter.height - 1

    return result;
}

// template<typename T, size_t Channels, size_t Filter_Channels = Channels>
// image<T, Channels> dft_based(image<T, Channels> img,
//     image<T, Filter_Channels> filter)
// {
//     static_assert(Filter_Channels == Channels || Filter_Channels == 1,
//         "The filter must either have a single channel or the same number as the filtered image");

//     using scalar_t = float;

//     // set up buffers and plan for `img`
//     auto padded_w = img.width() * 2;
//     auto padded_h = img.height() * 2;

//     auto frq_buffer_size = padded_h * padded_w;
//     auto spatial_buffer_size = padded_h * padded_w;

//     std::complex<scalar_t> * img_spatial = reinterpret_cast<std::complex<scalar_t>*>(
//         fftwf_malloc(sizeof(std::complex<scalar_t>) *
//         spatial_buffer_size));
//     std::complex<scalar_t> * img_frequency = reinterpret_cast<std::complex<scalar_t>*>(
//         fftwf_malloc(sizeof(std::complex<scalar_t>) *
//         frq_buffer_size));

//     // forward dft
//     fftwf_plan p_img_fwd = fftwf_plan_dft_2d(padded_h, padded_w,
//         reinterpret_cast<fftwf_complex*>(img_spatial),
//         reinterpret_cast<fftwf_complex*>(img_frequency),
//         FFTW_FORWARD, FFTW_ESTIMATE);

//     // inverse dft
//     fftwf_plan p_img_inv = fftwf_plan_dft_2d(padded_h, padded_w,
//         reinterpret_cast<fftwf_complex*>(img_frequency),
//         reinterpret_cast<fftwf_complex*>(img_spatial),
//         FFTW_BACKWARD, FFTW_ESTIMATE);
        
//     // set up buffers and plan for filter
//     std::complex<scalar_t> * filter_spatial = reinterpret_cast<std::complex<scalar_t>*>(
//         fftwf_malloc(sizeof(std::complex<scalar_t>) *
//         spatial_buffer_size));
//     std::complex<scalar_t> * filter_frequency = reinterpret_cast<std::complex<scalar_t>*>(
//         fftwf_malloc(sizeof(std::complex<scalar_t>) *
//         frq_buffer_size));

//     fftwf_plan p_filter_fwd = fftwf_plan_dft_2d(padded_h, padded_w,
//         reinterpret_cast<fftwf_complex*>(filter_spatial),
//         reinterpret_cast<fftwf_complex*>(filter_frequency),
//         FFTW_FORWARD, FFTW_ESTIMATE);

//     // set up image for result
//     image<T, Channels> result(img.width(), img.height());

//     // compute dft, multiplication and inverse dft channel-wise
//     for (size_t c = 0; c < img.channels(); ++c) {
//         // set up the filter if necessary
//         if (Filter_Channels == Channels || c == 0) {
//             // calculate offset to make filter centred in padded image
//             auto offset_x = padded_w / 2 - filter.width() / 2;
//             auto offset_y = padded_h / 2 - filter.height() / 2;

//             for (size_t y = 0; y < padded_h; ++y) {
//                 for (size_t x = 0; x < padded_w; ++x) {
//                     // using offsets to center the filter in padded_w X padded_h
//                     if (x >= offset_x && x < offset_x + filter.width() &&
//                         y >= offset_y && y < offset_y + filter.height()) {
//                         filter_spatial[y * padded_w + x]
//                             = filter(x - offset_x, y - offset_y, c);
//                     } else {
//                         // all other values are set to 0
//                         filter_spatial[y * padded_w + x] = scalar_t{};
//                     }
//                 }
//             }

//             // FOR DEBUGGING
//             // image<T, 1> filter_spatial_buffer(filter_spatial, padded_w,
//             //     padded_h);
//             // print::image(filter_spatial_buffer, 20);
//             // write_image("../data/testing/filter_buffer.jpg", filter_spatial_buffer);
//             // std::cout << "gaussian sum: " << std::reduce(
//             //     filter_spatial, filter_spatial + spatial_buffer_size) << "\n";

//             // apply dft to filter_spatial
//             fftwf_execute(p_filter_fwd);

//             // FOR DEBUGGING
//             // std::vector<T> filter_frq(frq_buffer_size);
//             // std::transform(filter_frequency,
//             //     filter_frequency + frq_buffer_size,
//             //     filter_frq.begin(),
//             //     [](auto v){ return std::abs(v) / 100; });
//             // image<T, 1> filter_frq_buffer(filter_frq.data(), padded_w,
//             //     padded_h);
//             // print::image(filter_frq_buffer, 20);
//             // write_image("../data/testing/filter_frq.jpg", filter_frq_buffer);
//         }

//         // copy channel from `img` to img_spatial buffer and multiply by (-1)^(x+y)
//         for (size_t y = 0; y < padded_h; ++y) {
//             for (size_t x = 0; x < padded_w; ++x) {
//                 img_spatial[y * padded_w + x] =
//                     img(std::min(x, img.width() - 1),
//                         std::min(y, img.height() - 1),
//                         c);

//                 // if (x < img.width() && y < img.height()) {
//                 //     img_spatial[y * padded_w + x]
//                 //         = img(x, y, c) * std::pow(-1, x + y);
//                 // } else {
//                 //     // all other values are set to 0
//                 //     img_spatial[y * padded_w + x] = scalar_t{};
//                 // }
//             }
//         }

//         // FOR DEBUGGING
//         // image<T, 1> image_spatial_buffer(img_spatial, padded_w,
//         //     padded_h);
//         // print::image(image_spatial_buffer, 20);

//         // apply dft to img_spatial
//         fftwf_execute(p_img_fwd);

//         // FOR DEBUGGING
//         // std::vector<T> img_frq(frq_buffer_size);
//         // std::transform(img_frequency,
//         //     img_frequency + frq_buffer_size,
//         //     img_frq.begin(),
//         //     [](auto v){ return std::abs(v) / 100; });
//         // image<T, 1> img_frq_buffer(img_frq.data(), padded_w,
//         //     padded_h);
//         // print::image(img_frq_buffer, 20);
//         // write_image(
//         //     (std::string("../data/testing/img_frq_") +
//         //         std::to_string(c) + ".jpg").c_str(),
//         //     img_frq_buffer);

//         // FOR DEBUGGING
//         // fftwf_execute(p_img_inv);
//         // image<T, 1> image_spatial_buffer_re_inverted(img_spatial, padded_w,
//         //     padded_h);
//         // print::image(image_spatial_buffer_re_inverted, 20);
//         // write_image(
//         //     (std::string("../data/testing/img_re_inverted_") +
//         //         std::to_string(c) + ".jpg").c_str(),
//         //     image_spatial_buffer_re_inverted);

//         // multiply img_frequency with filter_frequency
//         for (size_t i = 0; i < frq_buffer_size; ++i) {
//             img_frequency[i] *= filter_frequency[i];
//         }

//         // FOR DEBUGGING
//         // std::vector<T> img_frq_mult(frq_buffer_size);
//         // std::transform(img_frequency,
//         //     img_frequency + frq_buffer_size,
//         //     img_frq_mult.begin(),
//         //     [](auto v){ return std::abs(v) / 100; });
//         // image<T, 1> img_frq_mult_buffer(img_frq_mult.data(), padded_w,
//         //     padded_h);
//         // print::image(img_frq_mult_buffer, 20);
//         // write_image(
//         //     (std::string("../data/testing/img_frq_mult_") +
//         //         std::to_string(c) + ".jpg").c_str(),
//         //     img_frq_mult_buffer);

//         // compute inverse dft of img_spatial
//         fftwf_execute(p_img_inv);

//         // FOR DEBUGGING
//         // std::vector<T> img_spatial_buffer_final_inverted(spatial_buffer_size);
//         // std::transform(img_spatial,
//         //     img_spatial + spatial_buffer_size,
//         //     img_spatial_buffer_final_inverted.begin(),
//         //     [&](auto v){ return v.real() / (spatial_buffer_size); });
//         // image<T, 1> image_spatial_buffer_final_inverted(
//         //     img_spatial_buffer_final_inverted.data(), padded_w, padded_h);
//         // print::image(image_spatial_buffer_final_inverted, 20);
//         // write_image(
//         //     (std::string("../data/testing/img_final_inverted_") +
//         //         std::to_string(c) + ".jpg").c_str(),
//         //     image_spatial_buffer_final_inverted);

//         // copy channel from img_spatial to result
//         auto offset_x = padded_w / 2;
//         auto offset_y = padded_h / 2;
//         for (size_t y = 0; y < img.height(); ++y) {
//             for (size_t x = 0; x < img.width(); ++x) {
//                 result(x, y, c) = std::real(img_spatial[
//                         // offset to get the result from lower-right quadrant
//                         (offset_y + y) * padded_w + x + offset_x
//                     ]) / (spatial_buffer_size);
//             }
//         }
//     }

//     // free all our plans and temp buffers
//     fftwf_destroy_plan(p_img_fwd);
//     fftwf_destroy_plan(p_filter_fwd);
//     fftwf_free(img_spatial);
//     fftwf_free(img_frequency);
//     fftwf_free(filter_spatial);
//     fftwf_free(filter_frequency);

//     // extend img to have dimensions L1 * L2,
//     // i.e. img.width + filter.width - 1 * img.height + filter.height - 1
//     // take filter to be centered at 0, repeat with period L1 * L2 and pad the rest with zeros
//     // take DFT of both
//     // multiply
//     // take inverse dft of result
//     // chop off filter.width - 1/filter.height - 1

//     return result;
// }

// template<typename T, size_t Channels, size_t Filter_Channels = Channels>
// image<T, Channels> dft_based(image<T, Channels> img,
//     image<T, Filter_Channels> filter)
// {
//     static_assert(Filter_Channels == Channels || Filter_Channels == 1,
//         "The filter must either have a single channel or the same number as the filtered image");

//     using scalar_t = float;

//     // set up buffers and plan for `img`
//     auto padded_w = img.width() * 2;
//     auto padded_h = img.height() * 2;

//     // auto complex_buffer_height = (padded_h / 2 + 1);
//     auto complex_buffer_width = (padded_w / 2 + 1);
//     // auto complex_buffer_size = padded_w * complex_buffer_height;
//     auto complex_buffer_size = padded_h * complex_buffer_width;

//     scalar_t * img_spatial = reinterpret_cast<scalar_t*>(
//         fftwf_malloc(sizeof(scalar_t) *
//         padded_w * padded_h));
//     std::complex<scalar_t> * img_frequency = reinterpret_cast<std::complex<scalar_t>*>(
//         fftwf_malloc(sizeof(std::complex<scalar_t>) *
//         complex_buffer_size));

//     // forward dft
//     fftwf_plan p_img_fwd = fftwf_plan_dft_r2c_2d(padded_h, padded_w,
//         img_spatial, reinterpret_cast<fftwf_complex*>(img_frequency),
//         FFTW_ESTIMATE);

//     // inverse dft
//     fftwf_plan p_img_inv = fftwf_plan_dft_c2r_2d(padded_h, padded_w,
//         reinterpret_cast<fftwf_complex*>(img_frequency), img_spatial,
//         FFTW_ESTIMATE);
        
//     // set up buffers and plan for filter
//     scalar_t * filter_spatial = reinterpret_cast<scalar_t*>(
//         fftwf_malloc(sizeof(scalar_t) *
//         padded_w * padded_h));
//     std::complex<scalar_t> * filter_frequency = reinterpret_cast<std::complex<scalar_t>*>(
//         fftwf_malloc(sizeof(std::complex<scalar_t>) *
//         complex_buffer_size));

//     fftwf_plan p_filter_fwd = fftwf_plan_dft_r2c_2d(padded_h, padded_w,
//         filter_spatial, reinterpret_cast<fftwf_complex*>(filter_frequency),
//         FFTW_ESTIMATE);

//     // set up image for result
//     image<T, Channels> result(img.width(), img.height());

//     // compute dft, multiplication and inverse dft channel-wise
//     for (size_t c = 0; c < img.channels(); ++c) {
//         // set up the filter if necessary
//         if (Filter_Channels == Channels || c == 0) {
//             // calculate offset to make filter centred in padded image
//             auto offset_x = padded_w / 2 - filter.width() / 2;
//             auto offset_y = padded_h / 2 - filter.height() / 2;

//             for (size_t y = 0; y < padded_h; ++y) {
//                 for (size_t x = 0; x < padded_w; ++x) {
//                     // using offsets to center the filter in padded_w X padded_h
//                     if (x >= offset_x && x < offset_x + filter.width() &&
//                         y >= offset_y && y < offset_y + filter.height()) {
//                         filter_spatial[y * padded_w + x]
//                             = filter(x - offset_x, y - offset_y, c) *
//                                 std::pow(-1, x + y);
//                     } else {
//                         // all other values are set to 0
//                         filter_spatial[y * padded_w + x] = scalar_t{};
//                     }
//                 }
//             }
//             // TODO: initialise rest of buffer

//             // FOR DEBUGGING
//             image<T, 1> filter_spatial_buffer(filter_spatial, padded_w,
//                 padded_h);
//             print::image(filter_spatial_buffer, 20);
//             // write_image("../data/testing/filter_buffer.jpg", filter_spatial_buffer);
//             std::cout << "gaussian sum: " << std::reduce(
//                 filter_spatial, filter_spatial + padded_w * padded_h) << "\n";

//             // apply dft to filter_spatial
//             fftwf_execute(p_filter_fwd);

//             // FOR DEBUGGING
//             std::vector<T> filter_frq(complex_buffer_size);
//             std::transform(filter_frequency,
//                 filter_frequency + complex_buffer_size,
//                 filter_frq.begin(),
//                 [](auto v){ return std::abs(v) / 100; });
//             image<T, 1> filter_frq_buffer(filter_frq.data(), complex_buffer_width,
//                 padded_h);
//             print::image(filter_frq_buffer, 20);
//             write_image("../data/testing/filter_frq.jpg", filter_frq_buffer);
//         }

//         // copy channel from `img` to img_spatial buffer and multiply by (-1)^(x+y)
//         for (size_t y = 0; y < padded_h; ++y) {
//             for (size_t x = 0; x < padded_w; ++x) {
//                 img_spatial[y * padded_w + x] =
//                     img(std::min(x, img.width() - 1),
//                         std::min(y, img.height() - 1),
//                         c) * std::pow(-1, x + y);

//                 // if (x < img.width() && y < img.height()) {
//                 //     img_spatial[y * padded_w + x]
//                 //         = img(x, y, c) * std::pow(-1, x + y);
//                 // } else {
//                 //     // all other values are set to 0
//                 //     img_spatial[y * padded_w + x] = scalar_t{};
//                 // }
//             }
//         }
//         // TODO: check if rest of buffer is already initialised and also think about alternative padding

//         // FOR DEBUGGING
//         image<T, 1> image_spatial_buffer(img_spatial, padded_w,
//             padded_h);
//         print::image(image_spatial_buffer, 20);

//         // apply dft to img_spatial
//         fftwf_execute(p_img_fwd);

//         // FOR DEBUGGING
//         std::vector<T> img_frq(complex_buffer_size);
//         std::transform(img_frequency,
//             img_frequency + complex_buffer_size,
//             img_frq.begin(),
//             [](auto v){ return std::abs(v) / 100; });
//         image<T, 1> img_frq_buffer(img_frq.data(), complex_buffer_width,
//             padded_h);
//         print::image(img_frq_buffer, 20);
//         write_image(
//             (std::string("../data/testing/img_frq_") +
//                 std::to_string(c) + ".jpg").c_str(),
//             img_frq_buffer);

//         // FOR DEBUGGING
//         // fftwf_execute(p_img_inv);
//         // image<T, 1> image_spatial_buffer_re_inverted(img_spatial, padded_w,
//         //     padded_h);
//         // print::image(image_spatial_buffer_re_inverted, 20);
//         // write_image(
//         //     (std::string("../data/testing/img_re_inverted_") +
//         //         std::to_string(c) + ".jpg").c_str(),
//         //     image_spatial_buffer_re_inverted);

//         // multiply img_frequency with filter_frequency
//         for (size_t i = 0; i < complex_buffer_size; ++i) {
//             img_frequency[i] *= filter_frequency[i];
//         }

//         // FOR DEBUGGING
//         std::vector<T> img_frq_mult(complex_buffer_size);
//         std::transform(img_frequency,
//             img_frequency + complex_buffer_size,
//             img_frq_mult.begin(),
//             [](auto v){ return std::abs(v) / 100; });
//         image<T, 1> img_frq_mult_buffer(img_frq_mult.data(), complex_buffer_width,
//             padded_h);
//         print::image(img_frq_mult_buffer, 20);
//         write_image(
//             (std::string("../data/testing/img_frq_mult_") +
//                 std::to_string(c) + ".jpg").c_str(),
//             img_frq_mult_buffer);

//         // compute inverse dft of img_spatial
//         fftwf_execute(p_img_inv);

//         // FOR DEBUGGING
//         std::vector<T> img_spatial_buffer_final_inverted(padded_w * padded_h);
//         std::transform(img_spatial,
//             img_spatial + complex_buffer_size,
//             img_spatial_buffer_final_inverted.begin(),
//             [&](auto v){ return v / (padded_w * padded_h); });
//         image<T, 1> image_spatial_buffer_final_inverted(
//             img_spatial_buffer_final_inverted.data(), padded_w, padded_h);
//         print::image(image_spatial_buffer_final_inverted, 20);
//         write_image(
//             (std::string("../data/testing/img_final_inverted_") +
//                 std::to_string(c) + ".jpg").c_str(),
//             image_spatial_buffer_final_inverted);

//         // copy channel from img_spatial to result
//         for (size_t y = 0; y < img.height(); ++y) {
//             for (size_t x = 0; x < img.width(); ++x) {
//                 // TODO: do we have to normalise or is this taken care of by (-1)^(x+y)
//                 result(x, y, c) = img_spatial[y * padded_w + x] *
//                     std::pow(-1, x + y) / (padded_w * padded_h);
//             }
//         }
//     }

//     // free all our plans and temp buffers
//     fftwf_destroy_plan(p_img_fwd);
//     fftwf_destroy_plan(p_filter_fwd);
//     fftwf_free(img_spatial);
//     fftwf_free(img_frequency);
//     fftwf_free(filter_spatial);
//     fftwf_free(filter_frequency);

//     // extend img to have dimensions L1 * L2,
//     // i.e. img.width + filter.width - 1 * img.height + filter.height - 1
//     // take filter to be centered at 0, repeat with period L1 * L2 and pad the rest with zeros
//     // take DFT of both
//     // multiply
//     // take inverse dft of result
//     // chop off filter.width - 1/filter.height - 1

//     return result;
// }

}

}

#endif // SPICE_CONVOLUTION
