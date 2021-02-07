/**
 * \file noise_reduction.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief Contains implementations of noise reduction algorithms
 * \version 0.1
 * \date 2021-01-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SPICE_NOISE_REDUCTION
#define SPICE_NOISE_REDUCTION

#define _USE_MATH_DEFINES
#include <cmath>

#include <array>
#include <map>

#include "function.hpp"
#include "image.hpp"
#include "spatial_filtering.hpp"

namespace spice {

namespace non_local_means_internal {
    template<typename T, size_t Channels = 1>
    image<T, Channels> compute_integral_image(image<T, Channels> const & img) {
        image<T, Channels> iimg(img.width(), img.height());

        for (size_t c = 0; c < Channels; ++c) {
            iimg(0, 0, c) = img(0, 0, c);
            for (size_t x = 1; x < img.width(); ++x) {
                iimg(x, 0, c) = img(x, 0, c) + iimg(x - 1, 0, c);
            }
            for (size_t y = 1; y < img.height(); ++y) {
                for (size_t x = 0; x < img.width(); ++x) {
                    iimg(x, y, c) = img(x, y, c)
                        + iimg(x, y - 1, c)
                        + iimg(x - 1, y, c)
                        + iimg(x - 1, y - 1, c);
                }
            }
        }

        return iimg;
    }

    template<typename T, size_t Channels = 1>
    T sum_from_integral_image(image<T, Channels> const & iimg,
        size_t x1, size_t y1,
        size_t x2, size_t y2)
    {
        // make sure the user didn't muck up
        auto _x1 = std::min(x1, x2);
        auto _y1 = std::min(y1, y2);
        auto _x2 = std::max(x1, x2);
        auto _y2 = std::max(y1, y2);

        return iimg(_x2, _y2, 0)
            - iimg(_x2, _y1, 0)
            - iimg(_x1, _y2, 0)
            + iimg(_x1, _y1, 0);
    }

    /**
     * \brief box blur value at point p
     * 
     * \tparam T 
     * \param p_x 
     * \param p_y 
     * \param iimg the integral image
     * \return T 
     */
    template<typename T, size_t Channels = 1>
    T B(size_t p_x, size_t p_y,
        size_t radius,
        image<T, Channels> const & iimg)
    {
        auto x1 = (radius < p_x) * p_x - radius;
        auto y1 = (radius < p_y) * p_y - radius;
        auto x2 = (p_x + radius < iimg.width()) * p_x + radius;
        auto y2 = (p_y + radius < iimg.height()) * p_y + radius;

        auto w = x2 - x1;
        auto h = y2 - y1;

        return sum_from_integral_image(iimg, x1, y1, x2, y2) / (w * h);
    }

    /**
     * \brief Gaussian weighting function for nonlocal means
     * 
     * \tparam T 
     * \tparam Channels 
     * \param p_x 
     * \param p_y 
     * \param q_x 
     * \param q_y 
     * \param radius 
     * \param h filtering parameter
     * \param bblur box-blurred image 
     * \return T 
     */
    template<typename T, size_t Channels = 1>
    T weighting_fn(
        size_t p_x, size_t p_y,
        size_t q_x, size_t q_y,
        float h,
        image<T, Channels> const & bblur)
    {
        auto bb = bblur(q_x, q_y, 0) - bblur(p_x, p_y, 0);
        auto res = abs(std::pow(M_E, -((bb * bb) / (h * h))));
        return res;
    }

    // template<typename T, size_t Channels = 1>
    // T C(
    //     size_t p_x, size_t p_y,
    //     size_t radius,
    //     float h,
    //     image<T, Channels> const & iimg)
    // {
    //     T val = T{};

    //     for (size_t q_y = 0; q_y < iimg.height(); ++q_y) {
    //         for (size_t q_x = 0; q_x < iimg.width(); ++q_x) {
    //             val += weighting_fn(p_x, p_y, q_x, q_y, radius, h, iimg);
    //         }
    //     }

    //     return val;
    // }

    // template<typename T, size_t Channels = 1>
    // T non_local_means_for_pixel(
    //     size_t p_x, size_t p_y,
    //     size_t radius,
    //     float h,
    //     image<T, Channels> const & img,
    //     image<T, Channels> const & bblur)
    // {
    //     T val = T{};
    //     T normalization_factor = T{}; // aka C(p)

    //     for (size_t q_y = 0; q_y < img.height(); ++q_y) {
    //         for (size_t q_x = 0; q_x < img.width(); ++q_x) {
    //             val += img(q_x, q_y, 0) * weighting_fn(p_x, p_y, q_x, q_y,
    //                 h, bblur);
    //             normalization_factor += weighting_fn(p_x, p_y, q_x, q_y,
    //                 h, bblur);
    //         }
    //     }

    //     return val / normalization_factor;
    // }
    template<typename T, size_t Channels = 1>
    T non_local_means_for_pixel(
        typename std::vector<std::pair<T, std::pair<size_t, size_t>>>::iterator const & pxl,
        size_t radius,
        float h,
        std::vector<std::pair<T, std::pair<size_t, size_t>>> const & sorted_img,
        image<T, Channels> const & bblur,
        T min_contributing_weight)
    {
        T val = T{};
        T normalization_factor = T{}; // aka C(p)

        // traverse lower values
        for (auto lower_it = pxl; lower_it >= sorted_img.begin(); --lower_it) {
            auto weight = weighting_fn(
                pxl->second.first, pxl->second.second,
                lower_it->second.first, lower_it->second.second,
                h, bblur);

            // std::cout << "weight: " << weight << "\n";

            auto weighted_pxl = pxl->first * weight;
            val += weighted_pxl;
            normalization_factor += weight;

            // break if result is too small
            if (weight < min_contributing_weight) break;
        }
        // traverse higher values
        for (auto higher_it = pxl + 1; higher_it < sorted_img.end(); ++higher_it) {
            auto weight = weighting_fn(
                pxl->second.first, pxl->second.second,
                higher_it->second.first, higher_it->second.second,
                h, bblur);

            auto weighted_pxl = pxl->first * weight;
            val += weighted_pxl;
            normalization_factor += weight;

            // break if result is too small
            if (weight < min_contributing_weight) break;
        }
        

        // for (size_t q_y = 0; q_y < bblur.height(); ++q_y) {
        //     for (size_t q_x = 0; q_x < bblur.width(); ++q_x) {
        //         val += img(q_x, q_y, 0) * weighting_fn(p_x, p_y, q_x, q_y,
        //             h, bblur);
        //         normalization_factor += weighting_fn(p_x, p_y, q_x, q_y,
        //             h, bblur);
        //     }
        // }

        return val / normalization_factor;
    }
}

