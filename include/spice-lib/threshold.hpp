#ifndef SPICE_THRESHOLD
#define SPICE_THRESHOLD

#include "image.hpp"

#include <adaptive_threshold_generator.h>

#include <HalideBuffer.h>

namespace spice {
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
        Halide::Runtime::Buffer<float>::make_interleaved(
            // flatten `color` structure into just the channel values
            reinterpret_cast<float*>(input.data().data()),
            input.width(),
            input.height(),
            input.channels());

    Halide::Runtime::Buffer<float> out_buf =
        Halide::Runtime::Buffer<float>::make_interleaved(
            // flatten `color` structure into just the channel values
            reinterpret_cast<float*>(output.data().data()),
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