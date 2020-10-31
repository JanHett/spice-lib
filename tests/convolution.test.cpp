#include <gtest/gtest.h>

#include <spice-lib/function.hpp>
#include <spice-lib/convolution.hpp>

using namespace spice;

// TEST(convolution, separable_pre_seperated) {
//     auto img = load_image<float, 3>("../data/testing/boat.jpg");
//     auto filtered = convolve::separable(img,
//         {filter::sobel_avg<float>()}, {filter::sobel_diff<float>()});
//     // auto filtered_per_channel = convolve::separable(img, gaussian_per_channel);
// }

// TEST(convolution, separable_combined) {
//     auto gaussian
//     auto img = load_image<float, 3>("../data/testing/boat.jpg");
//     auto blurred = convolve::separable(img, gaussian);
//     auto blurred_per_channel = convolve::separable(img, gaussian_per_channel);
// }

TEST(convolution, naive) {
    float std_deviation = 5;
    size_t g_width = 3 * std_deviation;
    size_t g_height = 3 * std_deviation;
    float begin = -(static_cast<float>(g_width) / 2);
    float end   =   static_cast<float>(g_width) / 2 + 0.1;
    float step = 1;

    std::cout << begin << " " << end << " " << step << "\n";

    auto gaussian_data = function::evaluate_binary<float, float>(
        [](float x, float y){ return function::gaussian(2, x, y); },
        begin, end, step,
        begin, end, step);
    image<float, 1> gaussian_filter(gaussian_data.data(), g_width, g_height);

    auto img = load_image<float, 3>("../data/testing/boat.jpg");
    auto blurred = convolve::naive(img, gaussian_filter);
    // auto blurred_per_channel = convolve::separable(img, gaussian_per_channel);

    write_image("../data/testing/boat_spatially_convolved.jpg", blurred);
}