/**
 * \brief Reduce noise by non-local means filtering
 * 
 * \tparam T assumed to be a type that does not overflow at white/black points
 * \tparam Channels 
 * \param img 
 * \param std_deviation 
 * \param radius
 * \return image<T, Channels> 
 */
template<typename T, size_t Channels = 4>
image<T, Channels> non_local_means (image<T, Channels> const & img,
    float std_deviation, size_t radius)
{
    image<T, Channels> result(img.width(), img.height());
    auto bblur = box_blur(img, radius);
    spice::write_image("../data/testing/bblur.jpg", bblur);
    // auto iimg = non_local_means_internal::compute_integral_image(img);
    auto npixels = img.width() * img.height();

    // vector of {value, {x, y}}
    std::vector<std::pair<T, std::pair<size_t, size_t>>> v(npixels);

    for (size_t c = 0; c < Channels; ++c) {
        std::cout << "Channel " << c << " of " << Channels << "\n";

        image<T, 1> c_bblur(bblur.data() + c * npixels, bblur.width(), bblur.height());

        // copy value and coordinate pair to vector
        for (size_t y = 0; y < img.height(); ++y) {
            for (size_t x = 0; x < img.width(); ++x) {
                v[y * img.width() + x] = {
                    img(x, y, c),
                    {x, y}
                };
            }
        }
        std::cout << "copying done\n";

        // sort vector by value
        std:sort(v.begin(), v.end(),
            [&](auto & a, auto & b){
                return c_bblur(a.second.first, a.second.second, 0) < c_bblur(b.second.first, b.second.second, 0);
                // return a.first < b.first;
            });

        // for (auto & vv: v) {
        //     std::cout << "{ " << vv.first << ", { " << vv.second.first << ", " << vv.second.second << " }\n";
        // }

        std::cout << "sorting done\n";

        for (auto pxl = v.begin(); pxl != v.end(); ++pxl) {
            if (!(distance(v.begin(), pxl) % 1000)) {
                std::cout << "Calculating pixel " << distance(v.begin(), pxl) << "\n";
            }

            // quadratic threshold function => fewer iterations for higher
            // values where noise is less apparent
            // TODO: derive threshold from weighting function and scale that one
            auto threshold = pxl->first * pxl->first + 0.01f;
            result(pxl->second.first, pxl->second.second, c) = non_local_means_internal::
                non_local_means_for_pixel(
                    pxl,
                    radius, std_deviation,
                    v, c_bblur,
                    threshold);
        }

        // for (size_t p_y = 0; p_y < img.height(); ++p_y) {
        //     std::cout << "line " << p_y << " of " << img.height() << "\n";
        //     for (size_t p_x = 0; p_x < img.width(); ++p_x) {
        //         result(p_x, p_y, c) = non_local_means_internal::
        //             non_local_means_for_pixel(
        //                 p_x, p_y,
        //                 radius, std_deviation,
        //                 c_img, c_iimg);
        //     }
        // }
    }
    // for (size_t c = 0; c < Channels; ++c) {
    //     std::cout << "Channel " << c << " of " << Channels << "\n";
    //     image<T, 1> c_img(img.data() + c * npixels, img.width(), img.height());
    //     image<T, 1> c_bblur(bblur.data() + c * npixels, bblur.width(), bblur.height());

    //     for (size_t p_y = 0; p_y < img.height(); ++p_y) {
    //         std::cout << "line " << p_y << " of " << img.height() << "\n";
    //         for (size_t p_x = 0; p_x < img.width(); ++p_x) {
    //             result(p_x, p_y, c) = non_local_means_internal::
    //                 non_local_means_for_pixel(
    //                     p_x, p_y,
    //                     radius, std_deviation,
    //                     c_img, c_bblur);
    //         }
    //     }
    // }

    return result;
}

