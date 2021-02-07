/**
 * \file spatial_filtering.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief Defines common spatial filters
 * \version 0.1
 * \date 2021-01-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SPICE_SPACIAL_FILTERING
#define SPICE_SPACIAL_FILTERING

#include "image.hpp"
#include "convolution.hpp"

namespace spice {

/**
 * \brief Returns a copy of the image that was convolved with a gaussian filter
 * 
 * \tparam T 
 * \tparam Channels 
 * \param img 
 * \param std_deviation 
 * \return image<T, Channels> 
 */
template<typename T, size_t Channels>
image<T, Channels> gaussian_blur(image<T, Channels> img, float std_deviation)
{
    float g_width  = 6 * std_deviation + 1;
    float g_height = 6 * std_deviation + 1;
    float begin    = -(g_width / 2);
    float end      =   g_width / 2;
    float step     = 1;

    auto gaussian_data = function::evaluate_unary<float, float>(
        [&](float x){ return function::gaussian(std_deviation, x); },
        begin, end, step);
    image<float, 1> gaussian_filter_h(gaussian_data.data(), g_width, 1);
    image<float, 1> gaussian_filter_v(gaussian_data.data(), 1, g_height);

    return convolve::separable(img, gaussian_filter_h, gaussian_filter_v);
}

template<typename T, size_t Channels>
image<T, Channels> box_blur(image<T, Channels> img, size_t radius) {
    auto d = radius * 2 + 1;
    std::vector<T> datavec(radius * 2 + 1, 1.f / d);
    image<float, 1> box_filter_h(datavec.data(), d, 1);
    image<float, 1> box_filter_v(datavec.data(), 1, d);

    return convolve::separable(img, box_filter_h, box_filter_v);
}

}

#endif // SPICE_SPACIAL_FILTERING