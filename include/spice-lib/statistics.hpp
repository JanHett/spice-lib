/**
 * \file
 * This header defines a collection of statistical algorithms to analyse
 * images.
 */
#ifndef SPICE_STATISTICS
#define SPICE_STATISTICS

#include <vector>
#include <cmath>

#include "image.hpp"

namespace spice {
/**
 * \brief This namespace exports a number of functions for calculating
 * statistics on images
 */
namespace statistics {
    /**
     * Calculates the source image's histogram. The resulting histogram will
     * contain the absolute count of pixel values matching each class.
     *
     * \param source The image to analyse
     * \param samples The granularity with which to divide the intensity range
     * of the image.
     * \returns A vector of vectors of length `samples` where each of the
     * sub-vectors represents the histogram of one channel.
     */
    template<typename T, size_t Channels>
    std::vector<std::vector<size_t>> histogram(
        image<T, Channels> const & source,
        size_t samples)
    {
        // create the empty histogram
        std::vector<std::vector<size_t>> hist;
        for (size_t chan = 0; chan < Channels; ++chan)
            hist.push_back(std::vector<size_t>(samples, 0));

        auto data_length = source.width() * source.height();
        // convert the max to double - this is mainly to avoid issues with
        // uint8_t
        double max_val = static_cast<double>(image<T>::max);
        // do the counting
        for (size_t chan = 0; chan < Channels; ++chan)
        {
            for (
                size_t offset = 0;
                offset < data_length;
                ++offset)
            {
                T clamped_val = std::clamp(
                    source.data()[chan * data_length + offset],
                    image<T>::min,
                    image<T>::max);
                // calculate which class the pixel's channel value belongs to...
                size_t intensity_class = std::lround(
                    (clamped_val / max_val)
                    * (samples - 1));

                // and increment the relevant class' counter
                ++hist[chan][intensity_class];
            }
        }

        return hist;
    }
}
}

#endif // SPICE_STATISTICS