// namespace non_local_means_internal {
//     template<typename T, size_t Channels = 1>
//     image<T, Channels> compute_integral_image(image<T, Channels> const & img) {
//         image<T, Channels> iimg(img.width(), img.height());

//         for (size_t c = 0; c < Channels; ++c) {
//             iimg(0, 0, c) = img(0, 0, c);
//             for (size_t x = 1; x < img.width(); ++x) {
//                 iimg(x, 0, c) = img(x, 0, c) + iimg(x - 1, 0, c);
//             }
//             for (size_t y = 1; y < img.height(); ++y) {
//                 for (size_t x = 0; x < img.width(); ++x) {
//                     iimg(x, y, c) = img(x, y, c)
//                         + iimg(x, y - 1, c)
//                         + iimg(x - 1, y, c)
//                         + iimg(x - 1, y - 1, c);
//                 }
//             }
//         }

//         return iimg;
//     }

//     template<typename T, size_t Channels = 1>
//     T sum_from_integral_image(image<T, Channels> const & iimg,
//         size_t x1, size_t y1,
//         size_t x2, size_t y2)
//     {
//         // make sure the user didn't muck up
//         auto _x1 = std::min(x1, x2);
//         auto _y1 = std::min(y1, y2);
//         auto _x2 = std::max(x1, x2);
//         auto _y2 = std::max(y1, y2);

//         return iimg(_x2, _y2, 0)
//             - iimg(_x2, _y1, 0)
//             - iimg(_x1, _y2, 0)
//             + iimg(_x1, _y1, 0);
//     }

//     /**
//      * \brief box blur value at point p
//      * 
//      * \tparam T 
//      * \param p_x 
//      * \param p_y 
//      * \param iimg the integral image
//      * \return T 
//      */
//     template<typename T, size_t Channels = 1>
//     T B(size_t p_x, size_t p_y,
//         size_t radius,
//         image<T, Channels> const & iimg)
//     {
//         auto x1 = (radius < p_x) * p_x - radius;
//         auto y1 = (radius < p_y) * p_y - radius;
//         auto x2 = (p_x + radius < iimg.width()) * p_x + radius;
//         auto y2 = (p_y + radius < iimg.height()) * p_y + radius;

