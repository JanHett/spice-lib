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
    print::image<float, 3>(boat, 10);

    auto thresholded = threshold::adaptive(boat, 0.5f, 50);

    // print result for debugging
    print::image<float, 3>(thresholded, 10);
}
