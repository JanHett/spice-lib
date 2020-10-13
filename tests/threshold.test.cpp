#include <gtest/gtest.h>

#include <type_traits>
#include <limits>
#include <numeric>
#include <cstdint>
#include <fstream>

#include <gsl/span>

#include <spice-lib/image.hpp>
#include <spice-lib/statistics.hpp>
#include <spice-lib/print.hpp>
#include <spice-lib/threshold.hpp>

#include "test_utils.hpp"

using namespace spice;

TEST(threshold, adaptive) {
    auto boat = load_image<float, 3>("../data/testing/boat.jpg");

    // print image for debugging
    // print::image<float, 3>(boat, 10);

    auto thresholded = threshold::adaptive(boat, 0.5f, 50);

    // print result for debugging
    // print::image<float, 3>(thresholded, 10);

    // result's histpgram should only have two modes
    auto hist = statistics::histogram(thresholded, 50);

    // print histogram for debugging
    // print::histogram(hist, 100);

    // inactive pixels should be non-zero
    EXPECT_EQ(76186, *hist[0].begin());
    EXPECT_EQ(64183, *hist[1].begin());
    EXPECT_EQ(80841, *hist[2].begin());
    
    // fully activated pixels should be non-zero
    EXPECT_EQ(133734, *--hist[0].end());
    EXPECT_EQ(145737, *--hist[1].end());
    EXPECT_EQ(129079, *--hist[2].end());
    
    // anything else should be 0
    for (auto channel_hist : hist) {
        for (auto sample = ++channel_hist.begin();
            sample < --channel_hist.end();
            ++sample)
        {
            EXPECT_EQ(0, *sample);
        }
    }
}