//         auto w = x2 - x1;
//         auto h = y2 - y1;

//         return sum_from_integral_image(iimg, x1, y1, x2, y2) / (w * h);
//     }

//     /**
//      * \brief Gaussian weighting function for nonlocal means
//      * 
//      * \tparam T 
//      * \tparam Channels 
//      * \param p_x 
//      * \param p_y 
//      * \param q_x 
//      * \param q_y 
//      * \param radius 
//      * \param h filtering parameter
//      * \param iimg 
//      * \return T 
//      */
//     template<typename T, size_t Channels = 1>
//     T weighting_fn(
//         size_t p_x, size_t p_y,
//         size_t q_x, size_t q_y,
//         size_t radius,
//         float h,
//         image<T, Channels> const & iimg)
//     {
//         auto bb = B(q_x, q_y, radius, iimg);
//         return std::pow(M_E, -((bb * bb) / (h * h)));
//     }

//     // template<typename T, size_t Channels = 1>
//     // T C(
//     //     size_t p_x, size_t p_y,
//     //     size_t radius,
//     //     float h,
//     //     image<T, Channels> const & iimg)
//     // {
//     //     T val = T{};

//     //     for (size_t q_y = 0; q_y < iimg.height(); ++q_y) {
//     //         for (size_t q_x = 0; q_x < iimg.width(); ++q_x) {
//     //             val += weighting_fn(p_x, p_y, q_x, q_y, radius, h, iimg);
//     //         }
//     //     }

//     //     return val;
//     // }

//     template<typename T, size_t Channels = 1>
//     T non_local_means_for_pixel(
//         size_t p_x, size_t p_y,
//         size_t radius,
//         float h,
//         image<T, Channels> const & img,
//         image<T, Channels> const & iimg
//         )
//     {
//         T val = T{};
//         T normalization_factor = T{}; // aka C(p)

//         for (size_t q_y = 0; q_y < img.height(); ++q_y) {
//             for (size_t q_x = 0; q_x < img.width(); ++q_x) {
//                 val += img(q_x, q_y, 0) * weighting_fn(p_x, p_y, q_x, q_y, radius, h, iimg);
//                 normalization_factor += weighting_fn(p_x, p_y, q_x, q_y, radius, h, iimg);
//             }
//         }

//         return val / normalization_factor;
//     }
// }

// /**
//  * \brief Reduce noise by non-local means filtering
//  * 
//  * \tparam T assumed to be a type that does not overflow at white/black points
//  * \tparam Channels 
//  * \param img 
//  * \param std_deviation 
//  * \param radius
//  * \return image<T, Channels> 
//  */
// template<typename T, size_t Channels = 4>
// image<T, Channels> non_local_means (image<T, Channels> const & img,
//     float std_deviation, size_t radius)
// {
//     image<T, Channels> result(img.width(), img.height());
//     auto iimg = non_local_means_internal::compute_integral_image(img);
//     auto npixels = img.width() * img.height();

//     for (size_t c = 0; c < Channels; ++c) {
//         std::cout << "Channel " << c << " of " << Channels << "\n";
//         image<T, 1> c_img(img.data() + c * npixels, img.width(), img.height());
//         image<T, 1> c_iimg(iimg.data() + c * npixels, iimg.width(), iimg.height());

//         for (size_t p_y = 0; p_y < img.height(); ++p_y) {
//             std::cout << "line " << p_y << " of " << img.height() << "\n";
//             for (size_t p_x = 0; p_x < img.width(); ++p_x) {
//                 result(p_x, p_y, c) = non_local_means_internal::
//                     non_local_means_for_pixel(
//                         p_x, p_y,
//                         radius, std_deviation,
//                         c_img, c_iimg);
//             }
//         }
//     }

//     return result;
// }

