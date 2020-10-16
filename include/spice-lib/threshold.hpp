/**
 * \file threshold.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief 
 * \version 0.1
 * \date 2020-10-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SPICE_THRESHOLD
#define SPICE_THRESHOLD

#include "image.hpp"

#include <adaptive_threshold_generator.h>

#include <HalideBuffer.h>

namespace spice {
/**
 * \brief The namespace `threshold` includes functions for determining thresholds
 * as well as segmenting images according to a pre-determined threshold
 * 
 */
namespace threshold {

// helpers
namespace {
    template<typename T, size_t Channels>
    T * flat_cast (color<T, Channels> * c) {
        return reinterpret_cast<T *>(c);
    }
}

/**
 * \brief Segment the image channel-wise using an adaptive thresholding
 * mechanism
 *
 * The implemented algorithm is adapted from the paper ["Adaptive Thresholding
 * Using the Integral Image" by Bradley and Roth](
 * https://people.scs.carleton.ca/~roth/iit-publications-iti/docs/gerh-50002.pdf)
 * 
 * \todo Generalise this to arbitrary image types and adapt the Halide code to
 * work with different channel counts
 * 
 * \tparam Channels 
 * \param input 
 * \param threshold 
 * \param radius 
 * \return image<float, Channels> 
 */
template<size_t Channels>
image<float, Channels> adaptive(image<float, Channels> & input,
    float threshold,
    int radius)
{
    image<float, Channels> output(input.width(), input.height());

    Halide::Runtime::Buffer<float> in_buf =
        Halide::Runtime::Buffer<float>(
            input.data(),
            input.width(),
            input.height(),
            input.channels());

    Halide::Runtime::Buffer<float> out_buf =
        Halide::Runtime::Buffer<float>(
            output.data(),
            output.width(),
            output.height(),
            output.channels());

    auto success = adaptive_threshold_generator(in_buf, threshold, radius, out_buf);

    if (success != 0)
        std::cout << "Failed to apply adaptive threshold\n";

    return output;
}

}
}

#endif // SPICE_THRESHOLD