// namespace non_local_means_internal {
//     template<typename T, typename F_weight>
//     void normalization_fn(color_view<T> const & p,
//         image<T> const & img,
//         float radius,
//         F_weight weighting_fn,
//         color_view<T> & out)
//     {
//         out = T{};
//         for (size_t y = 0; y < img.height(); ++y) {
//             for (size_t x = 0; x < img.width(); ++x) {
//                 out += weighting_fn(p, img(x, y), radius)
//             }
//         }
//     }

//     template<typename T>
//     void gaussian_weighting_fn(color_view<T> const & p,
//         color_view<T> const & q,
//         float h,
//         color_view<T> & out)
//     {
//         out = std::pow(M_E, );
//     }
// }

// /**
//  * \brief Calculates the non-local means over the image
//  * 
//  * \tparam T 
//  * \tparam F_weight A callable of the form
//  * `f(color_view<T> a, color_view<T> b, float h)` where b is the value to be
//  * compared to a and h is the filtering parameter
//  *
//  * \param img The noisy input image
//  * \param radius The radius for the weighting function
//  * \param weighting_fn A function to determine the similarity of two points in
//  * the image
//  * \return image<T> 
//  */
// template<typename T, typename F_weight>
// image<T> non_local_means (image<T> const & img,
//     float radius, F_weight weighting_fn)
// {
//     image<T> filtered(img.width(), img.height(), img.channels());
//     std::vector<T> sum_cv_data(img.channels());
//     color_view<T> sum(sum_cv_data.data());
    
//     std::vector<T> normalization_cv_data(img.channels());
//     color_view<T> normalization(normalization_cv_data.data());

//     for (size_t y = 0; y < img.height(); ++y) {
//         for (size_t x = 0; x < img.width(); ++x) {
//             sum = T{};
//             for (size_t y_sum = 0; y_sum < img.height(); ++y_sum) {
//                 for (size_t x_sum = 0; x_sum < img.width(); ++x_sum) {
//                     sum += img(x_sum, y_sum)
//                         * weighting_fn(img(x, y), img(x_sum, y_sum), radius)
//                         * non_local_means_internal::normalization_fn(img(x, y),
//                             img, radius, weighting_fn, normalization)
//                 }
//             }
//             filtered(x, y) = sum;
//         }
//     }
//     return img;
// }

// /**
//  * \brief Reduce noise by non-local means filtering
//  * 
//  * \tparam T assumed to be a type that does not overflow at white/black points
//  * \tparam Channels 
//  * \param img 
//  * \param std_deviation 
//  * \param max_difference maximum difference of pixel value to consider -- TODO: derive this from std_deviation?
//  * \return image<T, Channels> 
//  */
// template<typename T, size_t Channels = 4>
// image<T, Channels> non_local_means (image<T, Channels> const & img,
//     float std_deviation, T const & max_difference)
// {
//     auto pixel_count = img.width() * img.height();
//     // gaussian-blur the image
//     auto blurred_image = gaussian_blur(img, std_deviation);
//     std::cout << "blurring done\n";

//     // separate the blurred image into channels
//     // std::array<image<T, 1>, Channels> blurred_images;
//     // for (size_t c = 0; c < Channels; ++c) {
//     //     // TODO: avoid that copy
//     //     blurred_images[c] = image<T, 1>(
//     //         blurred_image.data() + c * pixel_count,
//     //         blurred_image.width(), blurred_image.height()
//     //         );
//     // }

//     // std::cout << "separation done\n";

//     // one map per channel: value => coordinate, sorted by value
//     using value_coordinate_map_t = std::multimap<T, std::pair<size_t, size_t>>;
//     std::array<value_coordinate_map_t, Channels> sorted_values;
//     for (size_t c = 0; c < Channels; ++c) {
//         for (size_t y = 0; y < blurred_image.height(); ++y) {
//             for (size_t x = 0; x < blurred_image.width(); ++x) {
//                 sorted_values[c].insert({
//                     blurred_image(x, y, c),
//                     {x, y}
//                 });
//             }
//         }
//     }

//     std::cout << "maps created:\n";
//     for (auto & m: sorted_values) {
//         std::cout << "size: " << m.size() << '\n';
//     }

//     image<T, Channels> result(img.width(), img.height());

//     // for each channel:
//     for (size_t c = 0; c < Channels; ++c) {
//         std::cout << ">>>> Channel " << c + 1 << " of " << Channels << "\n";
//         // for each pixel:
//         for (size_t p = 0; p < pixel_count; ++p) {
//             // find interval of "interesting pixels" (to define)
//             auto min_interesting_value = img.data()[c * pixel_count + p]
//                 - max_difference;
//             auto max_interesting_value = img.data()[c * pixel_count + p]
//                 + max_difference;

//             auto min_val_it = sorted_values[c].lower_bound(
//                 min_interesting_value);

//             std::advance(min_val_it, -(min_val_it != sorted_values[c].begin()));

//             // calculate weighted average of pixels at "interesting pixel" locations in the original image
//             T avg = T{};
//             if (!(p % 1000)) {
//                 std::cout << ">> Pixel " << p + 1 << " of " << pixel_count << " in channel " << c + 1 << " of " << Channels << "\n";
//                 std::cout << ">> +-> min_interesting_value: " << min_interesting_value << ", max_interesting_value: " << max_interesting_value << "\n"
//                     << ">> +-> min_val_it: " << min_val_it->first << "\n";
//             }
//             size_t contributing_pixels = 0;
//             for(; min_val_it->first <= max_interesting_value && min_val_it != sorted_values[c].end(); ++min_val_it) {
//                 auto xy = min_val_it->second;
//                 avg += img(xy.first, xy.second, c);
//                 ++contributing_pixels;
//             }

//             if (!(p % 1000)) {
//                 std::cout << ">> +-> " << "averaging over " << contributing_pixels << " contributing_pixels\n";
//             }

//             avg /= contributing_pixels; // TODO: straight average here, turn this into a weighted one

//             // set that value in the output image
//             result.data()[c * pixel_count + p] = avg;
//         }
//     }
//     // for (size_t c = 0; c < Channels; ++c) {
//     //     std::cout << ">>>> Channel " << c + 1 << " of " << Channels << "\n";
//     //     // for each pixel:
//     //     for (size_t p = 0; p < pixel_count; ++p) {
//     //         // find interval of "interesting pixels" (to define)
//     //         auto min_interesting_value = img.data()[c * pixel_count + p]
//     //             - max_difference;
//     //         auto max_interesting_value = img.data()[c * pixel_count + p]
//     //             + max_difference;

//     //         auto min_val_it = sorted_values[c].lower_bound(
//     //             min_interesting_value);
//     //         auto max_val_it = sorted_values[c].upper_bound(
//     //             max_interesting_value);

//     //         std::advance(min_val_it, -(min_val_it == max_val_it && min_val_it != sorted_values[c].begin()));
//     //         std::advance(max_val_it, min_val_it == max_val_it && max_val_it != sorted_values[c].end());
//     //         // calculate weighted average of pixels at "interesting pixel" locations in the original image
//     //         T avg = T{};
//     //         // this is linear time but I don't see how to avoid this when we're
//     //         // calculating a weighted average
//     //         auto contributing_pixels = std::distance(min_val_it, max_val_it);
//     //         if (!(p % 1000)) {
//     //             std::cout << ">> Pixel " << p + 1 << " of " << pixel_count << " in channel " << c + 1 << " of " << Channels << "\n";
//     //             std::cout << ">> +-> " << "averaging over " << contributing_pixels << " contributing_pixels\n";
//     //             std::cout << ">> +-> min_interesting_value: " << min_interesting_value << ", max_interesting_value: " << max_interesting_value << "\n"
//     //                 << ">> +-> min_val_it: " << min_val_it->first << ", max_val_it: " << max_val_it->first << "\n";
//     //         }
//     //         for(; min_val_it != max_val_it && min_val_it != sorted_values[c].end(); ++min_val_it) {
//     //             auto xy = min_val_it->second;
//     //             avg += img(xy.first, xy.second, c) / contributing_pixels; // TODO: straight average here, turn this into a weighted one
//     //         }

//     //         // set that value in the output image
//     //         result.data()[c * pixel_count + p] = avg;
//     //     }
//     // }

//     std::cout << "result done\n";

//     return result;
// }

/**
 * \brief Reduce noise by non-local means filtering
 * 
 * \tparam T assumed to be a type that does not overflow at white/black points
 * \tparam Channels 
 * \param img 
 * \param std_deviation 
 * \param max_difference maximum difference of pixel value to consider -- TODO: derive this from std_deviation?
 * \param max_contributors maximum number of pixels to average over
 * \return image<T, Channels> 
 */
// template<typename T, size_t Channels = 4>
// image<T, Channels> non_local_means (image<T, Channels> const & img,
//     float std_deviation, T const & max_difference, size_t max_contributors)
// {
//     auto pixel_count = img.width() * img.height();
//     // gaussian-blur the image
//     // auto blurred = image<T, Channels>(img);
//     auto blurred = gaussian_blur(img, std_deviation); // maybe needs a box blur instead?
//     std::cout << "blurring done\n";

//     // vector of {value, {x, y}}
//     std::vector<std::pair<T, std::pair<size_t, size_t>>> v(pixel_count);
//     image<T, Channels> result(img.width(), img.height());

//     // for each channel
//     for (size_t c = 0; c < img.channels(); ++c) {
//         std::cout << "starting channel " << c << "\n";
//         // copy value and coordinate pair to vector
//         for (size_t y = 0; y < blurred.height(); ++y) {
//             for (size_t x = 0; x < blurred.width(); ++x) {
//                 v[y * blurred.width() + x] = std::pair{
//                     blurred(x, y, c),
//                     std::pair{x, y}
//                 };
//             }
//         }
//         std::cout << "copying done\n";

//         // for (auto & vv: v) {
//         //     std::cout << "{ " << vv.first << ", { " << vv.second.first << ", " << vv.second.second << " }\n";
//         // }

//         // sort vector by value
//         std:sort(v.begin(), v.end(),
//             [](auto & a, auto & b){ return a.first < b.first; });

//         std::cout << "sorting done\n";

//         // step through values and calculate average
//         for (auto val = v.begin(); val < v.end(); ++val) {
//             // find interval of "interesting pixels" (to define)
//             auto min_interesting_value = val->first - max_difference;
//             auto max_interesting_value = val->first + max_difference;

//             T avg = T{};
//             size_t contributing_pixels = 0;

//             for (size_t offset = 0; offset < max_contributors; ++offset) {
//                 auto lower_val = val - offset - 1;
//                 auto higher_val = val + offset;

//                 if (lower_val->first >= min_interesting_value && lower_val >= v.begin()) {
//                     avg += function::gaussian(
//                         std_deviation,
//                         lower_val->second.first,
//                         lower_val->second.second
//                     ) * img(lower_val->second.first, lower_val->second.second, c);
//                     ++contributing_pixels;
//                 }
//                 if (higher_val->first <= max_interesting_value && higher_val < v.end()) {
//                     avg += function::gaussian(
//                         std_deviation,
//                         higher_val->second.first,
//                         higher_val->second.second
//                     ) * img(higher_val->second.first, higher_val->second.second, c);
//                     ++contributing_pixels;
//                 }

//                 if (lower_val < v.begin() && higher_val >= v.end()) {
//                     break;
//                 }
//             }

//             // for (
//             //     auto lower_val = val;
//             //     lower_val->first >= min_interesting_value && lower_val >= v.begin();
//             //     --lower_val)
//             // {
//             //     avg += lower_val->first;
//             //     ++contributing_pixels;
//             // }
//             // for (
//             //     auto higher_val = val + 1;
//             //     higher_val->first <= max_interesting_value && higher_val < v.end();
//             //     ++higher_val)
//             // {
//             //     avg += higher_val->first;
//             //     ++contributing_pixels;
//             // }


//             // avg /= contributing_pixels;
//             // std::cout << ">> +-> " << "average over " << contributing_pixels << " contributing_pixels: " << avg << "\n";

//             // write this average to the position
//             // std::cout << "writing " << avg << " to " << val->second.first << ", " << val->second.second << ", " << c << "\n";
//             result(val->second.first, val->second.second, c) = avg;

//         }
//         std::cout << "channel " << c << " done\n";
//     }
//     std::cout << "result done\n";

//     return result;
// }

}

#endif // SPICE_NOISE_REDUCTION
